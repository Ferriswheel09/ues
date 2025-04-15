program:
	g++ client_server.cpp -o program

server: program
	./program server

client: program 
	./program client

clean:
	rm -rf ./program