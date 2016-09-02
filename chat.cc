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



void send_voice(const SoundBuffer &buffer, vector<string> &tokens, socket &s, string userName, int  tiempo){

    const Int16 *sample = buffer.getSamples();
    size_t count = buffer.getSampleCount();
    size_t rate = buffer.getSampleRate();
    size_t channelCount = buffer.getChannelCount();
    message m;
    
    m << tokens[0] << tokens[1] << count << rate << channelCount;
    m.add_raw(sample, count * sizeof(sf::Int16));
    m << tiempo << userName;
    
    s.send(m);
}

void voice(vector< string > &tokens, socket &s, string userName, SoundBufferRecorder &recorder){
  // first check if an input audio device is available on the system
  if (!sf::SoundBufferRecorder::isAvailable())
  {
     cout << "no se puede grabar sin microfono" << endl;
  }
  cout << " CUANDO QUIERA PARAR DE GRABAR ESCRIBA STOP" << endl;
  string tiempo;
  // create the recorder

  // start the capture
  Clock clock;
  recorder.start();
  //Time time1= clock.getElapsedTime()

  cin >> tiempo;
  if(tiempo == "stop"){

    // stop the capture
    recorder.stop();
    Time time1 = clock.restart();

    // retrieve the buffer that contains the captured audio data
    const sf::SoundBuffer& buffer = recorder.getBuffer();
    send_voice(buffer, tokens, s, userName, time1.asMilliseconds());
  }
  else {
    recorder.stop();
    Time time1 =clock.restart();
    const sf::SoundBuffer& buffer = recorder.getBuffer();
    send_voice(buffer, tokens, s, userName, time1.asMilliseconds());
  }
}

void voice_call( socket &s, string userName, SoundBufferRecorder &recorder, bool &call_state, const string &name_sender){
if (!sf::SoundBufferRecorder::isAvailable())
  {
     cout << "no se puede grabar sin microfono" << endl;
  }
  while(call_state){
      
    vector<string > tokens;
    recorder.start();
    int tiempo = 500;
    sleep(milliseconds(tiempo));
    recorder.stop();
    tokens.push_back("voice");
    tokens.push_back(name_sender);
    //tokens.push_front("voice");
    const SoundBuffer& buffer = recorder.getBuffer();
    send_voice(buffer, tokens, s, userName, tiempo);

  }
}

SoundBuffer reconstruction(message &m){
  size_t sampleCount;
  m >> sampleCount;

  size_t sampleRate;
  m >> sampleRate;

  size_t sampleChannelCount;
  m >> sampleChannelCount;

  const Int16 *sample;
  m >> sample;
  /////////////////////////////////////////
  SoundBuffer buffer;
  buffer.loadFromSamples(sample, sampleCount, sampleChannelCount, sampleRate);
  return buffer; 
}

void play_sound_call(message &m, Sound &sound, bool &call_state){
  SoundBuffer buffer = reconstruction(m);
  int tiempo;
  m >> tiempo;
  string name;
  m >> name;
  cout << "voice from: "  << endl;
    sound.setBuffer(buffer);
    sound.play();
    sleep(milliseconds(500));
}



void play_voice(message &m, socket &s, Sound &sound){
  SoundBuffer buffer = reconstruction(m);
  int tiempo;
  m >> tiempo;
  string name;
  m >> name;
  cout << "voice from: " << name << endl;
  sound.setBuffer(buffer);
  sound.play();
  sleep(milliseconds(tiempo));
}

/*void record(bool &t, string id_from, socket &sckt){
  while (!t){
    start
    sleep(10000);
    stop
    send

  }
}*/
void server(message &m,socket &s,string &userName, bool &call_state, Sound &sound, SoundBufferRecorder &recorder){
  vector < string > v;
  string text;
  string aux;
  string name;
  thread *speak;
  v.clear();
  //cout << "msg parts " << m.parts()<< endl;
  for(int i = 0; i < m.parts() - 1; i++){
    m >> aux;
    v.push_back(aux);
    if(v[0] == "voice" || v[0] == "voiceG" || v[0] == "call" ) break;
    if(v[0] == "stop" && v[1] == "call"){call_state = false; break;}
    text += aux + " ";
  }
  
  //v.push_back(name);
  //cout << name << " asfasdfa " << m.parts() << endl;
  if(v[0] == "voice"){
    play_voice(m,s, sound);
  }
  else if(v[0] == "voiceG"){

    string group_name;
    m >> group_name;
    cout << "voice in group " << group_name << " ";
    play_voice(m,s, sound);

  }else if (v[0] == "call"){
    call_state = true;
    string name_sender;
    m >> name_sender;
    speak = new thread(voice_call,ref(s),ref(userName),ref(recorder),ref(call_state),ref(name_sender));
    //speak.join();

  } else if(v[0] == "stop" && v[1] == "call"){
      call_state = false;
      speak->join();

  } else{
    m >> name;
    cout << name <<" say : " << text << endl;
  }
}

void consola(vector< string > &tokens, socket &s, string &userName, Sound &sound, SoundBufferRecorder &recorder, bool &call_state){
  thread *speak2;
  if(tokens[0] == "voice" && tokens.size() == 2){
      voice(tokens,s,userName, recorder);
    }else if(tokens[0] == "call"){
      message m;
      m << "call" << tokens[1] << userName;
      s.send(m);
      call_state = true;
      speak2 = new thread(voice_call,ref(s),ref(userName),ref(recorder),ref(call_state),ref(tokens[1]));
      //speak2.join();
      cout << "call " << tokens[1] << endl;
    }else if(tokens[0] == "stop" && tokens[1] == "call"){
      call_state = false;
      speak2->join();
    }
    else{
      message m;

      for (const string &str : tokens)
        m << str;

      m << userName;
      s.send(m);
    } 
}


int main(int argc, char const *argv[]) {
  if (argc != 4) {
    cerr << "Invalid arguments" << endl;
    return EXIT_FAILURE;
  }

  Sound sound; //objeto de tipo Sound
  SoundBufferRecorder recorder;
  
  string address(argv[1]);
  string userName(argv[2]);
  string breakword(argv[3]);
  string sckt("tcp://");
  sckt += address;
  context ctx;
  socket s(ctx, socket_type::xrequest);

  cout << "Connecting to: " << sckt << endl;
  s.connect(sckt);


  message login;
  login << "login" << userName << breakword;
  s.send(login);
  bool call_state = false;
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
        server(m,s,userName,call_state, sound, recorder);
        
      }
      if (poll.has_input(console)) {
        // Handle input from console
        string input;
        getline(cin, input);
        if(input != ""){
          vector<string> tokens = tokenize(input);
          consola(tokens, s, userName, sound, recorder,call_state);
        }
      }
    }
  }
  return EXIT_SUCCESS;
}