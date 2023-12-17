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
    int orders_processed=0;
    OrderBook RoseOrders;
    OrderBook LavenderOrders;
    OrderBook LotusOrders;
    OrderBook TulipOrders;
    OrderBook OrchidOrders;

    std::vector<Order> execReport;
public:
    const std::vector<Order> &getExecReport() const;

private:
    void MatchOrder(Order order);

public:
    void processOrder(const std::string& line);
    void matchBuyOrder(OrderBook& sellOrderBook, Order& newBuyOrder, std::vector<Order>& execReport);
    void matchSellOrder(OrderBook& buyOrderBook, Order& newSellOrder, std::vector<Order>& execReport);
    void PrintReport();
    void WriteToFile(const std::string& filename);
    std::string transaction_time();
    void add_to_report(Order &order);

};

#endif //IMPLEMENTATION_MATCHINGENGINE_H
