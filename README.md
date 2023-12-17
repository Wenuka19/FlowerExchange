# FlowerExchange

This repository contains the code for the FlowerExchange project.

The Flower Exchange is a system that facilitates basic trading of flowers. It consists of two main applications:

1. Trader Application: Traders can submit buy or sell orders for flowers using this application.

2. Exchange Application: This application processes incoming orders against existing orders in the order container, known as the Order Book. It performs full or partial executions of orders and sends an Execution Report to the trader, indicating the status of the order. Orders may be rejected due to quantity limitations, invalid flower types, and other factors.

The Flower Exchange utilizes a socket-based system for communication between the Trader Application and the Exchange Application.


## Prerequisites

Before building and running the project, make sure you have the following:

- Operating System: Linux
- C++17-compatible compiler (GCC, Clang, etc.)

## Getting Started

To get started with the project, follow these steps:

1. Clone the repository:
    ```
    git clone https://github.com/Wenuka19/FlowerExchange.git
    ```

2. Add orders to the `order.csv` file in the root directory of the project.

3. Navigate to the repository and run the following commands in the terminal:
    ```
    make clean
    make run_server
    make run_client
    ```

[//]: # (Add more instructions here if needed)
