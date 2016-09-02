#include <iostream>
#include <zmqpp/zmqpp.hpp>
#include <math.h>
#include <bits/stdc++.h>

using namespace std;
using namespace zmqpp;




int main(int argc, char const *argv[]) {
 

  

 string data;           // file contains an undermined number of integer values
 ifstream fin;     // declare stream variable name
 int a;
 fin.open("archivo.txt",ios::in);    // open file
     
 fin >> data;  
 //string a= to_string(data);      
 cout<< data << endl;
 fin >> a;
 cout<< a;


  return 0;
}

