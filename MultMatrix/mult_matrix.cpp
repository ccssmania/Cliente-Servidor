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

void show_matrix(vector<vector<int>> &m1) {
  for (int i = 0; i < m1[0].size(); i++) {
    for (int j = 0; j < m1.size(); j++) {
      cout << m1[j][i] << " ";
    }
    cout << endl;
  }
}

void mult_matrix_profe(const vector<vector<int>> &m, const vector<int> &coll,
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
void dijtra_matrix_profe(const vector<vector<int>> &m, const vector<int> &coll,
                         vector<int> &res, int &estado) {
  // cout << "inicio" << endl;
  for (int i = 0; i < m[0].size(); i++) {
    int mn = std::numeric_limits<int>::max();
    for (int j = 0; j < coll.size(); j++) {
      mn = min(mn, m[j][i] + coll[j]);
    }

    res[i] = mn;
  }
  // cout << "fin" << endl;
  estado = 1;
}

void multMatrix(vector<vector<int>> &m1, vector<vector<int>> &m2,
                vector<thread *> &hilo, vector<int> &estado) {
  vector<vector<int>> res(m1[0].size(), vector<int>(m2.size()));
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
      hilo[i] = new thread(mult_matrix_profe, cref(m1), cref(m2[count]),
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

void dijtra(vector<vector<int>> &m, vector<thread *> &hilo,
            vector<int> &estado) {
  vector<vector<int>> m2 = m;
  cout << "-------M-------" << endl;
  // show_matrix(m);
  cout << endl;
  vector<vector<int>> res2(m.size(), vector<int>(m[0].size()));
  for (int j = 0; j < m.size() - 1; j++) {
    int i = 0;
    int count = 0;
    // cout << "j: " << j << endl;
    while (true) {
      cout << count << endl;
      if (i == thread::hardware_concurrency()) i = 0;
      // cout << "count " << count << endl;
      // cout << " estado[ " << i << " ]" << estado[i] << endl;

      if (estado[i] == 1) {
        hilo[i] = new thread(dijtra_matrix_profe, cref(m), cref(m2[count]),
                             ref(res2[count]), ref(estado[i]));
        estado[i] = 0;
        count++;
        i++;
      } else if (i == 0 && estado[i + 1] == 1) {
        hilo[i + 1] = new thread(dijtra_matrix_profe, cref(m), cref(m2[count]),
                                 ref(res2[count]), ref(estado[i]));
        estado[i + 1] = 0;
        count++;
        i++;
      } else if (i == 1 && estado[i - 1] == 1) {
        hilo[i - 1] = new thread(dijtra_matrix_profe, cref(m), cref(m2[count]),
                                 ref(res2[count]), ref(estado[i]));
        estado[i - 1] = 0;
        count++;
        i++;
      }

      if (count == m.size()) break;
    }
    for (int i = 0; i < hilo.size(); i++) {
      // cout << "hola" << endl;

      hilo[i]->join();
    }
    cout << "iter :" << j + 1 << endl;
    // show_matrix(res2);
    m.clear();
    m = res2;
  }
}
vector<vector<int>> dijtra_blocks(vector<vector<int>> &m, int &estado) {
  vector<vector<int>> m2 = m;
  cout << "-------M-------" << endl;
  show_matrix(m);
  cout << endl;
  vector<vector<int>> res2(m.size(), vector<int>(m[0].size()));
  for (int j = 0; j < m.size() - 1; j++) {
    int count = 0;
    while (true) {
      dijtra_matrix_profe(m, m2[count], res2[count], estado);

      count++;

      if (count == m.size()) break;
    }

    cout << "iter :" << j + 1 << endl;
    show_matrix(res2);
    m.clear();
    m = res2;
  }
  return res2;
}

vector<vector<int>> subMatrix(vector<vector<int>> &m, int initcolls,
                              int initrows, int endcolls, int endrows) {
  vector<vector<int>> res((endcolls - initcolls),
                          vector<int>(endrows - initrows));

  for (int i = initcolls; i < endcolls; i++) {
    for (int j = initrows; j < endrows; j++) {
      res[j - initrows][i - initcolls] = m[j][i];
    }
  }
  return res;
}

void mult_blocks(vector<vector<int>> &m1, int &estado,
                 vector<vector<int>> &res) {
  if (m1.size() > 3) {
    vector<vector<vector<int>>> subm1(
        4, vector<vector<int>>(m1[0].size() / 2, vector<int>(m1.size() / 2)));

    subm1[0] = subMatrix(m1, 0, 0, m1[0].size() / 2, m1[0].size() / 2);
    show_matrix(subm1[0]);
    subm1[1] =
        subMatrix(m1, 0, m1[0].size() / 2, m1[0].size() / 2, m1[0].size());
    show_matrix(subm1[1]);
    subm1[2] =
        subMatrix(m1, m1[0].size() / 2, 0, m1[0].size(), m1[0].size() / 2);

    show_matrix(subm1[2]);
    subm1[3] = subMatrix(m1, m1[0].size() / 2, m1[0].size() / 2, m1[0].size(),
                         m1[0].size());

    show_matrix(subm1[3]);

    mult_blocks(subm1[0], estado, res);
    mult_blocks(subm1[1], estado, res);
    mult_blocks(subm1[2], estado, res);
    mult_blocks(subm1[3], estado, res);
  } else
    res = dijtra_blocks(m1, estado);

  estado = 1;
}

void multParalelo(vector<vector<int>> &m1, vector<thread *> &hilo,
                  vector<int> &estado) {
  vector<vector<vector<int>>> subm1(
      4, vector<vector<int>>(m1[0].size() / 2, vector<int>(m1.size() / 2)));
  vector<vector<vector<int>>> res(
      4, vector<vector<int>>(m1[0].size() / 2, vector<int>(m1.size() / 2)));
  subm1[0] = subMatrix(m1, 0, 0, m1[0].size() / 2, m1[0].size() / 2);
  show_matrix(subm1[0]);
  cout << endl;
  subm1[1] = subMatrix(m1, 0, m1[0].size() / 2, m1[0].size() / 2, m1[0].size());
  show_matrix(subm1[1]);
  cout << endl;
  subm1[2] = subMatrix(m1, m1[0].size() / 2, 0, m1[0].size(), m1[0].size() / 2);
  show_matrix(subm1[2]);
  cout << endl;
  subm1[3] = subMatrix(m1, m1[0].size() / 2, m1[0].size() / 2, m1[0].size(),
                       m1[0].size());
  show_matrix(subm1[3]);
  cout << endl;
  int i = 0;
  int count = 0;
  while (true) {
    if (i == thread::hardware_concurrency()) i = 0;

    if (estado[i] == 1) {
      hilo[i] = new thread(mult_blocks, ref(subm1[count]), ref(estado[i]),
                           ref(res[count]));
      estado[i] = 0;
      count++;
      i++;
    }

    if (count == 4) break;
  }
  for (int i = 0; i < hilo.size(); i++) {
    hilo[i]->join();
  }
  cout << "res " << endl;
  show_matrix(res[0]);
  cout << "res " << endl;
  show_matrix(res[1]);
  cout << "res " << endl;
  show_matrix(res[2]);
  cout << "res " << endl;
  show_matrix(res[3]);
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

  vector<vector<int>> m1(colls, vector<int>(rows));

  for (int i = 0; i < m1[0].size(); i++) {
    for (int j = 0; j < m1.size(); j++) {
      int aux;
      fin >> aux;
      m1[j][i] = aux;
    }
  }
  show_matrix(m1);

  /*int filas, columnas;
  fin >> filas >> columnas;
  vector<vector<int>> m2(columnas, vector<int>(filas));

  for (int i = 0; i < m2[0].size(); i++) {
    for (int j = 0; j < m2.size(); j++) {
      int aux;
      fin >> aux;
      m2[j][i] = aux;
    }
  }*/
  /*
  cout << "---DIJTRA---" << endl;
  for (int i = 0; i < hilo.size(); i++) {
    delete hilo[i];
  }*/
  high_resolution_clock::time_point t1 = high_resolution_clock::now();
  // vector<thread *> hilo2(thread::hardware_concurrency());
  multParalelo(m1, hilo, estado);
  high_resolution_clock::time_point t2 = high_resolution_clock::now();
  auto duration = duration_cast<microseconds>(t2 - t1).count();
  cout << "tiempo dijtra : " << duration << endl;
  /*high_resolution_clock::time_point t1 = high_resolution_clock::now();

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
*/
  for (int i = 0; i < hilo.size(); i++) {
    delete hilo[i];
  }
}