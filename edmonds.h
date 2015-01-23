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

namespace nsp{
typedef std::pair<int,int> edge_t;
typedef std::vector<int>::iterator iter;

class InconsistentStructureException : public std::exception {};
class UnexpectedResultException : public std::exception {};

struct Neighbours
{
private:
	iter a,b;
public:
	Neighbours():nil(true)
	{
		std::vector<int> x;
		a=x.begin();
		b=x.end();
	}
	Neighbours(iter beg,iter end):a(beg),b(end),nil(false) {}

	inline iter begin() const { return a; }
	inline iter end() const { return b; }
	inline int size() const { return b-a; }

	bool nil;
};

class Graph
{
public:

	inline int getVertices() const { return vertices; }

	inline int getEdges() const { return e_used_; }

	Graph(int vertices_l=V_COUNT_DEF,int edges_l=E_COUNT_DEF):
		edges(edges_l),
		vertices(vertices_l),
		e_used_(0),
		neighbours_(vertices_l,std::vector<int>(edges_l,-1))
	{
	}

	void print() const;

	bool unsetEdge(int,int);

	bool setEdge(int,int);

	Neighbours getNeighbours(int v);

	bool neighbours(int, int) const;

	bool operator == (const Graph&) const;

	bool isMapping() const;

#ifndef DEBUG
private:
#endif
	static const int V_COUNT_DEF=1;
	static const int E_COUNT_DEF=1;

	int edges,vertices,e_used_;

	std::vector<std::vector<int> > neighbours_;

	inline bool legal_pair(int x,int y) const
	{
		return (x>=0)&&(x<=vertices)&&(y>=0)&&(y<=vertices);
	}

	inline bool legal_edge(int x,int y) const
	{
		return (e_used_<edges)&&legal_pair(x,y);
	}
};

struct WRecord
{
public:
	int v,h;
	edge_t e;

	WRecord(int v_l,int h_l,edge_t e_l):v(v_l),h(h_l),e(e_l)
	{}

	WRecord(int v_l,int h_l):v(v_l),h(h_l)
	{}
} ;

enum Result{
	AP,BLOSSOM,NONE
};

class MappingFinder{
#ifndef DEBUG
private:
#endif
#ifdef DEBUG
public:
#endif

	typedef std::map<int,WRecord> les_t;
	typedef std::deque<int> queue_t;
	typedef std::deque<edge_t> set_t;
	typedef std::pair<int,WRecord> pair_t;
	
	//0
	static inline bool blossom(Graph & graph_,Graph & mapping_,set_t & set,const les_t l)
	{
		std::cout<<"Blossom"<<std::endl;
		std::cout<<"Graf"<<std::endl;
		graph_.print();

		std::cout<<"Parovani"<<std::endl;
		mapping_.print();

		std::cout<<"Set"<<std::endl;
		for(set_t::iterator it=set.begin();it!=set.end();++it)
		{
			std::cout<<"("<<(*it).first<<","<<(*it).second<<")"<<std::endl;
		}
		std::cout<<std::endl<<std::endl<<std::endl;
		//kontrakce kvetu
		Graph g_k=graph_;
		Graph m_k=mapping_;
		//urizni stonek
		cut(set);
		//zkontrahuj kvet
		shrink(g_k,m_k,set);
		//zavolej se na G.K, M.K
		if(step(g_k,m_k))
		{//pokud M.K lze zlepsit - zlepsi M, konec
			expand(g_k,m_k,graph_,mapping_,set);
			//return true;
			return false;
		}
		//pokud M.K nejde zlepsit - M je nejlepsi
		return false;
	}

	//0
	static inline queue_t prepare(Graph & mapping_, les_t & l)
	{
		queue_t f;
		//najdi volne vrcholy a vloz do f a do l na hladinu 0
		//volny vrchol - neni v zadne parovaci hrane
		for(int i=0;i<mapping_.getVertices();i++)
		{
			if(mapping_.getNeighbours(i).begin()==mapping_.getNeighbours(i).end())
			{
				//v parovani nema sousedy - izolovany vrchol - volny vrchol
				f.push_back(i);
				l.insert(pair_t(i,WRecord(i,0)));
			}
		}

		return f;
	}

	//0
	static Result find(Graph & graph_,Graph & mapping_,set_t & set, les_t & l) throw (InconsistentStructureException);

	//0
	static int lookup_root(int vertex, set_t & set, const les_t & l);

	//0
	static inline Result finder(int v,int y, set_t & set, const les_t & l){
		//najdu VSC nebo Kytku -> konec
		//hledej z v cestu do korene k1

		int k1=lookup_root(v,set,l);
		if(k1==-1)
			throw "Error";
		//hledej v y cestu do korene k2
		int k2=lookup_root(y,set,l);
		if(k2==-1)
			throw "Error";
		//k1=k2 -> kytka, k1!=k2 -> VSC
		if(k1==k2)
		{
			set.push_back(std::pair<int,int>(v,y));
			return BLOSSOM;
		}
		else
		{
			set.push_back(edge_t(v,y));
			return AP;
		}
	}

	//0
	static inline void augment(Graph & mapping_,set_t & set){
		//zlepsi M - otocit hrany v /vne parovani
		//jde o VSC, pro dvojice vrcholu prohazuji: hrana v/vne parovani
		if(!set.empty()){
			while(!set.empty()){
				edge_t e=set.front();
				int a=e.first;
				int b=e.second;
				set.pop_front();
				if(mapping_.neighbours(a,b))
				{
					mapping_.unsetEdge(a,b);
				}
				else
				{
					mapping_.setEdge(a,b);
				}
				a=b;
			}
		}
	}

	//2
	static void shrink(Graph & graph_,Graph & mapping_,const set_t & set);
	//kontrakce kvetu (set)
	//v grafu: necham jeden vrchol v, pro zbytek: hrany mimo set napojit na v, odebrat vrcholy setu
	//zaroven v parovani totez


	static void cut(set_t & set);


	static void expand(const Graph & g_k_,const Graph & m_k_, Graph & graph_, Graph & mapping_,const set_t & set)
		//TODO: pokud M.K lze zlepsit - zlepsi M, konec
	{
		std::cout<<"Expand"<<std::endl;
	}

	//true ... zvetsili jsme parovani
	//0
	static bool step(Graph & g,Graph & m);

#ifndef DEBUG
public:
#endif
	static inline Graph FindMaxMapping(Graph & g)
	{
		Graph m(g.getVertices(),g.getEdges());
		while(step(g,m));;
		return m;
	}


};//MappingFinder
};//nsp
#endif//EDMONDS_H_
