program:
	g++ client_server.cpp -o program
	g++ benchmark_client.cpp -o benchmark

server: program
	./program server

client: program 
	./program client

clean:
	rm -rf ./program
	rm -rf ./benchmark