#include <iostream>
#include <vector>
#include <string>
#include <memory>

using namespace std;

struct SampleStack {
  double timestamp;
  std::vector<std::string> funtionStack;
};

struct Trace {
  std::string operation;
  double timestamp;
  std::string functionName;
};

class IFilter {
public:
    virtual void analyzeTrace(const std::vector<Trace> originalTraces) = 0;
    virtual std::vector<Trace> getFilteredTraces() = 0;
};

/**
 * Since it is a contiguous problem, this will be a sliding window approach.
 *   a.When the Function name changes, that means it is not contiguous, hence update the starting boundary, 
 *     frequency and tracking function name to the current.
 *   b. Before that, make sure to check if the previous tracked function has exceeded the threshold of contiguous.
 *      If yes, then add to the list. IF not, continue with Step #A.
 *   c. If the Function name didn't change, always increement the frequency when you see END operation.
 *   d. Once the loop is over, check if the last tracked function's frequency exceeded threshold. If  yes, repeat Step #B.
 */
class ContiguousFrequencyFilter : public IFilter {
private:
    int m_threshold = 3;
    std::vector<Trace> m_FilteredTraces;
public:

    void analyzeTrace(const std::vector<Trace> originalTraces) override {
        int currentTrackingFunctionFirstIndex = 0;
        std::string currentTrackingFunctionName = "";
        int currentTrackingFunctionFrequency = 0;
        for (int index = 0; index < originalTraces.size(); index++) {
            Trace currentTrace = originalTraces[index];

            // If the current trace does not match previous,
            if (currentTrace.functionName != currentTrackingFunctionName) {
                
                // and the previous trace frequency is > threshold, persist that information.
                if (currentTrackingFunctionFrequency >= m_threshold) {
                    for (int i = currentTrackingFunctionFirstIndex; i < index; i++) {
                        m_FilteredTraces.push_back(originalTraces.at(i));
                    }
                }

                // Once persisted, set the current tracked function name and its first index
                currentTrackingFunctionName = currentTrace.functionName;
                currentTrackingFunctionFirstIndex = index;
                currentTrackingFunctionFrequency = 0;
                continue;
            }

            // Once you land here that means it is contiguous. So, only increment the frequency when the operation is END.
            if (currentTrace.operation == "END") {
                currentTrackingFunctionFrequency++;
            }
        }

        // Checking the last function
        if (currentTrackingFunctionFrequency >= m_threshold) {
            for (int i = currentTrackingFunctionFirstIndex; i < originalTraces.size(); i++) {
                m_FilteredTraces.push_back(originalTraces.at(i));
            }
        }
    }

    std::vector<Trace> getFilteredTraces() override {
        return m_FilteredTraces;
    }
};

/**
 * Logic is compare the START and END of traces and see if their duration is above a minute.
 *  1. Since all the traces are in chronological order, keep adding to the stack if function name don't match.
 *  2. If function name and operation name both matched, push to the stack as they could be recursive in nature.
 *  3. If function name matched but operation name didn't (START != END), pop the front element and compare the 
 *     timestamp and add both of them to filtered traces list.
 */
class EliminateNoisyFilter : public IFilter {
private:
    long m_threshold = 60;
    std::vector<Trace> m_FilteredTraces;
public:

    // This eliminates based on non-exclusive timestamp
    void analyzeTrace(const std::vector<Trace> originalTraces) override {
       std::stack<Trace> traceStack;
       traceStack.push({"START", -1, "Garbage"});
       for (auto& trace : originalTraces) {
            if (trace.functionName == traceStack.top().functionName && trace.operation != traceStack.top().operation) {
                // IF function name match but operation names don't match, its time to evaluate.
                Trace topTrace = traceStack.top();
                traceStack.pop();
                if (trace.timestamp - topTrace.timestamp >= m_threshold) {
                    m_FilteredTraces.push_back(topTrace); // START TRACE
                    m_FilteredTraces.push_back(trace); // END TRACE
                }
            } else {
                // If function names don't match, blindly push
                // If function names match and the operation name also matched, that means there is a recursive function, push
                traceStack.push(trace);
            }
        }
    }

    // This eliminates based on exclusive timestamp
    void analyzeTraceByCalculatingExclusiveTimestamps(const std::vector<Trace> originalTraces) {
        std::stack<Trace> traceStack;
        std::unordered_map<string, int> functionNameToTimeToSubstract;
        std::unordered_map<string, int> functionNameToExclusiveTimestamp;

        for (const Trace& trace : originalTraces) {
            if (trace.operation == "END" && !traceStack.empty() && traceStack.top().functionName == trace.functionName) {
                // STEP 1: Calculate the non-exclusive timestamp for that function
                Trace& traceStart = traceStack.top();
                traceStack.pop();
                int nonExclusiveTimestamp = trace.timestamp - traceStart.timestamp + 1;
                // STEP 2: Put the non-exclusive timestamp for the current function to the top of the stack (for nested behavior)
                if (!traceStack.empty()) {
                    functionNameToTimeToSubstract[traceStack.top().functionName] += nonExclusiveTimestamp;
                }
                // STEP 3: Calculate the exclusive timestamp for that function.
                if (functionNameToTimeToSubstract.count(trace.functionName)) {
                    functionNameToExclusiveTimestamp[trace.functionName] += (nonExclusiveTimestamp - functionNameToTimeToSubstract[trace.functionName]);
                } else {
                    functionNameToExclusiveTimestamp[trace.functionName] += nonExclusiveTimestamp;
                }
                // STEP 4: Filtering
                if (functionNameToExclusiveTimestamp[trace.functionName] > m_threshold) {
                    m_FilteredTraces.push_back(traceStart); // START TRACE
                    m_FilteredTraces.push_back(trace); // END TRACE
                }
            } else {
                traceStack.push(trace);
            }
        }
    }

    std::vector<Trace> getFilteredTraces() override {
        return m_FilteredTraces;
    }
};

/**
 * Logic is -  Compare the new and previous function stack and find the index where both the function stack differ.
 *  1. From end of previous function stack till the index, push the END events
 *  2. From the index till the end of new function stack, push the START events
 */
class Profiler {
private:
    std::vector<std::string> m_previousFunctionStack;
    std::vector<Trace> m_traces;
    std::vector<std::unique_ptr<IFilter>> m_filters;

    /**
     * FOLLOWUP: We can also achieve this using post-fix but we need to reverse both m_currentFunctionStack and new function stack
     * and then count the commonFunctionIndex from the last of the list.
     */
    /** Find the index of the first mismatch between current function stack and the new function stack */
    int getIndexOfFirstMismatch(const std::vector<string> newFunctionStack) {
        int minimumLength = std::min(newFunctionStack.size(), m_previousFunctionStack.size());
        int commonFunctionIndex = 0;
        for (int index = 0; index < minimumLength; index++) {
            if (newFunctionStack[index] == m_previousFunctionStack[index]) {
                commonFunctionIndex++;
            } else {
                break;
            }
        }
        return commonFunctionIndex;
    }

public:
    Profiler() {
        m_filters.push_back(std::make_unique<ContiguousFrequencyFilter>());
        m_filters.push_back(std::make_unique<EliminateNoisyFilter>());
    }

    void convertSampleStacksToTraces(const std::vector<SampleStack> sampleStackList) {
        for (auto& sampleStack : sampleStackList) {

            // STEP 1: Get the index of the first mismatch between new and current function stack
            int commonLastFunctionIndex = getIndexOfFirstMismatch(sampleStack.funtionStack);

            // STEP 2: From commonLastFunctionIndex till the end of the current function stack, eveything got ended.
            // Hence, we need to print them in reverse as the function call are in stack format
            for (int index = m_previousFunctionStack.size() - 1; index >= commonLastFunctionIndex; index--) {
                m_traces.push_back({"END", sampleStack.timestamp, m_previousFunctionStack[index]});
            }

            // STEP 3: From commonLastFunctionIndex to the end of the new function stack, everything got started. 
            // Hence, we need to create them.
            for (int index = commonLastFunctionIndex; index < sampleStack.funtionStack.size(); index++) {
                m_traces.push_back({"START", sampleStack.timestamp, sampleStack.funtionStack[index]});
            }

            // STEP 4: Update the new function stack as the current one
            m_previousFunctionStack = sampleStack.funtionStack;
        }
    }

    void filterTraces(int index) {
        m_filters.at(index)->analyzeTrace(getTraces());
        m_traces = m_filters.at(index)->getFilteredTraces();
    }

    std::vector<Trace> getTraces() {
        return m_traces;
    }
};

void printTestHeader(const std::string& testName) {
    std::cout << "\n" << std::string(50, '=') << std::endl;
    std::cout << "TEST: " << testName << std::endl;
    std::cout << std::string(50, '=') << std::endl;
}

void printTraces(const std::vector<Trace>& traces) {
    std::cout << "Generated traces:" << std::endl;
    for (const auto& trace : traces) {
        std::cout << trace.operation << " " << trace.timestamp << " " << trace.functionName << std::endl;
    }
    std::cout << "Total events: " << traces.size() << std::endl;
}

int main() {
    // Test 1: Basic Nested Function Calls
    printTestHeader("Basic Nested Function Calls");
    {
        Profiler p;
        
        std::vector<SampleStack> samples = {
            {1.0, {"main"}},
            {2.0, {"main", "func1"}}, 
            {3.0, {"main", "func1", "func2"}},
            {4.0, {"main", "func1"}},  // func2 ends
            {5.0, {"main"}},           // func1 ends
            {6.0, {}}                  // main ends (empty stack)
        };
        
        std::cout << "Input samples:" << std::endl;
        for (const auto& sample : samples) {
            std::cout << "Time " << sample.timestamp << ": [";
            for (const auto& func : sample.funtionStack) {
                std::cout << func << " ";
            }
            std::cout << "]" << std::endl;
        }
        
        p.convertSampleStacksToTraces(samples);
        printTraces(p.getTraces());
    }
    
    // Test 2: Contiguous Frequency Filter (Functions appearing >= 3 times)
    printTestHeader("Frequency Filter - Functions Called 3+ Times Consecutively");
    {
        Profiler p;
        
        std::vector<SampleStack> samples = {
            {1.0, {"main"}},
            {2.0, {"main", "frequentFunc"}},     // frequentFunc call #1
            {2.5, {"main"}},                     // frequentFunc ends
            {3.0, {"main", "frequentFunc"}},     // frequentFunc call #2  
            {3.5, {"main"}},                     // frequentFunc ends
            {4.0, {"main", "frequentFunc"}},     // frequentFunc call #3
            {4.5, {"main"}},                     // frequentFunc ends
            {5.0, {"main", "frequentFunc"}},     // frequentFunc call #4 (>= 3, should qualify)
            {5.5, {"main"}},                     // frequentFunc ends
            {6.0, {"main", "rareFunc"}},         // rareFunc call #1 (only appears once)
            {6.5, {"main"}},                     // rareFunc ends
            {7.0, {}}                            // main ends
        };
        
        std::cout << "Input samples:" << std::endl;
        for (const auto& sample : samples) {
            std::cout << "Time " << sample.timestamp << ": [";
            for (const auto& func : sample.funtionStack) {
                std::cout << func << " ";
            }
            std::cout << "]" << std::endl;
        }
        
        // Generate all traces first
        p.convertSampleStacksToTraces(samples);
        std::cout << "\nAll generated traces:" << std::endl;
        printTraces(p.getTraces());
        
        // Apply frequency filter (threshold = 3)
        p.filterTraces(0);
        std::cout << "Expected: frequentFunc events should be included, rareFunc should be filtered out" << std::endl;
        printTraces(p.getTraces());
    }

    // Test 3: Recursive Function Calls with EliminateNoisyFilter
    printTestHeader("Recursive Function Calls with Duration Filtering");
    {
        Profiler p;
        
        std::vector<SampleStack> samples = {
            {1.0, {"main"}},
            {2.0, {"main", "recursiveFuncA"}},                         // 1st recursiveFunc starts
            {3.0, {"main", "recursiveFuncA", "recursiveFuncB"}},        // 2nd recursiveFunc starts (recursive call)
            {65.0, {"main", "recursiveFuncA"}},                        // 2nd recursiveFunc ends (62 seconds - long duration)
            {66.0, {"main", "recursiveFuncA", "recursiveFuncC"}},       // 3rd recursiveFunc starts
            {67.0, {"main", "recursiveFuncA"}},                        // 3rd recursiveFunc ends (1 second - short duration)
            {130.0, {"main"}},                                        // 1st recursiveFunc ends (128 seconds - very long duration)
            {131.0, {}}                                               // main ends
        };
        
        std::cout << "Input samples (recursive functions with different durations):" << std::endl;
        for (const auto& sample : samples) {
            std::cout << "Time " << sample.timestamp << ": [";
            for (const auto& func : sample.funtionStack) {
                std::cout << func << " ";
            }
            std::cout << "]" << std::endl;
        }
        
        // Generate all traces first
        p.convertSampleStacksToTraces(samples);
        std::cout << "\nAll generated traces (showing recursive function handling):" << std::endl;
        printTraces(p.getTraces());
        
        // Apply duration filter to show stack-based start/end pairing
        p.filterTraces(1);  // Use EliminateNoisyFilter (index 1)
        std::cout << "\nAll Duration filterec traces :" << std::endl;
        printTraces(p.getTraces());
    }
    
    // Test 4: Combined Filters - Both Frequency and Duration
    printTestHeader("Combined Filter Test - Frequent AND Long Duration");
    {
        Profiler p;
        
        std::vector<SampleStack> samples = {
            {1.0, {"main"}},
            {2.0, {"main", "heavyFunc"}},        // heavyFunc call #1 starts
            {65.0, {"main"}},                    // heavyFunc call #1 ends (63 seconds)
            {66.0, {"main", "heavyFunc"}},       // heavyFunc call #2 starts  
            {130.0, {"main"}},                   // heavyFunc call #2 ends (64 seconds)
            {131.0, {"main", "heavyFunc"}},      // heavyFunc call #3 starts
            {195.0, {"main"}},                   // heavyFunc call #3 ends (64 seconds)
            {196.0, {"main", "quickFunc"}},      // quickFunc appears once, short duration
            {197.0, {"main"}},                   // quickFunc ends (1 second)
            {200.0, {}}                          // main ends
        };
        
        std::cout << "Input samples (testing both frequency and duration):" << std::endl;
        for (const auto& sample : samples) {
            std::cout << "Time " << sample.timestamp << ": [";
            for (const auto& func : sample.funtionStack) {
                std::cout << func << " ";
            }
            std::cout << "]" << std::endl;
        }
        
        // Generate all traces first
        p.convertSampleStacksToTraces(samples);
        std::cout << "\nAll generated traces:" << std::endl;
        printTraces(p.getTraces());
        
        // Test frequency filter first
        p.filterTraces(0);  // ContiguousFrequencyFilter
        std::cout << "\nAfter Frequency Filter (>= 3 consecutive calls):" << std::endl;
        printTraces(p.getTraces());
        
        // Reset and test duration filter  
        p.convertSampleStacksToTraces(samples);  // Regenerate original traces
        p.filterTraces(1);  // EliminateNoisyFilter
        std::cout << "\nAfter Duration Filter (> 60 seconds):" << std::endl;
        printTraces(p.getTraces());
    }

    std::cout << "\n" << std::string(60, '=') << std::endl;
    std::cout << "ALL PROFILER TESTS COMPLETED" << std::endl;
    std::cout << std::string(60, '=') << std::endl;
    
    return 0;
}
