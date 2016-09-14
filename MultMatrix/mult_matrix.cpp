#include <time.h>
#include <chrono>
#include <future>
#include <iostream>
#include <thread>
#include <utility>
#include <vector>

#include <bits/stdc++.h>
using namespace std::chrono;
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

void multMatrix(vector<vector<int> > &m1, vector<vector<int> > &m2,
                vector<thread *> &hilo, vector<int> &estado) {
  vector<vector<int> > res(m1[0].size(), vector<int>(m2.size()));
  cout << "-------M1------------" << endl;
  show_matrix(m1);
  cout << endl << "----------M2---------" << endl;

  show_matrix(m2);
  cout << endl;
  cout << "----------" << endl;

  int i = 0;
  int count = 0;
  while (true) {
    if (i == thread::hardware_concurrency()) i = 0;

    if (estado[i] == 1) {
      hilo[i] = new thread(dijtra_matrix_profe, cref(m1), cref(m2[count]),
                           ref(res[count]), ref(estado[i]));
      estado[i] = 0;
      count++;
      i++;
    }

    if (count == m2.size()) break;
  }
  for (int i = 0; i < hilo.size(); i++) {
    hilo[i]->join();
  }
  show_matrix(res);
}

void dijtra(vector<vector<int> > m, vector<thread *> &hilo,
            vector<int> &estado) {
  vector<vector<int> > m2 = m;
  cout << "-------M-------" << endl;
  show_matrix(m);
  cout << endl;
  vector<vector<int> > res2(m.size(), vector<int>(m[0].size()));
  for (int j = 0; j < m.size() - 1; j++) {
    int i = 0;
    int count = 0;
    while (true) {
      if (i == thread::hardware_concurrency()) i = 0;

      if (estado[i] == 1) {
        hilo[i] = new thread(dijtra_matrix_profe, cref(m), cref(m2[count]),
                             ref(res2[count]), ref(estado[i]));
        estado[i] = 0;
        count++;
        i++;
      }

      if (count == m.size()) break;
    }
    for (int i = 0; i < hilo.size(); i++) {
      hilo[i]->join();
    }
    cout << "iter :" << j + 1 << endl;
    show_matrix(res2);
    m.clear();
    m = res2;
  }
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
  vector<vector<int> > m1(colls, vector<int>(rows));

  for (int i = 0; i < m1[0].size(); i++) {
    for (int j = 0; j < m1.size(); j++) {
      int aux;
      fin >> aux;
      m1[j][i] = aux;
    }
  }

  int filas, columnas;
  fin >> filas >> columnas;
  vector<vector<int> > m2(columnas, vector<int>(filas));

  for (int i = 0; i < m2[0].size(); i++) {
    for (int j = 0; j < m2.size(); j++) {
      int aux;
      fin >> aux;
      m2[j][i] = aux;
    }
  }

  high_resolution_clock::time_point t1 = high_resolution_clock::now();

  dijtra(m1, hilo, estado);
  high_resolution_clock::time_point t2 = high_resolution_clock::now();
  auto duration = duration_cast<microseconds>(t2 - t1).count();
  cout << "tiempo dijtra : " << duration << endl;
  cout << "-----------MULT-------------------" << endl << endl;
  high_resolution_clock::time_point t3 = high_resolution_clock::now();
  multMatrix(m1, m2, hilo, estado);
  high_resolution_clock::time_point t4 = high_resolution_clock::now();
  auto duration2 = duration_cast<microseconds>(t4 - t3).count();
  cout << "tiempo :" << duration2 << endl;

  for (int i = 0; i < hilo.size(); i++) {
    delete hilo[i];
  }
}