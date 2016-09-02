#include <cassert>
#include <cstdlib>
#include <iostream>
#include <string>
#include <zmqpp/zmqpp.hpp>
#include <vector>
#include <sstream>
using namespace std;
using namespace zmqpp;

vector<string> tokenize(string& input) {
    stringstream ss(input);
    vector<string> result;
    string s;
    while (ss >> s) result.push_back(s);
    return result;
}

int main(int argc, char* argv[]) {
    if (argc != 5) {
        cerr << "Invalid arguments" << endl;
        return EXIT_FAILURE;
    }

    string address(argv[1]);
    string action(argv[2]);
    string userName(argv[3]);
    string password(argv[4]);
    string sckt("tcp://");
    sckt += address;

    context ctx;
    socket s(ctx, socket_type::xrequest);

    cout << "Connecting to: " << sckt << endl;
    s.connect(sckt);

    message msg;
    msg << action << userName << password;
    s.send(msg);

    int console = fileno(stdin);
    poller poll;
    poll.add(s, poller::poll_in);
    poll.add(console, poller::poll_in);
    while (true) {
        if (poll.poll()) {  // There are events in at least one of the sockets
            if (poll.has_input(s)) {
                // Handle input in socket
                message m;
                s.receive(m);
                string response;
                m >> response;
                cout << "Socket> " << response << endl;
            }
            if (poll.has_input(console)) {
                // Handle input from console
                string input;
                getline(cin, input);
                if (input.size() != 0) {
                    vector<string> tokens = tokenize(input);
                    message m;
                    for (const auto& str : tokens) m << str;
                    s.send(m);
                }
            }
        }
    }
    return EXIT_SUCCESS;
}