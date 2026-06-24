#include <gtest/gtest.h>
#include "../OrderBook.h"

TEST(OrderBookTest, FullFill)
{
    OrderBook book;

    book.add_order(OrderType::BUY, 100.0, 10);
    book.add_order(OrderType::SELL, 100.0, 10);

    EXPECT_EQ(book.trade_count(), 1UL);

    const auto& trades = book.get_trade_history();

    ASSERT_EQ(trades.size(), 1UL);

    EXPECT_EQ(trades[0].quantity, 10);
    EXPECT_DOUBLE_EQ(trades[0].price, 100.0);
}

TEST(OrderBookTest, PartialFill)
{
    OrderBook book;

    book.add_order(OrderType::BUY, 100.0, 20);
    book.add_order(OrderType::SELL, 100.0, 10);

    EXPECT_EQ(book.trade_count(), 1UL);

    const auto& trades = book.get_trade_history();

    ASSERT_EQ(trades.size(), 1UL);

    EXPECT_EQ(trades[0].quantity, 10);
    EXPECT_DOUBLE_EQ(trades[0].price, 100.0);
}

TEST(OrderBookTest, PricePriority)
{
    OrderBook book;

    book.add_order(OrderType::BUY, 100.0, 10);
    book.add_order(OrderType::BUY, 105.0, 10);

    book.add_order(OrderType::SELL, 105.0, 10);

    EXPECT_EQ(book.trade_count(), 1UL);

    const auto& trades = book.get_trade_history();

    ASSERT_EQ(trades.size(), 1UL);

    EXPECT_EQ(trades[0].quantity, 10);

    // Higher-priced bid should have executed
    EXPECT_EQ(trades[0].buy_order_id, 6);
}

TEST(OrderBookTest, TimePriority)
{
    OrderBook book;

    int first_buy  = book.add_order(OrderType::BUY, 100.0, 10);
    book.add_order(OrderType::BUY, 100.0, 10);

    book.add_order(OrderType::SELL, 100.0, 10);

    EXPECT_EQ(book.trade_count(), 1UL);

    const auto& trades = book.get_trade_history();

    ASSERT_EQ(trades.size(), 1UL);

    EXPECT_EQ(trades[0].buy_order_id, first_buy);
}

TEST(OrderBookTest, CancelOrder)
{
    OrderBook book;

    int buy_id = book.add_order(OrderType::BUY, 100.0, 10);

    book.cancel_order(buy_id);

    book.add_order(OrderType::SELL, 100.0, 10);

    EXPECT_EQ(book.trade_count(), 0UL);

    EXPECT_TRUE(book.get_trade_history().empty());
}

TEST(OrderBookTest, MultipleFills)
{
    OrderBook book;

    book.add_order(OrderType::BUY, 100.0, 30);

    book.add_order(OrderType::SELL, 100.0, 10);
    book.add_order(OrderType::SELL, 100.0, 10);
    book.add_order(OrderType::SELL, 100.0, 10);

    EXPECT_EQ(book.trade_count(), 3UL);

    const auto& trades = book.get_trade_history();

    ASSERT_EQ(trades.size(), 3UL);

    EXPECT_EQ(trades[0].quantity, 10);
    EXPECT_EQ(trades[1].quantity, 10);
    EXPECT_EQ(trades[2].quantity, 10);
}

TEST(OrderBookTest, MarketBuyConsumesBestAsk)
{
    OrderBook book;

    book.add_order(OrderType::SELL, 100.0, 10);

    book.add_order(
        OrderType::BUY,
        0.0,
        10,
        OrderExecutionType::MARKET
    );

    EXPECT_EQ(book.trade_count(), 1UL);

    const auto& trades = book.get_trade_history();

    ASSERT_EQ(trades.size(), 1UL);

    EXPECT_EQ(trades[0].quantity, 10);
    EXPECT_DOUBLE_EQ(trades[0].price, 100.0);
}

TEST(OrderBookTest, MarketSellConsumesBestBid)
{
    OrderBook book;

    book.add_order(OrderType::BUY, 100.0, 10);

    book.add_order(
        OrderType::SELL,
        0.0,
        10,
        OrderExecutionType::MARKET
    );

    EXPECT_EQ(book.trade_count(), 1UL);

    const auto& trades = book.get_trade_history();

    ASSERT_EQ(trades.size(), 1UL);

    EXPECT_EQ(trades[0].quantity, 10);
    EXPECT_DOUBLE_EQ(trades[0].price, 100.0);
}

TEST(OrderBookTest, MarketOrderWithoutLiquidity)
{
    OrderBook book;

    book.add_order(
        OrderType::BUY,
        0.0,
        10,
        OrderExecutionType::MARKET
    );

    EXPECT_EQ(book.trade_count(), 0UL);

    EXPECT_TRUE(book.get_trade_history().empty());
}

TEST(OrderBookTest, IOCLeavesNoResidual)
{
    OrderBook book;

    book.add_order(OrderType::SELL, 100.0, 10);

    book.add_order(
        OrderType::BUY,
        100.0,
        20,
        OrderExecutionType::IOC
    );

    EXPECT_EQ(book.trade_count(), 1UL);

    const auto& trades = book.get_trade_history();

    ASSERT_EQ(trades.size(), 1UL);

    EXPECT_EQ(trades[0].quantity, 10);

    EXPECT_EQ(book.active_order_count(), 0UL);
}

TEST(OrderBookTest, FOKFailsWithoutFullLiquidity)
{
    OrderBook book;

    book.add_order(
        OrderType::SELL,
        100.0,
        10
    );

    book.add_order(
        OrderType::BUY,
        100.0,
        20,
        OrderExecutionType::FOK
    );

    EXPECT_EQ(book.trade_count(), 0UL);

    EXPECT_EQ(
        book.active_order_count(),
        1UL
    );
}

TEST(OrderBookTest, BestBidReturnsHighestBuyPrice)
{
    OrderBook book;

    book.add_order(OrderType::BUY, 100.0, 10);
    book.add_order(OrderType::BUY, 105.0, 10);
    book.add_order(OrderType::BUY, 102.0, 10);

    EXPECT_DOUBLE_EQ(book.get_best_bid(), 105.0);
}

TEST(OrderBookTest, BestAskReturnsLowestSellPrice)
{
    OrderBook book;

    book.add_order(OrderType::SELL, 105.0, 10);
    book.add_order(OrderType::SELL, 100.0, 10);
    book.add_order(OrderType::SELL, 102.0, 10);

    EXPECT_DOUBLE_EQ(book.get_best_ask(), 100.0);
}