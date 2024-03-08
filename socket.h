#pragma once
// Copied - ish from https://stackoverflow.com/a/69294100
// networking
#include <sys/socket.h>
#include <arpa/inet.h>

// generic
#include <iostream>
#include <string>
#include <unistd.h>

class JSocket {
    private:
    int sock;

    public:
    JSocket(int socket) : sock(socket) {
        if (sock < 0) throw std::runtime_error("JSocket creation error");
    }

    JSocket() : JSocket(socket(AF_INET, SOCK_STREAM, 0)) {}

    // read from buffer
    std::string rx() {
        char buffer[1024] = {0};
        int n = read(sock, buffer, sizeof(buffer));
        return std::string(buffer, n);
    }

    void tx(std::string s) {
        send(sock, s.c_str(), s.length(), 0);
    }

    int get_socket() {
        return sock;
    }
};

// Client connect to server.
class JConnection : public JSocket {
    public:
    JConnection(int socket) : JSocket(socket) {};
    JConnection(std::string address, unsigned short port) : JSocket() {
        struct sockaddr_in serv_addr;
        serv_addr.sin_family = AF_INET;  // use ip addresses.  I think if i change this it will break the socket.
        serv_addr.sin_port = htons(port);  // convert byte order.

        // Convert IPv4 and IPv6 addresses from text to binary form
        if (inet_pton(
            AF_INET,
            address.c_str(),
            &serv_addr.sin_addr
        ) <= 0) throw std::runtime_error("Invalid address: Address not supported");

        if (connect(
            get_socket(),
            (struct sockaddr *) &serv_addr,
            sizeof(serv_addr)
        ) < 0 ) throw std::runtime_error("\nConnection Failed\n");
    }
};

// server listen
class JPort_Listener {
    private:
    JSocket server; // fd is created in default JSocket constructor
    struct sockaddr_in address;
    int opt = 1;

    public:
    JPort_Listener(unsigned short port) {

        // Attaching to port 8080
        if (setsockopt(
            server.get_socket(),
            SOL_SOCKET,
            SO_REUSEADDR | SO_REUSEPORT,
            &opt,
            sizeof(opt)
        )) throw std::runtime_error("ERROR: setsockopt");

        address.sin_family = AF_INET;
        address.sin_addr.s_addr = INADDR_ANY;
        address.sin_port = htons(port);

        if (bind(
            server.get_socket(),
            (struct sockaddr *) &address,
            sizeof(address)
        ) < 0) throw std::runtime_error("ERRO: bind failed");

        if (listen(server.get_socket(), 3) < 0) {  // I think this is max 3 connections.
            throw std::runtime_error("ERROR: Listen");
        }
    }
    
    JConnection wait_for_connection() {
        int new_socket;
        int addrlen = sizeof(struct sockaddr_in);
        new_socket = accept(
            server.get_socket(),
            (struct sockaddr *) &address,
            (socklen_t*) &addrlen
        );
        if (new_socket < 0) throw std::runtime_error("ERROR: accept");
        return JConnection(new_socket);
    }
};