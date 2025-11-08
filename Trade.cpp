#include "Trade.h"
#include <iostream>
#include <iomanip>
#include <chrono>

std::atomic<int> Trade::next_trade_id(1);

Trade::Trade(int buy_order_id, int sell_order_id, double price, int quantity, long timestamp)
    : trade_id(next_trade_id.fetch_add(1)),
      buy_order_id(buy_order_id),
      sell_order_id(sell_order_id),
      price(price),
      quantity(quantity),
      timestamp(timestamp) {
}

void Trade::print() const {
    std::cout << "Trade " << trade_id 
              << ": " << quantity << " @ " 
              << std::fixed << std::setprecision(2) << price
              << " (Buy: " << buy_order_id 
              << ", Sell: " << sell_order_id << ")" << std::endl;
}


