#include <time.h>
#include <iostream>
#include <thread>
#include <utility>
#include <vector>

using namespace std;

void show_matrix(vector<vector<int> > &m1) {
  for (int i = 0; i < m1[i].size(); i++) {
    for (int j = 0; j < m1.size(); j++) {
      cout << m1[i][j] << " ";
    }
    cout << endl;
  }
}

void mult_matrix_profe(const vector<vector<int> > &m, const vector<int> &coll,
                       int colls, vector<int> &res, bool &estado) {
  vector<int> aux;
  for (int i = 0; i < colls; i++) {
    int sum = 0;
    for (int j = 0; j < coll.size(); j++) {
      sum += m[i][j] * coll[j];
      // cout << "sum : " << sum << endl;
    }
    // cout << "hola" << endl;
    res[i] = sum;
  }
  estado = true;
}
void dijtra_matrix_profe(const vector<vector<int> > &m, const vector<int> &coll,
                         int rows, vector<int> &res, bool *estado) {
  vector<int> aux;
  for (int i = 0; i < rows; i++) {
    int mn = std::numeric_limits<int>::max();
    for (int j = 0; j < coll.size(); j++) {
      mn = min(mn, m[i][j] + coll[j]);
      // cout << "sum : " << sum << endl;
    }
    // cout << "hola" << endl;
    res[i] = mn;
  }
  estado = true;
}
int main() {
  int size = 1000;
  vector<vector<int> > m1(size, vector<int>(size));
  vector<vector<int> > m2(size, vector<int>(size));
  vector<vector<int> > res(size, vector<int>(size));
  vector<thread *> hilo(hardware_concurrency());
  std::vector<bool *> estado(hardware_concurrency(), true);

  for (int i = 1; i <= size; i++) {
    for (int j = 1; j <= size; j++) {
      m1[i][j] = i + i;
    }
  }
  for (int i = 1; i <= size; i++) {
    for (int j = 1; j <= size; j++) {
      m2[i][j] = i + i;
    }
  }
  clock_t t;
  t = clock();
  int i = 0;
  int count = 0;
  while (true) {
    if (estado[i] == true) {
      hilo[i] = new thread(mult_matrix_profe, cref(m1), cref(m2[count]), size,
                           ref(res[count]), ref(*estado[i]));
      estado[i] = false;
    }
    if (i == 3) i = 0;
    if (count == size - 1) break;
    i++;
    count++;
  }
  for (int i = 0; i < hilo.size(); i++) {
    hilo[i]->join();
  }
  t = clock() - t;
  cout << "tiempo :" << ((double)t) / CLOCKS_PER_SEC << endl;
}