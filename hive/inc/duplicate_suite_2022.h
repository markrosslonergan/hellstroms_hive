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
#include <map>
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

        std::map<int,bool> m_runs;
        std::map<int,bool> m_subruns;
        std::map<int,bool> m_events;

        //This map will tell us if a given RSE is in this runlist
        std::map<RSE,int> m_rses;
        //This map will tell us if a given RS is in this runlist
        std::map<RSE,int> m_rss;



        //Is this R S in this?
        int inList(int &run, int &subrun){
            RSE this_rse(run,subrun,-1); 

            if(m_runs.count(run)>0){
                if(m_rss.count(this_rse)>0){
                    return m_rss[this_rse];
                }
            }
            return 0;
        }

        //Is this r, s r in this?
        bool inList(int &run, int &subrun, const int &event){

            if(event<0) return inList(run,subrun);

            RSE this_rse(run,subrun,event); 
            if(m_runs.count(run)>0 && m_subruns.count(subrun)>0){
                return m_rses[this_rse];
            }
            return 0;
        }

        //Constructor from the vector of RSE
        runlist(std::vector<RSE> rses){
            f_rses = rses;

            for(auto &rse: rses){

                m_rses[rse] = ( m_rses.count(rse)==0 ? 1 : m_rses.count(rse)+1 );

                m_runs[rse.f_rse[0]] = true;
                m_subruns[rse.f_rse[1]] = true;
                m_events[rse.f_rse[2]] = true;

                RSE rs = rse;rs.f_rse[2]=-1;
                m_rss[rs] = true;
            }
        }



        //returns the intersection of the runlist R and the current runlist. Might be 0.
        runlist Intersection(const  runlist &  R){

            std::vector<RSE> ans;

            for(auto &e: R.f_rses){
                if(m_rses.count(e)>0) ans.push_back(e);
            }

            runlist Rans(ans);
            return Rans;
        }

        

        runlist checkDuplicates(){

            std::vector<RSE> ans;

            double dup = 0;
            for(auto &r: f_rses){
                if(m_rses[r]>1){
                    r.Print();
                    std::cout<<m_rses[r]<<std::endl;
                    dup+=1;
                }
            }

            std::cout<<"Total Duplicate Entries "<<dup<<" / "<<f_rses.size()<<std::endl;

            runlist Rans(ans);
            return Rans;

        }

        //This bit is new
        runlist getSubSet(double per){
            std::vector<RSE> ans;
            std::map<RSE,bool> tmp_rss;
            TRandom3 * rang = new TRandom3(0);

            for(auto &rse: f_rses){
                int run = rse.f_rse[0];
                int subrun = rse.f_rse[1];
                RSE rs(run,subrun,-1000);

                if( tmp_rss.count(rs)==1){//have we looked at this subrun before
                    if(tmp_rss[rs]){//is it in the SubSet
                        ans.emplace_back(run,subrun,rse.f_rse[2]);
                    }else{//its not
                        //nothing happends then. 
                    }

                }else{//Lets choose to accept or reject
                    double ran = rang->Uniform(0,1);
                    if(ran>per){
                        tmp_rss[rs] = true;
                        ans.emplace_back(run,subrun,rse.f_rse[2]);
                    }else{
                        tmp_rss[rs] = false;
                    }


                }//end of first pass

            }//end of f_rses loop

            runlist Rans(ans);

            return Rans;
        }



        //Just prints them simply
        int Print(){
            for(auto &e: f_rses){
                e.Print();
            }
            return 0;
        }


};


#endif
