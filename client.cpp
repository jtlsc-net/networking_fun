#include "socket.h"
#include <iostream>

int main(int argc, char const *argv[]) {
    using std::cerr, std::cout, std::endl;

    try {
        JConnection conn("127.0.0.1", 8080);
        conn.tx("Hello from client");
        cout << "Hello message sent from client" << endl;
        std::string s = conn.rx();
        cout << s << endl;
    } catch (std::exception &e) {
        cerr << e.what() << endl;
        return EXIT_FAILURE;
    }

    return 0;
}