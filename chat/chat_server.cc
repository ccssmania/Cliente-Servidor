//#include "json.hpp"
#include <SFML/Audio.hpp>
#include <cassert>
#include <iostream>
#include <sstream>
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

  const string &identity() const { return netId; }

  const string &userName() const { return name; }

  bool state() { return connected; }
};

class ServerState {
 private:
  // Connected users
  unordered_map<string, User> users;
  unordered_map<string, list<User>> groups;
  socket &sckt;

 public:
  // serverState() {}
  ServerState(socket &s) : sckt(s) {}

  void send(message &m) { sckt.send(m); }

  void newUser(const string &name, const string &pwd, const string &id) {
    // TODO: check that the user does not exist.
    users[name] = User(name, pwd, id);
    // users[id] = User(name, pwd, id);
  }
  bool login(const string &name, const string &pwd, const string &id) {
    if (users.count(name) > 0) {
      // User is registered
      bool ok = users[name].isPassword(pwd);
      if (ok) users[name].connect(id);
      return ok;
    }
    return false;
  }

  bool exist(const string &nombre) {
    if (users[nombre].userName() == nombre)
      return true;
    else
      false;
  }

  bool conectado(string &name) {
    if (users[name].state() == true) return true;
  }

  void sendMessage(const string &dest, const string &text,
                   const string &sender) {
    message m;
    // users[dest].identity;
    cout << "NOMBRE :" << dest << endl;
    m << users[dest].identity() << text << sender;

    send(m);
  }

  void newGroup(const string &creator, const string &name,
                const string &sender) {
    if (groups.count(name)) {
      cerr << "Group " << name << " is already created" << endl;
      message res;
      res << sender << "El grupo ya fue creado"
          << "server";
      send(res);
    } else {
      message res;
      groups[name].push_front(users[creator]);
      cout << "Group created" << endl;
      res << sender << "el grupo de nombre :" << name << "fue creado"
          << "server";
      send(res);
    }
  }

  void addToGroup(string &nameGroup, string &name, const string &sender) {
    message res;
    if (!groups.count(name)) {
      groups[nameGroup].push_front(users[name]);
      res << sender << "Se a unido al grupo exitosamente"
          << "server";
      send(res);
    } else {
      cout << "Group name does not exist" << endl;
      res << sender << "Group name does not exist"
          << "server";
      send(res);
    }
  }

  bool isGroup(const string &groupName) {  // BUSCA GRUPO POR NOMBRE DE GRUPO
    if (groups.count(groupName))
      return true;
    else
      return false;
  }

  bool isGroup_name(
      const string &name) {  // BUSCA UN GRUPO POR NOMBRE DEL USUARIO
    string groupName;
    for (unordered_map<string, list<User>>::iterator i = groups.begin();
         i != groups.end(); i++) {
      for (auto &j : i->second) {
        if (j.userName() == name) return true;
      }
    }
    return false;
  }

  void exit_group(const string &name_sender, const string &name_group,
                  const string &sender) {
    if (isGroup(name_group)) {
      list<User>::iterator user;
      for (user = groups[name_group].begin(); user != groups[name_group].end();
           user++) {
        if ((*user).userName() == name_sender) {
          groups[name_group].erase(user);
          message res;
          res << sender << "se ha salido del grupo : " << name_group
              << "server";
          send(res);
          break;
        }
      }
    }
  }

  string nameGroup(const string &name) {  // RETORNA EL NOMBRE DEL GRUPO AL CUAL
                                          // EL USUARIO PERTENECE
    for (unordered_map<string, list<User>>::iterator it = groups.begin();
         it != groups.end(); ++it) {
      for (const auto &user : it->second) {
        // if (user.userName() != it->first) {
        if (user.userName() == name) return it->first;
      }
    }
  }

  /*int number_of_groups(const string &name){
    int count;
    for(unordered_map<string, list <User>>::iterator i = groups.begin(); i !=
  groups.end(); i++){
          for(auto& j : i->second){
              if(j.userName() == name) count ++;
          }
      }
      return count;
  }*/
  void sendCallGroup(const string &dest, const string &sender) {
    for (const auto &user : groups[dest]) {
      if (user.identity() != sender) {
        message m;
        m << user.identity() << "call_group" << dest;
        send(m);
      }
    }
  }
  void sendGroupMessage(const string &dest, const string &nombre,
                        const string &text, const string &sender) {
    for (const auto &user : groups[dest]) {
      if (user.identity() != sender) {
        message m;
        cout << "Enviando a :" << user.userName() << " " << endl;
        m << user.identity() << "in Group " << dest << " :" << text << nombre;
        send(m);
      }
    }
  }
  void send_voice(message &m, const string &voice_to, const string &action) {
    size_t sampleCount;
    m >> sampleCount;

    size_t sampleRate;
    m >> sampleRate;

    size_t sampleChannelCount;
    m >> sampleChannelCount;

    const int16_t *sample;
    m >> sample;

    int tiempo;
    m >> tiempo;

    string userName;
    m >> userName;

    message res;

    res << users[voice_to].identity() << action << sampleCount << sampleRate
        << sampleChannelCount;
    res.add_raw(sample, sampleCount * sizeof(int16_t));
    res << tiempo << userName;
    send(res);
  }

  void send_voiceGroup(message &m, const string &voice_to,
                       const string &sender) {
    size_t sampleCount;
    m >> sampleCount;

    size_t sampleRate;
    m >> sampleRate;

    size_t sampleChannelCount;
    m >> sampleChannelCount;

    const int16_t *sample;
    m >> sample;

    int tiempo;
    m >> tiempo;

    string userName;
    m >> userName;
    for (const auto &user : groups[voice_to]) {
      if (user.identity() != sender) {
        message res;

        res << user.identity() << "voiceG" << voice_to << sampleCount
            << sampleRate << sampleChannelCount;
        res.add_raw(sample, sampleCount * sizeof(int16_t));
        res << tiempo << userName;
        cout << "Enviando a :" << user.userName() << " " << endl;

        // m << user.identity() << "in Group " << dest << " :" << text <<
        // nombre;
        send(res);
      }
    }
  }
};

void sendMessage(message &msg, const string dest, const string &sender,
                 ServerState &server) {
  string text;
  string aux;
  for (int i = 0; i < msg.parts() - 3; i++) {
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
    message m;
    m << sender << "El usurio : " << userName << "se a unido al servidor"
      << "server";
    server.send(m);

  } else {
    cerr << "Wrong user/password " << endl;
  }
}

void singUp(message &msg, const string &sender, ServerState &server) {
  string userName;
  msg >> userName;
  string password;
  msg >> password;
  server.newUser(userName, password, sender);
}

/*void sendMessage_group(const string &name, const string text, ServerState
&server){
  server.sendMessage_group(name,text);
}*/

void dispatch(message &msg, ServerState &server) {
  if (msg.parts() > 1) {
    string sender;
    msg >> sender;

    string action;
    msg >> action;

    cout << "action " << action << endl;

    if (action == "login") {
      login(msg, sender, server);

    } else if (server.conectado(action) == true &&
               server.exist(action) == true) {
      sendMessage(msg, action, sender, server);

    } else if (action == "newUser") {
      string name;
      msg >> name;
      if (server.exist(name) == false) {
        string password;
        msg >> password;

        server.newUser(name, password, sender);
      } else {
        message res;
        res << sender << "El usuario ya existe"
            << "server";
        server.send(res);
      }

    } else if (action == "voice" && msg.parts() == 9) {
      string voice_to;
      msg >> voice_to;
      if (server.conectado(voice_to) == true &&
          server.exist(voice_to) == true) {
        server.send_voice(msg, voice_to, action);

      } else if (server.isGroup(voice_to)) {
        server.send_voiceGroup(msg, voice_to, sender);
      } else {
        message err;
        err << sender << "  el usurio no existe o no esta conectado "
            << "server";
        server.send(err);
      }

    } else if (action == "voicec" && msg.parts() == 9) {
      string voice_to;
      msg >> voice_to;
      if (server.conectado(voice_to) == true &&
          server.exist(voice_to) == true) {
        server.send_voice(msg, voice_to, action);

      } else if (server.isGroup(voice_to)) {
        server.send_voiceGroup(msg, voice_to, sender);
      } else {
        message err;
        err << sender << "  el usurio no existe o no esta conectado "
            << "server";
        server.send(err);
      }
    } else if (action == "newGroup" && msg.parts() == 4) {
      string name_group;
      msg >> name_group;
      string name_creater;
      msg >> name_creater;
      server.newGroup(name_creater, name_group, sender);

    } else if (action == "addToGroup" && msg.parts() == 4) {
      string nameGroup;
      msg >> nameGroup;
      string user;
      msg >> user;
      server.addToGroup(nameGroup, user, sender);
      cout << "El usuario " << user << " A accedido al grupo : " << nameGroup
           << endl;

    } else if (action == "call" && msg.parts() == 4) {
      string dest;
      msg >> dest;
      cout << "call" << endl;
      if (server.conectado(dest) == true && server.exist(dest) == true) {
        string name_sender;
        msg >> name_sender;
        if (name_sender != dest) {
          server.sendMessage(dest, action, name_sender);
        } else {
          message res;
          res << sender << "no te puedes llamar a ti mismo "
              << "server";
          server.send(res);
        }
      } else if (server.isGroup(dest)) {
        cout << endl;
        string name_sender;
        msg >> name_sender;

        if (server.isGroup_name(name_sender)) {
          server.sendCallGroup(dest, sender);
        } else {
          message res;
          res << sender << "el usuario no pertenece al grupo " << dest
              << "server";
          server.send(res);
        }
      }
    } else if (action == "stop" && msg.parts() == 4) {
      string dest;
      msg >> dest;

      string name_sender;
      msg >> name_sender;

      server.sendMessage(dest, action, name_sender);

    } else if (action == "salir" && msg.parts() == 4) {
      string name_sender;
      string name_group;

      msg >> name_group;
      msg >> name_sender;
      server.exit_group(name_sender, name_group, sender);

    } else if (msg.parts() >= 2) {
      string aux;
      string text = action + " ";
      for (int i = 0; i < msg.parts() - 3; i++) {
        msg >> aux;
        text += aux + " ";
      }

      string name;
      cout << "text : " << text << endl;
      msg >> name;
      if (server.isGroup(action)) {
        server.sendGroupMessage(action, name, text, sender);

      } else if (server.isGroup_name(name)) {
        string name_group = server.nameGroup(name);

        cout << "NOMBRE : " << name << endl;
        server.sendGroupMessage(name_group, name, text, sender);

      } else {
        cerr << "Action not supported/implemented" << endl;
        message reply;
        reply << sender << " unsupported " << action << "Server";
        server.send(reply);
      }
    }
  } else {
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
  state.newUser("Cristian", "123", "");

  while (true) {
    message req;
    s.receive(req);

    dispatch(req, state);
  }
  cout << "Finished." << endl;
}
