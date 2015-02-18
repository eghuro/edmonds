/*
 * Copyright 2015 Alexandr Mansurov
 */

#include <iostream>
#include <fstream>
#include <sstream>
#include <memory>
#include <string>

#include "./edmonds.h"

using nsp::Graph;
using nsp::Dispatcher;

/**
 * Vypise informaci o pouziti
 * @param name jmeno spustitelneho souboru
 */
void
Dispatcher::usage(std::string name) {
  std::cout << "Usage: " << name << " <filename>" << std::endl;
}

/**
 * Nacte graf ze souboru
 * @param file soubor
 * @return ukazatel na vytvoreny graf
 */
std::unique_ptr<nsp::Graph>
Dispatcher::parse(const std::string & file) {
  std::cout << "Parsing file " << file << std::endl;
  std::ifstream f;
  f.open(file.c_str(), std::ios::in);
  if (!f.is_open()) {
    f.close();
    std::cout << "Failed to open file" << std::endl;
    return nullptr;
  } else {
    int edges = -1;
    int vertices = -1;

    std::string line;
    getline(f, line);
    std::istringstream iss(line);
    iss >> edges;
    iss >> vertices;
    if ((edges < 0) || (vertices < 0)) {
      std::cout << "Format error" << std::endl;
      f.close();
      return nullptr;
    } else {
      // vytvorit graf
      std::unique_ptr<Graph> g(new Graph(vertices, edges));
        // v pripade chyby vstupu se smaze sam
      for (int i = 0; i < edges; i++) {
        line = "";
        getline(f, line);
        if (line == "") {
          std::cout << "Error: Empty line" << std::endl;
          return nullptr;
        }
        std::istringstream s2(line);
        s2.str(line);
        int x = -1;
        int y = -1;
        s2 >> x;
        s2 >> y;

        if ((x >= 0) && (x < vertices) && (y >= 0) && (y < vertices)) {
          if (x != y) {
            if (!(*g).setEdge(x, y)) {
              std::cout << "Edge error (" << x << "," << y << ")" << std::endl;
              return nullptr;
            }
          } else {
            std::cout << "Forbidden edge (" <<x<< "," << y << ")" << std::endl;
            return nullptr;
          }
        } else {
          std::cout << "Range error for edge (" << x << "," << y << ")"
            << std::endl;
          return nullptr;
        }
      }
      f.close();
      return g;
    }
  }
}

/**
 * Entry point - nacte data, vyhleda parovani, vypise vystup
 */
int
main(int argc, char** argv) {
  // nacti parametry
  if (argc <= 1) {
    Dispatcher::usage(std::string(argv[0]));
    return 1;
  }

  // naparsuj soubor, ziskej pointer na graf
  std::string file = argv[1];
  std::unique_ptr<nsp::Graph> g = Dispatcher::parse(file);
  if (g == nullptr) {
    std::cout << "fail" << std::endl;
    return 1;
  }
  // potvrd korektnost
  g->print();
  std::cout << "Continue? [y/n]" << std::endl;
  char c = 'x';
  std::cin >> c;
  if ((c == 'y') || (c == 'Y')) {
    // pust hledac
    std::unique_ptr<nsp::Graph> m = nsp::MappingFinder::FindMaxMapping(g.get());
    (*m).print();
  }
  return 0;
}


