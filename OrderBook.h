#ifndef ORDERBOOK_H
#define ORDERBOOK_H

#include "Order.h"
#include "Trade.h"
#include <queue>
#include <unordered_map>
#include <vector>
#include <memory>
#include <cmath>

static constexpr double PRICE_EPSILON = 1e-9;

// Internal alias — not part of public API
using OrderPtr = std::shared_ptr<Order>;

struct BookLevel {
    double price;
    int quantity;
};

struct CompareBids {
    bool operator()(const OrderPtr& a, const OrderPtr& b) const {
        if (std::fabs(a->price - b->price) > PRICE_EPSILON) {
            return a->price < b->price;  // Max-heap: higher price has priority
        }
        return a->timestamp > b->timestamp;  // Older timestamp (smaller value) has priority
    }
};

struct CompareAsks {
    bool operator()(const OrderPtr& a, const OrderPtr& b) const {
        if (std::fabs(a->price - b->price) > PRICE_EPSILON) {
            return a->price > b->price;  // Min-heap: lower price has priority
        }
        return a->timestamp > b->timestamp;  // Older timestamp (smaller value) has priority
    }
};

class OrderBook {
private:
    std::priority_queue<OrderPtr, std::vector<OrderPtr>, CompareBids> bids;
    std::priority_queue<OrderPtr, std::vector<OrderPtr>, CompareAsks> asks;
    std::unordered_map<int, OrderPtr> order_map;
    std::vector<Trade> trade_history;

    void match_orders();
    bool can_fully_fill(

    OrderType type,

    double price,

    int quantity

 ) const;

public:
    int add_order(
        OrderType type, 
        double price, 
        int quantity,
        OrderExecutionType execution_type = OrderExecutionType::LIMIT);
    void cancel_order(int order_id);
    void print_book() const;
    void print_trades() const;

    // Testing accessors
    size_t trade_count() const;
    const std::vector<Trade>& get_trade_history() const;
    size_t active_order_count() const;

    double get_best_bid() const;
    double get_best_ask() const;
    double get_spread() const;
<<<<<<< HEAD
    double get_mid_price() const;

    std::vector<BookLevel> get_bid_depth(int levels) const;
    std::vector<BookLevel> get_ask_depth(int levels) const;
=======
>>>>>>> dfd83db (feat: add spread calculation)
};

#endif // ORDERBOOK_H