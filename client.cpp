#include <iostream>
#include <fstream>
#include <cstring>
#include <unistd.h>
#include <arpa/inet.h>
#include <sstream>

#define PORT 12346
#define BUFFER_SIZE 1024

int main() {
    int clientSocket;
    struct sockaddr_in serverAddr;

    // Create socket
    if ((clientSocket = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
        perror("Socket creation failed");
        return -1;
    }

    // Set up server address structure
    serverAddr.sin_family = AF_INET;
    serverAddr.sin_port = htons(PORT);
    serverAddr.sin_addr.s_addr = inet_addr("127.0.0.1");  // Assuming server is on the same machine

    // Connect to the server
    if (connect(clientSocket, (struct sockaddr*)&serverAddr, sizeof(serverAddr)) < 0) {
        perror("Connection failed");
        return -1;
    }

    std::cout << "Connected to server.\n";

    // Read CSV file line by line and send each line to the server
    std::ifstream inputFile("order.csv");
    std::string line;
    std::getline(inputFile, line);
    while (std::getline(inputFile, line)) {
        // Send each line to the server
        send(clientSocket, line.c_str(), line.size(), 0);
        usleep(100);  // Add a short delay to separate lines (adjust as needed)
    }


    // Notify the server that the data transmission is complete
    send(clientSocket, "END_OF_FILE", strlen("END_OF_FILE"), 0);

    char bufferRecv[BUFFER_SIZE];
    std::string executionReport;

    while (true) {
        int bytesRead = recv(clientSocket, bufferRecv, BUFFER_SIZE, 0);
        if (bytesRead <= 0)
            break;

        executionReport.append(bufferRecv, bytesRead);
    }
    std::string file_name = "exec_report"+std::to_string(clientSocket)+".csv";

    // Write execution report to a new CSV file
    std::ofstream outputFile(file_name);
    outputFile << executionReport;
    outputFile.close();

    // Close socket
    close(clientSocket);

    std::cout << "Execution report received and written to exec_report.csv. Connection closed.\n";

    return 0;
}
