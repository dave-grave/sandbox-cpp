build:
	mkdir -p output
	g++ -o output/getaddrinfo getaddrinfo.cpp -lcrypto -lssl

debug:
	mkdir -p output
	g++ -g -o output/getaddrinfo getaddrinfo.cpp -lcrypto -lssl

clean: 
	rm output/getaddrinfo