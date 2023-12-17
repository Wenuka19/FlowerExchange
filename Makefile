CC = g++
CFLAGS = -std=c++17 -Wall
LDFLAGS = -pthread 

SERVER := server
CLIENT := client

all: $(SERVER) $(CLIENT)

$(SERVER): server.cpp Order.cpp OrderBook.cpp MatchingEngine.cpp
	$(CXX) $(CXXFLAGS) -o $@ $^

$(CLIENT): client.cpp Order.cpp OrderBook.cpp MatchingEngine.cpp
	$(CXX) $(CXXFLAGS) -o $@ $^

run_server: $(SERVER)
	gnome-terminal -- bash -c "./$(SERVER); exec bash"

run_client: $(CLIENT)
	gnome-terminal -- bash -c "./$(CLIENT); exec bash"

clean:
	rm -f $(SERVER) $(CLIENT)
