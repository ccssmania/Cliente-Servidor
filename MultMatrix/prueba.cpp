#include <bits/stdc++.h>

#include <algorithm>
#include <atomic>
#include <boost/multi_index/member.hpp>
#include <boost/multi_index/ordered_index.hpp>
#include <boost/multi_index_container.hpp>
#include <chrono>
#include <condition_variable>
#include <fstream>
#include <iostream>
#include <mutex>
#include <queue>
#include <set>
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
  vector<map<int, T>> val;

 public:
  SparseMatrix() : rows(0), cols(0), val() {}
  SparseMatrix(int r, int c) : rows(r), cols(c), val(r) {}

  T get(int r, int c) const {
    auto &row = val[r];
    auto it = row.find(c);
    if (it != row.end()) {
      assert(it->second != 0);
      return it->second;
    }

    return T();
  }

  void set(T valor, int r, int c) {
    if (valor != 0) val[r][c] = valor;
  }

  const map<int, T> &operator[](int i) const { return val[i]; }
  int getsize(vector<pair<int, int>> &m) const { return m.size(); }
  int getNumRows() const { return rows; }
  int getNumCols() const { return cols; }
  int zeroRows() const {
    int i = 0;
    for (const auto &r : val) {
      if (r.empty()) i++;
    }
    return i;
  }
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
void dijtra_matrix_profe2(const map<int, int> &v, const SparseMatrix<int> &b,
                          int referencia, SparseMatrix<int> &res) {
  for (const pair<int, int> &e : v) {
    for (const pair<int, int> &f : b[e.first]) {
      if (res.get(referencia, f.first) == 0) {
        res.set(e.second + f.second, referencia, f.first);
      } else
        res.set(min(res.get(referencia, f.first), e.second + f.second),
                referencia, f.first);
    }
  }
}

SparseMatrix<int> dijtra_reduce(const SparseMatrix<int> &m,
                                const SparseMatrix<int> &m2) {
  SparseMatrix<int> res(m.getNumRows(), m2.getNumCols());
  if (m.zeroRows() == m.getNumRows()) return m2;
  if (m2.zeroRows() == m2.getNumRows()) return m;
  for (int i = 0; i < m.getNumCols() && !m[i].empty(); i++) {
    dijtra_matrix_profe2(m[i], m2, i, res);
  }
  return res;
}

SparseMatrix<int> dijtra_reduce_concurrent(const SparseMatrix<int> &m,
                                           const SparseMatrix<int> &m2) {
  SparseMatrix<int> res(m.getNumRows(), m2.getNumCols());
  thread_pool pool;
  int count = 0;
  if (m.zeroRows() == m.getNumRows()) return m2;
  if (m2.zeroRows() == m2.getNumRows()) return m;
  for (int i = 0; i < m.getNumCols() && !m[i].empty(); i++) {
    auto w = [&m, &m2, i, &res] { dijtra_matrix_profe2(m[i], m2, i, res); };
    pool.submit(w);
  }
  return res;
}

SparseMatrix<int> subMatrix(const SparseMatrix<int> &m, int initrows,
                            int initcolls, int endrows, int endcolls) {
  SparseMatrix<int> res(m.getNumRows() / 2, m.getNumCols() / 2);
  for (int i = initrows; i < endrows; i++) {
    for (const pair<int, int> &e : m[i]) {
      if (e.first >= endcolls) break;
      if (e.first >= initcolls) {
        res.set(e.second, (i - initrows), e.first - initcolls);
      }
    }
  }
  return res;
}

SparseMatrix<int> min_matrix(const SparseMatrix<int> &a,
                             const SparseMatrix<int> &b) {
  SparseMatrix<int> res(a.getNumRows(), a.getNumCols());

  for (int i = 0; i < a.getNumRows(); i++) {
    for (int j = 0; j < b.getNumCols(); j++) {
      if (a.get(i, j) == 0 && b.get(i, j) != 0) {
        res.set(b.get(i, j), i, j);

      } else if (a.get(i, j) != 0 && b.get(i, j) == 0) {
        res.set(a.get(i, j), i, j);

      } else if (a.get(i, j) != 0 && b.get(i, j) != 0) {
        res.set(min(a.get(i, j), b.get(i, j)), i, j);
      }
    }
  }
  return res;
}

void reconstruction(SparseMatrix<int> &res, const int &indFila,
                    const int &indice, const SparseMatrix<int> &m1,
                    const SparseMatrix<int> &m2) {
  for (int i = 0; i < m1.getNumRows(); i++) {
    if (m1.get(indFila, i) != 0) {
      res.set(m1.get(indFila, i), indice, i);
    }
  }

  for (int i = 0; i < m2.getNumRows(); i++) {
    if (m2.get(indFila, i) != 0) {
      res.set(m2.get(indFila, i), indice, i + m2.getNumRows());
    }
  }
}
int contador = 0;

SparseMatrix<int> mult_blocks(const SparseMatrix<int> &m1,
                              const SparseMatrix<int> &m2) {
  if (m1.getNumCols() > 4) {
    SparseMatrix<int> a0 =
        subMatrix(m1, 0, 0, m1.getNumCols() / 2, m1.getNumCols() / 2);
    // showmatrix(a0);
    SparseMatrix<int> a1 = subMatrix(m1, 0, m1.getNumCols() / 2,
                                     m1.getNumCols() / 2, m1.getNumCols());
    // showmatrix(a1);
    SparseMatrix<int> a2 = subMatrix(m1, m1.getNumCols() / 2, 0,
                                     m1.getNumCols(), m1.getNumCols() / 2);
    // showmatrix(a2);
    SparseMatrix<int> a3 =
        subMatrix(m1, m1.getNumCols() / 2, m1.getNumCols() / 2, m1.getNumCols(),
                  m1.getNumCols());

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
  } else {
    contador++;
    cout << contador << endl;
    return dijtra_reduce(m1, m2);
  }
}

int main() {
  // cout << "thread::hardware_concurrency() " << thread::hardware_concurrency()
  //   << endl;
  std::vector<int> estado(thread::hardware_concurrency(), 1);
  ifstream fin;
  fin.open("road.txt", ios::in);

  int rows, colls;
  fin >> rows >> colls;
  int aux;

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
    }
    m1.set(val, i - 1, j - 1);
  }
  /*for (int i = 0; i < rows; i++) {
    for (int j = 0; j < rows; j++) {
      int aux;
      fin >> aux;
      m1.set(aux, i, j);
    }
  }*/
  high_resolution_clock::time_point t1 = high_resolution_clock::now();
  int n = rows - 1;
  SparseMatrix<int> res;

  while (n > 1) {
    cout << "n " << n << endl;
    if (n % 2 == 0) {
      n = n / 2;
      res = dijtra_reduce_concurrent(m1, m1);

    } else {
      res = dijtra_reduce_concurrent(m1, dijtra_reduce_concurrent(m1, m1));
      n = (n - 1) / 2;
    }
  }

  high_resolution_clock::time_point t2 = high_resolution_clock::now();
  auto duration = duration_cast<microseconds>(t2 - t1).count();
  cout << "tiempo dijtra : " << duration / 1000 << endl;

  /*for (int i = 0; i < hilo.size(); i++) {
    delete hilo[i];
  }*/
}