#ifndef DUP_SUI_H
#define DUP_SUI_H

#include <vector> 
#include "TRandom3.h"
#include "TTree.h"
#include "TFile.h"
#include <string> 
#include <set> 
#include <numeric> 
#include <iostream> 
#include <algorithm>
#include <memory>
struct RSE{

    //simple structure to store run,subrun,events, will be expanded to pot and so firth
    std::vector<int> f_rse;
    RSE(int r, int s, int e){
        f_rse.resize(3);
        f_rse[0]=r;
        f_rse[1]=s;
        f_rse[2]=e;
    }

    //Ideally should define hash to use unordered_set.
    //some comparative operators, for mapping and such. What does it mean for a RSE to be < = or > than another RSE,
    bool operator==( const RSE& rhs)
    {
        return (this->f_rse[0]==rhs.f_rse[0] &&this->f_rse[1]==rhs.f_rse[1] && this->f_rse[2]==rhs.f_rse[2] );
    }

   
    bool operator<(const RSE& X) const {
        if(this->f_rse[0]>X.f_rse[0]) return false;
        if(this->f_rse[0]==X.f_rse[0] && this->f_rse[1]>X.f_rse[1]) return false;
        if(this->f_rse[0]==X.f_rse[0] && this->f_rse[1]==X.f_rse[1] && this->f_rse[2]>=X.f_rse[2]) return false;
        return true;
    }

    bool operator>(const RSE& X) const {
        if(this->f_rse[0]<X.f_rse[0]) return false;
        if(this->f_rse[0]==X.f_rse[0] && this->f_rse[1]<X.f_rse[1]) return false;
        if(this->f_rse[0]==X.f_rse[0] && this->f_rse[1]==X.f_rse[1] && this->f_rse[2]<=X.f_rse[2]) return false;
        return true;
    }

    int Print(){
        std::cout<<f_rse[0]<<" "<<f_rse[1]<<" "<<f_rse[2]<<std::endl;
        return 0;
    }

};


struct runlist{
    //Although a vector of RSE is fine, want some structur for counting, maps and intersection, union..etc..

    public:
        std::vector<RSE> f_rses; 

	// Unique set of {run,subrun, event} lists
	// For every (run,subrun) pair, {run,subrun,-1} is also included
        std::set<RSE> m_rses;
	int unique_events;


        //Constructor from the vector of RSE
        runlist(std::vector<RSE> rses): unique_events(0){
            f_rses = rses;

            for(auto &rse: rses){
		int run = rse.f_rse[0], subrun = rse.f_rse[1], event = rse.f_rse[2];
		if(m_rses.count(rse) == 0){
		    ++unique_events;
		    m_rses.insert(rse);
		    m_rses.insert(RSE(run, subrun, -1));
		}
            }
        }

        runlist(): unique_events(0){}

        //Is this R S in this?
        int inList(const int &run, const int &subrun){
            return m_rses.count(RSE(run,subrun,-1));
        }

        //Is this r, s r in this?
        bool inList(const int &run, const int &subrun, const int &event){

            if(event<0) return inList(run,subrun);
	    return m_rses.count(RSE(run,subrun,event));
        }




        //returns the intersection of the runlist R and the current runlist. Might be 0.
        runlist Intersection(const  runlist &  R){

            std::vector<RSE> ans;

            for(auto &e: R.m_rses){
                if(e.f_rse[2]!=-1 && m_rses.count(e)>0 ) ans.push_back(e);
            }

            return runlist(ans);
        }

        
	//Print out duplicate (run,subrun, event)
        runlist checkDuplicates(){

            std::set<RSE> m_dups = m_rses;
            std::vector<RSE> ans;

            std::cout<<"Total Duplicate Entries "<< f_rses.size() - unique_events <<" / "<<f_rses.size()<<std::endl;
            for(auto &r: f_rses){
                if(m_dups.erase(r) == 0){
		   r.Print();
		}
            }


            runlist Rans(ans);
            return Rans;

        }

        //This bit is new
        //Guanqun comment: randomly grab subruns
        runlist getSubSet(double per){
            std::vector<RSE> ans;
            std::set<RSE> visited_rss_pass, visited_rss_fail;  //Set to record (run,subrun) pairs seen already
            TRandom3 * rang = new TRandom3(0);

            for(auto &rse: f_rses){
                int run = rse.f_rse[0];
                int subrun = rse.f_rse[1];
                RSE rs(run,subrun,-1000);

		if(visited_rss_pass.count(rs)) //If this (run,subrun) pair is seen before and accepted
		    ans.push_back(rse);
		else if(visited_rss_fail.count(rs))  // /If this (run,subrun) pair is seen before but rejected
		    continue;
		else{				    //For new (run,subrun) pair, Lets choose to accept or reject
                    double ran = rang->Uniform(0,1);
                    if(ran>per){
			visited_rss_pass.insert(rs);
                        ans.push_back(rse);
                    }else{
			visited_rss_fail.insert(rs);
                    }
		}
	    } //end of f_rses loop

            return runlist(ans);
        }



        //Just prints them simply
        int Print(){
            for(auto &e: f_rses){
                e.Print();
            }
            return 0;
        }

	int size() const{return f_rses.size();}
};


#endif
