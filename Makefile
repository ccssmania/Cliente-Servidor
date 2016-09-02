# export LD_LIBRARY_PATH=$LD_LIBRARY_PATH:/home/cristian/Descargas/zmqlib
ZMQ=/home/cristian/Descargas/zmqlib
CC=g++ -std=c++11 -I$(ZMQ)/include -L$(ZMQ)/lib

all: client server client_chat server_chat prueba

client: client.cc
	$(CC) -o client client.cc -lzmq -lzmqpp

server: server.cc
	$(CC) -o server server.cc -lzmq -lzmqpp

client_chat: chat.cc
	$(CC) -o chat chat.cc -lzmq -lzmqpp -lsfml-system -lsfml-audio -pthread	

server_chat: chat_server.cc
	$(CC) -o server_chat chat_server.cc -lzmq -lzmqpp -lsfml-system -lsfml-audio

prueba: prueba.cpp
	 $(CC) -o a prueba.cpp -lzmq -lzmqpp