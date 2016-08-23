#include <iostream>
#include <string>
#include <zmqpp/zmqpp.hpp>
#include <cmath>
#include <sstream>
#include <bits/stdc++.h>

using namespace std;
using namespace zmqpp;

int main(int argc, char *argv[]) {
  const string endpoint = "tcp://localhost:4242";
  /*if (argc != 3 && argc != 4 && argc != 8) {
    cerr << "Error calling the program" << endl;
    return 1;
  }*/
  // initialize the 0MQ context
  context ctx;
  socket s(ctx, socket_type::request);

  // open the connection
  cout << "Opening connection to " << endpoint << "..." << endl;
  s.connect(endpoint);
  // send a message
  cout << "Sending operation..." << endl;
  message req;

  string op;    //se declaran los posibles operandos
  int op1 = 0;
  int op2 = 0;
  int op3 = 0;
  int op4 = 0;

  ifstream fin;                    //variable tipo ifstream para la lectura y escritura de archivos
  fin.open("archivo.txt",ios::in); //abre el archivo texto
  fin >> op;                       // asigna a op lo primero del archivo

  /*if (argc == 3) {
    op1 = atoi(argv[2]);
    req << op << op1;
  }
  if (argc == 4) {
    op1 = atoi(argv[2]);
    op2 = atoi(argv[3]);
    req << op << op1 << op2;
  }*/
//-----------------Multiplicacion de matrices---------
  if (op == "mulM"){    // si op es mulM se crea los strings m y m1 en representacion de las matrices
    fin >> op1;         // op1, op2, op3 y op4 van a ser las filas y columnas de las matrices respectivamente
    fin >> op2;
    string m;
    string m1;

    for(int i = 0; i < op1*op2; i++){    //se convierten las matrices del archivo en strings
      string aux;
      fin >> aux;
      m += aux + " ";    
    }
    fin >> op3;
    fin >> op4;
    for(int i = 0; i < op3*op4; i++){
      string aux;
      fin >> aux;
      m1 += aux + " ";
    }
    
    if( op1 == op4) req << op << op1 << op2 << op3 << op4 << m << m1; // se guardan los valores en req que es de tipo message
    else {                                                            
      cout << " operadores invalidos " << endl; // solo si la matriz no se puede multiplicar
      return 0;
    }
  }
//--------------DETERMINANTE--------------

  if(op == "det"){
    fin >> op1;
    fin >> op2;
    string m;
    for(int i = 0; i < op1*op2; i++){    //se convierten las matrices del archivo en strings
      string aux;
      fin >> aux;
      m += aux + " ";    
    }

    if(op1 == op2) req << op << op1 << op2 << m;
    else {
      cout << "solo se saca el determinante de matrices cuadradas" << endl;
      return 0;
    }
  }
  s.send(req);                                  //se envian los datos al servidor
  cout << "Request sent." << endl;


//------------------------RESPUESTA DEL SERVIDOR
  if (op == "mulM"){ // si es de multiplicacion de matrices el resultado es tipo string
    string result;
    message rep;
    s.receive(rep);
    rep >> result;
    cout << "Response " << endl << result << endl;
    cout << "Finished." << endl;
  }
  else{
    message rep;        // se recive la respuesta del servidor tipo entero
    s.receive(rep);
    int result = 0;
    rep >> result;
    cout << "Response " << result << endl << endl;
    cout << "Finished." << endl;
    return 0;

  }
  fin.close();
}