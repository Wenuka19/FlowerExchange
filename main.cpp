#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <queue>
#include <mutex>
#include <thread>
#include <condition_variable>
#include "Order.h"
#include "OrderBook.h"
#include "MatchingEngine.h"


// TODO: 
// 0. Order ID config
// 1. Add reason for rejection
// 2. Add timestamp
// 3. Factory pattern 

void readCSVFile(std::queue<std::string>& csvQueue, std::mutex& queueMutex,
                 std::condition_variable& cv, bool& stopFlag, const std::string& filename) {
    std::ifstream file(filename);
    std::string line;

    if (!getline(file, line)) {
        std::cerr << "Error reading the file or file is empty." << std::endl;
        return;  // Exit with an error code
    }

    while (std::getline(file, line) && !stopFlag) {
        // Lock the queue before pushing
        std::lock_guard<std::mutex> lock(queueMutex);
        csvQueue.push(line);
        cv.notify_one();  // Notify the processing thread
    }

    file.close();

    // Notify processing thread that reading is complete
    cv.notify_one();
}

void processRecords(MatchingEngine* matchingEngine,std::queue<std::string>& csvQueue, std::mutex& queueMutex,
                    std::condition_variable& cv, bool& stopFlag) {
    while (true) {
        // Lock the queue before checking and popping
        std::unique_lock<std::mutex> lock(queueMutex);
        cv.wait(lock, [&csvQueue, &stopFlag]{ return !csvQueue.empty() || stopFlag; });

        if (!csvQueue.empty()) {
            std::string csvRecord = csvQueue.front();
            csvQueue.pop();
            lock.unlock();  // Unlock before processing

            // Process the CSV record (you can modify this part based on your logic)
            matchingEngine->processOrder(csvRecord);
        } else if (stopFlag) {
            break;  // Exit loop when stopFlag is set
        }
    }
}

int main() {
    MatchingEngine Matching_Engine;
    std::string input_file_name = "order.csv";
    std::string output_file_name = "exec_report.csv";
    std::queue<std::string> csvQueue;
    std::mutex queueMutex;
    std::condition_variable cv;

    bool stopFlag = false;

    std::thread readerThread(readCSVFile, std::ref(csvQueue), std::ref(queueMutex),
                             std::ref(cv), std::ref(stopFlag), input_file_name);
    std::thread processorThread(processRecords,&Matching_Engine, std::ref(csvQueue), std::ref(queueMutex),
                                std::ref(cv), std::ref(stopFlag));

    // Wait for the reading thread to finish
    readerThread.join();

    // Signal processing thread to stop
    stopFlag = true;
    cv.notify_one();  // Notify the processing thread to check the stopFlag

    // Wait for the processing thread to finish
    processorThread.join();
    Matching_Engine.PrintReport();
    Matching_Engine.WriteToFile(output_file_name);

    return 0;
}