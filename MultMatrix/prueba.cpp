#include <bits/stdc++.h>

#include <algorithm>
#include <atomic>
#include <chrono>
#include <condition_variable>
#include <fstream>
#include <iostream>
#include <mutex>
#include <queue>
#include <thread>
#include <vector>
// g++ mult_matrix.cpp -std=c++11 -o mult -pthread

using namespace std;
using namespace std::chrono;
//
// template <typename T>

class join_threads {
  std::vector<std::thread> &threads;

 public:
  explicit join_threads(std::vector<std::thread> &threads_)
      : threads(threads_) {}
  ~join_threads() {
    // std::cerr << "destructing joiner\n";
    for (unsigned long i = 0; i < threads.size(); ++i) {
      if (threads[i].joinable()) threads[i].join();
    }
  }
};

template <typename T>
class threadsafe_queue {
 private:
  mutable std::mutex mut;
  std::queue<T> data_queue;
  std::condition_variable data_cond;

 public:
  threadsafe_queue() {}
  void push(T data) {
    std::lock_guard<std::mutex> lk(mut);
    data_queue.push(std::move(data));
    data_cond.notify_one();
  }
  void wait_and_pop(T &value) {
    std::unique_lock<std::mutex> lk(mut);
    data_cond.wait(lk, [this] { return !data_queue.empty(); });
    value = std::move(data_queue.front());
    data_queue.pop();
  }
  std::shared_ptr<T> wait_and_pop() {
    std::unique_lock<std::mutex> lk(mut);
    data_cond.wait(lk, [this] { return !data_queue.empty(); });
    std::shared_ptr<T> res(std::make_shared<T>(std::move(data_queue.front())));

    data_queue.pop();
    return res;
  }
  bool try_pop(T &value) {
    std::lock_guard<std::mutex> lk(mut);
    if (data_queue.empty()) return false;
    value = std::move(data_queue.front());
    data_queue.pop();
    return true;
  }
  std::shared_ptr<T> try_pop() {
    std::lock_guard<std::mutex> lk(mut);
    if (data_queue.empty()) return std::shared_ptr<T>();
    std::shared_ptr<T> res(std::make_shared<T>(std::move(data_queue.front())));
    data_queue.pop();
    return res;
  }
  bool empty() const {
    std::lock_guard<std::mutex> lk(mut);
    return data_queue.empty();
  }
};

class thread_pool {
  std::atomic_bool done;
  threadsafe_queue<std::function<void()>> work_queue;
  std::vector<std::thread> threads;
  join_threads *joiner;
  void worker_thread() {
    while (!done && !work_queue.empty()) {
      std::function<void()> task;
      if (work_queue.try_pop(task)) {
        task();
      } else {
        std::this_thread::yield();
      }
    }
  }

 public:
  thread_pool() : done(false), joiner(new join_threads(threads)) {
    // joiner(new join_threads(threads));
    unsigned const thread_count = std::thread::hardware_concurrency();
    try {
      for (unsigned i = 0; i < thread_count; ++i) {
        threads.push_back(std::thread(&thread_pool::worker_thread, this));
      }
    } catch (...) {
      done = true;
      throw;
    }
  }
  ~thread_pool() {
    joiner->~join_threads();
    done = true;
    // std::string s("Destructing pool ");
    // s += std::to_string(work_queue.empty());
    // s += '\n';
    // std::cerr << s;
  }
  template <typename FunctionType>
  void submit(FunctionType f) {
    work_queue.push(std::function<void()>(f));
    //    std::cerr << std::this_thread::get_id() << std::endl;
  }
};

template <class T>

class SparseMatrix {
 private:
  int rows;
  int cols;
  int count;
  int countrows;
  vector<T> val;
  vector<int> colInd;
  vector<int> rowPtr;

 public:
  SparseMatrix(int r, int c)
      : rows(r), cols(c), rowPtr(r + 1, 1), count(0), countrows(0) {}
  SparseMatrix(int r, int c, int aristas)
      : rows(r),
        cols(c),
        rowPtr(r + 1, 1),
        count(0),
        countrows(0),
        val(aristas) {}
  const int &getNumCols() const { return this->cols; }
  const int &getNumRows() const { return this->rows; }

  T get(int r, int c) const {
    if (r == 0)
      for (int i = 0; i < rowPtr[r + 1]; i++) {
        if (c == colInd[i]) {
          return val[0 + i];
          break;
        }
      }
    else if (r > 0) {
      // cout << "r : " << r << " c: " << c << endl;
      for (int i = rowPtr[r]; i < rowPtr[r + 1]; i++) {
        if (c == colInd[i]) {
          // cout << " i: " << i << endl;
          return val[i];
          break;
        }
      }
    }
  }
  void set(T valor, int r, int c, bool newRow) {
    val.push_back(valor);
    colInd.push_back(c);
    if (newRow == true) {
      rowPtr[countrows] = count;
      countrows++;
    }
    count++;
    rowPtr[rowPtr.size() - 1] = count;
  }
  SparseMatrix<T> mult(SparseMatrix<T> &b) {
    bool salto = true;
    SparseMatrix<T> res(this->rows, b.getNumCols());
    if (cols == b.getNumRows()) {
      for (int i = 0; i < this->rowPtr.size() - 1; i++) {
        for (int j = 0; j < b.getNumCols(); j++) {
          int aux = 0;
          for (int m = rowPtr[i]; m < rowPtr[i + 1]; m++) {
            aux += this->get(i, this->colInd[m]) * b.get(indiceCol(m), j);
          }
          if (aux != 0) {
            res.set(aux, i, j, salto);
            salto = false;
          }
        }
        salto = true;
      }

      return res;
    } else
      cout << " la multiplicacion no se puede hacer" << endl;
  }
  const int &indiceCol(int a) const { return colInd[a]; }
  void show() {
    for (int i = 0; i < rowPtr.size(); i++) {
      cout << rowPtr[i] << " ";
    }
    cout << endl;
  }
  void showval() {
    for (int i = 0; i < val.size(); i++) {
      cout << val[i] << " ";
    }
    cout << endl;
  }
  const void showcol() const {
    for (int i = 0; i < colInd.size(); i++) {
      cout << colInd[i] << " ";
    }
    cout << endl;
  }

  void clear() {
    val.clear();
    rowPtr.clear();
    colInd.clear();
  }
  const vector<T> &getRowPtr() const { return rowPtr; }
  const vector<T> &getColInd() const { return colInd; }
};

template <class T>
void showmatrix(const SparseMatrix<T> &m) {
  for (int i = 0; i < m.getNumRows(); i++) {
    for (int j = 0; j < m.getNumCols(); j++) {
      cout << m.get(i, j) << " ";
    }
    cout << endl;
  }
}
void dijtra_matrix_profe(const SparseMatrix<int> &m, const vector<int> &rowP,
                         const SparseMatrix<int> &b, int referencia,
                         SparseMatrix<int> &res) {
  // cout << "inicio" << endl;
  bool salto = true;
  for (int i = 0; i < b.getNumCols(); i++) {
    int sum = std::numeric_limits<int>::max();
    for (int j = rowP[referencia]; j < rowP[referencia + 1]; j++) {
      sum = min(sum,
                (m.get(referencia, m.indiceCol(j)) + b.get(m.indiceCol(j), i)));
    }
    res.set(sum, referencia, i, salto);
    salto = false;
  }
}
SparseMatrix<int> dijtra_reduce(const SparseMatrix<int> &m,
                                const SparseMatrix<int> &m2) {
  vector<int> rowP = m.getRowPtr();
  SparseMatrix<int> res(m.getNumRows(), m.getNumCols());
  int count = 0;
  while (true) {
    dijtra_matrix_profe(m, rowP, m2, count, res);

    count++;
    if (count == m.getNumCols()) break;
  }
  return res;
}
SparseMatrix<int> dijtra_blocks(const SparseMatrix<int> &m) {
  // vector<thread *> hilo(thread::hardware_concurrency());
  vector<SparseMatrix<int>> m2(
      m.getNumCols(), SparseMatrix<int>(m.getNumRows(), m.getNumCols()));
  m2[0] = m;
  SparseMatrix<int> res(m.getNumRows(), m.getNumCols());
  // // // showmatrix(m);
  cout << endl;
  vector<int> rowP = m.getRowPtr();
  int j = 0;
  int n = m.getNumCols() - 1;
  while (n > 1) {
    cout << "n " << n << endl;
    if (n % 2 == 0) {
      n = n / 2;
      res = dijtra_reduce(m, m2[j]);
      // cout << "iter :" << j + 1 << endl;
      m2[j + 1] = res;
      j++;
    } else {
      res = dijtra_reduce(m, dijtra_reduce(m, m2[j]));
      n = (n - 1) / 2;
      // cout << "iter :" << j + 1 << endl;
      m2[j + 1] = res;
      j++;
    }
  }
  return res;
}

SparseMatrix<int> subMatrix(const SparseMatrix<int> &m, int initrows,
                            int initcolls, int endrows, int endcolls) {
  SparseMatrix<int> res(m.getNumRows() / 2, m.getNumCols() / 2);
  bool salto = true;
  vector<int> col = m.getColInd();
  vector<int> row = m.getRowPtr();
  for (int i = initrows; i < endrows; i++) {
    // cout << "i " << i << endl;
    int indice = 0;
    for (int j = row[initrows];
         j < row[initrows + 1] && indice < res.getNumCols(); j++) {
      if (col[j] < endcolls && col[j] >= initcolls) {
        res.set(m.get(i, col[j]), (i - initrows), indice, salto);
        salto = false;
        indice++;
      }
    }
    salto = true;
  }
  // showmatrix(res);
  return res;
}
SparseMatrix<int> min_matrix(const SparseMatrix<int> &a,
                             const SparseMatrix<int> &b) {
  // cout << endl << "a " << endl;
  // // showmatrix(a);
  // cout << endl << "b " << endl;
  // // showmatrix(b);

  SparseMatrix<int> res(a.getNumRows(), a.getNumCols());
  bool salto = true;
  for (int i = 0; i < a.getNumRows(); i++) {
    for (int j = 0; j < b.getNumCols(); j++) {
      if (a.get(i, j) == 0 && b.get(i, j) != 0) {
        res.set(b.get(i, j), i, j, salto);
        salto = false;
      } else if (a.get(i, j) != 0 && b.get(i, j) == 0) {
        res.set(a.get(i, j), i, j, salto);
        salto = false;
      } else if (a.get(i, j) != 0 && b.get(i, j) != 0) {
        res.set(min(a.get(i, j), b.get(i, j)), i, j, salto);
        salto = false;
      }
    }
    salto = true;
  }
  return res;
}

void reconstruction(SparseMatrix<int> &res, const int &indFila,
                    const int &indice, const SparseMatrix<int> &m1,
                    const SparseMatrix<int> &m2) {
  bool salto = true;

  for (int i = 0; i < m1.getNumRows(); i++) {
    if (m1.get(indFila, i) != 0) {
      res.set(m1.get(indFila, i), indice, i, salto);
      salto = false;
    }
  }

  for (int i = 0; i < m2.getNumRows(); i++) {
    if (m2.get(indFila, i) != 0) {
      res.set(m2.get(indFila, i), indice, i + m2.getNumRows(), salto);
    }
  }
}

SparseMatrix<int> mult_blocks(const SparseMatrix<int> &m1,
                              const SparseMatrix<int> &m2) {
  // cout << "filas : " << m1.getNumRows() << endl
  //    << " columnas " << m1.getNumCols() << endl;
  if (m1.getNumCols() > 2) {
    SparseMatrix<int> a0 =
        subMatrix(m1, 0, 0, m1.getNumCols() / 2, m1.getNumCols() / 2);
    // // showmatrix(a0);
    // cout << endl;
    SparseMatrix<int> a1 = subMatrix(m1, 0, m1.getNumCols() / 2,
                                     m1.getNumCols() / 2, m1.getNumCols());
    // // showmatrix(a1);
    // cout << endl;
    SparseMatrix<int> a2 = subMatrix(m1, m1.getNumCols() / 2, 0,
                                     m1.getNumCols(), m1.getNumCols() / 2);

    // // showmatrix(a2);
    // cout << endl;
    SparseMatrix<int> a3 =
        subMatrix(m1, m1.getNumCols() / 2, m1.getNumCols() / 2, m1.getNumCols(),
                  m1.getNumCols());
    // // showmatrix(a3);
    // cout << endl;

    SparseMatrix<int> b0 =
        subMatrix(m2, 0, 0, m2.getNumCols() / 2, m2.getNumCols() / 2);
    // // showmatrix(b0);
    SparseMatrix<int> b1 = subMatrix(m2, 0, m2.getNumCols() / 2,
                                     m2.getNumCols() / 2, m2.getNumCols());
    // // showmatrix(b1);
    SparseMatrix<int> b2 = subMatrix(m2, m2.getNumCols() / 2, 0,
                                     m2.getNumCols(), m2.getNumCols() / 2);

    // // showmatrix(b2);
    SparseMatrix<int> b3 =
        subMatrix(m2, m2.getNumCols() / 2, m2.getNumCols() / 2, m2.getNumCols(),
                  m2.getNumCols());
    // // showmatrix(b3);
    // cout << "hola" << endl;
    SparseMatrix<int> r0 = min_matrix(mult_blocks(a0, b0), mult_blocks(a1, b2));
    SparseMatrix<int> r1 = min_matrix(mult_blocks(a0, b1), mult_blocks(a1, b3));
    SparseMatrix<int> r2 = min_matrix(mult_blocks(a2, b0), mult_blocks(a3, b2));
    SparseMatrix<int> r3 = min_matrix(mult_blocks(a2, b1), mult_blocks(a3, b3));

    // cout << " r0 " << endl;
    // showmatrix(r0);
    // cout << endl;
    // cout << " r1 " << endl;
    // showmatrix(r1);
    // cout << endl;
    // cout << " r2 " << endl;
    // showmatrix(r2);
    // cout << endl;
    // cout << " r3 " << endl;
    // showmatrix(r3);
    // cout << endl;

    int j = 0;
    SparseMatrix<int> res(m1.getNumRows(), m2.getNumCols());
    for (int i = 0; i < res.getNumRows() / 2; i++) {
      reconstruction(res, i, j, r0, r1);
      j++;
    }
    for (int i = 0; i < res.getNumRows() / 2; i++) {
      reconstruction(res, i, j, r2, r3);
      j++;
    }

    // cout << "res :" << endl;
    // // showmatrix(res);
    return res;
  } else
    return dijtra_reduce(m1, m2);
}

int main() {
  // vector<thread *> hilo(thread::hardware_concurrency());
  std::vector<int> estado(thread::hardware_concurrency(), 1);
  // cout << "thread::hardware_concurrency() " << thread::hardware_concurrency()
  //   << endl;

  ifstream fin;
  fin.open("sorted.txt", ios::in);

  int rows, colls;
  fin >> rows >> colls;
  int aux;
  bool salto;
  int i = 0;
  int num;

  int powRow = pow(2, ceil(log2(double(rows))));
  cout << "pow " << powRow << endl;
  if (rows != powRow) {
    rows = powRow;
  }
  SparseMatrix<int> m1(rows, rows);
  while (!fin.eof()) {
    string a;
    int j;
    int val;
    aux = i - 1;
    fin >> a >> i >> j >> val;
    if (aux == 0 || aux < i - 1) {
      salto = true;
    }
    m1.set(val, i - 1, j - 1, salto);
    salto = false;
  }

  // SparseMatrix<int> m1(rows, colls);

  /*for (int i = 0; i < m1.getNumRows(); i++) {
    for (int j = 0; j < m1.getNumCols(); j++) {
      int aux;
      fin >> aux;
      // cout << aux << endl;
      if (aux != 0) {
        m1.set(aux, i, j, salto);
        salto = false;
      }
    }
    salto = true;
  }*/
  // cout << endl;
  // showmatrix(m1);
  // cout << endl;

  high_resolution_clock::time_point t1 = high_resolution_clock::now();
  int n = m1.getNumCols() - 1;
  int j = 0;
  while (n > 1) {
    cout << "n " << n << endl;
    if (n % 2 == 0) {
      n = n / 2;
      SparseMatrix<int> res = mult_blocks(m1, m1);
      j++;
    } else {
      SparseMatrix<int> res = mult_blocks(m1, mult_blocks(m1, m1));
      n = (n - 1) / 2;
      j++;
    }
  }
  // // showmatrix(res);
  high_resolution_clock::time_point t2 = high_resolution_clock::now();

  auto duration = duration_cast<microseconds>(t2 - t1).count();

  cout << "tiempo dijtra : " << duration << endl;

  /*for (int i = 0; i < hilo.size(); i++) {
    delete hilo[i];
  }*/
}