#include <time.h>
#include <iostream>
#include <thread>
#include <utility>
#include <vector>

#include <bits/stdc++.h>

using namespace std;

void show_matrix(vector<vector<int> > &m1) {
  for (int i = 0; i < m1[0].size(); i++) {
    for (int j = 0; j < m1.size(); j++) {
      cout << m1[j][i] << " ";
    }
    cout << endl;
  }
}

void mult_matrix_profe(const vector<vector<int> > &m, const vector<int> &coll,
                       vector<int> &res, int &estado) {
  for (int i = 0; i < coll.size(); i++) {
    cout << coll[i] << " " << endl;
  }
  cout << endl;

  for (int i = 0; i < m[0].size(); i++) {
    int sum = 0;
    for (int j = 0; j < coll.size(); j++) {
      sum += m[j][i] * coll[j];
    }
    res[i] = sum;
  }
  estado = 1;
}
void dijtra_matrix_profe(const vector<vector<int> > &m, const vector<int> &coll,
                         vector<int> &res, int &estado) {
  for (int i = 0; i < coll.size(); i++) {
    cout << coll[i] << " " << endl;
  }
  cout << endl;
  for (int k = 0; k < size.)
    for (int i = 0; i < m[0].size(); i++) {
      int mn = std::numeric_limits<int>::max();
      for (int j = 0; j < coll.size(); j++) {
        mn = min(mn, m[j][i] + coll[j]);
      }
      // cout << "hola" << endl;
      res[i] = mn;
    }
  estado = 1;
}

int main() {
  vector<thread *> hilo(thread::hardware_concurrency());
  std::vector<int> estado(thread::hardware_concurrency(), 1);
  cout << "thread::hardware_concurrency() " << thread::hardware_concurrency()
       << endl;

  ifstream fin;
  fin.open("archivo.txt", ios::in);

  int rows, colls;
  fin >> rows >> colls;
  cout << rows << " " << colls << endl;
  vector<vector<int> > m1(colls, vector<int>(rows));
  cout << "m1.sizw " << m1.size() << endl << "m1[0 " << m1[0].size() << endl;
  for (int i = 0; i < m1[0].size(); i++) {
    for (int j = 0; j < m1.size(); j++) {
      int aux;
      fin >> aux;
      m1[j][i] = aux;
    }
  }
  show_matrix(m1);
  cout << endl;

  int filas, columnas;
  fin >> filas >> columnas;
  vector<vector<int> > m2(columnas, vector<int>(filas));
  vector<vector<int> > res(columnas, vector<int>(rows));
  vector<vector<int> > res2(colls, vector<int>(rows));
  for (int i = 0; i < m2[0].size(); i++) {
    for (int j = 0; j < m2.size(); j++) {
      int aux;
      fin >> aux;
      m2[j][i] = aux;
    }
  }

  show_matrix(m2);
  cout << endl;

  clock_t t;
  t = clock();
  int i = 0;
  int count = 0;
  while (true) {
    if (i == thread::hardware_concurrency()) i = 0;

    if (estado[i] == 1) {
      hilo[i] = new thread(dijtra_matrix_profe, cref(m1), cref(m1[count]),
                           ref(res2[count]), ref(estado[i]));
      estado[i] = 0;
      count++;
      i++;
    }

    if (count == columnas) break;
  }
  for (int i = 0; i < hilo.size(); i++) {
    hilo[i]->join();
  }
  t = clock() - t;
  show_matrix(res2);

  cout << "tiempo :" << ((double)t) / CLOCKS_PER_SEC << endl;
}