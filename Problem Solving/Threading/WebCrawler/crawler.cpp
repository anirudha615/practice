#include <iostream>
#include <chrono>
#include <thread>
#include <future>
#include <memory>
#include <condition_variable>
#include <mutex>

#include <queue>
#include <set>
#include <unordered_map>

using namespace std;

std::unordered_map<string, std::vector<string>> pageStructure = {
    // Home page - links to main sections
    {"example.com", {"example.com/about", "example.com/blog", "example.com/products", "example.com/services", "example.com/contact"}},
    
    // About section
    {"example.com/about", {"example.com/about/team", "example.com/about/history", "example.com/about/careers", "example.com/contact"}},
    {"example.com/about/team", {"example.com/about/team/ceo", "example.com/about/team/cto", "example.com/about/careers"}},
    {"example.com/about/history", {"example.com/about", "example.com/blog/company-news"}},
    {"example.com/about/careers", {"example.com/about/team", "example.com/contact", "example.com/about/careers/engineering"}},
    {"example.com/about/team/ceo", {"example.com/about/team", "example.com/blog/leadership"}},
    {"example.com/about/team/cto", {"example.com/about/team", "example.com/products/technical"}},
    {"example.com/about/careers/engineering", {"example.com/about/careers", "example.com/products"}},
    
    // Blog section
    {"example.com/blog", {"example.com/blog/tech", "example.com/blog/company-news", "example.com/blog/tutorials", "example.com/about", "external.com"}},
    {"example.com/blog/tech", {"example.com/blog/tech/ai", "example.com/blog/tech/web-dev", "example.com/products/technical"}},
    {"example.com/blog/company-news", {"example.com/blog", "example.com/about/history", "example.com/about/team"}},
    {"example.com/blog/tutorials", {"example.com/blog/tech", "example.com/services/training", "example.com/blog/tutorials/beginner"}},
    {"example.com/blog/tech/ai", {"example.com/blog/tech", "example.com/products/ai-tools"}},
    {"example.com/blog/tech/web-dev", {"example.com/blog/tech", "example.com/services/web-development"}},
    {"example.com/blog/leadership", {"example.com/blog", "example.com/about/team/ceo"}},
    {"example.com/blog/tutorials/beginner", {"example.com/blog/tutorials", "example.com/services/training"}},
    
    // Products section
    {"example.com/products", {"example.com/products/software", "example.com/products/hardware", "example.com/products/ai-tools", "example.com/products/technical"}},
    {"example.com/products/software", {"example.com/products", "example.com/products/software/mobile", "example.com/products/software/web"}},
    {"example.com/products/hardware", {"example.com/products", "example.com/products/hardware/servers", "example.com/contact"}},
    {"example.com/products/ai-tools", {"example.com/products", "example.com/blog/tech/ai", "example.com/services/consulting"}},
    {"example.com/products/technical", {"example.com/products", "example.com/about/team/cto", "example.com/blog/tech"}},
    {"example.com/products/software/mobile", {"example.com/products/software", "example.com/services/app-development"}},
    {"example.com/products/software/web", {"example.com/products/software", "example.com/services/web-development"}},
    {"example.com/products/hardware/servers", {"example.com/products/hardware", "example.com/services/infrastructure"}},
    
    // Services section
    {"example.com/services", {"example.com/services/consulting", "example.com/services/training", "example.com/services/web-development", "example.com/services/app-development"}},
    {"example.com/services/consulting", {"example.com/services", "example.com/products/ai-tools", "example.com/contact"}},
    {"example.com/services/training", {"example.com/services", "example.com/blog/tutorials", "example.com/about/careers"}},
    {"example.com/services/web-development", {"example.com/services", "example.com/products/software/web", "example.com/blog/tech/web-dev"}},
    {"example.com/services/app-development", {"example.com/services", "example.com/products/software/mobile"}},
    {"example.com/services/infrastructure", {"example.com/services", "example.com/products/hardware/servers"}},
    
    // Contact section
    {"example.com/contact", {"example.com/about", "example.com/services", "example.com/contact/locations", "example.com/contact/support"}},
    {"example.com/contact/locations", {"example.com/contact", "example.com/about"}},
    {"example.com/contact/support", {"example.com/contact", "example.com/services", "example.com/blog/tutorials"}}
};

/**
 * Link: https://www.hellointerview.com/community/questions/web-crawler-multithreaded/cm5eguhad02yw838ot9p159t6
 */
class Crawler {
private:
// Communication channel between main thread and thread pool
std::condition_variable m_doorbell;
std::vector<std::thread> m_threadPool; // thread pool
std::atomic<int32_t> m_activeWorkers{0};

// shared state
std::queue<string> m_queue;
std::set<string> m_visitedURL;
std::vector<string> m_URLsCrawled;

// mutex for the shared state
std::mutex m_sharedStateMutex;
std::atomic<bool> m_shutDownCrawling {false};
std::atomic<bool> m_resumeCrawling {false};

string m_primaryDomainURL;

    // url example could be https://example.com:443/about. 
    // However, we know domain would be present for sure.
    bool isDomainSame(const std::string url) {
        size_t start = 0;
        size_t found = url.find("://", start);
        // checking if protocol like HTTP or HTTPS exists
        if (found != string::npos) {
            start = found + 3; // 3 for "://"
        }

        found = url.find_first_of(":/", start); // here start could be 0 if there is no protocol
        if (found == string::npos) {
            found = url.length(); //we didn't find any port or path and since we know domain is 100% present, we take found = url.length
        }

        string domain = url.substr(start, found-start); // found includes either ":" or "/", so found would be exclusive and start woud be inclusive
        return domain == m_primaryDomainURL;
    }

    void startCrawling() {
        while (!m_shutDownCrawling.load()) {
            std::string urlToCrawl;

            {
                // lock the mutex
                std::unique_lock<std::mutex> lock(m_sharedStateMutex);

                // Make all threads sleep for 10 seconds until either of the conditions are true -
                // 1. Queue is not empty and you receive a resume signal
                // 2. You receive a shutdown signal
                // The thread will automatically wake up after every 10 seconds 
                m_doorbell.wait_for(lock, std::chrono::seconds(10), [this] {
                    return (!m_queue.empty() && m_resumeCrawling.load()) || m_shutDownCrawling.load();
                });

                // STEP 1: Once the thread wakes up, it evaluates the exit condition which is either - 
                //  1. Shut down signal is called and 
                //  2. Queue is empty and active workers are 0
                if (m_shutDownCrawling.load() || (m_queue.empty() && !m_activeWorkers.load())) {
                    break; // exit the loop
                }

                // If you reached here, that means either the signal is to resume or thread woke up after 10 seconds
                // STEP 2: Check the condition for waking up again and then, pop the front of the queue and mark it as visited
                if (!m_queue.empty() && m_resumeCrawling.load()) {
                    ++m_activeWorkers; // A thread has started to do work
                    urlToCrawl = m_queue.front();
                    m_queue.pop();
                    m_visitedURL.insert(urlToCrawl);
                }
            }

            // STEP 4: Crawl the URL
            if (!urlToCrawl.empty()) {
                // Simulate Crawling by adding a timer
                // You will receive a list of URLs which is similar to pageStructure.at(urlToCrawl)
                std::this_thread::sleep_for(std::chrono::seconds(5));
                {
                    std::lock_guard<std::mutex> lock(m_sharedStateMutex);
                    for (string linkedURL : pageStructure[urlToCrawl]) {
                        if (isDomainSame(linkedURL) && !m_visitedURL.count(linkedURL)) {
                            m_queue.push(linkedURL);
                        }
                    }
                    m_URLsCrawled.push_back(urlToCrawl);
                }
                --m_activeWorkers; // A thread has finished the work
            }
        }
    }

public:

    Crawler(const std::string domainURL, int maxThreads) 
    : m_primaryDomainURL(domainURL) {
        for (int i = 0; i < maxThreads; i++) {
            m_threadPool.emplace_back(&Crawler::startCrawling, this);
        }
        m_queue.push(domainURL);
    }

    void pauseCrawling() {
        m_resumeCrawling = false;
        m_doorbell.notify_all();
    }

    void resumeCrawling() {
        m_resumeCrawling = true;
        m_doorbell.notify_all();
    }

    void shutDownCrawling() {
        m_shutDownCrawling = true;
        m_doorbell.notify_all();

        for (auto &worker : m_threadPool) {
            if (worker.joinable()) {
                worker.join(); // cleanup threads gracefully
            }
        }
    }

    std::vector<std::string> getCrawledURLs() {
        return m_URLsCrawled;
    }
};



void printCrawledURLs(const std::vector<std::string>& urls) {
    std::cout << "Crawled URLs (in BFS order):" << std::endl;
    for (size_t i = 0; i < urls.size(); ++i) {
        std::cout << "  " << (i + 1) << ". " << urls[i] << std::endl;
    }
    std::cout << "Total pages crawled: " << urls.size() << std::endl;
}

int main() {
    
    // Test: Pause and Resume Functionality  
    Crawler crawler("example.com", 7);
    std::cout << "Starting crawler with 7 threads..." << std::endl;
    
    // Start crawling
    crawler.resumeCrawling();
    std::cout << "Crawling started and let it crawl for 20 secodns" << std::endl;
    std::this_thread::sleep_for(std::chrono::seconds(20));
    
    // Pause crawling
    crawler.pauseCrawling();
    std::cout << "Paused crawler and waiting for 2 seconds" << std::endl;
    std::this_thread::sleep_for(std::chrono::seconds(2));
    auto urlsAfterPause = crawler.getCrawledURLs();
    printCrawledURLs(urlsAfterPause);
    
    // Resume crawling
    crawler.resumeCrawling();
    std::cout << "Resumed crawler and let it crawl of 20 seconds" << std::endl;
    std::this_thread::sleep_for(std::chrono::seconds(20));

    // Shut down crawling
    crawler.shutDownCrawling();
    std::cout << "shut down crawler and waiting for 2 seconds" << std::endl;
    std::this_thread::sleep_for(std::chrono::seconds(2));
    auto finalURLs = crawler.getCrawledURLs();
    printCrawledURLs(finalURLs);  

    std::cout << "\n" << std::string(60, '=') << std::endl;
    std::cout << "ALL TESTS COMPLETED" << std::endl;
    std::cout << std::string(60, '=') << std::endl;

    return 0;
}
