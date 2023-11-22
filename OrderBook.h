//
// Created by wenuka on 10/30/23.
//

#ifndef IMPLEMENTATION_ORDERBOOK_H
#define IMPLEMENTATION_ORDERBOOK_H
#include <set>
#include "Order.h"

class OrderBook {
private:
    std::set<Order, Order::BuyCompare> buyOrders;
    std::set<Order, Order::SellCompare> sellOrders;

public:
    void addOrder(const Order& order);
    void displayOrders() const;
};

#endif // ORDERBOOK_H
