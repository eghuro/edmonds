/*
 * test.cpp
 *
 *  Created on: 15 Jan 2015
 *      Author: alex
 */

#define DEBUG
#include "edmonds.h"
#include <iostream>
using namespace nsp;

void
init()
{
	Graph g(0,0);
	if((g.getVertices()==0)&&(g.getEdges()==0)&&(!g.legal_edge(0,0)))
	{
		std::cout<<"OK"<<std::endl;
	}
	else
	{
		std::cout<<"Fail  init #0"<<std::endl;
	}

	g=Graph(1,0);
	if((g.getVertices()==1)&&(g.getEdges()==0)&&(!g.legal_edge(0,0)))
	{
		std::cout<<"OK"<<std::endl;
	}
	else
	{
		std::cout<<"Fail  init #1"<<std::endl;
	}

	g=Graph(1,1);
	if((g.getVertices()==1)&&(g.legal_edge(0,0))&&g.setEdge(0,0)&&(g.getEdges()==1))
	{
		std::cout<<"OK"<<std::endl;
	}
	else
	{
		std::cout<<"Fail  init #2"<<std::endl;
	}
	g=Graph(3,3);
	if((g.getVertices()==3)&&g.setEdge(0,1)&&g.setEdge(0,2)&&g.setEdge(1,2)&&g.neighbours(0,1)&&g.neighbours(2,0)&&g.neighbours(1,2)&&(g.getEdges()==3))
	{
		std::cout<<"OK"<<std::endl;
	}
	else
	{
		g.print();
		std::cout<<"Fail  init #3"<<std::endl;
	}
	if(g.unsetEdge(0,2)){
		std::cout<<"OK"<<std::endl;
	}
	else
	{
		g.print();
		std::cout<<"Fail #4"<<std::endl;
	}

	Neighbours n1=g.getNeighbours(1);
	std::vector<int>::iterator it=n1.begin();
	if((*it)!=0) std::cout<<"Fail #5"<<std::endl;
	++it;
	if((*it)!=2) std::cout<<"Fail #6"<<std::endl;

	if(!g.getNeighbours(5).nil)
	{
		std::cout<<"Fail #7"<<std::endl;
	}

	if(!g.neighbours(0,1)||g.neighbours(0,2))
	{
		std::cout<<"Fail #8"<<std::endl;
	}
}

void
test0()
{
	Graph g(6,6);
	g.setEdge(0,1);
	g.setEdge(1,2);
	g.setEdge(1,3);
	g.setEdge(2,3);
	g.setEdge(3,4);
	g.setEdge(2,5);

	Graph m(6,6);
	MappingFinder::les_t l;
	MappingFinder::queue_t s = MappingFinder::prepare(m,l);
	if(s.size()==6)
	{
		std::cout<<"Velikost OK"<<std::endl;
	}
	else
	{
		std::cout<<"Velikost KO"<<std::endl;
	}

	MappingFinder::set_t set;

	for(int i=0;i<5;++i)
	{
		MappingFinder::les_t l;
		Result r=MappingFinder::find(g,m,set,l);
		switch(r)
		{
		case AP:
			MappingFinder::augment(m,set);
		break;
		default:
			break;
		}
		if(!m.isMapping()){
			std::cout<<"Chybny mezivystup - nejde o parovani"<<std::endl;
			break;
		}
	}

	Graph expected(6,3);
	expected.setEdge(0,1);
	expected.setEdge(4,3);
	expected.setEdge(2,5);

	if(m==expected)
	{
		std::cout<<"OK"<<std::endl;
	}
	else
	{
		std::cout<<"KO"<<std::endl;
	}
}

void
test1()
{
	Graph g(6,6);
	g.setEdge(0,1);
	g.setEdge(1,2);
	g.setEdge(2,3);
	g.setEdge(2,4);
	g.setEdge(3,4);
	g.setEdge(4,5);

	Graph m=MappingFinder::FindMaxMapping(g);

	if(!m.isMapping())
	{
		std::cout<<"KO - not a mapping"<<std::endl;
	}

	Graph expected(6,3);
	expected.setEdge(0,1);
	expected.setEdge(2,3);
	expected.setEdge(4,5);
	if(m==expected)
	{
		std::cout<<"OK"<<std::endl;
	}
	else
	{
		std::cout<<"KO"<<std::endl;
		m.print();
		std::cout<<"Expected"<<std::endl;
		expected.print();
	}

}

void
test2()
{
	Graph g(16,18);
	g.setEdge(0,1);
	g.setEdge(1,2);
	g.setEdge(2,3);
	g.setEdge(15,3);
	g.setEdge(3,4);
	g.setEdge(2,4);
	g.setEdge(4,5);
	g.setEdge(4,7);
	g.setEdge(7,8);
	g.setEdge(7,6);
	g.setEdge(6,14);
	g.setEdge(14,5);
	g.setEdge(5,9);
	g.setEdge(14,11);
	g.setEdge(10,14);
	g.setEdge(10,11);
	g.setEdge(11,12);
	g.setEdge(12,13);
	Graph m=MappingFinder::FindMaxMapping(g);
	if(m.isMapping()&&m.getEdges()==8) std::cout<<"OK"<<std::endl; else{
		std::cout<<"KO"<<std::endl;
		m.print();
	}
}

void
test3()
{
	Graph g(10,11);
	g.setEdge(0,1);
	g.setEdge(1,2);
	g.setEdge(2,3);
	g.setEdge(2,4);
	g.setEdge(3,4);
	g.setEdge(4,7);
	g.setEdge(4,5);
	g.setEdge(4,6);
	g.setEdge(5,6);
	g.setEdge(6,8);
	g.setEdge(8,9);

	Graph m=MappingFinder::FindMaxMapping(g);
	if(m.isMapping()&&m.getEdges()==5) std::cout<<"OK"<<std::endl; else std::cout<<"KO"<<std::endl;
	//5 hran na 10 vrcholech je perfektni parovani a je validni
}

void
test4()
{
	Graph g(16,16);
	g.setEdge(0,1);
	g.setEdge(1,2);
	g.setEdge(2,3);
	g.setEdge(3,4);
	g.setEdge(4,5);
	g.setEdge(5,6);
	g.setEdge(6,7);
	g.setEdge(7,8);
	g.setEdge(6,9);
	g.setEdge(9,10);
	g.setEdge(10,11);
	g.setEdge(11,12);
	g.setEdge(12,4);
	g.setEdge(11,13);
	g.setEdge(13,14);
	g.setEdge(13,15);

	Graph m(16,7);
	m.setEdge(0,1);
	m.setEdge(2,3);
	m.setEdge(4,5);
	m.setEdge(6,7);
	m.setEdge(9,10);
	m.setEdge(11,12);
	m.setEdge(13,14);
	Graph m0=MappingFinder::FindMaxMapping(g);
	if(m==m0) std::cout<<"OK"<<std::endl;
	else{
		std::cout<<"KO"<<std::endl;
		m0.print();
	}
}

int
main(int argc, char** argv)
{
	std::cout<<"Init"<<std::endl;
	init();
	std::cout<<"Test 0"<<std::endl;
	test0();
	std::cout<<"Test 1"<<std::endl;
	test1();
	std::cout<<"Test 3"<<std::endl;
	test3();
	std::cout<<"Test 2"<<std::endl;
	test2();
	std::cout<<"Test 4"<<std::endl;
	test4();
}
