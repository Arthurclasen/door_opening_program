/*
server code for conteiners
credits to Beej's Guide to Network Programming
*/

#define _POSIX_C_SOURCE 200112L

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include <arpa/inet.h>
#include <sys/wait.h>
#include <signal.h>
#include <asm-generic/signal-defs.h>
#include "../cJSON/cJSON.h"
#include "network.h"
#include <openssl/ssl.h>
#include "../security/tls.h"

#define BACKLOG 10  // how many pending connections queue will have

// used to remove dead processes from system to not overload server
void sigchld_handler(int s)
{
    (void)s;    // quiet unused variable warning

    // waitpid() might overwrite errno, so saving is necessary
    int saved_errno = errno;

    // wait all child processes to die
    while (waitpid(-1, NULL, WNOHANG) > 0); 

    errno = saved_errno;
}

// get sockaddr, IPv4 or IPv6
void *get_in_addr(struct sockaddr *sa)
{
    if (sa->sa_family == AF_INET)
        return &(((struct sockaddr_in*)sa)->sin_addr);
    
    return &(((struct sockaddr_in6*)sa)->sin6_addr);
}

// 'restAPI' function
void dispatch_request(SSL *ssl, int client_fd, char *method, char *path, cJSON *json_data, Route *routes)
{
    int i = 0;
    int found = 0;
    // try to find the sentinel {NULL, NULL, NULL, 0}
    while (routes[i].path != NULL)
    {
        // checks if infos are ok (method and path)
        if (strcmp(routes[i].method, method) == 0 && strcmp(routes[i].path, path) == 0)
        {
            // if authentication is required
            if (routes[i].auth_required == 1)
            {
                printf("Verify JWT to protected route...\n");
                /* 
                -----------------------
                 JWT function verifier
                 MUST BE BUILT
                -----------------------
                 */
            }

            // code execution (ex : execute login, create etc...)
            routes[i].handler(client_fd, json_data);

            found = 1;
            break;
        }
        i++;
    }

    if (found == 0)
    {
        char *not_found = "HTTP/1.1 404 Not Found\r\nContent-Length: 0\r\n\r\n";
        SSL_write(ssl, not_found, strlen(not_found));
    }
}

int server_create(Route routes[], const char *port)
{
    
    // listen on sock_fd, new connection on new_fd
    int                     sockfd, new_fd;
    struct addrinfo         hints, *servinfo, *p;
    struct sockaddr_storage their_addr; // connector's address info
    socklen_t               sin_size;
    struct sigaction        sa;
    int yes=1;
    char s[INET6_ADDRSTRLEN];
    int rv;

    // OpenSSL variables (for TLS)
    OSSL_LIB_CTX *libctx = OSSL_LIB_CTX_new();
    // TLS context built in OpenSSL
    SSL_CTX *ctx = SSL_CTX_new_ex(libctx, NULL, TLS_server_method());   // context knowing it's library
    if (ctx == NULL) 
    {
        ERR_print_errors_fp(stderr);
        return -1;
    }

    memset(&hints, 0, sizeof hints);
    hints.ai_family     = AF_INET;        // ipv4
    hints.ai_socktype   = SOCK_STREAM;    // tcp connection
    hints.ai_flags      = AI_PASSIVE;     // use my IP

    // get the address accordingly to the hints and relates it to a group of sockets 
    if ((rv = getaddrinfo(NULL, port, &hints, &servinfo)) != 0)
    {
        fprintf(stderr, "getaddrinfo: %s\n", gai_strerror(rv));
        return 1; 
    }

    // loop through all results and bind the first possible
    for (p = servinfo; p != NULL; p = p->ai_next)
    {
        // creating the socket 
        if ((sockfd = socket(p->ai_family, p->ai_socktype, p->ai_protocol)) == -1)
        {
            perror("server: socket");
            continue;
        }

        // socket especifications
        if (setsockopt(sockfd, SOL_SOCKET, SO_REUSEADDR, &yes, sizeof(int)) == -1)
        {
            perror("setsockopt");
            return 1;
        }

        // binds the socket to an address
        if (bind(sockfd, p->ai_addr, p->ai_addrlen) == -1)
        {
            close(sockfd);
            perror("server: bind");
            continue;
        }

        break;  // if everything is alright we use the socket with the address found
    }

    freeaddrinfo(servinfo); // this will not be used anymore

    if (p == NULL)  // if can't find adresses to relate
    {
        fprintf(stderr, "server: failed to bind\n");
        return 1;
    }

    if (listen(sockfd, BACKLOG) == -1)  // try to listen in the socket
    {
        perror("listen");
        return 1;
    }

    sa.sa_handler = sigchld_handler;    // reap all dead processes

    // clear all signals from the group of addresses related to mask
    sigemptyset(&sa.sa_mask);           
    sa.sa_flags = SA_RESTART;
    if (sigaction(SIGCHLD, &sa, NULL) == -1)
    {
        perror("sigaction");
        return 1;
    }

    printf("server: waiting for connections...\n");
    tls_create(ctx, 3);         // stablishing TLS connection

    while(1)    // main accept() loop
    {
        sin_size = sizeof their_addr;
        new_fd = accept(sockfd, (struct sockaddr *)&their_addr, &sin_size);

        if (new_fd == -1)
        {
            perror("accept");
            continue;
        }

        // transforms interface data into binary
        inet_ntop(their_addr.ss_family, get_in_addr((struct sockaddr *)&their_addr), s, sizeof s);
        if (!fork())    // child process
        {
            close(sockfd);  // child doesn't need listener
            SSL *ssl = SSL_new(ctx);  

            // connecting using TLS  
            if (tls_connect(ssl, new_fd) != 0) {
                SSL_free(ssl);
                close(new_fd);
                exit(1);
            }
            char buf[1024];
            // function defined here

            // receiving HTTP data
            int bytes_received = SSL_read(ssl, buf, sizeof(buf) - 1);
            if (bytes_received > 0)
            {
                buf[bytes_received] = '\0';
                HttpRequest httpRequest = {0};
                sscanf(buf, "%7s %255s", httpRequest.method, httpRequest.path);

                // checks data size
                char *cl = strstr(buf, "Content-Length:");
                httpRequest.content_length = 0;
                if (cl) {
                    sscanf(cl, "Content-Length: %d\n", &httpRequest.content_length);
                    printf("Content-Length: %d\n", httpRequest.content_length);
                }

                // extract data
                char *body = strstr(buf, "\r\n\r\n");
                if (body)
                {
                    body += 4;
                    if (httpRequest.content_length > 0)
                    {
                        httpRequest.json = cJSON_Parse(body);
                    }
                }

                // running endpoint function
                dispatch_request(ssl, new_fd, httpRequest.method, httpRequest.path, httpRequest.json, routes);
                
                const char *json_body = "{\"status\":\"connection succeeded\"}";
                char http_response[512];

                // calculates the body size
                int body_len = strlen(json_body);

                // mounts the response
                sprintf(http_response,
                    "HTTP/1.1 200 OK\r\n"
                    "Content-Type: application/json\r\n"
                    "Content-Length: %d\r\n"
                    "Connection: close\r\n"
                    "\r\n"
                    "%s", 
                    body_len, json_body);

                // returns the data status
                if (SSL_write(ssl, http_response, strlen(http_response)) <= 0)
                    perror("SSL write");     
            }  
            else 
            {
                printf("Error while reading\n");
            }        

            // ---------------------
            SSL_shutdown(ssl);
            SSL_free(ssl);
            close(new_fd);
            exit(0);
        }
        close(new_fd);
    }
    
    return 0;
}

