#include "bdt_boxcut.h"

int bdt_boxcut::doCuts(TFile *ftest){
	double plot_pot = 6.6e20;
	ftest->cd();

	

	for(int i=0; i< vars.size(); i++){

		
		TCanvas *c = new TCanvas(vars.at(i).safe_name.c_str());
		c->Divide(2,1);
		c->cd(1);
		TLegend *l0 = new TLegend(0.11,0.72,0.89,0.89);
		l0->SetNColumns(2);


		for(auto &f: mc_stack->stack){
			std::string allcuts = this->core_cuts+"&&"+ f->getStageCuts(0,-9,-9);
			for(int i=0; i<boxcuts.size(); i++){
				allcuts+="&&"+ boxcuts.at(i);
			}


			TH1* htmp= (TH1*)f->getTH1(vars.at(i),allcuts, f->tag+"_"+vars.at(i).safe_name, plot_pot );
			std::cout<<f->tag<<" "<<htmp->GetSumOfWeights()<<std::endl;
			c->cd(1);
			htmp->Draw("same hist");
			htmp->SetMaximum( htmp->GetMaximum()*1.4);
			
			l0->AddEntry(htmp,("#splitline{"+f->tag+"}{"+to_string_prec(htmp->Integral(),2)+"}").c_str(),"lp");	
			c->cd(2);	
			TH1* htmp_scale = (TH1*)htmp->Clone(("scale_"+ f->tag+"_"+vars.at(i).safe_name).c_str());	
			htmp_scale->Scale(1.0/htmp_scale->Integral());	
			c->cd(2);
			htmp_scale->Draw("same hist");
		}


		l0->Draw();
		l0->SetLineWidth(0);
		l0->SetLineColor(0);
		l0->SetFillStyle(0);
		l0->SetTextSize(0.03);
		ftest->cd();
		c->Write();
		c->SaveAs(("boxcut/"+tag+"_"+vars.at(i).safe_unit+".pdf").c_str(),"pdf");



	}

	return 0;
}


int bdt_boxcut::setUpFolders(){





    return 0;
}


	/*
	}else if(mode_option == "boxcut"){
		TFile * fout = new TFile("box.root","recreate");

		std::vector<std::string> boxcuts;

		boxcuts.push_back("reco_shower_dedx_plane2[0]>2"); // reco_shower_dedx_plane2[0]
		boxcuts.push_back("reco_shower_helper_energy[0]>0.2"); // reco_shower_helper_energy[0]
		boxcuts.push_back("1"); // reco_shower_length[0]
		boxcuts.push_back("1"); // reco_shower_opening_angle[0]
		boxcuts.push_back("reco_shower_dist_to_closest_flashzcenter[0]<100"); // reco_shower_dist_to_closest_flashzcenter[0e	
		boxcuts.push_back(fiducial_vertex); // reco_nu_vtx_dust to closset wal
		boxcuts.push_back("1"); // showe bp to wall
		boxcuts.push_back("1"); // vrtx X
		boxcuts.push_back("1"); // vrt Y
		boxcuts.push_back("1"); //vert z
		boxcuts.push_back("cos(atan2(reco_shower_diry[0],reco_shower_dirz[0]))> 0.6"); // shower cos theta
		boxcuts.push_back("cos(atan2(reco_shower_diry[0],reco_shower_dirx[0]))> 0.6"); // shower cos phi
		boxcuts.push_back("1"); //shower end x
		boxcuts.push_back("1"); // shower end y
		boxcuts.push_back("1"); // showr end z
		boxcuts.push_back("1"); //pi0num near 
		boxcuts.push_back("reco_track_displacement[0]<100"); //  track length
		boxcuts.push_back("track_info.reco_track_mean_dEdx[0]>2"); // mean dedx
		boxcuts.push_back("track_info.reco_track_start_mean_dEdx[0]/track_info.reco_track_end_mean_dEdx[0]> 1"); //  ratio mean
		boxcuts.push_back("track_info.reco_track_PIDA[0] >  10"); // pida
		boxcuts.push_back("track_info.reco_track_braggA[0]> 10"); // brag A
		boxcuts.push_back("shortest_asso_shower_to_vert_dist[0]>1"); // photon conversion
		boxcuts.push_back("1"); //  invariant mass
		boxcuts.push_back("cos(atan2(reco_track_diry[0],reco_track_dirz[0]))>0"); // track theta
		boxcuts.push_back("1"); // track theta
		boxcuts.push_back(angle_track_shower+">-0.95 && "+ angle_track_shower +"<0.95"); //  track showr angle
		boxcuts.push_back("1"); // track end x
		boxcuts.push_back("1"); // track end y
		boxcuts.push_back("1"); //  track end z
		boxcuts.push_back("1"); // track eneryy

		bdt_stack *obs3 = new bdt_stack(istrack+"_box");
		obs3->plot_pot = 6.6e20;
		obs3->addToStack(signal_cosmics);
		obs3->addToStack(bnb_cosmics);
		bnbext->col = intime->col;	
		bnbext->fillstyle = 3333;
		obs3->addToStack(bnbext);


		bdt_boxcut BOX("boxcut", obs3, boxcuts, vars);
		BOX.core_cuts = "totalpe_ibg_sum > 20 && track_info.reco_track_good_calo[0]>0";
		BOX.doCuts(fout);




	*/


