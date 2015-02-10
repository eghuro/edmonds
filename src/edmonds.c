/*
 * edmonds.c
 *
 *  Created on: 26 Dec 2014
 *      Author: alex
 */


#include "edmonds.h"

using namespace nsp;

int main(int argc, char** argv)
{
	Graph g,m;
 	while(MappingFinder::step(g,m));//zlepsuj parovani, dokud to jde
 	//v m je maximalni parovani

 
 	return 0;

}
