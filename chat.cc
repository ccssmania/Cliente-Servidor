#include <cassert>
#include <cstdlib>
#include <iostream>
#include <string>
#include <vector>
#include <sstream>
#include <zmqpp/zmqpp.hpp>
#include <thread>
#include <SFML/Audio.hpp>
using namespace std;
using namespace zmqpp;
using namespace sf;


vector<string> tokenize(string &input) {
  stringstream ss(input);
  vector<string> result;
  string s;
  while (ss >> s)
    result.push_back(s);
  return result;
}

void voice(vector< string > &tokens, socket &s, string userName){
  // first check if an input audio device is available on the system
  if (!sf::SoundBufferRecorder::isAvailable())
  {
     cout << "no se puede grabar sin microfono" << endl;
  }
  SoundBufferRecorder recorder;
  cout << " CUANDO QUIERA PARAR DE GRABAR ESCRIBA STOP" << endl;
  string tiempo;
  // create the recorder

  // start the capture
  recorder.start();

  cin >> tiempo;
  if(tiempo == "stop"){

    // stop the capture
    recorder.stop();

    // retrieve the buffer that contains the captured audio data
    const sf::SoundBuffer& buffer = recorder.getBuffer();
    const Int16 *sample = buffer.getSamples();
    size_t count = buffer.getSampleCount();
    size_t rate = buffer.getSampleRate();
    size_t channelCount = buffer.getChannelCount();
    message m;
    m << tokens[0] << tokens[1] << count << rate << channelCount;
    m.add_raw(sample, count * sizeof(sf::Int16));
    m << userName;
    s.send(m);
  }
  else recorder.stop();
}

void play_voice(message &m, socket &s, Sound &sound){
  size_t sampleCount;
  m >> sampleCount;

  size_t sampleRate;
  m >> sampleRate;

  size_t sampleChannelCount;
  m >> sampleChannelCount;

  const Int16 *sample;
  m >> sample;
  /////////////////////////////////////////
  string name;
  m >> name;
  cout << "voice from: " << name << endl; 
  SoundBuffer buffer;
  buffer.loadFromSamples(sample, sampleCount, sampleChannelCount, sampleRate);
  sound.setBuffer(buffer);
  sound.play();
  sleep(milliseconds(5000));
}

/*void record(bool &t, string id_from, socket &sckt){
  while (!t){
    start
    sleep(10000);
    stop
    send

  }
}*/

int main(int argc, char const *argv[]) {
  if (argc != 4) {
    cerr << "Invalid arguments" << endl;
    return EXIT_FAILURE;
  }

  Sound sound; //objeto de tipo Sound
  
  string address(argv[1]);
  string userName(argv[2]);
  string password(argv[3]);
  string sckt("tcp://");
  sckt += address;
  context ctx;
  socket s(ctx, socket_type::xrequest);

  cout << "Connecting to: " << sckt << endl;
  s.connect(sckt);

  //cout << "hola";

  message login;
  login << "login" << userName << password;
  s.send(login);

  int console = fileno(stdin);
  poller poll;
  poll.add(s, poller::poll_in);
  poll.add(console, poller::poll_in);
  while (true) {
    if (poll.poll()) { // There are events in at least one of the sockets
      if (poll.has_input(s)) {
        // Handle input in socket
        message m;
        s.receive(m);
        vector < string > v;
        string text;
        string aux;
        string name;
        for(int i = 0; i < m.parts() - 1; i++){
          m >> aux;
          v.push_back(aux);
          if(v[0] == "voice") break;
          text += aux + " ";
        }
        
        //v.push_back(name);
        //cout << name << " asfasdfa " << m.parts() << endl;
        if(v[0] == "voice"){
          play_voice(m,s, sound);
        }
        else
          m >> name;
          cout << name <<" say : " << text << endl;
      }
      if (poll.has_input(console)) {
        // Handle input from console
        string input;
        getline(cin, input);
        vector<string> tokens = tokenize(input);
        if(tokens[0] == "voice"){
          voice(tokens,s,userName);
        }
        else{
          message m;
          for (const string &str : tokens)
            m << str;
          m << userName;
          s.send(m);
        }
      }
    }
  }
  return EXIT_SUCCESS;
}