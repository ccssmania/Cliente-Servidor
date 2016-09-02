#include <cmath>
#include <iostream>
#include <string>
#include <sstream>
#include <vector>
#include <zmqpp/zmqpp.hpp>

using namespace std;
using namespace zmqpp;



//----------------Convertir el string en una matriz de enteros-----------------
vector< vector< int > > convert(int row, int col, string m){

  vector< vector< int > > mat;
  int aux = 0;                                    //el auxiliar nos permite castiar los strings para convertirlos en enteros
  stringstream ss;                               //buffer que nos permite convertir facilmente de string a entero
  ss << m;
  for(int i = 0; i < row; i++){
    vector<int> x;                                //vector  auxiliar para llenar la matriz
      for(int j = 0; j < col; j++){
          ss >> aux;                              //se convierte los dijitos de la matriz de string a entero
          x.push_back(aux);                        //se agrega el dato tipo entero al vector auxiliar
    }
    mat.push_back(x);                          //se agrega el vector auxiliar a la matriz
  }
  //---------------------MOSTRAR LAS MATRICES--------------
cout << endl << "MATRIZ :" << endl;

for(int i = 0; i < row; i++){
  for(int j = 0; j < col; j++){
    cout << mat[i][j] << " ";
  }
  cout <<endl;
}

  return mat;
} 
//------------------------------funcion que convieerte strings en una matriz de doubles
vector< vector< double > > convertdouble(int row, int col, string m){

  vector< vector< double > > mat;
  double aux = 0;                                    //el auxiliar nos permite castiar los strings para convertirlos en enteros
  stringstream ss;                               //buffer que nos permite convertir facilmente de string a entero
  ss << m;
  for(int i = 0; i < row; i++){
    vector<double> x;                                //vector  auxiliar para llenar la matriz
      for(int j = 0; j < col; j++){
          ss >> aux;                              //se convierte los dijitos de la matriz de string a entero
          x.push_back(aux);                        //se agrega el dato tipo entero al vector auxiliar
    }
    mat.push_back(x);                          //se agrega el vector auxiliar a la matriz
  }
  //---------------------MOSTRAR LAS MATRICES--------------
cout << endl << "MATRIZ :" << endl;

for(int i = 0; i < row; i++){
  for(int j = 0; j < col; j++){
    cout << mat[i][j] << " ";
  }
  cout <<endl;
}

  return mat;
} 


//-------------------FUNCION DEL DETERMINANTE DE UNA MATRIZ-----------ALGORITMO SACADO DEL LIBRO Introduction to algorithms

pair< vector< vector<double> >, vector<vector<double> > > lu_descomposition(vector< vector<double> >* mat){
  vector< vector<double> > x = *mat;
  int n = x.size();
  vector< vector<double> > L;
  vector< vector<double> > U; 
  for(int i = 0 ; i < n;  i++){
    vector< double > auxiliar_L,auxiliar_U;
    for(int j = 0; j < n; j++){
      if(i == j){
        auxiliar_L.push_back(1);
        auxiliar_U.push_back(1);
      }
      else if(i > j){
        auxiliar_L.push_back(0);
        auxiliar_U.push_back(0);
      }
      else if(i < j){
        auxiliar_U.push_back(0);
        auxiliar_L.push_back(0);
      }

    }
    L.push_back(auxiliar_L);
    U.push_back(auxiliar_U);
  }
  for(int k = 1; k <= n; k++){
    U[k-1][k-1] = x[k-1][k-1];
    for(int i = k+1; i <= n; i++){
      L[i-1][k-1] = (x[i-1][k-1])/(U[k-1][k-1]);
      U[k-1][i-1] = x[k-1][i-1];
    }
    for(int i = k+1; i <= n; i++){
      for(int j= k+1; j <= n; j++){
        x[i-1][j-1] = x[i-1][j-1] - L[i-1][k-1]*U[k-1][j-1];
      }
    }
  }
  return make_pair(L,U);
  
}


//----------------FUNCION DE MULTIPLICACION DE MATRICES---------------------
vector< vector< int >> multm(int op1, int op2, int op3, int op4, string m, string m1){ //retorna un vector de vectores, reciviendo las matrices como strings y los ops como las filas y columnas respectivamente
  vector<vector<int>> matrix1 = convert(op1,op2,m);                                                         //Se crean las matrices a para multiplicar
  vector<vector<int>> matrix2 = convert(op3,op4,m1);
  vector<vector<int>> result;
  
  cout << "op1: " << op1 << endl << "op2: "<< op2 << endl << "op3: " << op3 << endl << "op4: " << op4 << endl;

//------------------------------------------------------------------------------------



//-----------------------------------------------------------------------

//----------------MULTIPLICACION DE MATRICES-------------------
  for(int i = 0; i < op1; i++){
    vector<int> x;
      for (int j = 0; j < op4; j++)
      {
        int aux = 0;
        for(int k = 0; k < op2; k++){
          aux += matrix1[i][k] * matrix2[k][j];
        }
        x.push_back(aux);
      }
    result.push_back(x);
  }
  cout << endl;
  return result;
}
  

//--------------------MAIN----------------------

int main(int argc, char *argv[]) {
  const string endpoint = "tcp://*:4242";

  // initialize the 0MQ context
  context ctx;

  // generate a pull socket
  socket s(ctx, socket_type::reply);

  // bind to the socket
  cout << "Binding to " << endpoint << "..." << endl;
  s.bind(endpoint);

  // receive the message
  while (true) {                               
    cout << "Receiving message..." << endl;
    message req;
    s.receive(req);                         //recive el mensaje del cliente
//---------INICIALIZAR---------------------------
    string op;
    int op1 = 0;
    int op2 = 0;
    int op3 = 0;                //SE INICIALIZAN LOS VALORES QUE PODRIAN LLEGAR DEL CLIENTE
    int op4 = 0;
    string m;
    string m1;
    req >> op >> op1;
    vector< vector< int > > res; // matriz de enteros
    vector< vector< double > > resd;  // matriz de doubles
    int result;
//----------------------------------------

//-------POSIBLES OPERACIONES O PETICIONES DEL CLIENTE---
    if (op == "sqrt") {
      result = sqrt(op1);
    }
    if (op == "ln") {
      result = log(op1);
    }
    if (op == "add") {
      req >> op2;
      result = op1 + op2;
    }
    if (op == "sub") {
      req >> op2;
      result = op1 - op2;
    }
    if (op == "mult") {
      req >> op2;
      result = op1 * op2;
    }
    if (op == "div") {
      req >> op2;
      result = op1 / op2;
    }
//-----------DETERMINANTE--------------------
    if(op == "det"){

      req >> op2 >> m;
      resd = convertdouble(op1,op2,m);
      vector< vector<double> > L,U;
      pair< vector< vector<double> > , vector< vector<double> > > p;
      p = lu_descomposition(&resd);
      L = p.first;
      U = p.second;
      int n = L.size();
      double deter = 1.0;
      for(int i = 0; i < n; i++ ){
        deter*=L[i][i]*U[i][i];
      }
      cout << deter << endl;
      result = (int)deter;
    }
//-----------MULTIPLICACION DE MATRICES-------
    if (op == "mulM"){                      
     req >> op2 >> op3 >> op4 >> m >> m1;
     //cout << m << "         " << m1 << endl;
     res = multm(op1,op2,op3,op4,m,m1);     // se llama la funcion de multiplicación de matrices
     
     cout << endl << "MATRIZ FINAL" << endl;
//--------------SE IMPRIME LA MATRIZ RESULTANTE DE LA MULTIPLICACION--------------
    for(int i = 0; i < op1; i++){             
      for(int j = 0; j < op4; j++){
        cout << res[i][j] << " ";
      }
      cout << endl; 
    }
  }
//------------------------------------------------------------------------------------
    if(op != "mulM"){
      message rep;
      rep << result;
      s.send(rep);
      cout << "Sent: " << result << endl;
    }
//------------SI LA OPERACION FUE MULTIPLICACION--------
    else{                    
      message rep;
      string a;
      for(int i = 0; i < op1; i++){
        for(int j= 0; j < op4; j++){

            a += to_string(res[i][j]) + " ";   //se convierte a cadena de strings
          
        }
        a += "\n";
      }
      rep << a;                                 // se envia la cadena que va a ser el resultado de la multiplicacion
      s.send(rep);
    }


  cout << "Finished." << endl;
}
}