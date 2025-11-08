#ifndef ORDER_H
#define ORDER_H

#include <atomic>
#include <chrono>

enum class OrderType {
    BUY,
    SELL
};

class Order {
public:
    static std::atomic<int> next_order_id;
    
    int order_id;
    OrderType type;
    double price;
    int quantity;
    int remaining_quantity;
    long timestamp;
    bool is_active;
    
    Order(OrderType type, double price, int quantity);
};

#endif // ORDER_H


