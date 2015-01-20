/*
 * graph.cpp
 *
 *  Created on: 19 Jan 2015
 *      Author: alex
 */
#include "edmonds.h"
#include <iostream>

using namespace nsp;

bool Graph::unsetEdge(int x,int y)
{
//	std::cout<<"unset edge("<<x<<","<<y<<")"<<std::endl;
	if(legal_pair(x,y)){
		for(int i=0;i<edges;++i)
		{
			if(neighbours_[x][i]==y)
			{
				neighbours_[x][i]=-1;
	//			std::cout<<"@@["<<x<<"]["<<i<<"]=-1"<<std::endl;
				break;
			}
		}
		int j=0;
		for(int i=edges-1;i>=0;)
		{
			if((neighbours_[x][i]!=-1)&&(j<i))
			{
				int pom=neighbours_[x][i];
				neighbours_[x][i]=neighbours_[x][j];
				//std::cout<<"["<<x<<"]["<<i<<"]="<<neighbours_[x][j]<<std::endl;
				neighbours_[x][j]=pom;
				//std::cout<<"["<<x<<"]["<<j<<"]="<<pom<<std::endl;
				j++;
			}
			else
			{
				--i;
			}
		}

		for(int i=0;i<edges;++i)
		{
			if(neighbours_[y][i]==x)
			{
				neighbours_[y][i]=-1;
				//std::cout<<"@@["<<y<<"]["<<i<<"]=-1"<<std::endl;
				break;
			}
		}
		j=0;
		for(int i=edges-1;i>=0;)
		{
			if((neighbours_[y][i]!=-1)&&(j<i))
			{
				int pom=neighbours_[y][i];
				neighbours_[y][i]=neighbours_[y][j];
				//std::cout<<"["<<y<<"]["<<i<<"]="<<neighbours_[y][j]<<std::endl;
				neighbours_[y][j]=pom;
				//std::cout<<"["<<y<<"]["<<j<<"]="<<pom<<std::endl;
				j++;
			}
			else
			{
				--i;
			}
		}

		e_used_--;
		//print();
		return true;
	}
	else
	{
		return false;
	}
}

bool Graph::setEdge(int x,int y)
{
	if(legal_edge(x,y))
	{
		for(int i=0;i<edges;++i)
		{
			if(neighbours_[x][i]==y)
			{
				break;
			}
			else if(neighbours_[x][i]==-1)
			{
				neighbours_[x][i]=y;
				break;
			}
		}

		for(int i=0;i<edges;++i)
		{
			if(neighbours_[y][i]==x)
			{
				break;
			}
			else if(neighbours_[y][i]==-1)
			{
				neighbours_[y][i]=x;
				break;
			}
		}

		e_used_++;

		return true;
	}
	else
	{
		return false;
	}

}

void Graph::print() const
{
	std::cout<<"Neighbours"<<std::endl;
	for(int i=0;i<vertices;++i)
	{
		std::cout<<"vertex "<<i<<": ";
		for(int j=0;j<edges;++j)
		{
			if(neighbours_[i][j]!=-1){
				std::cout<<neighbours_[i][j]<<" ";
			}
		}
		std::cout<<std::endl;
	}
}

Neighbours Graph::getNeighbours(int v)
{
	//pokud nenastane, ze odebereme hranu a nevratime
	if((v>=0)&&(v<=vertices)){
		iter beg = neighbours_[v].begin();
		int count=0;
		for(int i=0;i<edges;++i){
			if(neighbours_[v][i]==-1)
				break;
			else
				count++;
		}
		iter end = beg+count;
		return Neighbours(beg,end);
	}
	else return Neighbours();
}

bool Graph::neighbours(int x, int y) const
{
	if(legal_pair(x,y))
	{
		for(int i=0;i<edges;++i)
		{
			if(neighbours_[x][i]==y)
			{
				return true;
			}
			/*else if(neighbours_[x][i]==-1)
			{
				return false;
			}*/
		}
		return false;
	}
	else{
		return false;
	}
}

//TODO potreba overit isomorfismus nebo vyhodit
bool Graph::operator == (const Graph& x) const
{
	if(vertices==x.getVertices())
	{
		if(getEdges()==x.getEdges())
		{
			for(int i=0;i<vertices;++i)
			{
				for(int j=0;j<vertices;++j)
				{
					if(x.neighbours(i,j)!=neighbours(i,j))
					{
						return false;
					}
				}
			}
			return true;
		}
	}
	return false;
}

bool Graph::isMapping() const
{
	for(int i=0;i<vertices;++i)
	{
		int neighbour_c=0;
		for(int j=0;j<edges;++j)
		{
			if(neighbours(i,j)){
				if((++neighbour_c)>1)
				{
					return false;
				}
			}
		}
	}
	return true;
}
