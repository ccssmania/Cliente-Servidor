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
  vector<vector<pair<int, T>>> val;

 public:
  SparseMatrix(int r, int c) : rows(r), cols(c), val(r) {}

  T get(int r, int c) const {
    for (int i = 0; i < val[r].size(); i++) {
      if (c == val[r][i].first) return val[r][i].second;
    }
  }
  const T binary_search(const vector<pair<int, int>> &v, int inicio, int fin,
                        int c) const {
    int m = ((inicio + fin) / 2);
    if (inicio > fin) return 0;
    if (v[m].first == c) {
      return v[m].second;
    } else if (v[m].first > c)
      return binary_search(v, inicio, m - 1, c);
    else
      return binary_search(v, m + 1, fin, c);
  }
  const T get2(int r, int c) const {
    int aux = this->binary_search(val[r], 0, val[r].size(), c);
    return aux;
  }
  void set(T valor, int r, int c) {
    val[r].push_back(std::make_pair(c, valor));
  }
  void sort() {
    for (int i = 0; i < val.size(); i++) {
      std::sort(val[i].begin(), val[i].end(),
                boost::bind(&std::pair<int, int>::first, _1) <
                    boost::bind(&std::pair<int, int>::first, _2));
    }
  }
  const vector<pair<int, int>> operator[](int i) const { return val[i]; }
  const pair<int, int> operator()(int &i, int &j) const { return val[i][j]; }
  const int &getsize(vector<pair<int, int>> &m) const { return m.size(); }
  const int &getNumRows() const { return rows; }
  const int &getNumCols() const { return cols; }
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
void dijtra_matrix_profe(const vector<pair<int, int>> &m,
                         const SparseMatrix<int> &b, int referencia,
                         SparseMatrix<int> &res) {
  // cout << "inicio" << endl;

  for (int i = 0; i < b.getNumRows(); i++) {
    int sum = std::numeric_limits<int>::max();
    for (int j = 0; j < b[i].size(); j++) {
      int aux = b.get2(m[j].first, i);
      if (aux == 0) {
        sum = min(sum, std::numeric_limits<int>::max());
      } else {
        sum = min(sum, (m[j].second + aux));
      }
    }
    res.set(sum, referencia, i);
  }
}
SparseMatrix<int> dijtra_reduce(const SparseMatrix<int> &m,
                                const SparseMatrix<int> &m2) {
  SparseMatrix<int> res(m.getNumRows(), m2.getNumCols());
  int count = 0;
  while (true) {
    high_resolution_clock::time_point t1 = high_resolution_clock::now();
    dijtra_matrix_profe(m[count], m2, count, res);

    count++;
    if (count == m.getNumCols()) break;
    high_resolution_clock::time_point t2 = high_resolution_clock::now();

    auto duration = duration_cast<microseconds>(t2 - t1).count();

    cout << "tiempo dijtra : " << duration / 1000 << endl;
  }

  return res;
}

SparseMatrix<int> dijtra_reduce_concurrent(const SparseMatrix<int> &m,
                                           const SparseMatrix<int> &m2) {
  SparseMatrix<int> res(m.getNumRows(), m2.getNumCols());
  thread_pool pool;
  int count = 0;
  while (true) {
    auto w = [&m, &m2, count, &res] {
      dijtra_matrix_profe(m[count], m2, count, res);
    };
    pool.submit(w);
    count++;
    if (count == m.getNumCols()) break;
  }
  return res;
}

SparseMatrix<int> subMatrix(const SparseMatrix<int> &m, int initrows,
                            int initcolls, int endrows, int endcolls) {
  SparseMatrix<int> res(m.getNumRows() / 2, m.getNumCols() / 2);

  for (int i = initrows; i < endrows; i++) {
    // cout << "i " << i << endl;
    int indice = 0;
    for (int j = initcolls; j < endcolls; j++) {
      res.set(m.get(i, j), (i - initrows), indice);

      indice++;
    }
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

SparseMatrix<int> mult_blocks(const SparseMatrix<int> &m1,
                              const SparseMatrix<int> &m2) {
  // cout << "filas : " << m1.getNumRows() << endl
  //    << " columnas " << m1.getNumCols() << endl;
  if (m1.getNumCols() > 512) {
    SparseMatrix<int> a0 =
        subMatrix(m1, 0, 0, m1.getNumCols() / 2, m1.getNumCols() / 2);
    showmatrix(a0);
    cout << endl;
    SparseMatrix<int> a1 = subMatrix(m1, 0, m1.getNumCols() / 2,
                                     m1.getNumCols() / 2, m1.getNumCols());
    showmatrix(a1);
    cout << endl;
    SparseMatrix<int> a2 = subMatrix(m1, m1.getNumCols() / 2, 0,
                                     m1.getNumCols(), m1.getNumCols() / 2);

    showmatrix(a2);
    cout << endl;
    SparseMatrix<int> a3 =
        subMatrix(m1, m1.getNumCols() / 2, m1.getNumCols() / 2, m1.getNumCols(),
                  m1.getNumCols());
    showmatrix(a3);
    cout << endl;

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

  // SparseMatrix<int> m1(rows, colls);

  /*for (int i = 0; i < m1.getNumRows(); i++) {
    for (int j = 0; j < m1.getNumCols(); j++) {
      int aux;
      fin >> aux;
      // cout << aux << endl;
      if (aux != 0) {
        m1.set(aux, i, j);
      }
    }
  }
  // cout << endl;
  showmatrix(m1);
  cout << endl;
*/
  high_resolution_clock::time_point t1 = high_resolution_clock::now();
  /*int n = m1.getNumCols() - 1;
  int j = 0;
  while (n > 1) {
    cout << "n " << n << endl;
    if (n % 2 == 0) {
      n = n / 2;
      SparseMatrix<int> res = mult_blocks(m1, m1);
      j++;
      cout << "res " << endl;
      showmatrix(res);
    } else {
      SparseMatrix<int> res = mult_blocks(m1, mult_blocks(m1, m1));
      n = (n - 1) / 2;
      j++;
      cout << "res " << endl;
      showmatrix(res);
    }
  }*/
  // showmatrix(res);
  // m1.sort();

  SparseMatrix<int> res = mult_blocks(m1, m1);
  high_resolution_clock::time_point t2 = high_resolution_clock::now();
  // m1.sort();
  auto duration = duration_cast<microseconds>(t2 - t1).count();

  cout << "tiempo dijtra : " << duration << endl;

  /*for (int i = 0; i < hilo.size(); i++) {
    delete hilo[i];
  }*/
}