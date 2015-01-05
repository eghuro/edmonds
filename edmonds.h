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

namespace nsp{
typedef std::pair<int,int> edge_t;

class Neighbours{
public:
	Neighbours():nil(true){
		std::vector<int> x;
		a=x.begin();
		b=x.end();

	}
	Neighbours(std::vector<int>::iterator beg,std::vector<int>::iterator end):a(beg),b(end),nil(false)
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

	bool nil;
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
		neighbours_(_vertices,std::vector<int>(_edges,-1))
	{
	}

	char unsetEdge(int x,int y){
		if(e_used_<edges){
			if((x>=0)&&(x<=vertices)&&(y>=0)&&(y<=vertices))
			{
				for(int i=0;i<edges;++i)
				{
					if(neighbours_[x][i]!=y)
					{
						neighbours_[x][i]=-1;
					}
				}

				for(int i=0;i<edges;++i)
				{
					if(neighbours_[y][i]!=y)
					{
						neighbours_[y][i]=-1;
						break;
					}
				}

				e_used_--;

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

	char setEdge(int x,int y){
		if(e_used_<edges){
			if((x>=0)&&(x<=vertices)&&(y>=0)&&(y<=vertices))
			{
				for(int i=0;i<edges;++i)
				{
					if(neighbours_[x][i]==y)
					{
						break;
					}
					else if(neighbours_[x][i]!=-1)
					{
						neighbours_[x][i]=y;
						break;
					}
				}

				for(int i=0;i<edges;++i)
				{
					if(neighbours_[y][i]!=-1)
					{
						neighbours_[y][i]=x;
						break;
					}
				}

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
		//pokud nenastane, ze odebereme hranu a nevratime
		if((v>=0)&&(v<=vertices)){
			std::vector<int>::iterator beg = neighbours_[v].begin();
			int count=0;
			for(int i=0;i<edges;++i){
				if(neighbours_[v][i]==-1)
					break;
				else
					count++;
			}
			std::vector<int>::iterator end = beg+count;
			return Neighbours(beg,end);
		}
		else return Neighbours();
	}

	bool neighbours(int x, int y){
		if((x>=0)&&(x<=vertices)&&(y>=0)&&(y<=vertices)){
			for(int i=0;i<edges;++i){
				if(neighbours_[x][i]==y)
				{
					return true;
				}
				else if(neighbours_[x][i]==-1)
				{
					return false;
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
		
	std::vector<std::vector<int> > neighbours_;
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

	static bool step(Graph & g,Graph & m){
		std::deque<int> set;
		les_t l;
		char x=find(g,m,set,l);
		Graph _m;

		switch(x)
		{
		case AP:
			//zlepsi M - otocit hrany v /vne parovani - 
			augment(m,set);
			return true;
			break;
		case BLOSSOM:
			//zkontrahuj kvet
			return blossom(g,m,set,l);
			break;
		case NONE:
			//m je nejvetsi
			return true;
			break;
		default:throw "fatal";
		}
	}
	
#ifndef DEBUG
private:
#endif
	static const char AP=0x10;
	static const char BLOSSOM=0x11;
	static const char NONE=0x12;

	typedef std::map<int,WRecord> les_t;
	typedef std::deque<int> set_t;
	
	static bool blossom(Graph & graph_,Graph & mapping_,const set_t & set,const les_t l){
		//zkontrahuj kvet
		Graph g_k=graph_;
		Graph m_k=mapping_;
		shrink(g_k,m_k,set);
		//zavolej se na G.K, M.K
		if(step(g_k,m_k))
		{//pokud M.K lze zlepsit - zlepsi M, konec
			expand(g_k,m_k,graph_,mapping_,set);
		}
		//pokud M.K nejde zlepsit - M je nejlepsi
		return true;
	}

	static char find(Graph & graph_,Graph & mapping_,set_t & set, les_t & l){
		typedef std::pair<int,WRecord> pair_t;
		set_t f;
		//najdi volne vrcholy a vloz do f a do l na hladinu 0
		//volny vrchol - neni v zadne parovaci hrane
		for(int i=0;i<graph_.vertices;i++){
			if(mapping_.getNeigbours(i).begin()==mapping_.getNeigbours(i).end()){
				//v parovani nema sousedy - izolovany vrchol - volny vrchol
				f.push_back(i);
				l.insert(pair_t(i,WRecord(i,0)));
			}
		}
		//hlavni cyklus - dokud fronta neni prazdna
		for(set_t::iterator i=f.begin();i<f.end();++i)
		{
			int v=*i;

			les_t::iterator x=l.find(v);
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
						return finder(v,y,set,l);
					}
					else
					{//y not in L
						f.push_back(y);
						edge_t e=(*x).second.e;
						WRecord wr=WRecord(y,h+1,e);
						l.insert(pair_t(y,wr));
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

					les_t pom;
					for(std::set<int>::iterator x=y.begin();x!=y.end();x++){
						//existuje vrchol v y, ktery patri do l a je na sude hladine?
						les_t::iterator z=l.find(*x);
						if(z!=l.end())
						{
							if((*z).second.h%2==0){
								//najdu VSC nebo kytku -> konec
								return finder((*z).second.v,v,set,l);
							}
						}else{
							//x nepatri do l
							pom.insert(pair_t(*x,(*z).second));
						}
					}
					//pro kazdy vrchol y nepatrici do L : zarad y do fronty
					for(les_t::iterator y=pom.begin();y!=pom.end();y++){
						f.push_back((*y).second.v);
						WRecord vy=WRecord((*y).second.v,(*y).second.h+1,edge_t(v,(*y).second.v));
						l.insert(pair_t((*y).second.v,vy));
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

	static int lookup_root(int vertex, set_t & set, const les_t & l)
	{
		//hledam koren ve strome (l)
		int v=vertex;
		for(les_t::const_iterator x=l.find(v);x!=l.end();){
			set.push_back((*x).first);

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
		return -1;
	}

	static char finder(int v,int y, set_t & set, const les_t & l){
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
			return BLOSSOM;
		}
		else
		{
			return AP;
		}
	}

	static void augment(Graph & mapping_,set_t & set){
		//zlepsi M - otocit hrany v /vne parovani
		//jde o VSC, pro dvojice vrcholu prohazuji: hrana v/vne parovani
		if(!set.empty()){
			int a=set.front();
			set.pop_front();
			while(!set.empty()){
				int b=set.front();
				set.pop_front();

				if(mapping_.neighbours(a,b)){
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

	static void shrink(Graph & graph_,Graph & mapping_,const set_t & set)
	//kontrakce kvetu (set)
	//v grafu: necham jeden vrchol v, pro zbytek: hrany mimo set napojit na v, odebrat vrcholy setu
	//TODO: zaroven v parovani totez
	{
		int v = set.front();
		//zjistit, zda ex. hrana (v,v) v grafu
		bool b=graph_.neighbours(v,v);
		//vrcholy kvetu
		for(set_t::const_iterator it=(++set.begin());it<set.end();++it)
		{
			int x=(*it);//vrchol
			graph_.unsetEdge(v,x);
			//sousede
			for(std::vector<int>::iterator yt=graph_.getNeigbours(x).begin();yt<graph_.getNeigbours(v).end();++yt)
			{
				int y=(*yt);//soused x
				/*
				 * i pokud y je v set: prepoji se y na v a zrusi se hrana (x,y)
				 * tim se zajisti, ze vzdy existuje hrana (x,v)
				 */
				//(v,y) neni v graph_ a mapping_
				//napojeni souseda na v
				graph_.unsetEdge(x,y);
				graph_.setEdge(v,y);
			}

			//TODO: vrcholy uvedene v set odebrat z grafu - resp. co s izolovanymi vrcholy?
			//graph_.remove(x);
		}
		//pokud existovala hrana (v,v) nastavit
		if(b){
			graph_.setEdge(v,v);
		}
	}

	static void expand(const Graph & g_k_,const Graph & m_k_, Graph & graph_, Graph & mapping_,const set_t & set)
		//TODO: pokud M.K lze zlepsit - zlepsi M, konec
	{}
};//MappingFinder
};//nsp

#endif//EDMONDS_H_
