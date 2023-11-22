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
        SELL=1,
        BUY=2
    };

    enum Instrument{
        DEFAULT=' ',
        ROSE='Rose',
        LAVENDER='Lavender',
        LOTUS = 'Lotus',
        TULIP = 'Tulip',
        ORCHID = 'Orchid'
    };

    std::string ID;
    double price;
    int quantity;
    State state;
    std::string reason;
    Instrument instrument;
    int Side;

    Order(std::string i,double p, int q,int side,State st=NEW,Instrument inst=DEFAULT) :ID(i), price(p), quantity(q), Side(side) ,state(st),reason(""),instrument(inst){}

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
