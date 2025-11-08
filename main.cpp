#include "OrderBook.h"
#include <iostream>
#include <sstream>
#include <string>
#include <algorithm>
#include <cctype>

void print_help() {
    std::cout << "\n=== ORDER BOOK SIMULATOR ===" << std::endl;
    std::cout << "Commands:" << std::endl;
    std::cout << "  BUY <quantity> <price>  - Add a buy order" << std::endl;
    std::cout << "  SELL <quantity> <price> - Add a sell order" << std::endl;
    std::cout << "  CANCEL <order_id>       - Cancel an order" << std::endl;
    std::cout << "  BOOK                    - Display the order book" << std::endl;
    std::cout << "  TRADES                  - Display trade history" << std::endl;
    std::cout << "  HELP                    - Show this help message" << std::endl;
    std::cout << "  EXIT                    - Exit the program" << std::endl;
    std::cout << std::endl;
}

std::string to_upper(const std::string& str) {
    std::string result = str;
    std::transform(result.begin(), result.end(), result.begin(), ::toupper);
    return result;
}

int main() {
    OrderBook book;
    std::string line;
    
    std::cout << "Order Book Simulator - Type 'HELP' for commands" << std::endl;
    
    while (true) {
        std::cout << "> ";
        std::getline(std::cin, line);
        
        if (line.empty()) {
            continue;
        }
        
        std::stringstream ss(line);
        std::string command;
        ss >> command;
        
        command = to_upper(command);
        
        try {
            if (command == "BUY") {
                int quantity;
                double price;
                if (ss >> quantity >> price) {
                    book.add_order(OrderType::BUY, price, quantity);
                } else {
                    std::cout << "Error: Invalid BUY command. Usage: BUY <quantity> <price>" << std::endl;
                }
            }
            else if (command == "SELL") {
                int quantity;
                double price;
                if (ss >> quantity >> price) {
                    book.add_order(OrderType::SELL, price, quantity);
                } else {
                    std::cout << "Error: Invalid SELL command. Usage: SELL <quantity> <price>" << std::endl;
                }
            }
            else if (command == "CANCEL") {
                int order_id;
                if (ss >> order_id) {
                    book.cancel_order(order_id);
                } else {
                    std::cout << "Error: Invalid CANCEL command. Usage: CANCEL <order_id>" << std::endl;
                }
            }
            else if (command == "BOOK") {
                book.print_book();
            }
            else if (command == "TRADES") {
                book.print_trades();
            }
            else if (command == "HELP") {
                print_help();
            }
            else if (command == "EXIT") {
                std::cout << "Exiting..." << std::endl;
                break;
            }
            else {
                std::cout << "Unknown command: " << command << ". Type 'HELP' for available commands." << std::endl;
            }
        }
        catch (const std::exception& e) {
            std::cout << "Error: " << e.what() << std::endl;
        }
    }
    
    return 0;
}


