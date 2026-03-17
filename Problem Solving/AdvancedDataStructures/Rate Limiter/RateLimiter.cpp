#include <iostream>
#include <unordered_map>
#include <memory>
#include <thread>
#include <mutex>
#include <chrono>
#include <algorithm>
#include <cmath>

using namespace std;

/**
 * 2 types of Rate Limiting - 
 *   1. Concurrency Rate Limiting - Maximum In-flight requests at any moment 
 *   2. Time-window Rate Limiting - Number of requests in a given window. If the window slides, more requests can be accomodated.
 * 
 * The Time-window Rate Limiting works on three levels - 
 *    a. Per User Per microservice
 *    b. Per IP Per microservice
 *    c. Globally for 1 system (for all microservice)
 * 
 * Algorithms for Time-window Rate Limiting - 
 *  1. Fixed Window/Tumbling window: 
 *      a. You will need to create new keys for all the three levels as soon timestamp rolls over to a new boundary. Hence, Key management is a pain.
 *      b. One sudden burst would fill the window and you will have to wait for the rest of the window.
 *  2. Sliding window:
 *      a. Similar to Fixed window, you will need to create new keys as timestamp rolls over to a new boundary. However, you will also need to preserve the previous window. 
 *      b. This is because when when its time to evaluate if the request is allowed, you will see what portion of the current window falls in rate limiting window
 *         and get the remaining count from the previous window. For example if the TPS is X and rate limting window is 1 sec and the current window
 *         is only filled for 400ms. Then the current window weight is 40% and the previous window weight is 60%. So, we fetch the 
 *         previous window count + current window count and check if it is below the threshold.
 *         60% of cache[previousWindow] + cache[currentWindow] <= threshold --> Request Allowed.
 *      c. Similar to Fixed window, Key Management is a huge pain.
 *      d. Sudden burst will be a lesser issue as compared to Fixed window as we are playing with weights across two windows.
 *         So, if the previous window gets a burst at the starting of its window at 100ms, it will be accounted when we are calculating 60% 
 *         of the previous window count, which is last 600ms. So, technically it shouldn't be included but 
 *         the algorithm puts weight on the previous window uniformly.
 *  3. Token Bucket - 
 *      a. You don't have to create new keys for every timestamp. So, key management is relatively easier as compared to Sliding Window logic.
 *      b. For every request, you need to validate if the bucket needs to be refilled and then check if the request can pass through.
 *         You should refill only if it is required.
 *      c. One sudden burst would empty the token but at the next request, we will check if the bucket needs to be refilled and then evaluate again.
 * 
 * Prefer Token Bucket over Sliding window in interviews.
 */

 /**
  * Requirements - 
  *   1. The thresholds of rate limiting is static and is not dynamic.
  *   2. The system receives request with user/clientId and endpoint
    {
        "clientId" ""
        "endpoint": "/search",
        "algorithm": "TokenBucket", or Sliding Window
        "algoConfig": {
            "capacity": 1000,
            "refillRatePerSecond": 10
        }
    }
      3. System enforces rate limiting againt client Id per endpoint and globally.
      4. If the request does not have "algorithm", use default "Token bucket"
      5. The Returned Structure would be {"isAllowed": true/false, "remaining": int, retryAfterMs: long}
  */

  /**
   * Model Classes first - 
   *    1. Rate Limiting Interface - Rate Limiting Algorithms
   *    2. Rate Limiting Request (Input)
   *    3. Rate Limiting Response (Output)
   *    4. RateLimitingAlgorithm (ENUM)
   *    5. Individual configuration per Algorithm
   * 
   * 
   * REVISE - 
   *     a. Allow asynchronous requests between clients and make bucket update synchronous for each client.
   */

// Decorated Input for Rate Limiting Interface
class RateLimitingRequest {
    std::vector<std::string> m_requestParams;
public:
    RateLimitingRequest(const std::string& userId, const std::string& ipAddress, const std::string& endpoint) 
        : m_requestParams({userId, ipAddress, endpoint}) {}
    
    std::vector<std::string> getParams() {
        return m_requestParams;
    }
};

// Decorated Output for Rate Limiting Interface
class RateLimitingResponse {
private:
    bool m_isAllowed;
    double m_remainingQuota;
    long m_retryAfter;
public:
    RateLimitingResponse(bool isAllowed, double remainingQuota, long retryAfter) 
    : m_isAllowed(isAllowed),
    m_remainingQuota(remainingQuota),
    m_retryAfter(retryAfter) {}

    void setIsAllowed(bool isAllowed) {
        m_isAllowed = isAllowed;
    }

    bool getIsAllowed() {
        return m_isAllowed;
    }

    void setRemainingQuota(int remainingQuota) {
        m_remainingQuota = remainingQuota;
    }

    double getRemainingQuota() {
        return m_remainingQuota;
    }

    void setRetryAfter(int retryAfter) {
        m_retryAfter = retryAfter;
    }

    long getRetryAfter() {
        return m_retryAfter;
    }
};

// Levels of Rate Limiting
enum RateLimitingLevel {
    User,
    IPAddress,
    EndPoint,
    count
};

// Interface for Rate Limiting
class IRateLimiter {
public:
    virtual ~IRateLimiter() = default;
    virtual RateLimitingResponse isAllowed(RateLimitingRequest request) = 0;
};

class TokenBucketRateLimiter : public IRateLimiter {
private:
    struct TokenBucket {
        // Preventing multiple threads to modify the same token bucket via fine-grained locking
        std::mutex bucket_mutex;
        double tokens; // differ per bucket
        long lastRefillTime; // differ per bucket

        TokenBucket() = default;
        TokenBucket(double initialTokens, long time)
            : tokens(initialTokens), lastRefillTime(time) {}
    };

    // Prevent multiple threads to update the cache via fine-grained locking
    std::mutex cache_mutex; 

    // This cache holds <UserId, RespectiveTokenBucket>, <IPAddress, RespectiveTokenBucket>
    // Extending to <APIEndpoint, RespectiveTokenBucket>
    std::unordered_map<std::string, std::shared_ptr<TokenBucket>> m_cache;
    double m_threshold; // same for all buckets
    double m_refillRatePerSecond; // same for all buckets

public:
    TokenBucketRateLimiter(double threshold, double refillRate) 
        : m_threshold(threshold), 
          m_refillRatePerSecond(refillRate) {}

    long getCurrentTimeInSec() {
        return std::chrono::duration_cast<std::chrono::seconds>(
            std::chrono::system_clock::now().time_since_epoch()).count();
    }
    
    std::shared_ptr<TokenBucket> getOrCreateTokenBucket(const std::string& key) {
        std::lock_guard<std::mutex> lock(cache_mutex);
        if (m_cache.count(key)) {
            return m_cache[key];
        }

        std::shared_ptr<TokenBucket> bucket  = std::make_shared<TokenBucket>(m_threshold, getCurrentTimeInSec());
        m_cache[key] = bucket;
        return bucket;
    }

    RateLimitingResponse isAllowed(RateLimitingRequest request) override {        
        RateLimitingResponse response (false, INT_MAX, 0);
        std::vector<std::string> requestParams = request.getParams();
        for (int index = 0; index < static_cast<int>(RateLimitingLevel::count); index++) {
            string param = requestParams.at(index);

            // STEP 1: Get the token bucket for the user first
            std::shared_ptr<TokenBucket> tokenBucketPerParam = getOrCreateTokenBucket(param);

            {
                // STEP 2: Lock that bucket so that if the same user send the request at the same time via 2 or more threads, the bucket get updated once.
                std::lock_guard<std::mutex> lock(tokenBucketPerParam->bucket_mutex);

                // STEP 2: Calculate total Tokens after checking if refill is necessary (Pessimistic locking)
                long currentTimeInSeconds = getCurrentTimeInSec();
                double tokenToBeRefilled = (currentTimeInSeconds - tokenBucketPerParam->lastRefillTime) * m_refillRatePerSecond;
                double totalTokensToEvaluateAgainst = std::min(tokenBucketPerParam->tokens + tokenToBeRefilled, m_threshold); // READ

                // STEP 3: Check totalTokensToEvaluateAgainst == existing tokens in the bucket. If no, update token bucket
                if (totalTokensToEvaluateAgainst != tokenBucketPerParam->tokens) {
                    tokenBucketPerParam->tokens = totalTokensToEvaluateAgainst; // WRITE
                    tokenBucketPerParam->lastRefillTime = currentTimeInSeconds;
                }

                // STEP 4: Check if the request should be allowed. If yes, deduct 1 token and update the cache
                if (tokenBucketPerParam->tokens >= 1) {
                    tokenBucketPerParam->tokens = tokenBucketPerParam->tokens - 1;
                    response.setIsAllowed(true);
                    response.setRemainingQuota(std::min(response.getRemainingQuota(), tokenBucketPerParam->tokens));
                    response.setRetryAfter(0);
                    continue; // To check for the second bucket
                }
            }

            // If any of user and IP bucket does not allow, return immediately with retryAfter to be = time to generate 1 token
            return RateLimitingResponse(false, 0, 1000/m_refillRatePerSecond);
        }

        return response;
    }
};

class RateLimitingFactory {
public:
    static std::unique_ptr<IRateLimiter> getRateLimiter(const std::string& algorithm, const double threshold, const double refillRate) {
        if (algorithm == "TokenBucket") {
            return std::make_unique<TokenBucketRateLimiter>(threshold, refillRate);
        }

        return std::make_unique<TokenBucketRateLimiter>(threshold, refillRate);
    }
};

/**
 * Workflow - 
 *   1. From the algorithm and config in UserRequest, select the right rate limting algorithm.
 *   2. Convert the user Request to RateLimitingRequest
 *   3. Call isAllowed via the interface to check if the request is valid for all three levels and then return the response
 */
class RateLimiterController {
private:
    std::unique_ptr<IRateLimiter> m_rateLimiter;
    RateLimitingFactory factory;
public:
    RateLimiterController(const std::string& algo, const double threshold, const double refillRate)
    : m_rateLimiter(factory.getRateLimiter(algo, threshold, refillRate)) {}    

    void isRequestAllowed(const std::string& userId, const std::string& ipAddress, const std::string& endpoint) {
        RateLimitingRequest request (userId, ipAddress, endpoint);
        RateLimitingResponse response = m_rateLimiter->isAllowed(request);
        print(response);
    }

    void print(RateLimitingResponse response) {
        std::cout << response.getIsAllowed() << ", " << response.getRemainingQuota() << ", " << response.getRetryAfter() << std::endl;
    }
};

int main() {
    RateLimiterController orchestrator ("TokenBucket", 1000, 10);
    orchestrator.isRequestAllowed("123", "127.0.0.1", "search");
    orchestrator.isRequestAllowed("123", "127.0.0.1", "search");
    orchestrator.isRequestAllowed("123", "127.0.0.1", "search");
    orchestrator.isRequestAllowed("123", "127.0.0.1", "search");
    orchestrator.isRequestAllowed("123", "127.0.0.2", "post");
}
