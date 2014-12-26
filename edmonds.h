/*
 * edmonds.h
 *
 *  Created on: 25 Dec 2014
 *      Author: alex
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

namespace nsp{
typedef std::pair<int,int> edge_t;

class Neighbours{
public:
	static const Neighbours nil;
	Neighbours(std::vector<int>::iterator beg,std::vector<int>::iterator end):a(beg),b(end)
	{
	}

	std::vector<int>::iterator begin()
	{
		return a;
	}
	std::vector<int>::iterator end()
	{
		return b;
	}
private:
	std::vector<int>::iterator a,b;
};

class Graph{
public:

	static const char OK=0x0A;
	static const char BOUND=0x0B;
	static const char COUNT=0x0C;

	int vertices,edges;

	Graph(int _vertices=V_COUNT_DEF,int _edges=E_COUNT_DEF):
		vertices(_vertices),
		edges(_edges), 
		e_used_(0),		
		neighbours_(_edges/2+1),
		neighbour_meta_(_vertices)		
	{
	}

	char setEdge(int x,int y){
		/*
			seznam sousedu
			neighbours_[] obsahuje sousedy vrcholu "za sebou"
			neighbour_meta_[] obsahuje dvojice "zacatek", "pocet sousedu"
		*/
		if(e_used_<edges){
			if((x>=0)&&(x<=vertices)&&(y>=0)&&(y<=vertices))
			{
				int x_start = neighbour_meta_[x].first;
				int x_neighbours = neighbour_meta_[x].second;
				neighbours_[x_start+x_neighbours]=y;
				neighbour_meta_[x].second++;

				int y_start = neighbour_meta_[y].first;
				int y_neighbours = neighbour_meta_[y].second;
				neighbours_[y_start+y_neighbours]=x;
				neighbour_meta_[y].second++;

				e_used_++;

				return OK;
			}
			else
			{
				return BOUND;
			}
		}
		else
		{
			return COUNT;
		}
		
	}

	Neighbours getNeigbours(int v)
	{
		if((v>=0)&&(v<=vertices)){
			std::vector<int>::iterator beg = neighbours_.begin()+neighbour_meta_[v].first;
			std::vector<int>::iterator end = beg+neighbour_meta_[v].second;
			return Neighbours(beg,end);
		}
		else return Neighbours::nil;
	}

	bool neighbours(int x, int y){
		if((x>=0)&&(x<=vertices)&&(y>=0)&&(y<=vertices)){
			int x_start=neighbour_meta_[x].first;
			int x_neighbours = neighbour_meta_[x].second;
			for(int i=0;i<x_neighbours;++i){
				if(neighbours_[x_start+i]==y){
					return true;
				}
			}
			return false;
		}
		else{
			return false;
		}
	}

private:
	static const int V_COUNT_DEF=1;
	static const int E_COUNT_DEF=1;
	
	int e_used_;
		
	std::vector<int> neighbours_;
	std::vector<std::pair<int,int>> neighbour_meta_;
};

class WRecord {
public:
	int v,h;
	edge_t e;

	WRecord(int _v,int _h,edge_t _e):v(_v),h(_h),e(_e)
	{}

	WRecord(int _v,int _h):v(_v),h(_h)
	{}
} ;

class MappingFinder{
public:
	MappingFinder(Graph g,Graph m):
		mapping_(m),
		graph_(g)
	{
	}

	bool step(){
		switch(find())
		{
		case AP:
			//zlepsi M
			return true;
			break;
		case BLOSSOM:
			//zkontrahuj kvet
			//step
			//pokud jde zlepsit, zlepsi
			//jinak j m nejlepsi
			break;
		case NONE:
			//m je nejvetsi
			return true;
			break;
		default:throw "fatal";
		}
	}
	
	
private:
	static const char AP=0x10;
	static const char BLOSSOM=0x11;
	static const char NONE=0x12;

	Graph mapping_,graph_;
	int v_count_,e_count_;
	std::map<int,WRecord> l;
	std::deque<int> f;

	char find(){
		//najdi volne vrcholy a vloz do f a do l na hladinu 0
		//volny vrchol - neni v zadne parovaci hrane
		for(int i=0;i<graph_.vertices;i++){
			if(mapping_.getNeigbours(i).begin()==mapping_.getNeigbours(i).end()){
				//v parovani nema sousedy - izolovany vrchol - volny vrchol
				f.push_back(i);
				l.insert(std::pair<int,WRecord>(i,WRecord(i,0)));
			}
		}
		//hlavni cyklus - dokud fronta neni prazdna
		for(std::deque<int>::iterator i=f.begin();i<f.end();++i)
		{
			int v=*i;

			auto x=l.find(v);
			if(x!=l.end()){
				int h=(*x).second.h;//hladina v
				Neighbours n_v=mapping_.getNeigbours(v);

				if(h%2==1)
				{				
					int y=*(n_v.begin());//vrchol spojeny s v parovaci hranou
					if(l.find(y)!=l.end())
					{//y in l
						//y nutne na hladine h
						//najdu VSC nebo Kytku -> konec
						return finder(v,y);
					}
					else
					{//y not in L
						f.push_back(y);
						edge_t e=(*x).second.e;
						WRecord wr=WRecord(y,h+1,e);
						l.insert(std::pair<int,WRecord>(y,wr));
					}
				}
				else{
					std::set<int> y;
					//y mnozina vrcholu dosazitelnych z v po neparovaci hrane
					for(std::vector<int>::iterator x=n_v.begin();x<n_v.end();x++){
						//sousedi v
						if(!mapping_.neighbours(v,(*x))){
							//nejsou sousedi v parovani
							y.insert(*x);
						}
					}
					std::map<int,WRecord> pom;
					for(std::set<int>::iterator x=y.begin();x!=y.end();x++){
						//existuje vrchol v y, ktery patri do l a je na sude hladine?
						std::map<int,WRecord>::iterator z=l.find(*x);
						if(z!=l.end())
						{
							if((*z).second.h%2==0){
								//najdu VSC nebo kytku -> konec
								return finder((*z).second.v,v);
							}
						}else{
							//x nepatri do l
							pom.insert(std::pair<int,WRecord>(*x,(*z).second));
						}
					}
					//pro kazdy vrchol y nepatrici do L : zarad y do fronty
					for(std::map<int,WRecord>::iterator y=pom.begin();y!=pom.end();y++){
						f.push_back((*y).second.v);
						WRecord vy=WRecord((*y).second.v,(*y).second.h+1,edge_t(v,(*y).second.v));
						l.insert(std::pair<int,WRecord>((*y).second.v,vy));
					}
				}
			}
			else{
				//err?
				throw "Inconsistency";
			}
		}
		return NONE;
	}

	int lookup_root(int vertex)
	{
		//hledam koren ve strome (l)
		int v=vertex;
		auto x=l.find(v);
		for(;;){
			if(x!=l.end()){
				if((*x).second.h==0){
					return (*x).second.v;
				}
				else{
					if((*x).second.e.first==v){
						v=(*x).second.e.second;
					}
					else{
						v=(*x).second.e.first;
					}
				}
			}
		}
	}

	char finder(int v,int y){
		//najdu VSC nebo Kytku -> konec
		//hledej z v cestu do korene k1
		int k1=lookup_root(v);
		//hledej v y cestu do korene k2
		int k2=lookup_root(y);
		//k1=k2 -> kytka, k1!=k2 -> VSC
		if(k1==k2) return BLOSSOM;
		else return AP;
	}
};
};//nsp

#endif//EDMONDS_H_
