#include <cassert>
#include <iostream>
#include <string>
#include <unordered_map>
#include <zmqpp/zmqpp.hpp>

using namespace std;
using namespace zmqpp;

class User {
private:
    string name;
    string password;
    string netId;
    bool connected;
    list<string> contacts;

public:
    User() {}
    User(const string& name, const string& pwd, const string& id)
          : name(name), password(pwd), netId(id), connected(false) {}

    bool isPassword(const string& pwd) const {
        return password == pwd;
    }
    void connect(const string& id) {
        connected = true;
        netId = id;
    }

    bool inContacts(string& user) {
        for (const auto& it : contacts) {
            if (it == user) return true;
        }
        return false;
    }

    bool isConnected(void) {
        return connected;
    }

    const string& identity() const {
        return netId;
    }
};

class ServerState {
private:
    // connected users
    unordered_map<string, User> users;
    unordered_map<string, list<User>> groups;
    socket& sckt;

public:
    ServerState(socket& s) : sckt(s) {}

    void send(message& m) {
        sckt.send(m);
    }

    void newUser(const string& name, const string& pwd, const string& id) {
        users[name] = User(name, pwd, id);
    }

    bool login(const string& name, const string& pwd, const string& id) {
        if (users.count(name)) {
            // User is registered
            bool ok = users[name].isPassword(pwd);
            if (ok) users[name].connect(id);
            return ok;
        }
        return false;
    }

    void sendMessage(const string& dest, const string& text) {
        message m;
        m << users[dest].identity() << text;
        send(m);
    }

    User getUser(string& name) {
        if (users.count(name))
            return users[name];
        else {
            return User();  // No estoy seguro si esto funciona
        }
    }

    void newGroup(string& creator, string& name) {
        if (groups.count(name)) {
            cerr << "Group " << name << " is already created" << endl;
        } else {
            groups[name].push_front(users[creator]);
            cout << "Group created" << endl;
        }
    }

    void addToGroup(string& nameGroup, string& name) {
        if (!groups.count(name)) {
            groups[nameGroup].push_front(users[name]);
        } else {
            cout << "Group name does not exist" << endl;
        }
    }

    bool isGroup(string& name) {
        string groupName;
        for(unordered_map<string, list<string >>::iterator i = groups.begin(); i != groups.end(); i++){
            for(list<string>::iterator j = groups[i->first].begin(); j != groups[i->first].end(); j++){
                if(*j == name) return true;
            }
        }
        return false;
        
    }

    void sendGroupMessage(const string& dest, const string& sender,
                          const string& text) {
        for (const auto& user : groups[dest]) {
            if (user.identity() != sender) {
                message m;
                m << user.identity() << "Group " << dest << sender << " : "
                  << text;
                send(m);
            }
        }
    }
};

void login(message& msg, const string& sender, ServerState& server) {
    string userName;
    msg >> userName;
    string password;
    msg >> password;
    if (server.login(userName, password, sender)) {
        cout << "User " << userName << " joins the chat server" << endl;
    } else {
        cerr << "Wrong user/password " << endl;
    }
}

void sendMessage(message& msg, string& sender, ServerState& server) {
    string dest;  // Nombre de usuario destino o grupo
    msg >> dest;

    if (server.isGroup(dest)) {  // Envio uno a muchos
        string text;
        msg >> text;
        cout << "ha ingresado a chat de grupo" << endl;

        server.sendGroupMessage(dest, sender, text);

    } else if (  // server.getUser(sender).inContacts(dest) &&
        server.getUser(dest).isConnected()) {  // Envio uno a uno
        string text;
        msg >> text;
        server.sendMessage(dest, text);

    }

    else {
        cerr << "User not found in contacts/offline" << endl;
    }
}

void dispatch(message& msg, ServerState& server) {
    assert(msg.parts() > 2);
    string sender;
    msg >> sender;

    string action;
    msg >> action;
    message m;

    if (action == "login") {
        login(msg, sender, server);

    } else if (action == "newUser") {
        string name;
        msg >> name;

        string pwd;
        msg >> pwd;

        server.newUser(name, pwd, sender);
        cout << "sender created " << sender << endl;

        m << sender << "The user has been created";
        server.send(m);

    } else if (action == "msg") {
        sendMessage(msg, sender, server);
    } else if (action == "newGroup") {
        string name;
        msg >> name;

        server.newGroup(sender, name);

        m << sender << "The group has been created";
        server.send(m);
    } else if ("addToGroup") {
        string nameGroup, user;
        msg >> nameGroup;
        msg >> user;
        server.addToGroup(nameGroup, user);

        m << sender << "The user has been added";
        server.send(m);

    } else {
        cerr << "Action not supported/implemented" << endl;
    }
}

int main(int argc, char* argv[]) {
    const string endpoint = "tcp://*:4242";
    context ctx;

    socket s(ctx, socket_type::xreply);
    s.bind(endpoint);

    ServerState state(s);
    state.newUser("sebas", "123", "");
    state.newUser("william", "123", "");

    while (true) {
        message req;
        s.receive(req);
        dispatch(req, state);
    }

    cout << "Finished." << endl;
}

// Formato para enviar mensaje luego de haber sido logueado
// Id | action | Name | Name destino | mensaje de texto
