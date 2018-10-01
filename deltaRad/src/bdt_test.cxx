#include "bdt_test.h"

int bdt_test::RunTests(){

    TTreeFormula * tf_topological = new TTreeFormula("tf_top", file->flow.topological_cuts.c_str(),file->tvertex);
    TTreeFormula * tf_definition = new TTreeFormula("tf_def", file->flow.definition_cuts.c_str(), file->tvertex);

      int N = file->tvertex->GetEntries(); 
    for(int i=0; i< N; i++){
        file->tvertex->GetEntry(i);


         if(tf_definition->EvalInstance() && tf_topological->EvalInstance() ) {

        for( auto &v : vars){


            TTreeFormula * tf_v = new TTreeFormula(("tf_"+v.safe_name).c_str(), v.name.c_str(), file->tvertex);

          //  std::cout<<"bdt_test::RunTests()\t||\t Starting on file: "<<file->tag<<" with variable: "<<v.name<<std::endl;



            tf_topological->GetNdata();
            tf_definition->GetNdata();
            tf_v->GetNdata();




                double ans = tf_v->EvalInstance();

                if(ans !=ans){
                    std::cout<<"bdt_test::RunTests()\t||\tNAN ("<<ans<<") found in "<<file->tag<<" in variable "<<v.name<<" in entry "<<i<<std::endl;
                }
                if(std::isinf(ans)){
                    std::cout<<"bdt_test::RunTests()\t||\tINF ("<<ans<<") found in "<<file->tag<<" in variable "<<v.name<<" in entry "<<i<<std::endl;
                }


            delete tf_v;
            }
        }
         if( i%(N/100) == 0){ 
             std::cout<<std::setprecision(3)<<"\r"<< 100*i/N+1 <<"\% of "<< N <<" entries complete.";//+1 is for the display 100% 
             std::cout.flush();
         }

    }


    return 0;
}

