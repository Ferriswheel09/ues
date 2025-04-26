program:
	g++ client_server.cpp -o program

server: program
	./program server

benchmark: 
	g++ benchmark_client.cpp -o benchmark
	./benchmark


client: program 
	./program client

clean:
	rm -rf ./program