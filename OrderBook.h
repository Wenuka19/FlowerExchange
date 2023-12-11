//
// Created by wenuka on 10/30/23.
//

#ifndef IMPLEMENTATION_ORDERBOOK_H
#define IMPLEMENTATION_ORDERBOOK_H
#include <set>
#include <optional>
#include "Order.h"
#include "vector"

class OrderBook {
private:
    std::set<Order, Order::BuyCompare> buyOrders;
    std::set<Order, Order::SellCompare> sellOrders;

public:
    void addOrder(const Order& order);
    void displayOrders() const;
    std::optional<Order> getSellOrder();
    void removeSellOrderMatch();
    std::optional<Order> getBuyOrder();
    void removeBuyOrderMatch();
};

#endif // ORDERBOOK_H
