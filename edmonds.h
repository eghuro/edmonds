/*
 * Copyright 2015 Alexandr Mansurov
 */
#ifndef EDMONDS_H_
#define EDMONDS_H_
#include <utility>
#include <vector>
#include <deque>
#include <iterator>
#include <memory>
#include <map>
#include <set>
#include<algorithm>
#include<iostream>
#include<stdexcept>

namespace nsp {
typedef std::pair<int, int> edge_t;
typedef std::vector<int>::iterator iter;

class InconsistentStructureException : public std::exception {};
class UnexpectedResultException : public std::exception {};

/**
 * Struktura pro predavani sousedu nejakeho vrcholu v grafu pres 2 iteratory
 */
struct Neighbours {
 public:
  Neighbours():nil(true) {
  std::vector<int> x;
  a = x.begin();
  b = x.end();
  }
  Neighbours(iter beg, iter end):nil(false), a(beg), b(end)  {}

  inline iter begin() const { return a; }
  inline iter end() const { return b; }
  inline int size() const { return b-a; }

  bool nil;
 private:
  iter a, b;
};

/**
 * Graf seznamem sousedu
 */
class Graph {
 public:
 /**
  * Pocet vrcholu
  * @return pocet vrcholu
  */
  inline int getVertices() const { return vertices; }

  /**
   * Pocet hran
   * @return pocet pouzitych hran
   */
  inline int getEdges() const { return e_used_; }

  /**
   * Vytvori graf o danem poctu vrcholu a hran, pro oba parametry existuje default
   * @param edges_l Pocet hran slouzi jako horni mez
   * @param vertices_l Pocet vrcholu
   */
  explicit Graph(int vertices_l = V_COUNT_DEF, int edges_l = E_COUNT_DEF):
    edges(edges_l), vertices(vertices_l), e_used_(0),
    neighbours_(vertices_l, std::vector<int>(edges_l, -1)) {}

  /**
   * Vytiskne graf na stdout
   */
  void print() const;

  /**
   * Po zavolani dane metody nebude mezi vrcholy vest hrana
   * @param x vrchol
   * @param y vrchol
   * @return zda byla odstranena hrana
   */
  bool unsetEdge(int x, int y);

  /**
   * Po zavolani dane metody budde mezi vrcholy vest hrana
   * @param x vrchol
   * @param y vrchol
   * @return zda byla nastavena hrana
   */
  bool setEdge(int x, int y);

  /**
   * Vrati seznam sousedu daneho vrcholu
   * @param v vrchol
   * @return struktura umoznuici pristup k sousedum
   */
  Neighbours getNeighbours(int v);

  /**
   * Metoda pro zjisteni, zda jsou dva vrcholy spojeny hranou
   * @param x vrchol
   * @param y vrchol
   * @return existuje v grafu hrana mezi x a y
   */
  bool neighbours(int x, int y) const;

  /**
   * Porovnani dvou grafu pro usnadneni testovani
   * Netestuje isomorfismus, ale zavisi na spravnem ocislovani vrcholu
   * (testuje zda vrchol i je spojen s vrcholem j pro vsechny pripustne dvojice)
   */
  bool operator == (const Graph&) const;

  /**
   * Vrati zda dany graf je parovani - pro usnadneni testovani
   */
  bool isMapping() const;

#ifdef DEBUG //NOLINT
 public: //NOLINT
#else //NOLINT
 private: //NOLINT
#endif //NOLINT
  static const int V_COUNT_DEF = 1;
  static const int E_COUNT_DEF = 1;

  int edges, vertices, e_used_;

  std::vector<std::vector<int> > neighbours_;

  inline bool
  legal_pair(int x, int y) const {
    return (x >= 0) && (x <= vertices) && (y >= 0) && (y <= vertices);
  }

  inline bool
  legal_edge(int x, int y) const {
    return (e_used_ < edges) && legal_pair(x, y);
  }
};

/**
 * Pomocna struktura pro stavbu lesa v BFS
 */
struct WRecord {
 public:
  int v, h;
  edge_t e;

  WRecord(int v_l, int h_l, edge_t e_l):v(v_l), h(h_l), e(e_l) {}

  WRecord(int v_l, int h_l):v(v_l), h(h_l) {}
};

/**
 * Mozne vysledky hledani - zlepsujici cesta, kytka, nic
 */
enum Result{
  AP, BLOSSOM, NONE
};

/**
 * Hledac parovani
 */
class MappingFinder{
#ifndef DEBUG //NOLINT
 private: //NOLINT
#endif //NOLINT
#ifdef DEBUG //NOLINT
 public: //NOLINT
#endif //NOLINT

  typedef std::map<int, WRecord> les_t;
  typedef std::deque<int> queue_t;
  typedef std::deque<edge_t> set_t;
  typedef std::pair<int, WRecord> pair_t;

  /**
   * Byla nalezena kytka:
   * urizni stonek, zkontrahuj kvet, pokud M.K lze zlepsit, zlepsi M, jinak M je nejlepsi
   */
  static inline bool
  blossom(Graph & graph_, Graph & mapping_, set_t & set, const les_t l) {
    Graph g_k = graph_;
    Graph m_k = mapping_;
    int v = cut(set);
    shrink(g_k, m_k, set, v);
    bool st = false;
    while ( step(g_k, m_k) ) st = true;
    if (st) {  // pokud M.K lze zlepsit - zlepsi M, konec
      expand(g_k, m_k, graph_, mapping_, set, v);
      return true;
    }
    // pokud M.K nejde zlepsit - M je nejlepsi
    return false;
  }

  /**
   * najdi volne vrcholy a vloz do f a do l na hladinu 0
   * volny vrchol - neni v zadne parovaci hrane
   */
  static inline queue_t
  prepare(Graph & mapping_, les_t & l) {
    queue_t f;
    for (int i = 0; i < mapping_.getVertices(); i++) {
      if (mapping_.getNeighbours(i).begin() ==
                                             mapping_.getNeighbours(i).end()) {
        // v parovani nema sousedy - izolovany vrchol - volny vrchol
        f.push_back(i);
        l.insert(pair_t(i, WRecord(i, 0)));
      }
    }

    return f;
  }

  /**
   * zkus najit VSC nebo kytku
   */
  static Result
  find(Graph & graph_, Graph & mapping_, set_t & set, les_t & l)
  throw(InconsistentStructureException);

  /**
   * Najdi koren ve strome
   */
  static int
  lookup_root(int vertex, set_t & set, const les_t & l);

  /**
   * Najdu VSC nebo kytku
   */
  static inline Result
  finder(int v, int y, set_t & set, const les_t & l) {
    // hledej z v cestu do korene k1
    int k1 = lookup_root(v, set, l);
    if (k1 == -1) throw "Error";
    // hledej v y cestu do korene k2
    int k2 = lookup_root(y, set, l);
    if (k2 == -1) throw "Error";
    // k1=k2 -> kytka, k1!=k2 -> VSC
    if (k1 == k2) {
      set.push_back(std::pair<int, int>(v, y));
      return BLOSSOM;
    } else {
      set.push_back(edge_t(v, y));
      return AP;
    }
  }

  /**
   * Zlepsi parovani podel cesty
   * jde o VSC, pro dvojice vrcholu prohazuji: hrana v/vne parovani
   */
  static inline void
  augment(Graph & mapping_, set_t & set) {
    if (!set.empty()) {
      while (!set.empty()) {
        edge_t e = set.front();
        int a = e.first;
        int b = e.second;
        set.pop_front();
        if (mapping_.neighbours(a, b)) {
          mapping_.unsetEdge(a, b);
        } else {
          mapping_.setEdge(a, b);
        }
        a = b;
      }
    }
  }

  /**
   * Kontrakce kvetu v set
   *
   */
  static void
  shrink(Graph & graph_, Graph & mapping_, const set_t & set, int v);

  /**
   * Urizni stonek kytky
   */
  static int cut(set_t & set);

  /**
   * rozvin g.k a m.k zpet do g, m
   */
  static void
  expand(const Graph & g_k_, Graph & m_k_, Graph & graph_, Graph & mapping_,
         const set_t & set, int v);

  /**
   * Jeden krok algoritmu
   * @param g graf
   * @param m stavajici parovani, ktere mame zlepsit
   * @return true - parovani bylo zlepseno; false parovani je nejlepsi
   */
  static bool step(Graph & g, Graph & m);

#ifndef DEBUG // NOLINT
public: // NOLINT
#endif
  /**
   * Najdi nejlepsi parovani v grafu
   */
  static inline Graph
  FindMaxMapping(Graph & g) {
    Graph m(g.getVertices(), g.getEdges());
    while (step(g, m)) continue;
    return m;
  }
};  // MappingFinder
};  // namespace nsp
#endif//EDMONDS_H_ NOLINT
