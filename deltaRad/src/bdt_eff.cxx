#include "bdt_eff.h"


bdt_efficiency::bdt_efficiency(bdt_file* filein, std::string denomin, double c1, double c2) : file(filein), denominator(denomin){


		//First step, find event entrylist. In future we must actually track this from the event_tree
		int  event_number=0;
		int  run_number=0;
		int  subrun_number=0;
		double wei=0;
		int pass_denom = 0;
	
		double weighted_num = 0;
		double base_num = 0;

		file->tvertex->ResetBranchAddresses();

		file->tvertex->SetBranchAddress("event_number",&event_number);
		file->tvertex->SetBranchAddress("bnbcorrection_info.weight",&wei);

		//TPCActive
		//Z: 0 to 1036.8
		//X: 0 to 256.35
		//Y: -116.5 to 116.5

		std::set<int> eventIDs;
		event_entry_list = new TEntryList(file->tvertex);

		for(int k=0; k< file->tvertex->GetEntries();k++){
			file->tvertex->GetEntry(k);
			if(k%10000==0) std::cout<<k<<std::endl;	
			if(eventIDs.count(event_number)==0){
				eventIDs.insert(event_number);
				event_entry_list->Enter(k,file->tvertex);
				weighted_num+=wei;
				base_num++;
			}	
		}

		double MOD =file->scale_data*6.6e20/file->pot;
		double volCryo = 199668.427885;
		double volTPC =  101510.0;
		double volTPCActive=  86698.6;

		file->tvertex->SetEntryList(event_entry_list);


		double Ndenominator = file->GetEntries(denominator.c_str());

		std::cout<<"====================Raw Numbers of Events==================="<<std::endl;
		std::cout<<"1: Number of events in FillLightFiles : "<<file->numberofevents_raw<<std::endl;
		std::cout<<"2: Number of events in FillLightFiles scaled to TPCActive: "<<file->numberofevents_raw*volTPCActive/volTPC<<std::endl;
		std::cout<<"3: Number of unique events in vertexed_files : "<<base_num<<std::endl;
		std::cout<<"4: Number of unique events in vertexed_files with BNB_correction : "<<weighted_num<<std::endl;
		std::cout<<"5: Number of approximated events in FillLightFiles with BNB_correction : "<<file->numberofevents_raw*weighted_num/base_num<<std::endl;
		std::cout<<"6: Same scaled to TPCActive: "<<file->numberofevents_raw*weighted_num/base_num*volTPCActive/volTPC<<std::endl;
		double vertex_eff = (weighted_num)/(file->numberofevents_raw*weighted_num/base_num*volTPCActive/volTPC);
		std::cout<<"7: Ratio of (6) and (4): This is a measure of vertexing efficiency : "<<vertex_eff<<std::endl;

		std::cout<<"==================== Denominator events (Here on x3.1) ==================="<<std::endl;
		std::cout<<"Number of events in vertexed_files that pass denominator with BNB_correction : "<<Ndenominator<<std::endl;
		std::cout<<"Number of events in vertexed_files that pass denominator with BNB_correction with vertex eff removed : "<<Ndenominator/vertex_eff<<std::endl;
		double true_denominator = Ndenominator/vertex_eff*MOD;
		std::cout<<"Same scaled to 6.6e20 "<<true_denominator<<std::endl; 
	


		double nverticies;
		for(int s=0; s<4; s++){
			if(s==2) file->calcCosmicBDTEntryList(c1, c2);
			if(s==3) file->calcBNBBDTEntryList(c1, c2);
			file->setStageEntryList(s);

			double stage_entries = file->GetEntries("1")*MOD;
			if(s==0) nverticies = stage_entries;
			std::cout<<"Stage: "<<s<<" Verticies: "<<stage_entries<<" Efficiency: "<<stage_entries/true_denominator<<std::endl;
		}


		std::cout<<"==================== Precuts - One by One  ==================="<<std::endl;
			file->setStageEntryList(0);

			std::string thiscut = "1";
			for(int m=0; m< file->flow.vec_pre_cuts.size(); m++){
				thiscut += "&&"+ file->flow.vec_pre_cuts.at(m);
				double np = file->GetEntries(thiscut.c_str())*MOD;
				std::cout<<" + "<<file->flow.vec_pre_cuts.at(m)<<"\t||\t"<<np<<"\t("<<np/nverticies*100<<")\%"<<std::endl;
			}




}



