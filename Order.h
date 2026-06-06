#ifndef ORDER_H
#define ORDER_H

#include <atomic>
#include <chrono>
#include <cstdint>

enum class OrderType {
    BUY,
    SELL
};

class Order {
// Fields are intentionally public for performance — this is a single-translation-unit
// simulation, not a library. In a production system these would be private with
// accessors and mutation controlled through the OrderBook's matching engine only.
public:
    static std::atomic<int> next_order_id;
    
    int order_id;
    OrderType type;
    double price;
    int quantity;
    int remaining_quantity;
    int64_t timestamp;
    bool is_active;
    
    Order(OrderType type, double price, int quantity);
};

#endif // ORDER_H
