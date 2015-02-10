/*
 * Copyright 2015 Alexandr Mansurov
 */

#include <map>
#include <vector>
#include <utility>
#include <set>
#include <memory>

#include "./edmonds.h"

using nsp::Result;
using nsp::MappingFinder;
using nsp::Graph;

Result
MappingFinder::find(Graph * graph_, Graph * mapping_, set_t * set, les_t * l)
throw(InconsistentStructureException) {
  // najdi volne vrcholy a vloz do f a do l na hladinu 0
  std::unique_ptr<queue_t> f = prepare(mapping_, l);

  // hlavni cyklus - dokud fronta neni prazdna
  queue_t::iterator i = (*f).begin();
  queue_t::iterator end_it = (*f).end();
  while (i < end_it) {
    int v = *i;
    (*f).pop_front();

    les_t::iterator x = (*l).find(v);
    if (x != (*l).end()) {
      int h = (*x).second.h;  // hladina v

      Neighbours n_v = (*graph_).getNeighbours(v);

      if (h%2 == 1) {
        Neighbours m_v = (*mapping_).getNeighbours(v);
        int y = *(m_v.begin());  // vrchol spojeny s v parovaci hranou
        if ((*l).find(y) != (*l).end()) {  // y in l
          // y nutne na hladine h
          // najdu VSC nebo Kytku -> konec
          return finder(v, y, set, l);
        } else {  // y not in L
          (*f).push_back(y);
          edge_t e(v, y);
          WRecord wr = WRecord(y, h+1, e);
          (*l).insert(pair_t(y, wr));
        }
      } else {
        std::set<int> y;
        // y mnozina vrcholu dosazitelnych z v po neparovaci hrane
        for (iter x = n_v.begin(); x < n_v.end(); x++) {
          // sousedi v
          if (!(*mapping_).neighbours(v, (*x))) {
            // nejsou sousedi v parovani
            y.insert(*x);
          }
        }

        les_t pom;
        for (std::set<int>::iterator w = y.begin(); w != y.end(); ++w) {
          // existuje vrchol v y, ktery patri do l a je na sude hladine?
          les_t::iterator z = (*l).find(*w);
          if (z != (*l).end()) {
            if ((*z).second.h%2 == 0) {
              // najdu VSC nebo kytku -> konec
              return finder((*z).second.v, v, set, l);
            }
            // jinak y patri do l, ale neni na sude hladine, jdeme dale
          } else {
            // x nepatri do l
            // pozor (*z) se nesmi pouzit
            WRecord rec(*w, h+1, edge_t(v, *w));
            pom.insert(pair_t(*w, rec));
          }
        }

        // pro kazdy vrchol y nepatrici do L : zarad y do fronty
        for (les_t::iterator y = pom.begin(); y != pom.end(); ++y) {
          (*f).push_back((*y).first);
          (*l).insert(pair_t((*y).second.v, (*y).second));
        }
      }
    } else {  // err?
      throw InconsistentStructureException();
    }
    i = (*f).begin();
    end_it = (*f).end();
  }
  return NONE;
}

int
MappingFinder::lookup_root(int vertex, set_t * set, const les_t * l) {
  // hledam koren ve strome (l)
  int v = vertex;
  les_t::const_iterator x = (*l).find(v);
  if (x != (*l).end()) {
    WRecord r = (*x).second;
    while (r.h != 0) {
      edge_t e = r.e;
      int buddy = (e.first == r.v) ? e.second : e.first;
      x = (*l).find(buddy);
      if (x != (*l).end()) {
        r = (*x).second;
        (*set).push_back(e);
      } else {
        return -1;
      }
    }
    return r.v;
  }
  return -1;
}

bool
MappingFinder::step(Graph * g, Graph * m) {
  std::unique_ptr<set_t> set (new set_t);
  std::unique_ptr<les_t> l (new les_t);
  try {
    Result x = find(g, m, set.get(), l.get());
    // find muze hodit ISExc. ale to jen znamena bug v programu

    switch (x) {
      case AP:
        // zlepsi M - otocit hrany v /vne parovani -
        augment(m, set.get());
        return true;
        break;
      case BLOSSOM:
        // zkontrahuj kvet
        blossom(g, m, set.get(), l.get());
        while (find(g, m, set.get(), l.get()) == AP) augment(m, set.get());
        return false;
        break;
      case NONE:
        // m je nejvetsi
        return false;
        break;
      default: throw UnexpectedResultException();
    }
  } catch(...) {  // univerzalni catch blok
    std::cout << "Fatal Error" << std::endl;
    return false;
  }
}

int
MappingFinder::cut(set_t * set) {
  typedef std::map<int, std::vector<int> > str_t;
  str_t mapa;
  set_t remove;
  std::set<int> stonek;
  for (set_t::iterator it = (*set).begin(); it != (*set).end(); ++it) {
    bool duplicit = false;
    str_t::iterator a = mapa.find((*it).first);
    if (a != mapa.end()) {
      // rozsiruji vektor, resp. hledam duplikat
      for (std::vector<int>::iterator jt = (*a).second.begin();
           jt != (*a).second.end(); ++jt) {
        if ((*jt) == (*it).second) {
          // nalezen duplikat
          duplicit = true;
          remove.push_back(edge_t((*it).first, (*it).second));
          stonek.insert((*it).first);
          stonek.insert((*it).second);
          (*a).second.erase(jt);
          break;  // iterator ve foru neplatny
        }
      }
      if (!duplicit) {  // insert
        (*a).second.push_back((*it).second);
      }
    } else {
      // novy vektor
      std::vector<int> v;
      v.push_back((*it).second);
      mapa.insert(std::pair<int, std::vector<int> >((*it).first, v));
    }
    duplicit = false;
    str_t::iterator b = mapa.find((*it).second);
    if (b != mapa.end()) {
      for (std::vector<int>::iterator jt = (*b).second.begin();
           jt != (*b).second.end(); ++jt) {
        if ((*jt) == (*it).first) {
          duplicit = true;
          (*b).second.erase(jt);
          break;
        }
      }
    } else {  // novy vektor
      std::vector<int> v;
      v.push_back((*it).first);
      mapa.insert(std::pair<int, std::vector<int> >((*it).second, v));
    }
  }

  std::unique_ptr<set_t> set2 (new set_t);
  int vrchol = -1;
  for (set_t::iterator it = (*set).begin(); it != (*set).end(); ++it) {
    bool toRemove = false;
    for (set_t::iterator jt = remove.begin(); jt != remove.end(); ++jt) {
      if (((*jt).first == (*it).first) && ((*jt).second == (*it).second)) {
        toRemove = true;
        break;
      }
    }
    if (!toRemove) {
      (*set2).push_back(edge_t((*it).first, (*it).second));
      if (stonek.find((*it).first) != stonek.end()) {
        vrchol = (*it).first;
      } else if (stonek.find((*it).second) != stonek.end()) {
        vrchol = (*it).second;
      }
    }
  }
  set = set2.get();  // copy unique_ptr, don't bother with deallocation
  return vrchol;
}

void
MappingFinder::shrink(Graph * graph_, Graph * mapping_, const set_t * set,
                      int v) {
  // zjistit, zda ex. hrana (v,v) v grafu (v parovani nebyla)
  bool b = (*graph_).neighbours(v, v);

  std::set<int> pom;
  pom.insert(v);
  // hrany kvetu
  for (set_t::const_iterator it = (*set).begin(); it != (*set).end(); ++it) {
    int a = (*it).first;
    int b = (*it).second;

    if (pom.find(a) == pom.end()) {
      pom.insert(a);
      Neighbours n_a = (*graph_).getNeighbours(a);
      for (iter it = n_a.begin(); it != n_a.end(); ++it) {
        if (!(*graph_).neighbours(v, (*it))) {
          if ((v != (*it)) && ((*it) != -1)) {
            int i = (*it);
            (*graph_).unsetEdge(a, i);
            (*graph_).setEdge(v, i);
          }
        }
      }
    }
    if (pom.find(b) == pom.end()) {
      pom.insert(b);
      Neighbours n_b = (*graph_).getNeighbours(b);
      for (iter it = n_b.begin(); it != n_b.end(); ++it) {
        if (!(*graph_).neighbours(v, (*it))) {
          if ((v != (*it)) && ((*it) != -1)) {
            int i = (*it);
            (*graph_).unsetEdge(b, i);
            (*mapping_).unsetEdge(b, i);
            (*graph_).setEdge(v, i);
          }
        }
      }
    }
  }
  for (std::set<int>::iterator it = pom.begin(); it != pom.end(); ++it) {
    (*graph_).unsetEdge(v, (*it));
    (*mapping_).unsetEdge(v, (*it));
  }
  for (set_t::const_iterator it = (*set).begin(); it != (*set).end(); it++) {
    if (((*it).first != v) && ((*it).second != v)) {
      (*graph_).unsetEdge((*it).first, (*it).second);
    }
    (*mapping_).unsetEdge((*it).first, (*it).second);
  }
  // pokud existovala hrana (v,v) nastavit
  if (b) {
    (*graph_).setEdge(v, v);
  }
}

void
MappingFinder::expand(const Graph * g_k_, Graph * m_k_, Graph * graph_,
                      Graph * mapping_, const set_t * set, int v) {
  // pokud M.K lze zlepsit - zlepsi M, konec
  std::cout << "Expand" << std::endl;
  mapping_ = m_k_;

  std::set<int> mn;
  for (set_t::const_iterator it = (*set).begin(); it != (*set).end(); ++it) {
    mn.insert((*it).first);
    mn.insert((*it).second);

    Neighbours a = (*mapping_).getNeighbours((*it).first);
    for (iter yt = a.begin(); yt != a.end(); ++yt) {
      (*mapping_).unsetEdge((*it).first, (*yt));
    }
    a = (*mapping_).getNeighbours((*it).second);
    for (iter yt = a.begin(); yt != a.end(); ++yt) {
      (*mapping_).unsetEdge((*it).second, (*yt));
    }
  }

  Neighbours n_v = (*m_k_).getNeighbours(v);
  int n = -1;
  for (iter it = n_v.begin(); it != n_v.end(); ++it) {
    n = (*it);  // nejvyse 1 soused v
    break;
  }
  if (n != -1) {  // existuje
    (*mapping_).setEdge(v, n);
    mn.erase(v);
    for (unsigned int i = 0; i < (mn.size() / 2); ++i) {
      std::set<int>::iterator j = mn.begin();
      int a = (*j);
      ++j;
      for (; j != mn.end(); ++j) {
        if ((*graph_).neighbours(a, (*j))) {
          (*mapping_).setEdge(a, (*j));
          mn.erase(a);
          mn.erase(*j);
          break;
        }
      }
    }
  }
}
