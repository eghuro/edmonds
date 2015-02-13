/*
 * Copyright 2015 Alexandr Mansurov
 */
// Edmonds.cpp : Defines the entry point for the console application.
//

#include "./edmonds.h"
#include <iostream>
#include <fstream>
#include <sstream>
#include <memory>

// using namespace nsp;

/*int _tmain(int argc, _TCHAR* argv[])
{
	Graph g,m;
	if(MappingFinder::step(g,m)){
		//v m je parovani
	}

	return 0;
}*/

void usage() {
	std::cout << "Usage: edmonds <filename>" << std::endl;
}

std::unique_ptr<nsp::Graph> parse(const std::string & file) {
	std::cout << "Parsing file " << file << std::endl;
	std::ifstream f;
	f.open(file.c_str(), std::ios::in);
	if (! f.is_open()) {
		f.close();
		std::cout << "Failed to open file" << std::endl;
		return nullptr;
	} else {
		int edges = -1;
		int vertices = -1;

		std::string line;
		getline(f,line);
		std::istringstream iss(line);
		iss >> edges;
		iss >> vertices;
		if ((edges < 0) || (vertices < 0)) {
			std::cout<<"Format error"<<std::endl;
			f.close();
			return nullptr;
		} else {
			//vytvorit graf
			using nsp::Graph;
			std::unique_ptr<Graph> g(new Graph(vertices, edges));
			for (int i = 0; i < edges; i++) {
				getline(f,line);
				std::istringstream s2(line);
				s2.str(line);
				int x = -1;
				int y = -1;
				s2 >> x;
				s2 >> y;

				if ((x>=0) && (x<vertices) && (y>=0) && (y<vertices)) {
					if (x != y) {
						if (!(*g).setEdge(x,y)) {
							std::cout << "Edge error ("<<x<<","<<y<<")"<<std::endl;
							return nullptr;
						}
					} else {
						std::cout << "Forbidden edge ("<<x<<","<<y<<")"<<std::endl;
						return nullptr;
					}
				} else {
					std::cout << "Range error for edge ("<<x<<","<<y<<")"<<std::endl;
					return nullptr;
				}
			}
			f.close();
			return g;
			//graph = g.get();
		}
	}
}

int main(int argc, char** argv)
{
	//nacti parametry
	if (argc <= 1	) {
		usage();
		return 1;
	}

	//naparsuj soubor, ziskej pointer na graf
	std::string file = argv[1];
	std::unique_ptr<nsp::Graph> g = parse(file);
	if (g == nullptr) {
		std::cout << "fail" << std::endl;
		return 1;
	}
	//potvrd korektnost
	g->print();
	std::cout<<"Continue? [y/n]"<<std::endl;
	char c = 'x';
	std::cin >> c;
	if ((c == 'y') || (c == 'Y')) {
		//pust hledac
		std::unique_ptr<nsp::Graph> m = nsp::MappingFinder::FindMaxMapping(g.get());
		(*m).print();
	}
 	return 0;

}


