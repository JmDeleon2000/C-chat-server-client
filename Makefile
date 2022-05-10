all: client server

client:
	@echo "compiling client"
	g++ -o client proyecto.pb.cc client.cpp -lprotobuf -pthread
server:
	@echo "compiling server"
	g++ -o client proyecto.pb.cc server.cpp -lprotobuf -pthread

clean:
	*.txt
	*.o