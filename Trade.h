#ifndef TRADE_H
#define TRADE_H

#include <atomic>

class Trade {
public:
    static std::atomic<int> next_trade_id;
    
    int trade_id;
    int buy_order_id;
    int sell_order_id;
    double price;
    int quantity;
    long timestamp;
    
    Trade(int buy_order_id, int sell_order_id, double price, int quantity, long timestamp);
    void print() const;
};

#endif // TRADE_H


