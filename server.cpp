#include "socket.h"
#include <iostream>

int main(int argc, char const *argvp[]) {
    using std::cout;
    using std::endl;
    using std::cerr;

    try {
        JConnection conn = JPort_Listener(8080).wait_for_connection();
        cout << conn.rx() << endl;
        conn.tx("Hello from server");
        cout << "Hello message sent from server" << endl;
    } catch (std::runtime_error &e) {
        cerr << e.what() << endl;
        return EXIT_FAILURE;
    }

    return 0;
}