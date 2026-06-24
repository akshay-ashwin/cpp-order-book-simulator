#include "OrderBook.h"
#include <iostream>
#include <iomanip>
#include <algorithm>
#include <chrono>
#include <sstream>
#include <cstdint>

void OrderBook::match_orders() {
    while (!bids.empty() && !asks.empty()) {

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

        bool can_match = false;

        if (best_bid->execution_type == OrderExecutionType::MARKET ||
            best_ask->execution_type == OrderExecutionType::MARKET) {
            can_match = true;
        }
        else if (best_bid->price >= best_ask->price) {
            can_match = true;
        }

        if (!can_match) {
            break;
        }

        // Trade Execution
        int trade_quantity =
            std::min(best_bid->remaining_quantity,
                     best_ask->remaining_quantity);

        double trade_price;

        if (best_bid->execution_type == OrderExecutionType::MARKET) {
            trade_price = best_ask->price;
        }
        else if (best_ask->execution_type == OrderExecutionType::MARKET) {
            trade_price = best_bid->price;
        }
        else {
            trade_price =
                (best_bid->timestamp < best_ask->timestamp)
                    ? best_bid->price
                    : best_ask->price;
        }

        int64_t trade_timestamp =
            std::chrono::duration_cast<std::chrono::nanoseconds>(
                std::chrono::system_clock::now().time_since_epoch())
                .count();

        Trade trade(
            best_bid->order_id,
            best_ask->order_id,
            trade_price,
            trade_quantity,
            trade_timestamp
        );

        trade_history.push_back(trade);
        trade.print();

        best_bid->remaining_quantity -= trade_quantity;
        best_ask->remaining_quantity -= trade_quantity;

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

int OrderBook::add_order(
    OrderType type, 
    double price, 
    int quantity,
    OrderExecutionType execution_type
 ) {
    if (execution_type == OrderExecutionType::FOK && !can_fully_fill(type, price, quantity)){
        std::cout << "FOK order rejected: insufficient liquidity." << std::endl;
        return -1;
    }
    OrderPtr new_order = std::make_shared<Order>(
        type,
        price,
        quantity,
        execution_type
    );
    
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
    
    if (execution_type == OrderExecutionType::IOC &&

    new_order->remaining_quantity > 0)

 {

    new_order->is_active = false;

    order_map.erase(new_order->order_id);

 }

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
bool OrderBook::can_fully_fill(
    OrderType type,
    double price,
    int quantity
) const
{
    int available = 0;

    if (type == OrderType::BUY) {
        auto asks_copy = asks;

        while (!asks_copy.empty()) {
            auto order = asks_copy.top();
            asks_copy.pop();

            if (!order->is_active)
                continue;

            if (order->price > price)
                break;

            available += order->remaining_quantity;

            if (available >= quantity)
                return true;
        }
    }
    else {
        auto bids_copy = bids;

        while (!bids_copy.empty()) {
            auto order = bids_copy.top();
            bids_copy.pop();

            if (!order->is_active)
                continue;

            if (order->price < price)
                break;

            available += order->remaining_quantity;

            if (available >= quantity)
                return true;
        }
    }

    return false;
}

size_t OrderBook::trade_count() const {
    return trade_history.size();
}

const std::vector<Trade>& OrderBook::get_trade_history() const {
    return trade_history;
}

size_t OrderBook::active_order_count() const {
    return order_map.size();
}

double OrderBook::get_best_bid() const
{
    auto bids_copy = bids;

    while (!bids_copy.empty())
    {
        auto order = bids_copy.top();

        if (order->is_active)
        {
            return order->price;
        }

        bids_copy.pop();
    }

    return 0.0;
}

double OrderBook::get_best_ask() const
{
    auto asks_copy = asks;

    while (!asks_copy.empty())
    {
        auto order = asks_copy.top();

        if (order->is_active)
        {
            return order->price;
        }

        asks_copy.pop();
    }

    return 0.0;
}

double OrderBook::get_spread() const
{
    double best_bid = get_best_bid();
    double best_ask = get_best_ask();

    return best_ask - best_bid;

}

double OrderBook::get_mid_price() const
{
    double best_bid = get_best_bid();
    double best_ask = get_best_ask();

    return (best_bid + best_ask) / 2.0;

}

std::vector<BookLevel> OrderBook::get_bid_depth(int levels) const
{
    std::vector<BookLevel> depth;
    auto bids_copy = bids;

    while (!bids_copy.empty() &&
           depth.size() < static_cast<size_t>(levels))
    {
        auto order = bids_copy.top();
        bids_copy.pop();

        if (!order->is_active)
        {
            continue;
        }

        depth.push_back(
            {order->price, order->remaining_quantity}
        );
    }

    return depth;
}

std::vector<BookLevel> OrderBook::get_ask_depth(int levels) const
{

    std::vector<BookLevel> depth;

    auto asks_copy = asks;

    while (!asks_copy.empty() &&

           depth.size() < static_cast<size_t>(levels))

    {

        auto order = asks_copy.top();

        asks_copy.pop();

        if (!order->is_active)

        {

            continue;

        }

        depth.push_back(

            {order->price, order->remaining_quantity}

        );

    }

    return depth;
}