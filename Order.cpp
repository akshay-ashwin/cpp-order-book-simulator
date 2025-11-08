#include "Order.h"
#include <chrono>

std::atomic<int> Order::next_order_id(1);

Order::Order(OrderType type, double price, int quantity)
    : order_id(next_order_id.fetch_add(1)),
      type(type),
      price(price),
      quantity(quantity),
      remaining_quantity(quantity),
      timestamp(std::chrono::duration_cast<std::chrono::nanoseconds>(
          std::chrono::system_clock::now().time_since_epoch()).count()),
      is_active(true) {
}


