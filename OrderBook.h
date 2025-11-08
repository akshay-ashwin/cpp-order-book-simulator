#ifndef ORDERBOOK_H
#define ORDERBOOK_H

#include "Order.h"
#include "Trade.h"
#include <queue>
#include <unordered_map>
#include <vector>
#include <memory>

using OrderPtr = std::shared_ptr<Order>;

struct CompareBids {
    bool operator()(const OrderPtr& a, const OrderPtr& b) const {
        if (a->price != b->price) {
            return a->price < b->price;  // Max-heap: higher price has priority
        }
        return a->timestamp > b->timestamp;  // Older timestamp (smaller value) has priority
    }
};

struct CompareAsks {
    bool operator()(const OrderPtr& a, const OrderPtr& b) const {
        if (a->price != b->price) {
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
    
public:
    int add_order(OrderType type, double price, int quantity);
    void cancel_order(int order_id);
    void print_book() const;
    void print_trades() const;
};

#endif // ORDERBOOK_H


