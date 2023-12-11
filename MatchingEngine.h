//
// Created by wenuka on 10/30/23.
//
#include "OrderBook.h"
#include "Order.h"
#include <vector>

#ifndef IMPLEMENTATION_MATCHINGENGINE_H
#define IMPLEMENTATION_MATCHINGENGINE_H


class MatchingEngine {
private:
    OrderBook RoseOrders;
    OrderBook LavenderOrders;
    OrderBook LotusOrders;
    OrderBook TulipOrders;
    OrderBook OrchidOrders;

    std::vector<Order> execReport;
    void MatchOrder(Order order);

public:
    void processOrder(const std::string& line);
    void matchBuyOrder(OrderBook& sellOrderBook, Order& newBuyOrder, std::vector<Order>& execReport);
    void matchSellOrder(OrderBook& buyOrderBook, Order& newSellOrder, std::vector<Order>& execReport);
    void PrintReport();

};

#endif //IMPLEMENTATION_MATCHINGENGINE_H
