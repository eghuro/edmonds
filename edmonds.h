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

namespace nsp{
typedef std::pair<int,int> edge_t;
typedef std::vector<int>::iterator iter;

class Neighbours
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
	Neighbours(iter beg,iter end):a(beg),b(end),nil(false)
	{
	}

	iter begin() const
	{
		return a;
	}
	iter end() const
	{
		return b;
	}

	int size() const
	{
		return b-a;
	}

	bool nil;
};


class Graph
{
public:

	int getVertices() const
	{
		return vertices;
	}

	int getEdges() const
	{
		//return edges;
		return e_used_;
	}

	Graph(int vertices_l=V_COUNT_DEF,int edges_l=E_COUNT_DEF):
		edges(edges_l),
		vertices(vertices_l),
		e_used_(0),
		neighbours_(vertices_l,std::vector<int>(edges_l,-1))
	{
	}

	void print() const
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

	bool unsetEdge(int x,int y)
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

	bool setEdge(int x,int y)
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

	Neighbours getNeighbours(int v)
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

	bool neighbours(int x, int y) const
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
	bool operator == (Graph& x) const
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

	bool isMapping() const
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
#ifndef DEBUG
public:
#endif
	static const int V_COUNT_DEF=1;
	static const int E_COUNT_DEF=1;

	int vertices,edges;
	int e_used_;

	std::vector<std::vector<int> > neighbours_;

	bool legal_pair(int x,int y) const
	{
		return (x>=0)&&(x<=vertices)&&(y>=0)&&(y<=vertices);
	}

	bool legal_edge(int x,int y) const
	{
		return (e_used_<edges)&&legal_pair(x,y);
	}
};

class WRecord
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
	static bool blossom(Graph & graph_,Graph & mapping_,set_t & set,const les_t l)
	{
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
			return true;
		}
		//pokud M.K nejde zlepsit - M je nejlepsi
		return false;
	}

	static queue_t prepare(Graph & mapping_, les_t & l)
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

		std::cout<<"Volne vrcholy: ";
		for(queue_t::iterator it=f.begin();it!=f.end();++it)
		{
			std::cout<<(*it)<<" ";
		}
		std::cout<<std::endl;

		//std::cout<<"Parovani:"<<std::endl;
		//mapping_.print();
		return f;
	}

	//2
	static Result find(Graph & graph_,Graph & mapping_,set_t & set, les_t & l)
	{
		std::cout<<"Step"<<std::endl;

		//najdi volne vrcholy a vloz do f a do l na hladinu 0
		queue_t f=prepare(mapping_,l);

		//hlavni cyklus - dokud fronta neni prazdna
		//for(set_t::iterator i=f.begin();i<f.end();++i)
		queue_t::iterator i=f.begin();
		queue_t::iterator end_it=f.end();
		while(i<end_it)
		{
			int v=*i;
			f.pop_front();
			std::cout<<"Vrchol "<<v<<std::endl;

			les_t::iterator x=l.find(v);
			if(x!=l.end())
			{
				int h=(*x).second.h;//hladina v

				std::cout<<"Hladina "<<h<<std::endl;
				Neighbours n_v=graph_.getNeighbours(v);

				if(h%2==1)
				{				
					std::cout<<"Licha hladina"<<std::endl;
					Neighbours m_v = mapping_.getNeighbours(v);
					int y=*(m_v.begin());//vrchol spojeny s v parovaci hranou
					std::cout<<"Sparovan s "<<y<<std::endl;
					if(l.find(y)!=l.end())
					{//y in l
						std::cout<<y<<" je v l"<<std::endl;
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
				else
				{
					std::cout<<"Suda hladina"<<std::endl;
					std::set<int> y;
					//y mnozina vrcholu dosazitelnych z v po neparovaci hrane
					std::cout<<"Y={";
					for(iter x=n_v.begin();x<n_v.end();x++)
					{
						//sousedi v
						if(!mapping_.neighbours(v,(*x)))
						{
							//nejsou sousedi v parovani
							y.insert(*x);
							std::cout<<(*x)<<" ";
						}
					}
					std::cout<<"}"<<std::endl;

					les_t pom;
					for(std::set<int>::iterator w=y.begin();w!=y.end();++w)
					{
						//existuje vrchol v y, ktery patri do l a je na sude hladine?
						les_t::iterator z=l.find(*w);
						if(z!=l.end())
						{
							std::cout << "Vrchol "<<(*z).second.v<<", hladina "<<(*z).second.h<<std::endl;
							if((*z).second.h%2==0)
							{
								//najdu VSC nebo kytku -> konec
								return finder((*z).second.v,v,set,l);
							}
							//jinak y patri do l, ale neni na sude hladine, jdeme dale
						}
						else
						{
							//x nepatri do l
							//pozor (*z) se nesmi pouzit
							WRecord rec(*w,h+1,edge_t(v,*w));
							std::cout<<"Vkladam do pom:"<<(*w)<<", "<<rec.h<<", ("<<rec.e.first<<","<<rec.e.second<<")"<<std::endl;
							pom.insert(pair_t(*w,rec));
						}
					}
					std::cout<<"Vrcholy nepatrici do L: ";
					for(les_t::iterator it=pom.begin();it!=pom.end();++it)
					{
						std::cout<<(*it).first<<" ";
					}
					std::cout<<std::endl;

					//stav fronty
					std::cout<<"Queue:";
					for(queue_t::iterator it=f.begin();it!=f.end();++it)
					{
						std::cout<<*it<<" ";
					}
					std::cout<<std::endl;


					//pro kazdy vrchol y nepatrici do L : zarad y do fronty
					for(les_t::iterator y=pom.begin();y!=pom.end();++y)
					{
						std::cout<<"Vkladam: "<<(*y).first<<std::endl;
						f.push_back((*y).first);
						//WRecord vy=WRecord((*y).second.v,(*y).second.h,edge_t(v,(*y).second.v));
						l.insert(pair_t((*y).second.v,(*y).second));
					}
				}
			}
			else{
				//err?
				throw "Inconsistency";
			}
			//stav fronty
			std::cout<<"Queue:";
			for(queue_t::iterator it=f.begin();it!=f.end();++it)
			{
				std::cout<<"#"<<*it<<" ";
			}
			std::cout<<std::endl;
			i=f.begin();
			end_it=f.end();
		}
		return NONE;
	}

	//0
	static int lookup_root(int vertex, set_t & set, const les_t & l)
	{
		std::cout<<"Lookup root ("<<vertex<<")"<<std::endl;
		//hledam koren ve strome (l)
		int v=vertex;
		les_t::const_iterator x=l.find(v);
		if(x!=l.end()){
			WRecord r=(*x).second;
			while(r.h!=0)
			{
				edge_t e = r.e;
				std::cout<<"("<<e.first<<","<<e.second<<")"<<std::endl;
				int buddy=(e.first==r.v)?e.second:e.first;
				x=l.find(buddy);
				if(x!=l.end()){
					r=(*x).second;
					set.push_back(e);
				}else{
					return -1;
				}
			}
			return r.v;
		}
		return -1;
	}

	//0
	static Result finder(int v,int y, set_t & set, const les_t & l){
		std::cout<<"Finder ("<<v<<","<<y<<")"<<std::endl;
		//najdu VSC nebo Kytku -> konec
		//hledej z v cestu do korene k1
		int k1=lookup_root(v,set,l);
		std::cout<<"K1 = "<<k1<<std::endl;
		if(k1==-1)
			throw "Error";
		//hledej v y cestu do korene k2
		int k2=lookup_root(y,set,l);
		std::cout<<"K2 = "<<k2<<std::endl;
		if(k2==-1)
			throw "Error";
		//k1=k2 -> kytka, k1!=k2 -> VSC
		if(k1==k2)
		{
			return BLOSSOM;
		}
		else
		{
			set.push_back(edge_t(v,y));
			return AP;
		}
	}

	//0
	static void augment(Graph & mapping_,set_t & set){
		std::cout<<"Augment"<<std::endl;
		//mapping_.print();
		std::cout<<"set: ";
		for(set_t::iterator it=set.begin();it!=set.end();++it)
		{
			std::cout<<"("<<(*it).first<<","<<(*it).second<<") ";
		}
		std::cout<<std::endl;
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
	static void shrink(Graph & graph_,Graph & mapping_,const set_t & set)
	//kontrakce kvetu (set)
	//v grafu: necham jeden vrchol v, pro zbytek: hrany mimo set napojit na v, odebrat vrcholy setu
	//zaroven v parovani totez
	{
		/*int v = set.front();
		//zjistit, zda ex. hrana (v,v) v grafu (v parovani nebyla)
		bool b=graph_.neighbours(v,v);
		//vrcholy kvetu
		for(set_t::const_iterator it=(++set.begin());it<set.end();++it)
		{
			int x=(*it);//vrchol
			graph_.unsetEdge(v,x);
			mapping_.unsetEdge(v,x);
			//sousede
			for(iter yt=graph_.getNeighbours(x).begin();yt<graph_.getNeighbours(v).end();++yt)
			{
				int y=(*yt);//soused x
				/*
				 * i pokud y je v set: prepoji se y na v a zrusi se hrana (x,y)
				 * tim se zajisti, ze vzdy existuje hrana (x,v)
				 */
				//(v,y) neni v graph_ a mapping_
				//napojeni souseda na v
			/*	graph_.unsetEdge(x,y);
				graph_.setEdge(v,y);

				//hrany parovani jsou podmnozinou hran grafu
				if(mapping_.neighbours(x,y))
				{
					//pokud hrana byla i v parovani ...
					mapping_.unsetEdge(x,y);
					mapping_.setEdge(v,y);
				}
			}

			/*
			 * vrcholy uvedene v set odebrat z grafu - resp. co s izolovanymi vrcholy?
			 * v parovani nevadi, pritomnost izolovanych vrcholu, maximalitu neovlivni
			 */

		/*}
		//pokud existovala hrana (v,v) nastavit
		if(b){
			graph_.setEdge(v,v);
		}
		*/
	}

	static void cut(set_t & set)
	{
		//predpoklad:
	}

	static void expand(const Graph & g_k_,const Graph & m_k_, Graph & graph_, Graph & mapping_,const set_t & set)
		//TODO: pokud M.K lze zlepsit - zlepsi M, konec
	{}

	//true ... zvetsili jsme parovani
		//0
		static bool step(Graph & g,Graph & m)
		{
			std::cout<<"Step"<<std::endl;
			set_t set;
			les_t l;
			Result x=find(g,m,set,l);

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
				return false;
				break;
			default:throw "fatal";
			}
		}
#ifndef DEBUG
public:
#endif
	static Graph FindMaxMapping(Graph & g)
	{
		Graph m(g.getVertices(),g.getEdges());
		while(step(g,m));;
		return m;
	}


};//MappingFinder
};//nsp

#endif//EDMONDS_H_
