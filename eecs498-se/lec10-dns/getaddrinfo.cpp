// class ParsedUrl
// {
//   public:
//     const char *CompleteUrl;

//     char *Service, *Host, *Path, *Port;

//     ParsedUrl(const char *url);
//     ~ParsedUrl();
// }

#include <iostream>
#include <netdb.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/types.h>

using namespace std;

// Prints the address information of a specific socket address.
void PrintAddress(const sockaddr_in *s, const size_t saLength)
{
    const struct in_addr *ip = &s->sin_addr;

    uint32_t a = ntohl(ip->s_addr);

    cout << "Host address length = " << saLength << " bytes" << endl;
    cout << "Family = " << s->sin_family << ", port = " << ntohs(s->sin_port)
         << ", address = " << (a >> 24) << '.' << ((a >> 16) & 0xff) << '.'
         << ((a >> 8) & 0xff) << '.' << (a & 0xff) << endl;
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
    int getAddrResult = getaddrinfo("www.nytimes.com", "80", &hints, &address);
    PrintAddress((sockaddr_in *)address->ai_addr, sizeof(struct sockaddr_in));
};
