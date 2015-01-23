/*
 * finder.cpp
 *
 *  Created on: 20 Jan 2015
 *      Author: alex
 */




#include "edmonds.h"
using namespace nsp;

Result MappingFinder::find(Graph & graph_,Graph & mapping_,set_t & set, les_t & l) throw (InconsistentStructureException)
{
	//najdi volne vrcholy a vloz do f a do l na hladinu 0
	queue_t f=prepare(mapping_,l);

	//hlavni cyklus - dokud fronta neni prazdna
	queue_t::iterator i=f.begin();
	queue_t::iterator end_it=f.end();
	while(i<end_it)
	{
		int v=*i;
		f.pop_front();

		les_t::iterator x=l.find(v);
		if(x!=l.end())
		{
			int h=(*x).second.h;//hladina v

			Neighbours n_v=graph_.getNeighbours(v);

			if(h%2==1)
			{
				Neighbours m_v = mapping_.getNeighbours(v);
				int y=*(m_v.begin());//vrchol spojeny s v parovaci hranou
				if(l.find(y)!=l.end())
				{//y in l
					//y nutne na hladine h
					//najdu VSC nebo Kytku -> konec
					return finder(v,y,set,l);
				}
				else
				{//y not in L
					f.push_back(y);
					edge_t e(v,y);
					WRecord wr=WRecord(y,h+1,e);
					l.insert(pair_t(y,wr));
				}
			}
			else
			{
				std::set<int> y;
				//y mnozina vrcholu dosazitelnych z v po neparovaci hrane
				for(iter x=n_v.begin();x<n_v.end();x++)
				{
					//sousedi v
					if(!mapping_.neighbours(v,(*x)))
					{
						//nejsou sousedi v parovani
						y.insert(*x);
					}
				}

				les_t pom;
				for(std::set<int>::iterator w=y.begin();w!=y.end();++w)
				{
					//existuje vrchol v y, ktery patri do l a je na sude hladine?
					les_t::iterator z=l.find(*w);
					if(z!=l.end())
					{
						//std::cout << "Vrchol "<<(*z).second.v<<", hladina "<<(*z).second.h<<std::endl;
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
						pom.insert(pair_t(*w,rec));
					}
				}


				//pro kazdy vrchol y nepatrici do L : zarad y do fronty
				for(les_t::iterator y=pom.begin();y!=pom.end();++y)
				{
					f.push_back((*y).first);
					l.insert(pair_t((*y).second.v,(*y).second));
				}
			}
		}
		else{
			//err?
			throw InconsistentStructureException();
		}
		i=f.begin();
		end_it=f.end();
	}
	return NONE;
}

int MappingFinder::lookup_root(int vertex, set_t & set, const les_t & l)
{
	//hledam koren ve strome (l)
	int v=vertex;
	les_t::const_iterator x=l.find(v);
	if(x!=l.end()){
		WRecord r=(*x).second;
		while(r.h!=0)
		{
			edge_t e = r.e;
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

bool MappingFinder::step(Graph & g,Graph & m)
{
	//std::cout<<"Step"<<std::endl;
	set_t set;
	les_t l;
	try{
		Result x=find(g,m,set,l);//find muze hodit ISExc. ale to jen znamena bug v programu

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
		default:throw UnexpectedResultException();
		}
	}
	catch(...)//univerzalni catch blok
	{
		std::cout<<"Fatal Error"<<std::endl;
		return false;
	}
}

void MappingFinder::cut(set_t & set)
{
	//v set jsou hrany nalezene kytky i se stonkem
	//stonek se pozna tak, ze jeho hrany jsou v mnozine dvakrat
	//stonek je treba uriznout
	std::cout<<"cut"<<std::endl;
	typedef std::map<int,std::vector<int> > str_t;
	str_t mapa;
	set_t remove;
	for(set_t::iterator it=set.begin();it!=set.end();++it)
	{
		bool duplicit=false;
		str_t::iterator a=mapa.find((*it).first);
		if(a!=mapa.end())
		{
			//rozsiruji vektor, resp. hledam duplikat
			for(std::vector<int>::iterator jt=(*a).second.begin();jt!=(*a).second.end();++jt)
			{
				if((*jt)==(*it).second)
				{
					//nalezen duplikat
					duplicit=true;
					remove.push_back(edge_t((*it).first,(*it).second));
					(*a).second.erase(jt);
					break;//iterator ve foru neplatny
				}
			}
			if(!duplicit)
			{
				//insert
				(*a).second.push_back((*it).second);
			}
		}
		else
		{
			//novy vektor
			std::vector<int> v;
			v.push_back((*it).second);
			mapa.insert(std::pair<int,std::vector<int> >((*it).first,v));
		}
		duplicit=false;
		str_t::iterator b=mapa.find((*it).second);
		if(b!=mapa.end())
		{
			for(std::vector<int>::iterator jt=(*b).second.begin();jt!=(*b).second.end();++jt)
			{
				if((*jt)==(*it).first)
				{
					duplicit=true;
					(*b).second.erase(jt);
					break;
				}
			}
		}
		else
		{
			//novy vektor
			std::vector<int> v;
			v.push_back((*it).first);
			mapa.insert(std::pair<int,std::vector<int> >((*it).second,v));
		}
	}

	for(set_t::iterator it=remove.begin();it!=remove.end();++it)
	{
		std::cout<<"("<<(*it).first<<","<<(*it).second<<")"<<std::endl;
	}

	std::cout<<"==="<<std::endl<<"Remaining"<<std::endl;

	set_t set2;
	for(set_t::iterator it=set.begin();it!=set.end();++it)
	{
		bool toRemove=false;
		for(set_t::iterator jt=remove.begin();jt!=remove.end();++jt)
		{
			if(((*jt).first==(*it).first)&&((*jt).second==(*it).second))
			{
				toRemove=true;
				break;
			}
		}
		if(!toRemove)
		{
			set2.push_back(edge_t((*it).first,(*it).second));
		}
	}
	set=set2;//copy
	for(set_t::iterator it=set.begin();it!=set.end();++it)
	{
		std::cout<<"("<<(*it).first<<","<<(*it).second<<")"<<std::endl;
	}
}

void MappingFinder::shrink(Graph & graph_,Graph & mapping_,const set_t & set)
{
	int v = set.front().first;
	std::cout<<v<<std::endl;
	//zjistit, zda ex. hrana (v,v) v grafu (v parovani nebyla)
	bool b=graph_.neighbours(v,v);


	std::set<int> pom;
	pom.insert(v);
	//hrany kvetu
	for(set_t::const_iterator it=set.begin();it!=set.end();++it)
	{
		int a=(*it).first;
		int b=(*it).second;

		if(pom.find(a)==pom.end())
		{
			pom.insert(a);
			Neighbours n_a=graph_.getNeighbours(a);
			for(iter it=n_a.begin();it!=n_a.end();++it)
			{
				if(!graph_.neighbours(v,(*it)))
				{
					if((v!=(*it))&&((*it)!=-1))
					{
						int i=(*it);
						graph_.unsetEdge(a,i);
						mapping_.unsetEdge(a,i);
						graph_.setEdge(v,i);
					}
				}
			}
		}
		if(pom.find(b)==pom.end())
		{
			pom.insert(b);
			Neighbours n_b = graph_.getNeighbours(b);
			for(iter it=n_b.begin();it!=n_b.end();++it)
			{
				if(!graph_.neighbours(v,(*it)))
				{
					if((v!=(*it))&&((*it)!=-1))
					{
						int i=(*it);
						graph_.unsetEdge(b,i);
						mapping_.unsetEdge(b,i);
						graph_.setEdge(v,i);
					}
				}
			}
		}
	}
	for(std::set<int>::iterator it=pom.begin();it!=pom.end();++it)
	{
		graph_.unsetEdge(v,(*it));
		mapping_.unsetEdge(v,(*it));
	}
	for(set_t::const_iterator it=set.begin();it!=set.end();it++)
	{
		if(((*it).first!=v)&&((*it).second!=v))
		{
			graph_.unsetEdge((*it).first,(*it).second);
		}
		mapping_.unsetEdge((*it).first,(*it).second);
	}
	//pokud existovala hrana (v,v) nastavit
	if(b){
		graph_.setEdge(v,v);
	}
}
