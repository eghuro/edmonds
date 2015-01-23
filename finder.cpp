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
