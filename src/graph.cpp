/*
 * Copyright 2015 Alexandr Mansurov
 */

#include <iostream>
#include "./edmonds.h"

using nsp::Graph;
using nsp::Neighbours;

bool
Graph::unsetEdge(int x, int y) {
  bool change = false;
  if (legal_pair(x, y)) {
    for (int i=0; i < edges; ++i) {
      if (neighbours_[x][i] == y) {
        neighbours_[x][i] = -1;
        change = true;
        break;
      }
    }
    int j = 0;
    for (int i = edges-1; i >= 0; ) {
      if ((neighbours_[x][i] != -1) && (j < i)) {
        int pom = neighbours_[x][i];
        neighbours_[x][i] = neighbours_[x][j];
        neighbours_[x][j] = pom;
        j++;
      } else {
        --i;
      }
    }

    for (int i = 0; i < edges; ++i) {
      if (neighbours_[y][i] == x) {
        neighbours_[y][i] = -1;
        break;
      }
    }

    j = 0;
    for (int i = edges-1; i >= 0; ) {
      if ((neighbours_[y][i] != -1) && (j < i)) {
        int pom = neighbours_[y][i];
        neighbours_[y][i] = neighbours_[y][j];
        neighbours_[y][j] = pom;
        j++;
      } else {
        --i;
      }
    }

    if (change) {
      e_used_--;
    }

    return true;
  } else {
    return false;
  }
}

bool
Graph::setEdge(int x, int y) {
  bool change = false;
  if (legal_edge(x, y)) {
    for (int i = 0; i < edges; ++i) {
      if (neighbours_[x][i] == y) {
        break;
      } else if (neighbours_[x][i] == -1) {
        change = true;
        neighbours_[x][i] = y;
        break;
      }
    }

    for (int i = 0; i < edges; ++i) {
      if (neighbours_[y][i] == x) {
        break;
      } else if (neighbours_[y][i] == -1) {
        neighbours_[y][i] = x;
        break;
      }
    }

    if (change) {
      e_used_++;
    }

    return true;
  } else {
    return false;
  }
}

void
Graph::print() const {
  std::cout << "Neighbours" << std::endl;
  for (int i = 0; i < vertices; i++) {
    std::cout << "vertex " << i << ": ";
    for (int j = 0; j < edges; j++) {
      if (neighbours_[i][j] != -1) {
        std::cout << neighbours_[i][j] << " ";
      }
    }
    std::cout << std::endl;
  }
}

Neighbours
Graph::getNeighbours(int v) {
  // pokud nenastane, ze odebereme hranu a nevratime
  if ((v >= 0) && (v <= vertices)) {
    iter beg = neighbours_[v].begin();
    int count = 0;
    for (int i = 0; i < edges; ++i) {
      if (neighbours_[v][i] == -1) {
        break;
      } else {
        count++;
      }
    }
    iter end = beg + count;
    return Neighbours(beg, end);
  } else {
    return Neighbours();
  }
}

bool
Graph::neighbours(int x, int y) const {
  if (legal_pair(x, y)) {
    for (int i = 0; i < edges; ++i) {
      if (neighbours_[x][i] == y) {
        return true;
      }
    }
    return false;
  } else {
    return false;
  }
}

bool
Graph::operator == (const Graph& x) const {
  if (vertices == x.getVertices()) {
    if (getEdges() == x.getEdges()) {
      for (int i = 0; i < vertices; ++i) {
        for (int j = 0; j < vertices; ++j) {
          if (x.neighbours(i, j) != neighbours(i, j)) {
            return false;
          }
        }
      }
      return true;
    }
  }
  return false;
}

bool
Graph::isMapping() const {
  for (int i = 0; i < vertices; ++i) {
    int neighbour_c = 0;
    for (int j = 0; j < edges; ++j) {
      if (neighbours(i, j)) {
        if ((++neighbour_c) > 1) {
          return false;
        }
      }
    }
  }
  return true;
}
