//
// Created by wenuka on 10/30/23.
//

#include "OrderBook.h"
#include <iostream>

void OrderBook::addOrder(const Order& order) {
    if (order.Side == 1) {
        buyOrders.insert(order);
    } else if (order.Side == 2) {
        sellOrders.insert(order);
    }
}

void OrderBook::displayOrders() const {
    std::cout << "Buy Orders:" << std::endl;
    for (const auto& order : buyOrders) {
        std::cout << "ID: " << order.ID << ", Price: " << order.price << ", Quantity: " << order.quantity << std::endl;
    }

    std::cout << "\nSell Orders:" << std::endl;
    for (const auto& order : sellOrders) {
        std::cout << "ID: " << order.ID << ", Price: " << order.price << ", Quantity: " << order.quantity << std::endl;
    }
}