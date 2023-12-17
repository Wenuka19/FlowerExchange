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
#include <chrono>

std::string MatchingEngine::transaction_time() {
    auto now = std::chrono::system_clock::now();
    time_t currentTime = std::chrono::system_clock::to_time_t(now);
    tm *timeInfo = localtime(&currentTime);
    std::ostringstream oss;
    oss << std::put_time(timeInfo, "%Y%m%d-%H%M%S");

    auto milliseconds = std::chrono::duration_cast<std::chrono::milliseconds>(now.time_since_epoch() % std::chrono::seconds(1));
    oss << '.' << std::setfill('0') << std::setw(3) << milliseconds.count();
    return oss.str();
}

void MatchingEngine::add_to_report(Order &order) {
    order.transaction_time = transaction_time();
    execReport.push_back(order);
}

void MatchingEngine::MatchOrder(Order order){
    if (order.state == Order::REJECTED){
        add_to_report(order);
    }else{
        OrderBook* orderBook;
        // factory pattern?
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
        rejectionReason = "Client Order ID is missing";
    } else if (id.length() > 7) {
        isValid = false;
        rejectionReason = "Client Order ID exceeds the maximum length of 7 characters";
    }

    // Validate Instrument
    std::unordered_set<std::string> validInstruments = {"Rose", "Lavender", "Lotus", "Tulip", "Orchid"};
    if ( validInstruments.find(instrument) == validInstruments.end()) {
        isValid = false;
        rejectionReason = "Invalid Instrument";
    }

    // Validate Side
    if (sideStr != "1" && sideStr != "2") {
        isValid = false;
        side = std::stoi(sideStr);
        rejectionReason = "Invalid side";
    } else {
        side = std::stoi(sideStr);
    }

    // Validate Price
    price = std::stod(priceStr);
    if (price <= 0.0) {
        isValid = false;
        rejectionReason = "Invalid price";
    }

    // Validate Quantity
    quantity = std::stoi(quantityStr);
    if (quantity < 10 || quantity > 999 || quantity % 10 != 0) {
        isValid = false;
        rejectionReason = "Invalid size";
    }
    // If any validation failed, generate a Rejected execution report
    // if (!isValid) {
    //     Order rejectedOrder(id, orderId, price, quantity, side, instrument, Order::REJECTED);
    //     // rejectedOrder.rejectionReason = rejectionReason;
    //     add_to_report(rejectedOrder);
    //     return;
    // }

    // // Create a valid order and pass it to MatchOrder
    orderId = "ord"+ std::to_string(++orders_processed);
    Order new_ord = Order(id, orderId, price, quantity, side, instrument);
    if (!isValid){
        new_ord.state = Order::REJECTED;
        new_ord.reason = rejectionReason;
    }
    MatchOrder(new_ord);
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
                add_to_report(newBuyOrder);
                sellOrder->state = Order::FILL;
                add_to_report(*sellOrder);
                break;
            }
            else if (sellOrder->price <= newBuyOrder.price && sellOrder->quantity < newBuyOrder.quantity){
                orderBook.removeSellOrderMatch();
                int remaining_q = newBuyOrder.quantity - sellOrder->quantity;
                double ori_price = newBuyOrder.price;
                newBuyOrder.state = Order::PFILL;
                newBuyOrder.price = sellOrder->price;
                newBuyOrder.quantity = sellOrder->quantity;
                add_to_report(newBuyOrder);
                Order remaining_ord = Order(newBuyOrder.Client_ID,newBuyOrder.order_ID,ori_price,remaining_q,Order::BUY,newBuyOrder.instrument,Order::PFILL);
                remaining_ord.reason = newBuyOrder.reason;
                sellOrder->state = Order::FILL;
                add_to_report(*sellOrder);
                newBuyOrder = remaining_ord;
                match = true;
            }
            else if (sellOrder->price <= newBuyOrder.price && sellOrder->quantity > newBuyOrder.quantity){
                orderBook.removeSellOrderMatch();
                newBuyOrder.state = Order::FILL;
                newBuyOrder.price = sellOrder->price;
                add_to_report(newBuyOrder);
                int remaining_q = sellOrder->quantity - newBuyOrder.quantity;
                Order new_ord = Order(sellOrder->Client_ID,sellOrder->order_ID,sellOrder->price,remaining_q,Order::SELL,sellOrder->instrument,Order::NEW);
                new_ord.reason = sellOrder->reason;
                orderBook.addOrder(new_ord);
                sellOrder->state = Order::PFILL;
                add_to_report(*sellOrder);
                break;
            }
            else{
                orderBook.addOrder(newBuyOrder);
                if (newBuyOrder.state==Order::NEW){
                    add_to_report(newBuyOrder);
                }
                break;
            }
        }else{
            orderBook.addOrder(newBuyOrder);
            if (newBuyOrder.state==Order::NEW){
                add_to_report(newBuyOrder);
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
                add_to_report(newSellOrder);
                buyOrder->state = Order::FILL;
                add_to_report(*buyOrder);
                match = false;
                break;
            } else if (buyOrder->price >= newSellOrder.price && buyOrder->quantity < newSellOrder.quantity) {
                orderBook.removeBuyOrderMatch();
                int remaining_q = newSellOrder.quantity - buyOrder->quantity;
                double ori_price = newSellOrder.price;
                newSellOrder.state = Order::PFILL;
                newSellOrder.price = buyOrder->price;
                newSellOrder.quantity = buyOrder->quantity;
                add_to_report(newSellOrder);
                Order new_ord = Order(newSellOrder.Client_ID, newSellOrder.order_ID,ori_price, remaining_q, Order::SELL, newSellOrder.instrument, Order::PFILL);
                new_ord.reason = newSellOrder.reason;
                buyOrder->state = Order::FILL;
                add_to_report(*buyOrder);
                newSellOrder = new_ord;
            } else if (buyOrder->price >= newSellOrder.price && buyOrder->quantity > newSellOrder.quantity) {
                orderBook.removeBuyOrderMatch();
                newSellOrder.state = Order::FILL;
                newSellOrder.price = buyOrder->price;
                add_to_report(newSellOrder);
                int remaining_q = buyOrder->quantity - newSellOrder.quantity;
                Order new_ord = Order(buyOrder->Client_ID, newSellOrder.order_ID,buyOrder->price, remaining_q, Order::BUY, buyOrder->instrument, Order::NEW);
                new_ord.reason = buyOrder->reason;
                orderBook.addOrder(new_ord);
                buyOrder->state = Order::PFILL;
                add_to_report(*buyOrder);
                break;
            } else {
                orderBook.addOrder(newSellOrder);
                if (newSellOrder.state==Order::NEW){
                    add_to_report(newSellOrder);

                }
                add_to_report(newSellOrder);
                break;
            }
        } else {
            orderBook.addOrder(newSellOrder);
            if (newSellOrder.state==Order::NEW){
                add_to_report(newSellOrder);

            }
            break;
        }


    }
}

void MatchingEngine::PrintReport() {
    std::cout << std::setw(10) << "Order ID"<<std::setw(15) << "Cl. Ord. ID" << std::setw(15) << "Instrument" << std::setw(10) << "Side"
              << std::setw(12) << "ExecStatus" << std::setw(10) << "Quantity" << std::setw(10) << "Price" << std::setw(30) << "Rejected Reason (If any)"<<std::setw(20)<<"Transaction Time" << std::endl;

    for (const auto& order : execReport) {

        std::cout << std::setw(10) << order.order_ID<< std::setw(15) << order.Client_ID<< std::setw(15) << order.instrument
                  << std::setw(10) << order.Side << std::setw(12) << order.state << std::setw(10) << order.quantity
                  << std::setw(10) << std::fixed << std::setprecision(2)<<order.price << std::setw(30) << order.reason <<std::setw(20)<<order.transaction_time<< std::endl;
    }

}

void MatchingEngine::WriteToFile(const std::string& filename) {
    std::ofstream outputFile(filename);

    if (!outputFile.is_open()) {
        std::cerr << "Error opening file: " << filename << std::endl;
        return;
    }

    outputFile << "Order ID,Cl. Ord. ID,Instrument,Side,ExecStatus,Quantity,Price,Rejected Reason (If any),Transaction Time"<<std::endl;

    for (const auto& order : execReport) {
        outputFile << order.order_ID << "," << order.Client_ID << "," << order.instrument
                   << "," << order.Side << "," << order.state << "," << order.quantity
                   << "," << std::fixed << std::setprecision(2) << order.price << "," << order.reason << "," << order.transaction_time<<std::endl;
    }

    outputFile.close();
}

const std::vector<Order> &MatchingEngine::getExecReport() const {
    return execReport;
}
