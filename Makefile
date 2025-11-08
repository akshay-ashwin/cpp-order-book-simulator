CXX = g++
CXXFLAGS = -std=c++17 -Wall -Wextra -O2 -g
TARGET = orderbook
OBJS = Order.o Trade.o OrderBook.o main.o

all: $(TARGET)

$(TARGET): $(OBJS)
	$(CXX) $(CXXFLAGS) -o $(TARGET) $(OBJS)

%.o: %.cpp
	$(CXX) $(CXXFLAGS) -c $< -o $@

run: all
	./$(TARGET)

clean:
	rm -f $(OBJS) $(TARGET)

.PHONY: all run clean


