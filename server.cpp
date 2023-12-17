#include <iostream>
#include <fstream>
#include <sstream>
#include <vector>
#include <cstring>
#include <unistd.h>
#include <arpa/inet.h>
#include <thread>
#include <queue>
#include <condition_variable>

#define PORT 12346
#define BUFFER_SIZE 1024
#include "MatchingEngine.h"

void handleClient(int clientSocket,std::queue<std::string>& csvQueue, std::mutex& queueMutex,std::condition_variable& cv, bool& stopFlag) {
    char buffer[BUFFER_SIZE];
    while (true) {
        int bytesRead = recv(clientSocket, buffer, BUFFER_SIZE, 0);
        if (bytesRead <= 0)
            break;

        // Check for the end of file marker
        if (strncmp(buffer, "END_OF_FILE", strlen("END_OF_FILE")) == 0) {
            std::cout << "End of file marker received. Receiving complete.\n";
            break;
        }


        // Lock the queue before pushing
        std::lock_guard<std::mutex> lock(queueMutex);
        csvQueue.push(std::string(buffer, bytesRead));
        cv.notify_one();  // Notify the processing thread
    }
}


void processRecords(int clientSocket,MatchingEngine* matchingEngine,std::queue<std::string>& csvQueue, std::mutex& queueMutex,std::condition_variable& cv, bool& stopFlag) {
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

    std::string line =  "Order ID,Cl. Ord. ID,Instrument,Side,ExecStatus,Quantity,Price\n";


    std::vector<Order> exec_report = matchingEngine->getExecReport();
    send(clientSocket, line.c_str(), line.size(), 0);

    for (const auto& order : exec_report) {
        std::string record_line;
        record_line = order.order_ID + "," + order.Client_ID + "," + order.instrument + "," + std::to_string(order.Side) + "," + std::to_string(order.state) + "," + std::to_string(order.quantity)+","+std::to_string(order.price) + "\n";
        send(clientSocket,record_line.c_str(),record_line.size(),0);
        usleep(1000);
    }

    // Close the client socket
    close(clientSocket);

    std::cout << "Client connection closed.\n";
    return;
}

int main() {
    MatchingEngine Matching_Engine;
    std::queue<std::string> csvQueue;
    std::condition_variable cv;
    std::mutex queueMutex;
    int serverSocket, clientSocket;
    struct sockaddr_in serverAddr, clientAddr;
    bool stopFlag = false;

    socklen_t addrSize = sizeof(struct sockaddr_in);

    // Create socket
    if ((serverSocket = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
        perror("Socket creation failed");
        return -1;
    }

    // Set up server address structure
    serverAddr.sin_family = AF_INET;
    serverAddr.sin_addr.s_addr = INADDR_ANY;
    serverAddr.sin_port = htons(PORT);

    // Bind the socket
    if (bind(serverSocket, (struct sockaddr*)&serverAddr, sizeof(serverAddr)) < 0) {
        perror("Binding failed");
        return -1;
    }

    // Listen for connections
    if (listen(serverSocket, 5) < 0) {
        perror("Listening failed");
        return -1;
    }

    std::cout << "Server listening on port " << PORT << "...\n";


    if ((clientSocket = accept(serverSocket, (struct sockaddr*)&clientAddr, &addrSize)) < 0) {
        perror("Acceptance failed");
        return -1;
    }

    std::cout << "Connection accepted from " << inet_ntoa(clientAddr.sin_addr) << ":" << ntohs(clientAddr.sin_port) << "\n";

    std::thread readerThread(handleClient, clientSocket,std::ref(csvQueue), std::ref(queueMutex),
                             std::ref(cv), std::ref(stopFlag));

    std::thread processorThread(processRecords,clientSocket,&Matching_Engine,std::ref(csvQueue), std::ref(queueMutex),
                                std::ref(cv), std::ref(stopFlag));

    // Wait for the reading thread to finish
    readerThread.join();

    // Signal processing thread to stop
    stopFlag = true;
    cv.notify_one();  // Notify the processing thread to check the stopFlag

    // Wait for the processing thread to finish
    processorThread.join();
    close(serverSocket);

    return 0;

}