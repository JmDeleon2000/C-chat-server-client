all: client server

client:
	@echo "compiling client"
	g++ -o client proyecto.pb.cc client.cpp -lprotobuf -pthread 
server:
	@echo "compiling server"


clean:
	*.txt
	*.o