/*#include "object_helper.h"
#include "get_pot_info.h"
#include "train.h"
#include "app.h"
#include "merge.h"
#include "significance.h"
#include "get_mva_response_hists.h"
#include "plot_mva_response_hists.h"
#include "gen_tlimits.h"
#include "plotstack.h"*/
//#include "data_mc_testing_suite.h"
//#include "efficiency.h"

#include "load_mva_param.h"
#include "tinyxml.h"

#include <getopt.h>


#include "bdt_file.h"
#include "bdt_datamc.h"
#include "bdt_var.h"
#include "bdt_precalc.h"
#include "bdt_info.h"
#include "bdt_train.h"
#include "bdt_app.h"
#include "bdt_response.h"
#include "bdt_recomc.h"
#include "bdt_sig.h"
#include "bdt_spec.h"

#include <fstream>
#include <string>


int main (int argc, char *argv[]){

    // Just some simple argument things
    //===========================================================================================

    std::ifstream file("train_log_1g0p_v0.txt");
    std::string str;
    std::string file_contents;

    std::ofstream myfile;
    std::string outfile = "table_training_vars_1g0p.txt";
    myfile.open (outfile);
    // myfile << "Writing this to a file.\n";

    std::vector<std::string> bdt_tag;
    std::vector<std::string> var_name;
    std::vector<std::vector<std::string>> var_scores_vec;
    bool store_vars = false;
    bool first = true;
    std::string gain;
    std::size_t start_var;
    std::size_t end_var;

    std::size_t start_gain;
    std::size_t end_line;

    std::string temp_name;
    int i= 0;

    std::string rel_gain  = "relative gain:";
    std::string variable = "Variable:";
    while (std::getline(file, str))
    {
        if(str.rfind("----------- Sort By Uses:", 0)== 0){
            store_vars = false;
            first = false;
            i = 0;

        }

        if (store_vars ==true){

            if (first){
                std::string first_name = "";
                std::vector<std::string> temp_gain;

                start_var = str.find("Variable:");
                end_var = str.find("-");

                start_gain = str.find("relative gain:");
                end_line = str.find("\n");

                if (end_var>start_var){
                    first_name  = str.substr(start_var+ variable.length(),end_var-start_var);
                    var_name.push_back(first_name);
                }else{
                    std::cout<<"out of bounds"<<std::endl;
                    first_name = "";
                }

                if(end_line>start_gain){

                    gain = str.substr(start_gain + rel_gain.length(),end_line-(start_gain + rel_gain.length()));

                }else{
                    gain = "-";
                }


                temp_gain.push_back(gain);
                var_scores_vec.push_back(temp_gain);

                //   std::cout<<i<<"temp_name = "<<temp_name <<", gain = "<<gain<<std::endl;

            } else{
                start_gain = str.find("relative gain:");
                end_line = str.find("\n");

                if(end_line>start_gain){

                    gain = str.substr(start_gain + rel_gain.length(),end_line-(start_gain + rel_gain.length()));

                }else{
                    gain = "-";
                }

                start_var = str.find("Variable:");
                end_var = str.find("-");

                start_gain = str.find("relative gain:");
                end_line = str.find("\n");

                if (end_var>start_var){
                    temp_name  = str.substr(start_var +variable.length(),end_var-start_var- variable.length());
                } else{
                    temp_name = "";
                }

                if (gain!="-"){
                    var_name[i] = temp_name;
                }

                /*std::cout<<"var_name[i] = "<<var_name[i]<<", var_name[0] "<<var_name[0]<<", temp_name "<<temp_name<<std::endl; 
                  if (var_name[i]== var_name[0] && temp_name != var_name[0] ){
                // if (temp_name!= "Corrected Calorimetric Shower Energy [MeV]"){
                var_name[i] = temp_name;
                //}
                }

                //   if (bdt_tag.size() == 3){
                // std::cout<<i<<bdt_tag.size() <<"var name = "<<temp_name<<std::endl;
                // }
                */
                var_scores_vec[i].push_back(gain);
                i++;


            }

        }

        if (str.rfind("----------- Sort By Input Order:", 0) == 0){
            std::size_t start = str.find("1g");
            std::size_t end = str.find("----------------------");
            //   std::cout<<"start/end = "<<start<<"/"<<end<<std::endl; 
            std::string bdt  = str.substr(start,end-start);   
            store_vars = true;
            bdt_tag.push_back(bdt);
            //  std::cout<<bdt<<std::endl;  

            // file_contents += str;
            //file_contents.push_back('\n');
        }
    }

    for (std::string b: bdt_tag){
        myfile << b<< ", ";
    }
    myfile<<"\n";

    double percent ;
    for (int i = 0; i< var_scores_vec.size(); i++){
        std::vector<std::string> v =  var_scores_vec[i];
        std::string full_gain = "";
        bool train = false;
        for (std::string s: v){
            if (s != "-"){
                train = true;
                size_t ix;    
                percent= std::stod (s, &ix); 
                //percent = std::stod(s) * 100;

                full_gain+=" & "+ to_string_prec(percent*100, 1);

                // std::cout<<to_string_prec(s, 2)<<std::endl;
            } else{
                full_gain+=" & "+ s;
            }

            //std::cout<<"s "<<s<<std::endl;
        }
        //std::cout<<"\n";
        if(train== true){
            myfile<<var_name[i];
            myfile<<full_gain<<"\\\\ \\hline \n";
           // myfile<<"\\hline \n";
        }


    }  

    std::cout<<"writing to outfile "<<outfile<<std::endl;
    myfile.close();
    return 0;

}
