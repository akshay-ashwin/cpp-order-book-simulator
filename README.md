# C++ Real-Time Order Book Simulator

A command-line C++ application that simulates a real-time financial order book for a single asset. The system processes buy and sell orders, matches them based on price-time priority, and logs trades automatically.

## Features

- **Real-Time Matching Engine**: Automatically matches buy and sell orders when prices cross
- **Price-Time Priority**: Orders are matched based on best price first, then by time (FIFO)
- **Lazy Deletion**: Efficient order cancellation using lazy deletion technique
- **Partial Fills**: Large orders can be partially filled across multiple trades
- **Interactive CLI**: Simple command-line interface for order management

## Architecture

The system uses efficient data structures for optimal performance:

- **Max-Heap (Priority Queue)**: Stores all BUY orders, with highest price at the top
- **Min-Heap (Priority Queue)**: Stores all SELL orders, with lowest price at the top
- **Hash Map**: Provides O(1) lookup for order cancellation
- **Lazy Deletion**: Canceled orders are marked inactive and cleaned during matching

## Building

### Prerequisites

- C++17 compatible compiler (g++ recommended)
- Make

### Compilation

```bash
make
```

This will create an executable named `orderbook`.

### Running

```bash
make run
```

Or directly:

```bash
./orderbook
```

### Clean Build Artifacts

```bash
make clean
```

## Usage

### Commands

- `BUY <quantity> <price>` - Add a buy order
- `SELL <quantity> <price>` - Add a sell order
- `CANCEL <order_id>` - Cancel an existing order
- `BOOK` - Display the current order book
- `TRADES` - Display all executed trades
- `HELP` - Show available commands
- `EXIT` - Exit the program

### Example Session

```
> BUY 1000 50.00
Order 1: BUY 1000 @ 50.00

> SELL 500 49.50
Order 2: SELL 500 @ 49.50
Trade 1: 500 @ 50.00 (Buy: 1, Sell: 2)

> BOOK
=== ORDER BOOK ===
BIDS           ASKS           
Qty | Price    Price | Qty    
--------------------------------
500 | 50.00                    

> TRADES
=== TRADE HISTORY ===
Trade 1: 500 @ 50.00 (Buy: 1, Sell: 2)
```

## Project Structure

```
.
├── Order.h          # Order class definition
├── Order.cpp        # Order implementation
├── Trade.h          # Trade class definition
├── Trade.cpp        # Trade implementation
├── OrderBook.h      # OrderBook class and comparators
├── OrderBook.cpp    # OrderBook implementation
├── main.cpp         # Application entry point
├── Makefile         # Build configuration
└── README.md        # This file
```

## Key Components

### Order
Represents a single buy or sell order with:
- Unique order ID (atomic counter)
- Order type (BUY/SELL)
- Price and quantity
- Timestamp for time-priority
- Active status flag

### Trade
Represents an executed trade with:
- Unique trade ID
- Buy and sell order IDs
- Execution price and quantity
- Timestamp

### OrderBook
Main matching engine that:
- Manages order heaps and lookup map
- Executes trades when orders match
- Handles lazy deletion for cancellations
- Maintains trade history

## Matching Logic

1. When a new order is added, the matching engine checks if it can match
2. Matching occurs when: `highest_bid_price >= lowest_ask_price`
3. Trade price uses the price of the order that was in the book first (older timestamp)
4. Orders are matched in full or partially based on available quantities
5. Fully filled orders are removed from the book
6. Canceled orders are cleaned from heaps during matching (lazy deletion)

## License

This project is open source and available for educational purposes.

