CXX = g++
CXXFLAGS = -std=c++11 -Wall

all: server client

server: server.cpp
	$(CXX) $(CXXFLAGS) -o server server.cpp

client: client.cpp
	$(CXX) $(CXXFLAGS) -o client client.cpp -pthread

clean:
	rm -f server client 