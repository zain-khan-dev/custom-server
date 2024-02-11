# Define the compiler
CXX = g++

# Define flags
CXXFLAGS = -std=c++11 -Wall

server.o: obj/main.o obj/server_controller.o obj/socket_connection.o
	$(CXX) $(CXXFLAGS) obj/main.o obj/server_controller.o obj/socket_connection.o -o server.o

# Build the target executable
obj/main.o: main.cpp server/server_controller.h server/socket_connection.h
	$(CXX) $(CXXFLAGS) -c main.cpp -o obj/main.o

# Build the server_controller object file
obj/server_controller.o: server/server_controller.cpp server/server_controller.h
	$(CXX) $(CXXFLAGS) -c server/server_controller.cpp -o obj/server_controller.o

# Build the socket_connection object file
obj/socket_connection.o: server/socket_connection.cpp server/socket_connection.h
	$(CXX) $(CXXFLAGS) -c server/socket_connection.cpp -o obj/socket_connection.o

# Clean rule to remove compiled files
clean:
	rm -f obj/main.o obj/server_controller.o obj/socket_connection.o server.o

