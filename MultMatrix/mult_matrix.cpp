#include <time.h>
#include <iostream>
#include <thread>
#include <vector>

using namespace std;

void show_matrix(vector<vector<long long int> > &m1) {
  for (int i = 0; i < m1[i].size(); i++) {
    for (int j = 0; j < m1.size(); j++) {
      cout << m1[i][j] << " ";
    }
    cout << endl;
  }
}

void mult_matrix_profe(vector<vector<int> > m, vector<int> coll, int rows,
                       vector<vector<long long int> > &res) {
  int sum = 0;
  vector<long long int> aux;
  for (int i = 0; i < rows; i++) {
    for (int j = 0; j < coll.size(); j++) {
      sum += m[i][j] * coll[j];
      // cout << "sum : " << sum << endl;
    }
    // cout << "hola" << endl;
    aux.push_back(sum);
  }
  res.push_back(aux);
}

int main() {
  vector<vector<int> > m1;
  vector<vector<int> > m2;
  vector<vector<long long int> > res;
  vector<thread *> hilo(10000);
  int rows, colls;

  for (int i = 1; i <= 1000; i++) {
    vector<int> v;
    for (int j = 1; j <= 1000; j++) {
      v.push_back(i + i);
    }
    m1.push_back(v);
  }
  for (int i = 1; i <= 1000; i++) {
    vector<int> v;
    for (int j = 1; j <= 1000; j++) {
      v.push_back(i + i);
    }
    m2.push_back(v);
  }
  clock_t t;
  t = clock();
  for (int i = 0; i < 1000; i++) {
    hilo[i] = new thread(mult_matrix_profe, m1, m2[i], 1000, ref(res));
  }
  for (int i = 0; i < 1000; i++) {
    hilo[i]->join();
  }
  t = clock() - t;
  cout << "tiempo :" << ((double)t) / CLOCKS_PER_SEC << endl;
}