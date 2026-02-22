// ======================================================
// =                                                    =
// =                                                    =
// =            David's homemade c++ labs               =
// =       lecs 10-11: HTTP and HTTPS requests          =
// =                     2/22/26                        =
// =                                                    =
// =                                                    =
// ======================================================
//
// Description: sandbox environment for creating HTTP/S calls to nytimes and
// neverssl.com to fetch HTML content from websites.
//
// The main steps include:
// 0. parsing the URL
// 1. getaddrinfo() = perform DNS lookup on URL
// 2. socket()
// 3. connect() the socket
// 3.5. (if HTTPS) set up SSL
// 4. send GET request
// 5. receive response with recv() + send(), or SSL_read() + SSL_write()
// 6. celebrate!
//
// ======================================================

// Optional implementation that I won't be doing here.
// class ParsedUrl
// {
//   public:
//     const char *CompleteUrl;

//     * Assign host and path based on parsed string, port based on HTTP/HTTPS
//     * idk what Service does lmao
//     char *Service, *Host, *Path, *Port;

//     ParsedUrl(const char *url);
//     ~ParsedUrl();
// }

#include <cassert>
#include <iostream>
#include <netdb.h>
#include <openssl/ssl.h>
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
    int getAddrResult = getaddrinfo("www.nytimes.com", "443", &hints, &address);
    PrintAddress((sockaddr_in *)address->ai_addr, sizeof(struct sockaddr_in));

    // connect the HTTP address to the socket.
    int s = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
    assert(s != -1);

    int connectResult = connect(s, address->ai_addr, address->ai_addrlen);
    assert(connectResult == 0);

    // ==== begin ssl setup (FOR HTTPS) ====

    // And now for the more granular details on SSL setup...

    // 1. Must call this to initialize SSL ciphers and before running any SSL
    // functions
    // int SSL_library_init();
    SSL_library_init();

    // 2. Create new SSL context framework to be passed into SSl functions
    // int SSL_CTX *SSL_CTX_new(const SSL_METHOD *method);
    SSL_CTX *ctx = SSL_CTX_new(TLS_method());
    assert(ctx);

    // 3. Create SSL handler
    // SSL *SSL_new(SSL_CTX *ctx);
    SSL *ssl = SSL_new(ctx);

    // 4. Pass a file descriptor. it's usually the fd of the socket used for the
    // network connection.
    // int SSL_set_fd(SSL *ssl, int fd)
    SSL_set_fd(ssl, s);

    // Set the host name. This will take in the url.Host from the parsedUrl
    // struct.
    SSL_set_tlsext_host_name(ssl, "www.nytimes.com");

    // 5. Start the SSL handshake.
    // int SSL_connect(SSL *ssl);
    int sslConnectResult = SSL_connect(ssl);
    assert(sslConnectResult == 1);

    // ==== end ssl setup ====

    string getMessage;
    getMessage = "GET / HTTP/1.1\r\n"
                 "Host: www.nytimes.com\r\n"
                 "User-Agent: LinuxGetUrl/2.0 twoogle498@umich.edu (Linux)\r\n"
                 "Accept: */*\r\n"
                 "Accept-Encoding: identity\r\n"
                 "Connection: close\r\n"
                 "\r\n";

    // send the GET message to nytimes
    // send(s, getMessage.c_str(), getMessage.length(), 0); // for HTTP
    SSL_write(ssl, getMessage.c_str(), getMessage.length());

    // read the response
    char buffer[10240];
    int bytes;

    // Use recv() for HTTP, SSL_read() for HTTPS
    // while ((bytes = recv(s, buffer, sizeof(buffer), 0)) > 0) // for HTTP
    while ((bytes = SSL_read(ssl, buffer, sizeof(buffer))) > 0)
    {
        // Use write() for HTTP, SSL_write() for HTTPS.
        write(1, buffer, bytes); // for HTTP
        // SSL_write(ssl, buffer, bytes);
    }

    // Free up SSL and socket resources
    SSL_shutdown(ssl);
    SSL_free(ssl);
    SSL_CTX_free(ctx);

    close(s);

    // Free addrinfo (it's a linked list)
    freeaddrinfo(address);
};
