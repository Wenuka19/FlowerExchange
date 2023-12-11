#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include "Order.h"
#include "OrderBook.h"
#include "MatchingEngine.h"

int main() {
    MatchingEngine Matching_Engine;
    OrderBook RoseOrders;

    std::ifstream file("order.csv");
    std::string line;

    if (!getline(file, line)) {
        std::cerr << "Error reading the file or file is empty." << std::endl;
        return 1;  // Exit with an error code
    }

    while (getline(file, line)) {
        Matching_Engine.processOrder(line);

    }
    std::cout<<"PRINTING REPORT"<<"\n";
    Matching_Engine.PrintReport();

    return 0;
}