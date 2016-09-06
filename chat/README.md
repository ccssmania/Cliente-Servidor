# Cliente-Servidor



REQUIRED LIBRARIES

ZMQ - ZMQPP

SFML

EXECUTING

Type in your terminal:

  make

./server_chat
In other terminal:

  ./chat address
Solution to possible compiling error:

  export LD_LIBRARY_PATH=$LD_LIBRARY_PATH:/home/utp/zmq/lib
TYPES OF MESSAGES

Register

Register in chat.

  register username password
Example:

  newUser Carlos 123
  
Login:

Login username pasword
Example:

  login Gustavo 123

Send a message to a person

It is used to send a text message to a user.

  username message
  
Example:

  Cristian Hola, amigo

Add friend

It is used to add a friend to chat with.

  addFriend username

Create a group

It is used to create a group in the chat.

  newGroup groupName
Example:

  newGroup Family
  


It is used to add yourself in a group, this is required to chat in a group.

  addToGroup groupName
Example:

  addGroup Family
Send a message to a group

It is used to send a text message to a group.
if you stay in this group or in other group that you are created for to write in the last group, you just need write something example
Hola todos, como estan

but if you want to write to other group that you stay you need write the name of group and the message 
  <groupName <message>
Example:

  Family Hola familia. Los quiero mucho

Send a voice message to a person

It is used to send a voice message to a friend.

  voice friendName

Send a voice message to a group

voice groupName

It is used to start a call with a group.

  call groupName

When you want to leave a call from group.

  salir groupName
  
it is used to start a call with an other friend.
call FriendName

