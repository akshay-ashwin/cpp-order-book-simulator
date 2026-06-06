#include "OrderBook.h"
#include <iostream>
#include <iomanip>
#include <algorithm>
#include <chrono>
#include <sstream>
#include <cstdint>

void OrderBook::match_orders() {
    while (!bids.empty() && !asks.empty() && bids.top()->price >= asks.top()->price) {
        OrderPtr best_bid = bids.top();
        OrderPtr best_ask = asks.top();
        
        // Heap Cleaning (Lazy Deletion Handling)
        if (!best_bid->is_active) {
            bids.pop();
            continue;
        }
        
        if (!best_ask->is_active) {
            asks.pop();
            continue;
        }
        
        // Trade Execution
        int trade_quantity = std::min(best_bid->remaining_quantity, best_ask->remaining_quantity);
        
        // Determine trade price: use the price of the order with older timestamp
        double trade_price = (best_bid->timestamp < best_ask->timestamp) 
                             ? best_bid->price 
                             : best_ask->price;
        
        int64_t trade_timestamp = std::chrono::duration_cast<std::chrono::nanoseconds>(
            std::chrono::system_clock::now().time_since_epoch()).count();
        
        Trade trade(best_bid->order_id, best_ask->order_id, trade_price, trade_quantity, trade_timestamp);
        trade_history.push_back(trade);
        trade.print();
        
        // Update Orders
        best_bid->remaining_quantity -= trade_quantity;
        best_ask->remaining_quantity -= trade_quantity;
        
        // Handle Filled Orders
        if (best_bid->remaining_quantity == 0) {
            best_bid->is_active = false;
            bids.pop();
            order_map.erase(best_bid->order_id);
        }
        
        if (best_ask->remaining_quantity == 0) {
            best_ask->is_active = false;
            asks.pop();
            order_map.erase(best_ask->order_id);
        }
    }
}

int OrderBook::add_order(OrderType type, double price, int quantity) {
    OrderPtr new_order = std::make_shared<Order>(type, price, quantity);
    
    order_map[new_order->order_id] = new_order;
    
    if (type == OrderType::BUY) {
        bids.push(new_order);
    } else {
        asks.push(new_order);
    }
    
    std::cout << "Order " << new_order->order_id 
              << ": " << (type == OrderType::BUY ? "BUY" : "SELL")
              << " " << quantity << " @ " 
              << std::fixed << std::setprecision(2) << price << std::endl;
    
    match_orders();
    
    return new_order->order_id;
}

void OrderBook::cancel_order(int order_id) {
    auto it = order_map.find(order_id);
    if (it != order_map.end() && it->second->is_active) {
        it->second->is_active = false;
        order_map.erase(it);
        std::cout << "Order " << order_id << " canceled." << std::endl;
    } else {
        std::cout << "Error: Order " << order_id
                  << " not found or already inactive." << std::endl;
    }
}

void OrderBook::print_book() const {
    // Create copies of the priority queues
    auto bids_copy = bids;
    auto asks_copy = asks;
    
    std::cout << "\n=== ORDER BOOK ===" << std::endl;
    std::cout << std::left << std::setw(15) << "BIDS" 
              << std::setw(15) << "ASKS" << std::endl;
    std::cout << std::setw(15) << "Qty | Price" 
              << std::setw(15) << "Price | Qty" << std::endl;
    std::cout << "--------------------------------" << std::endl;
    
    while (!bids_copy.empty() || !asks_copy.empty()) {
        std::string bid_str = "";
        std::string ask_str = "";
        
        // Process bids
        while (!bids_copy.empty() && !bids_copy.top()->is_active) {
            bids_copy.pop();
        }
        
        if (!bids_copy.empty()) {
            OrderPtr top_bid = bids_copy.top();
            bids_copy.pop();
            std::stringstream bid_ss;
            bid_ss << top_bid->remaining_quantity << " | " 
                   << std::fixed << std::setprecision(2) << top_bid->price;
            bid_str = bid_ss.str();
        }
        
        // Process asks
        while (!asks_copy.empty() && !asks_copy.top()->is_active) {
            asks_copy.pop();
        }
        
        if (!asks_copy.empty()) {
            OrderPtr top_ask = asks_copy.top();
            asks_copy.pop();
            std::stringstream ask_ss;
            ask_ss << std::fixed << std::setprecision(2) << top_ask->price 
                   << " | " << top_ask->remaining_quantity;
            ask_str = ask_ss.str();
        }
        
        std::cout << std::left << std::setw(15) << bid_str 
                  << std::setw(15) << ask_str << std::endl;
    }
    
    std::cout << std::endl;
}

void OrderBook::print_trades() const {
    std::cout << "\n=== TRADE HISTORY ===" << std::endl;
    if (trade_history.empty()) {
        std::cout << "No trades executed yet." << std::endl;
    } else {
        for (const auto& trade : trade_history) {
            trade.print();
        }
    }
    std::cout << std::endl;
}
