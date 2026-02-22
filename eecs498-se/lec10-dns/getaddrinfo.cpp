// class ParsedUrl
// {
//   public:
//     const char *CompleteUrl;

//     char *Service, *Host, *Path, *Port;

//     ParsedUrl(const char *url);
//     ~ParsedUrl();
// }

#include <cassert>
#include <iostream>
#include <netdb.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <unistd.h>

using namespace std;

// Prints the address information of a specific socket address.
void PrintAddress(const sockaddr_in *s, const size_t saLength)
{
    const struct in_addr *ip = &s->sin_addr;

    uint32_t a = ntohl(ip->s_addr);

    cout << "Host address length = " << saLength << " bytes" << endl;
    cout << "Family = " << s->sin_family << ", port = " << ntohs(s->sin_port)
         << ", address = " << (a >> 24) << '.' << ((a >> 16) & 0xff) << '.'
         << ((a >> 8) & 0xff) << '.' << (a & 0xff) << '\n'
         << endl;
};

int main()
{
    // Declare addrinfo stuffs.
    struct addrinfo *address, hints;
    memset(&hints, 0, sizeof(hints));

    // Set up hints for the getaddrinfo() call.
    // I think these constant values are necessary for the getaddrinfo() call.
    hints.ai_family = AF_INET; // IPv4 address family
    hints.ai_socktype = SOCK_STREAM;
    hints.ai_protocol = IPPROTO_TCP;

    // getaddrinfo() call
    // apparently neverssl.com and nytimes.com have the same CDN or smth so we
    // can use both URLs and still be able to receive a successful GET response.
    int getAddrResult = getaddrinfo("neverssl.com", "80", &hints, &address);
    PrintAddress((sockaddr_in *)address->ai_addr, sizeof(struct sockaddr_in));

    // connect the HTTP address to the socket.
    int s = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
    assert(s != -1);

    int connectResult = connect(s, address->ai_addr, address->ai_addrlen);
    assert(connectResult == 0);

    string getMessage;
    getMessage = "GET / HTTP/1.1\r\n"
                 "Host: neverssl.com\r\n"
                 "User-Agent: LinuxGetUrl/2.0 twoogle498@umich.edu (Linux)\r\n"
                 "Accept: */*\r\n"
                 "Accept-Encoding: identity\r\n"
                 "Connection: close\r\n"
                 "\r\n";

    // send the GET message to nytimes
    send(s, getMessage.c_str(), getMessage.length(), 0);

    // read the response
    char buffer[10240];
    int bytes;
    while ((bytes = recv(s, buffer, sizeof(buffer), 0)) > 0)
    {
        write(1, buffer, bytes);
    }

    close(s);

    // free address
    freeaddrinfo(address);
};
