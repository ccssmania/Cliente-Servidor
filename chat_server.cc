//#include "json.hpp"
#include <cassert>
#include <iostream>
#include <string>
#include <unordered_map>
#include <zmqpp/zmqpp.hpp>
#include <sstream>
#include <SFML/Audio.hpp>

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
  User(const string &name, const string &pwd, const string &id)
      // Attribute initialization
      : name(name),
        password(pwd),
        netId(id),
        connected(false) {}
  bool isPassword(const string &pwd) const { return password == pwd; }
  void connect(const string &id) {
    connected = true;
    netId = id;
  }

  const string &identity() const{
    return netId;
  }

  const string &userName() const{
    
    return name;
  }

  bool state(){
    return connected;
  }
};

class ServerState {
private:
  // Connected users
  unordered_map<string, User> users;
  unordered_map<string, list<User>> groups;
  socket &sckt;

public:
  //serverState() {}
  ServerState(socket &s): sckt(s){}
  
  void send(message &m){
    sckt.send(m);
  }

  void newUser(const string &name, const string &pwd, const string &id) {
    // TODO: check that the user does not exist.
    users[name] = User(name, pwd, id);
    //users[id] = User(name, pwd, id);
  }
  bool login(const string &name, const string &pwd, const string &id) {
    if (users.count(name) > 0) {
      // User is registered
      bool ok = users[name].isPassword(pwd);
      if (ok)
        users[name].connect(id);
      return ok;
    }
    return false;
  }

  

  bool conectado(string &name){
    if(users[name].state() == true) return true;

  }

  void sendMessage(const string &dest, const string &text, const string &sender){
    message m;
    //users[dest].identity;
    cout << "estoy en server sendMessage" << endl;
    cout << "NOMBRE :" << sender << endl;
    m << users[dest].identity() << text << sender ;

    send(m);
  }
  
};

void sendMessage(message &msg, const string dest, const string &sender, ServerState &server){
  cout << "msg parts :"<< msg.parts() << endl;
  
  string text;
  string aux;
  for (int i = 0; i < msg.parts() -3; i++){
    
    msg >> aux;
    text += aux + " ";
  }
  string name;
  msg >> name;
  server.sendMessage(dest, text, name);
}

void login(message &msg, const string &sender, ServerState &server) {

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

void singUp(message &msg, const string &sender, ServerState &server){
  string userName;
  msg >> userName;
  string password;
  msg >> password;
  server.newUser(userName, password, sender); 
}

void dispatch(message &msg, ServerState &server) {
  assert(msg.parts() > 1);
  
  string sender;
  msg >> sender;

  string action;
  msg >> action;

  cout <<"action " << action << endl;

  //cout << " sender : " << sender << endl;
  bool estado = server.conectado(action);
  cout << "estado :" << estado << endl;

  if (action == "login") {

    login(msg, sender, server);

  } else if(server.conectado(action) == true){


    sendMessage(msg, action, sender, server);

  } else if(action == "singUp"){

    singUp(msg, sender, server);

  } else if(action == "newUser"){

    string name;
    msg >> name;
    string password;
    msg >> password;

    server.newUser(name,password,sender);

  }else {
      cerr << "Action not supported/implemented" << endl;
      message reply;
      reply << sender << " unsupported " << action;
      server.send(reply);
  }
}


int main(int argc, char *argv[]) {
  context ctx;
  socket s(ctx, socket_type::xreply);
  s.bind("tcp://*:4242");

  ServerState state(s);
  state.newUser("Gustavo", "123", "");
  state.newUser("Choque", "123", "");


  while (true) {
    message req;
    s.receive(req);

    dispatch(req, state);
  }
  cout << "Finished." << endl;
}
