#include <iostream>

int main() {
    int x = 42;
    int& refToX = x;

    std::cout << "Original value of x: " << x << std::endl;
    std::cout << "Value through reference: " << refToX << std::endl;

    refToX = 99;  // This modifies the original variable x

    std::cout << "Modified value of x: " << x <<"\n"<< std::endl;


    x = 43;
    int* ptrToX = &x;

    std::cout << "Original value of x: " << x << std::endl;
    std::cout << "Value through pointer: " << *ptrToX << std::endl;

    *ptrToX = 99;  // This modifies the original variable x

    std::cout << "Modified value of x: " << x << std::endl;

    return 0;
}






void MatchingEngine::processOrder(const std::string& line){
    std::stringstream ss(line);
    std::string id, instrument, sideStr, quantityStr, priceStr, orderId;
    int side, quantity;
    double price;

    // Separate the input line into parameters
    getline(ss, id, ',');
    getline(ss, instrument, ',');
    getline(ss, sideStr, ',');
    getline(ss, quantityStr, ',');
    getline(ss, priceStr, ',');
    getline(ss, orderId, ',');

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
        rejectedOrder.rejectionReason = rejectionReason;
        execReport.push_back(rejectedOrder);
        return;
    }

    // Create a valid order and pass it to MatchOrder
    Order validOrder(id, orderId, price, quantity, side, instrument);
    MatchOrder(validOrder);
}