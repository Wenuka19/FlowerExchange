//
// Created by wenuka on 10/30/23.
//
#include "OrderBook.h"
#include "Order.h"

#ifndef IMPLEMENTATION_MATCHINGENGINE_H
#define IMPLEMENTATION_MATCHINGENGINE_H


class MatchingEngine {
private:
    OrderBook RoseOrders;
    OrderBook LavenderOrders;
    OrderBook LotusOrders;
    OrderBook TulipOrders;
    OrderBook OrchidOrders;

    Order validateOrder(const std::string& line);

public:
    void MatchOrder(const Order order);

};

#endif //IMPLEMENTATION_MATCHINGENGINE_H
