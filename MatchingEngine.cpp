//
// Created by wenuka on 10/30/23.
//

#include <string> // Add the missing header file
#include <iostream>
#include <iomanip>
#include <fstream>
#include <sstream>
#include <unordered_set>
#include <unordered_map>
#include "MatchingEngine.h"
#include "Order.h"


void MatchingEngine::MatchOrder(Order order){
    if (order.state == Order::REJECTED){
        execReport.push_back(order);
    }else{
        OrderBook* orderBook;
           if (order.instrument == "Rose"){
               orderBook = &RoseOrders;
           }

           else if (order.instrument == "Lavender"){
               orderBook = &LavenderOrders;
           }

            else if (order.instrument == "Orchid"){
               orderBook = &OrchidOrders;
            }

            else if(order.instrument ==  "Lotus"){
                orderBook  = &LotusOrders;
            }

            else if (order.instrument == "Tulip"){
               orderBook = &TulipOrders;
           }
            if (order.Side == Order::BUY){
                matchBuyOrder(*orderBook,order,execReport);
            }else{
                matchSellOrder(*orderBook,order,execReport);
            }
    }
}


void MatchingEngine::processOrder(const std::string& line){

    std::istringstream iss(line);

    std::string id, instrument, sideStr, quantityStr, priceStr, orderId;
    int side, quantity;
    double price;

    // Separate the input line into parameters
    getline(iss, id, ','); // Fix the arguments of getline function
    getline(iss, instrument, ',');
    getline(iss, sideStr, ',');
    getline(iss, quantityStr, ',');
    getline(iss, priceStr, ',');
    getline(iss, orderId, ',');

    // Validate the parameters
    bool isValid = true;
    std::string rejectionReason;

    // Validate Client Order ID
    if (id.empty()) {
        isValid = false;
        rejectionReason += "Client Order ID is missing. ";
    } else if (id.length() > 7) {
        isValid = false;
        rejectionReason += "Client Order ID exceeds the maximum length of 7 characters. ";
    }

    // Validate Instrument
    std::unordered_set<std::string> validInstruments = {"Rose", "Lavender", "Lotus", "Tulip", "Orchid"};
    if (validInstruments.find(instrument) == validInstruments.end()) {
        isValid = false;
        rejectionReason += "Invalid Instrument. ";
    }

    // Validate Side
    if (sideStr != "1" && sideStr != "2") {
        isValid = false;
        rejectionReason += "Invalid Side. ";
    } else {
        side = std::stoi(sideStr);
    }

    // Validate Price
    price = std::stod(priceStr);
    if (price <= 0.0) {
        isValid = false;
        rejectionReason += "Price must be greater than 0. ";
    }

    // Validate Quantity
    quantity = std::stoi(quantityStr);
    if (quantity < 10 || quantity > 1000 || quantity % 10 != 0) {
        isValid = false;
        rejectionReason += "Quantity must be a multiple of 10 and between 10 and 1000. ";
    }

    // If any validation failed, generate a Rejected execution report
    if (!isValid) {
        Order rejectedOrder(id, orderId, price, quantity, side, instrument, Order::REJECTED);
        // rejectedOrder.rejectionReason = rejectionReason;
        execReport.push_back(rejectedOrder);
        return;
    }

    // // Create a valid order and pass it to MatchOrder
    Order validOrder(id, orderId, price, quantity, side, instrument);
    MatchOrder(validOrder);
}

void MatchingEngine::matchBuyOrder(OrderBook& orderBook, Order& newBuyOrder, std::vector<Order>& execReport){
    bool match = true;
    while (match){
        std::optional<Order> sellOrder = orderBook.getSellOrder();
        if (sellOrder.has_value()){
            if (sellOrder->price <= newBuyOrder.price && sellOrder->quantity == newBuyOrder.quantity){
                orderBook.removeSellOrderMatch();
                newBuyOrder.state = Order::FILL;
                newBuyOrder.price = sellOrder->price;
                execReport.push_back(newBuyOrder);
                sellOrder->state = Order::FILL;
                execReport.push_back(*sellOrder);
                break;
            }
            else if (sellOrder->price <= newBuyOrder.price && sellOrder->quantity < newBuyOrder.quantity){
                orderBook.removeSellOrderMatch();
                int remaining_q = newBuyOrder.quantity - sellOrder->quantity;
                double ori_price = newBuyOrder.price;
                newBuyOrder.state = Order::PFILL;
                newBuyOrder.price = sellOrder->price;
                newBuyOrder.quantity = sellOrder->quantity;
                execReport.push_back(newBuyOrder);
                Order remaining_ord = Order(newBuyOrder.Client_ID,newBuyOrder.order_ID,ori_price,remaining_q,Order::BUY,newBuyOrder.instrument,Order::PFILL);
                sellOrder->state = Order::FILL;
                execReport.push_back(*sellOrder);
                newBuyOrder = remaining_ord;
                match = true;
            }
            else if (sellOrder->price <= newBuyOrder.price && sellOrder->quantity > newBuyOrder.quantity){
                orderBook.removeSellOrderMatch();
                newBuyOrder.state = Order::FILL;
                newBuyOrder.price = sellOrder->price;
                execReport.push_back(newBuyOrder);
                int remaining_q = sellOrder->quantity - newBuyOrder.quantity;
                Order new_ord = Order(sellOrder->Client_ID,sellOrder->order_ID,sellOrder->price,remaining_q,Order::SELL,sellOrder->instrument,Order::NEW);
                orderBook.addOrder(new_ord);
                sellOrder->state = Order::PFILL;
                execReport.push_back(*sellOrder);
                break;
            }
            else{
                orderBook.addOrder(newBuyOrder);
                if (newBuyOrder.state==Order::NEW){
                    execReport.push_back(newBuyOrder);
                }
                break;
            }
        }else{
            orderBook.addOrder(newBuyOrder);
            if (newBuyOrder.state==Order::NEW){
                execReport.push_back(newBuyOrder);

            }
            break;
        }

    }
}

void MatchingEngine::matchSellOrder(OrderBook& orderBook, Order& newSellOrder, std::vector<Order>& execReport) {
    bool match = true;
    while (match) {
        std::optional<Order> buyOrder = orderBook.getBuyOrder();
        if (buyOrder.has_value()) {
            if (buyOrder->price >= newSellOrder.price && buyOrder->quantity == newSellOrder.quantity) {
                orderBook.removeBuyOrderMatch();
                newSellOrder.state = Order::FILL;
                newSellOrder.price = buyOrder->price;
                execReport.push_back(newSellOrder);
                buyOrder->state = Order::FILL;
                execReport.push_back(*buyOrder);
                match = false;
                break;
            } else if (buyOrder->price >= newSellOrder.price && buyOrder->quantity < newSellOrder.quantity) {
                orderBook.removeBuyOrderMatch();
                int remaining_q = newSellOrder.quantity - buyOrder->quantity;
                double ori_price = newSellOrder.price;
                newSellOrder.state = Order::PFILL;
                newSellOrder.price = buyOrder->price;
                newSellOrder.quantity = buyOrder->quantity;
                execReport.push_back(newSellOrder);
                Order new_ord = Order(newSellOrder.Client_ID, newSellOrder.order_ID,ori_price, remaining_q, Order::SELL, newSellOrder.instrument,Order::PFILL);
                buyOrder->state = Order::FILL;
                execReport.push_back(*buyOrder);
                newSellOrder = new_ord;
            } else if (buyOrder->price >= newSellOrder.price && buyOrder->quantity > newSellOrder.quantity) {
                orderBook.removeBuyOrderMatch();
                newSellOrder.state = Order::FILL;
                newSellOrder.price = buyOrder->price;
                execReport.push_back(newSellOrder);
                int remaining_q = buyOrder->quantity - newSellOrder.quantity;
                Order new_ord = Order(buyOrder->Client_ID, newSellOrder.order_ID,buyOrder->price, remaining_q, Order::BUY, buyOrder->instrument,Order::NEW);
                orderBook.addOrder(new_ord);
                buyOrder->state = Order::PFILL;
                execReport.push_back(*buyOrder);
                break;
            } else {
                orderBook.addOrder(newSellOrder);
                if (newSellOrder.state==Order::NEW){
                    execReport.push_back(newSellOrder);

                }
                execReport.push_back(newSellOrder);
                break;
            }
        } else {
            orderBook.addOrder(newSellOrder);
            if (newSellOrder.state==Order::NEW){
                execReport.push_back(newSellOrder);

            }
            break;
        }


    }
}

void MatchingEngine::PrintReport() {
    std::cout << std::setw(10) << "Order ID"<<std::setw(15) << "Cl. Ord. ID" << std::setw(15) << "Instrument" << std::setw(10) << "Side"
              << std::setw(12) << "ExecStatus" << std::setw(10) << "Quantity" << std::setw(10) << "Price" << std::endl;

    for (const auto& order : execReport) {
        std::string execStatus;
        if (order.state==Order::NEW){
            execStatus="New";
        }else if (order.state==Order::FILL){
            execStatus="Fill";
        }else if (order.state==Order::PFILL){
            execStatus="PFill";
        }else if (order.state==Order::REJECTED){
            execStatus="Rejected";
        }
        std::cout << std::setw(10) << order.order_ID<< std::setw(15) << order.Client_ID<< std::setw(15) << order.instrument
                  << std::setw(10) << order.Side << std::setw(12) << execStatus << std::setw(10) << order.quantity
                  << std::setw(10) << std::fixed << std::setprecision(2)<<order.price << std::endl;
    }

}

void MatchingEngine::WriteToFile(const std::string& filename) {
    std::ofstream outputFile(filename);

    if (!outputFile.is_open()) {
        std::cerr << "Error opening file: " << filename << std::endl;
        return;
    }

    outputFile << "Order ID,Cl. Ord. ID,Instrument,Side,ExecStatus,Quantity,Price" << std::endl;

    for (const auto& order : execReport) {
        std::string execStatus;
        if (order.state == Order::NEW) {
            execStatus = "New";
        } else if (order.state == Order::FILL) {
            execStatus = "Fill";
        } else if (order.state == Order::PFILL) {
            execStatus = "PFill";
        } else if (order.state == Order::REJECTED) {
            execStatus = "Rejected";
        }

        outputFile << order.order_ID << "," << order.Client_ID << "," << order.instrument
                   << "," << order.Side << "," << execStatus << "," << order.quantity
                   << "," << std::fixed << std::setprecision(2) << order.price << std::endl;
    }

    outputFile.close();
}