#include "bdt_test.h"


int bdt_test::CompareVars(std::vector<bdt_file*> comp_files){


        file->setStageEntryList(1);
        for(int i=0; i< comp_files.size(); i++){
            comp_files[i]->setStageEntryList(1);
        }


        for( auto &v : vars){
            std::cout<<"On variable "<<v.unit<<std::endl;

            TCanvas *c = new TCanvas((v.safe_unit+file->tag).c_str(),(v.safe_unit+file->tag).c_str(),1200,1200);
            TPad*p =(TPad*)c->cd();
            p->SetLogy();
              
            TH1* tmp = (TH1*)file->getTH1(v, "1",  (file->tag+"_"+v.safe_unit), 6.6e20);
            c->cd();
            tmp->SetLineStyle(1);
            tmp->DrawCopy("hist");

            for(int i=0; i<comp_files.size(); i++){
                TH1* tmp2 = (TH1*)comp_files[i]->getTH1(v, "1",  (comp_files[i]->tag+"_"+v.safe_unit), 6.6e20);
                c->cd();
                tmp2->SetLineStyle(2);
                tmp2->DrawCopy("hist same");
            }
            
            c->SaveAs(("test_compare_"+v.safe_name+".pdf").c_str(),"pdf");
        }




    return 0;

}


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

