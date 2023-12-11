//
// Created by wenuka on 10/30/23.
//

#include "OrderBook.h"
#include <iostream>
#include <optional>
#include <iomanip>

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
        std::cout << "ID: " << order.Client_ID << ", Price: " <<std::fixed << std::setprecision(2)<< order.price << ", Quantity: " << order.quantity << std::endl;
    }

    std::cout << "\nSell Orders:" << std::endl;
    for (const auto& order : sellOrders) {
        std::cout << "ID: " << order.Client_ID << ", Price: " <<std::fixed << std::setprecision(2)<< order.price << ", Quantity: " << order.quantity << std::endl;
    }
}

std::optional<Order> OrderBook::getSellOrder(){
    if (!sellOrders.empty()) {
        return *sellOrders.begin();
    }else{
        return std::nullopt;
    }
}

void OrderBook::removeSellOrderMatch() {
    if (!sellOrders.empty()){
        sellOrders.erase(sellOrders.begin());
    }
}

std::optional<Order> OrderBook::getBuyOrder(){
    if (!buyOrders.empty()) {
        return *buyOrders.begin();
    }else{
        return std::nullopt;
    }
}

void OrderBook::removeBuyOrderMatch() {
    if (!buyOrders.empty()){
        buyOrders.erase(buyOrders.begin());
    }
}