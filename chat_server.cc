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

  bool exist(const string &nombre){
    if(users[nombre].userName() == nombre) return true;
    else false;
  }

  bool conectado(string &name){
    if(users[name].state() == true) return true;

  }

  void sendMessage(const string &dest, const string &text, const string &sender){
    message m;
    //users[dest].identity;
    cout << "NOMBRE :" << sender << endl;
    m << users[dest].identity() << text << sender ;

    send(m);
  }

  void newGroup(const string& creator, const string& name, const string &sender) {
        if (groups.count(name)) {
            cerr << "Group " << name << " is already created" << endl;
            message res;
            res << sender << "El grupo ya fue creado" << "server";
            send(res);
        } else {
            message res;
            groups[name].push_front(users[creator]);
            cout << "Group created" << endl;
            res << sender << "el grupo de nombre :" << name << "fue creado" << "server";
            send(res);
        }
    }

    void addToGroup(string& nameGroup, string& name, const string &sender) {
        message res;
        if (!groups.count(name)) {
            groups[nameGroup].push_front(users[name]);
            res << sender << "Se a unido al grupo exitosamente" << "server";
            send(res);
        } else {
            cout << "Group name does not exist" << endl;
            res << sender <<"Group name does not exist" << "server";
            send(res);
        }
        
    }



     bool isGroup(const string &groupName){
        if(groups.count(groupName))return true;
        else return false;
     }

     bool isGroup_name(string& name) {
        string groupName;
        for(unordered_map<string, list <User>>::iterator i = groups.begin(); i != groups.end(); i++){
            for(auto& j : i->second){
                if(j.userName() == name) return true;
            }
        }
        return false;
        
    }

    string nameGroup(const string& name) {
        for (unordered_map<string, list<User>>::iterator it = groups.begin();
             it != groups.end(); ++it) {
            for (const auto& user : it->second) {
                // if (user.userName() != it->first) {
                if(user.userName() == name)
                  return it->first;
                
            }
        }
    }

    /*int number_of_groups(const string &name){
      int count;
      for(unordered_map<string, list <User>>::iterator i = groups.begin(); i != groups.end(); i++){
            for(auto& j : i->second){
                if(j.userName() == name) count ++;
            }
        }
        return count;
    }*/
    void sendGroupMessage(const string& dest, const string& nombre,
                          const string& text, const string &sender) {
        for (const auto& user : groups[dest]) {

            if (user.identity() != sender) {
                message m;
                cout << "Enviando a :" << user.userName() << " " << endl;
                m << user.identity() << "in Group " << dest << " :" << text << nombre;
                send(m);
            }
        }
    }
  void send_voice(message &m){
      string name;
      m >> name;
      size_t sampleCount;
      m >> sampleCount;

      size_t sampleRate;
      m >> sampleRate;

      size_t sampleChannelCount;
      m >> sampleChannelCount;

      const int16_t* sample;
      m >> sample;  

      string userName;
      m >> userName;

      message res;

      res << users[name].identity() << "voice" << sampleCount << sampleRate << sampleChannelCount;
      res.add_raw(sample,sampleCount*sizeof(int16_t));
      res << userName;

    }
};

void sendMessage(message &msg, const string dest, const string &sender, ServerState &server){

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


/*void sendMessage_group(const string &name, const string text, ServerState &server){
  server.sendMessage_group(name,text);
}*/


void dispatch(message &msg, ServerState &server) {
  if(msg.parts() > 1){
  
      string sender;
      msg >> sender;

      string action;
      msg >> action;

      cout <<"action " << action << endl;

  
      bool estado = server.conectado(action);
      cout << "estado :" << estado << endl;

      if (action == "login") {

        login(msg, sender, server);

      } else if(server.conectado(action) == true && server.exist(action)== true){


        sendMessage(msg, action, sender, server);

      } else if(action == "singUp"){

        singUp(msg, sender, server);

      } else if(action == "newUser" ){

        string name;
        msg >> name;
        string password;
        msg >> password;

        server.newUser(name,password,sender);

      }else if(action == "voice" && msg.parts() == 8){

        server.send_voice(msg);

      } else if(action == "newGroup"){

        string name_group;
        msg >> name_group;
        string name_creater;
        msg >> name_creater;

        server.newGroup(name_creater, name_group, sender);


      } else if(action == "addToGroup"){
          string nameGroup;
          msg >> nameGroup;

          string user;
          msg >> user;
          server.addToGroup(nameGroup, user, sender);
          cout << "El usuario " << user << " A accedido al grupo : " << nameGroup << endl;

       } else if(msg.parts() >=2){
            string aux;
            string text= action + " ";
            for(int i = 0; i < msg.parts()-3; i++){
                msg >> aux;
                text += aux + " ";
            }

            string name;
            cout << "text : " << text << endl;
            msg >> name;
            if(server.isGroup(action)){
                
                server.sendGroupMessage(action, name, text, sender);

          }else if (server.isGroup_name(name)){
              
                
                string name_group= server.nameGroup(name);
              
                cout << "NOMBRE : " << name << endl;
                server.sendGroupMessage(name_group, name, text, sender);

        } else {
            cerr << "Action not supported/implemented" << endl;
              message reply;
              reply << sender << " unsupported " << action;
              server.send(reply);
        }
    }
  }else{
      string sender;
      msg >> sender;

      message m;
      m << sender << "operacion invalida";
      server.send(m);

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
