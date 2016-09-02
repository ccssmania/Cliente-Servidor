#include <iostream>
#include <zmqpp/zmqpp.hpp>
#include <math.h>
#include <bits/stdc++.h>

using namespace std;
using namespace zmqpp;




int main(int argc, char const *argv[]) {
  if (argc != 2) {
    cerr << "Error calling the program" << endl;
    return 1;
  }


  if (argv[1]="producto")
  {

   int data;           // file contains an undermined number of integer values
   ifstream fin;     // declare stream variable name

   int rows1,cols1,rows2,cols2;

   fin.open("myfile.txt",ios::in);    // open file
       
   fin >> data;  
   rows1=data;      
   fin >> data; 
   cols1=data; 
   string  matrix1,matrix2;

   int size=cols1*rows1;
   
   int i=0;
                            
   while ( i<size )      //if not at end of file, continue reading numbers
   {
        //print numbers to screen
    int tem=10;

     fin >> data;
      
      //cout<<data;
      string temp=to_string(data);

      

     
     

     matrix1+=temp; //get next number from file
     matrix1+=',';//tokenize numbers
     i+=1;



   }
     i=0;
     fin >> data;
     rows2=data;
     fin >> data;
     cols2 = data;
     int size2=cols2*rows2;
     
     while (!fin.eof( ) && i<size2  )      //if not at end of file, continue reading numbers
   {
        //print numbers to screen
     fin >> data;
     string temp=to_string(data);

     
     matrix2+=temp; //get next number from file
     matrix2+=',';//tokenize numbers
     
     
     i+=1;

   }


     fin.close( );       //close file

  
  
    cout<<matrix1<<endl;
    cout<<matrix2<<endl;

 
  
   /*
   
  context ctx;
  socket s(ctx, socket_type::request);

  cout << "Opening connection" << endl;
  s.connect("tcp://localhost:4242");

  string op(argv[1]);
 // int op1 = atoi(argv[2]);
 // int op2 = atoi(argv[3]);
   
  message req;
  req << op << matrix1;
  s.send(req);
  cout << "Request sent." << endl;

  message rep;
  s.receive(rep);
  int result = 0;
  rep >> result;
  cout << "Response " << result << endl;

  cout << "Finished." << endl;

*/
  

   
  }



  return 0;
}


