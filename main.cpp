#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include "Order.h"
#include "OrderBook.h"

int main() {
    OrderBook RoseOrders;

    std::ifstream file("order.csv");
    std::string line;

    if (!getline(file, line)) {
        std::cerr << "Error reading the file or file is empty." << std::endl;
        return 1;  // Exit with an error code
    }

    while (getline(file, line)) {
        std::cout << "Processing line: " << line << std::endl;
        std::stringstream ss(line);
        std::string id, price, quantity, date,instrument;
        int side;
        getline(ss, id, ',');
        getline(ss,instrument,',');
        ss>>side;
        ss.ignore();
        getline(ss, quantity, ',');
        getline(ss, price, ',');

        Order order(id, stod(price), stoi(quantity),side);
        RoseOrders.addOrder(order);
    }

    // Display the orders
    RoseOrders.displayOrders();

    return 0;
}