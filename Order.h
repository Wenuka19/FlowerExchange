//
// Created by wenuka on 10/30/23.
//

#pragma  once
#ifndef IMPLEMENTATION_ORDER_H
#define IMPLEMENTATION_ORDER_H
#include <string>

struct Order {
    enum State {
        NEW = 0,
        REJECTED = 1,
        FILL = 2,
        PFILL = 3
    };
    enum Side{
        SELL=2,
        BUY=1
    };

    std::string Client_ID;
    float price;
    int quantity;
    State state;
    std::string reason,instrument;
    int Side;

    Order(std::string i,float p, int q,int side,std::string inst,State st=NEW) :Client_ID(i), price(p), quantity(q), Side(side) ,state(st),reason(""),instrument(inst){}

    struct BuyCompare {
        bool operator() (const Order& lhs, const Order& rhs) const{
            return lhs.price > rhs.price;
        };
    };

    struct SellCompare {
        bool operator() (const Order& lhs, const Order& rhs) const{
            return lhs.price < rhs.price;
        };
    };
};

#endif //IMPLEMENTATION_ORDER_H
