#include "variable_list.h"
using namespace std;

variable_list::variable_list(std::string analysis_tag_in): analysis_tag(analysis_tag_in){

    //*******************************************************************************//
    //*******************************************************************************//
    //****************   Single Photon ||   track  and ncdelta 1g0p ***********//
    //*******************************************************************************//
    //*******************************************************************************//


    if(analysis_tag == "track" || analysis_tag == "notrack" || analysis_tag == "1g2p"){



	//std::string fiducial_vertex_truth = "mctruth_nu_vertex_x > 10 && mctruth_nu_vertex_x < 246 && mctruth_nu_vertex_y > -107 && mctruth_nu_vertex_y < 107 && mctruth_nu_vertex_z > 10 && mctruth_nu_vertex_z < 1026 ";
	//
	std::string angle_track_shower ="(reco_track_dirx[0]*reco_shower_dirx[0]+reco_track_diry[0]*reco_shower_diry[0]+reco_track_dirz[0]*reco_shower_dirz[0])";
	std::string fiducial_vertex = "reco_vertex_x > 10 && reco_vertex_x < 246 && reco_vertex_y > -107 && reco_vertex_y < 107 && reco_vertex_z > 10 && reco_vertex_z < 1026 ";
	std::string fiducial_shower_end = "reco_shower_endx > 5 && reco_shower_endx < 245 && reco_shower_endy > -105 && reco_shower_endy < 95 && reco_shower_endz > 10 && reco_shower_endz < 1026 ";
	std::string fiducial_track_end = "reco_track_endx > 5 && reco_track_endx < 245 && reco_track_endy > -95 && reco_track_endy < 95 && reco_track_endz > 10 && reco_track_endz < 1026 ";
	std::string fiducial_cut = fiducial_vertex;//"&&"+fiducial_shower_end;

	std::string proton_mass = "0.938272";
	std::string reco_shower_momentum = "(reco_shower_energy[0]*0.001)"; 
	std::string reco_track_energy = "(reco_track_proton_kinetic_energy[0]+"+proton_mass + ")";
	std::string reco_track_momentum  = "sqrt("+reco_track_energy+"*"+reco_track_energy+"-"+proton_mass +"*"+proton_mass+")";
	std::string invariant_mass = "("+proton_mass+"*"+proton_mass+"+2.0*("+reco_track_energy+"*"+reco_shower_momentum+"-"+reco_track_momentum+"*"+reco_shower_momentum+"*"+angle_track_shower+"))";

	std::string delta_transverse_momentum = "sqrt( pow("+reco_track_energy +"*(reco_track_dirx[0])+ "+reco_shower_momentum+"*reco_shower_dirx[0],2)+ pow("+reco_track_energy +"*(reco_track_diry[0])+"+reco_shower_momentum +"*reco_shower_diry[0],2))";


	std::string new_precuts;
	std::string good_calo_cut = "reco_track_good_calo[0] > 0";
	std::string track_length_cut = "reco_track_displacement[0]<250";
	std::string min_shower_energy_cut = "reco_shower_energy[0]>0.00 ";
	std::string min_conversion_cut = "reco_shower_conversion_distance[0] > 0";
	std::string back_to_back_cut = "("+angle_track_shower+" > -1 &&"  + angle_track_shower + "< 1)";
	std::string pe_cut = "reco_flash_total_pe_in_beamgate[0] > 20";
	std::string shower_dedx_cut = "reco_shower_dEdx_plane2_nhits[0]>1";


	//************************************************* Shower Only Variables **********************************************//

	//all_vars.push_back(bdt_variable("sqrt( pow(mctruth_nu_vertex_x-reco_vertex_x,2)+ pow(mctruth_nu_vertex_y-reco_vertex_y,2)+ pow(mctruth_nu_vertex_z-reco_vertex_z,2))","(48,0,100)"," Reco-True Vetrex Dist [cm]",false,"d"));  //1

	all_vars.push_back(bdt_variable("reco_flash_total_pe_in_beamgate[0]","(48,0,5000)","Total in Beam-Gate PE",false,"d"));  //1
	all_vars.push_back(bdt_variable("reco_shower_dEdx_plane2_median[0]","(48,0,6.5)", "Median Shower dE/dx plane 2 [MeV/cm]",false,"d")); //2
	all_vars.push_back(bdt_variable("reco_shower_energy[0]/1000.0","(48,0,0.8)","Reconstructed Shower Energy [GeV]", false,"d"));  //3
	all_vars.push_back(bdt_variable("reco_shower_length[0]","(48,0,100)","Shower Length [cm]",false,"d"));//4

	all_vars.push_back(bdt_variable("reco_shower_opening_angle[0]","(48,0,0.8)","Shower Opening Angle",false,"d"));//5
	all_vars.push_back(bdt_variable("reco_shower_flash_shortest_distz[0]","(48,0,520)","Distance from Shower to Flashcenter [cm]",false,"d"));//6

	all_vars.push_back(bdt_variable("cos(reco_shower_theta_yz[0])","(48,-1,1)","Reconstructed Shower - Cosine Theta", false,"d"));//8
	all_vars.push_back(bdt_variable("cos(reco_shower_phi_yx[0])","(48,-1,1)","Reconstructed Shower - Cosine Phi", false,"d"));//9
	all_vars.push_back(bdt_variable("reco_shower_startx[0]","(48,0,250)"," Reconstructed Shower start X pos [cm]",false,"d"));//10
	all_vars.push_back(bdt_variable("reco_shower_starty[0]","(48,-110,+110)","Reconstructed Shower start Y pos [cm]",false,"d"));//11
	all_vars.push_back(bdt_variable("reco_shower_startz[0]","(48,0,1050)","Reconstructed Shower start Z pos [cm]",false,"d"));//12
	all_vars.push_back(bdt_variable("log10(reco_shower_conversion_distance[0])","(48,-2,4)","Log Reconstructed Shower conversion distance","false","d"));
	//  all_vars.push_back(bdt_variable("reco_shower_delaunay_area_plane2[0]","(48,0,200e3)","Reconstructed Shower delaunay area plane 2","false","d"));

	// all_vars.push_back(bdt_variable("reco_shower_start_to_nearest_dead_wire_plane0[0]","(48,0,250)"," Reconstructed Shower dead wire dist plane 0 pos [cm]",false,"d"));//10
	//all_vars.push_back(bdt_variable("reco_shower_start_to_nearest_dead_wire_plane1[0]","(48,-110,+110)","Reconstructed Shower dead wire dist plane 1 pos [cm]",false,"d"));//11
	//all_vars.push_back(bdt_variable("reco_shower_start_to_nearest_dead_wire_plane2[0]","(48,0,1050)","Reconstructed Shower dead wire dist plane 2 pos [cm]",false,"d"));//12


	//************************************************* Track + Shower Only Variables **********************************************//
	if(analysis_tag == "track"){
	    //all_vars.push_back(bdt_variable( "-log((track_info.reco_track_principal_2[0]+track_info.reco_track_principal_1[0])/track_info.reco_track_principal_0[0])","(48,0,30)","Track PCA straightness","true","d"));//15

	    all_vars.push_back(bdt_variable("reco_track_proton_kinetic_energy[0]","(48,0,2)","Reconstructed Track Kinetic Energy [GeV]","true","d"));
	    all_vars.push_back(bdt_variable( reco_track_energy +"*(reco_track_dirx[0])+ "+reco_shower_momentum+"*reco_shower_dirx[0]","(48,-2,2)","Reconstructed Delta Momentum X [GeV]",true,"d"));//19
	    all_vars.push_back(bdt_variable( reco_track_energy +"*(reco_track_diry[0])+ "+reco_shower_momentum+"*reco_shower_diry[0]","(48,-2,2)","Reconstructed Delta Momentum Y [GeV]",true,"d"));//20
	    all_vars.push_back(bdt_variable( reco_track_energy +"*(reco_track_dirz[0])+ "+reco_shower_momentum+"*reco_shower_dirz[0]","(48,-1.5,2)","Reconstructed Delta Momentum Z [GeV]",true,"d"));//21
	    all_vars.push_back(bdt_variable( "sqrt( pow("+reco_track_energy +"*(reco_track_dirx[0])+ "+reco_shower_momentum+"*reco_shower_dirx[0],2)+ pow("+reco_track_energy +"*(reco_track_diry[0])+"+reco_shower_momentum +"*reco_shower_diry[0],2))","(48,0,2)","Reconstucted Delta Transverse (YX) Momentum [GeV]",true,"d"));//22
	    all_vars.push_back(bdt_variable("reco_track_mean_trunc_dEdx[0]", "(48,0,12)","Mean Track dE/dx [MeV/cm]", true,"d"));//23
	    all_vars.push_back(bdt_variable("reco_track_mean_trunc_dEdx_start_half[0]/reco_track_mean_trunc_dEdx_end_half[0]", "(48,0,3.5)","Ratio of Mean Start/End Track dE/dx", true,"d"));//24
	    all_vars.push_back(bdt_variable("reco_track_trunc_PIDA[0]","(48,0,24)","Track PIDA",true,"d"));//25
	    all_vars.push_back(bdt_variable(invariant_mass,"(48,1,2.5)","Invariant Mass",true,"d"));//28
	    all_vars.push_back(bdt_variable("cos(reco_track_theta_yz[0])","(48,-1,1)","Reconstructed Track - Cosine Theta", true,"d"));//29
	    all_vars.push_back(bdt_variable("cos(reco_track_phi_yx[0])","(48,-1,1)","Reconstructed Track - Cosine Phi", true,"d"));//30
	    all_vars.push_back(bdt_variable(angle_track_shower,	"(48,-1,1)","Cosine Track-Shower Angle ",true,"d"));//31
	    all_vars.push_back(bdt_variable("reco_track_endx[0]","(48,0,250)"," Reconstructed Track End X pos [cm]",true,"d"));//32
	    all_vars.push_back(bdt_variable("reco_track_endy[0]","(48,-110,+110)","Reconstructed Track End Y pos [cm]",true,"d"));//33
	    all_vars.push_back(bdt_variable("reco_track_endz[0]","(48,0,1050)","Reconstructed Track End Z pos [cm]",true,"d"));//34
	    all_vars.push_back(bdt_variable("reco_track_startx[0]","(48,0,250)"," Reconstructed Track Start X pos [cm]",true,"d"));//32
	    all_vars.push_back(bdt_variable("reco_track_starty[0]","(48,-110,+110)","Reconstructed Track Start Y pos [cm]",true,"d"));//33
	    all_vars.push_back(bdt_variable("reco_track_startz[0]","(48,0,1050)","Reconstructed Track Start Z pos [cm]",true,"d"));//34
	    all_vars.push_back(bdt_variable("reco_track_spacepoint_principal0[0]","(48,0,1)","Recontstructed Track Spacepoint Principal0",true,"d"));//34

	}else if(analysis_tag == "1g2p"){

	    all_vars.push_back(bdt_variable("reco_track_proton_kinetic_energy[reco_track_ordered_energy_index[0]]","(48,0,2)","Reconstructed Track Kinetic Energy 1st [GeV]","true","d"));
	    all_vars.push_back(bdt_variable("reco_track_proton_kinetic_energy[reco_track_ordered_energy_index[1]]","(48,0,2)","Reconstructed Track Kinetic Energy 2nd [GeV]","true","d"));

	}

	all_precuts.push_back(min_shower_energy_cut);
	all_precuts.push_back(pe_cut);
	all_precuts.push_back(fiducial_cut);
	all_precuts.push_back(shower_dedx_cut);

	if(analysis_tag == "track"){
	    all_precuts.push_back(track_length_cut);
	    all_precuts.push_back(min_conversion_cut);
	    all_precuts.push_back(good_calo_cut);
	    all_precuts.push_back(back_to_back_cut);
	}else if(analysis_tag=="1g2p"){
	    all_precuts.push_back("reco_track_good_calo[0] > 0 && reco_track_good_calo[1]>0");
	}


	//*******************************************************************************//
	//*******************************************************************************//
	//*************  LEENue "electron1" for 1e1p and "electron0" for 0p  ************//
	//*******************************************************************************//
	//*******************************************************************************//

				//first two arguments are for analysis_tag 
    }else if( analysis_tag.compare(0,8,"electron",0,8) == 0){//if first 8 letters of tag matches electron
	
	//prepare variables:
	/* Particle Mass [GeV]
	 *  n	    0.93957
	 *  p	    0.93828
	 *  e	    0.000511
	 */  
	
	std::string angle_track_shower ="(reco_track_dirx[0]*reco_shower_dirx[0]+reco_track_diry[0]*reco_shower_diry[0]+reco_track_dirz[0]*reco_shower_dirz[0])";
	std::string fiducial_vertex = "reco_vertex_x > 10 && reco_vertex_x < 246 && reco_vertex_y > -107 && reco_vertex_y < 107 && reco_vertex_z > 10 && reco_vertex_z < 1026 ";
//	std::string fiducial_shower_end = "reco_shower_endx > 5 && reco_shower_endx < 245 && reco_shower_endy > -105 && reco_shower_endy < 95 && reco_shower_endz > 10 && reco_shower_endz < 1026 ";
//	std::string fiducial_track_end = "reco_track_endx > 5 && reco_track_endx < 245 && reco_track_endy > -95 && reco_track_endy < 95 && reco_track_endz > 10 && reco_track_endz < 1026 ";
	std::string fiducial_cut = fiducial_vertex;//"&&"+fiducial_shower_end;

	std::string proton_mass = "0.938272";
	std::string Mn_b = "(0.93957-0.04)";//neutron mass - binding energy	
	

	std::string reco_shower_momentum = "(reco_shower_energy[0])/1000"; //default unit for shower is MeV 
	std::string reco_track_energy = "(reco_track_proton_kinetic_energy[0]+"+proton_mass + ")";
	std::string reco_track_momentum  = "sqrt("+reco_track_energy+"*"+reco_track_energy+"-"+proton_mass +"*"+proton_mass+")";

	//std::string QEE_e = "8/8*(0.5* (2* "+Mn_b+"* (reco_shower_helpernew_energy[0]+0.000511)-(pow("+Mn_b+",2)+(pow(0.000511,2)-pow(0.93827,2))))/("+Mn_b+"-(reco_shower_helpernew_energy[0]+0.000511)+"+shower_mom+"* reco_shower_dirz[0]))";
	std::string QEE_e = "8/8*(0.5* (2* "+Mn_b+"* (reco_shower_energy[0]/1000)-(pow("+Mn_b+",2)+(-pow(0.93827,2))))/("+Mn_b+"-(reco_shower_energy[0]/1000)+"+reco_shower_momentum+"* reco_shower_dirz[0]))";
	std::string E_range = "reco_shower_energy[0]/1000+"+reco_track_energy+"-"+Mn_b;
//	std::string invariant_mass = "("+proton_mass+"*"+proton_mass+"+2.0*("+reco_track_energy+"*"+reco_shower_momentum+"-"+reco_track_momentum+"*"+reco_shower_momentum+"*"+angle_track_shower+"))";

//	std::string delta_transverse_momentum = "sqrt( pow("+reco_track_energy +"*(reco_track_dirx[0])+ "+reco_shower_momentum+"*reco_shower_dirx[0],2)+ pow("+reco_track_energy +"*(reco_track_diry[0])+"+reco_shower_momentum +"*reco_shower_diry[0],2))";

	//distance of track ahead of a shower (this "distance" is negative, if the track is ahead of the shower)
	std::string min_disz = "min(reco_track_endz[0]-reco_shower_startz[0],(reco_track_startz[0]-reco_shower_startz[0]))";
	std::string min_disy = "min(-reco_track_endy[0]+reco_shower_starty[0],(-reco_track_starty[0]+reco_shower_starty[0]))";
	std::string min_disx = "min(reco_track_endx[0]-reco_shower_startx[0],(reco_track_startx[0]-reco_shower_startx[0]))";

//	std::string new_precuts;
	std::string good_calo_cut = "reco_track_good_calo[0] > 0";
	std::string track_length_cut = "reco_track_displacement[0]<250";//in [cm]
	std::string min_shower_energy_cut = "reco_shower_energy[0]>0.00 ";
//	std::string min_conversion_cut = "reco_shower_conversion_distance[0] > 0";
//	std::string back_to_back_cut = "("+angle_track_shower+" > -1 &&"  + angle_track_shower + "< 1)";
	std::string pe_cut = "reco_flash_total_pe_in_beamgate[0] > 20";
	std::string shower_dedx_cut = "reco_shower_dEdx_plane2_nhits[0]>1";//it means shower detected in plane2?

	
	//-------------------- Shared Variables for all electron analysis -----------------------
	//1.precuts
	all_precuts.push_back(pe_cut);
	all_precuts.push_back(fiducial_cut);
	all_precuts.push_back(shower_dedx_cut);

	all_precuts.push_back(min_shower_energy_cut);
	
	//2.bnb cuts
	if( analysis_tag.compare(9,6,"electron*cosmic",9,6)!=0 ){//BNB varaibles


	all_vars.push_back(bdt_variable("1/1*reco_shower_dEdx_plane2_median[0]","(48,0,6.5)", "Median Shower dE/dx plane 2 [MeV/cm]",false,"d"));

	}
	//3.cosmic cuts
	
	//4.general cuts
	
	//-------------------- End of Shared Variables for all electron analysis ----------------


	//== 1e0p
	if( analysis_tag.compare(8,1,"electron0",8,1)==0 ){

	}


	//== 1e1p
	if( analysis_tag.compare(8,1,"electron1",8,1)==0 ){
	    //1.precuts

	    all_precuts.push_back(track_length_cut);
	    all_precuts.push_back(good_calo_cut);//track with sufficient hits.

	    //2.bnb cuts: N/A
	    //3.cosmic cuts
	    if( analysis_tag.compare(9,3,"electron1bnb",9,3)!=0 ){//cosmic varaibles

		all_vars.push_back(bdt_variable("2/2*0.5*(abs(reco_shower_diry[0]+reco_track_diry[0]))","(32,0,2)","Portion of Total Momentum Aligned to the y-direction", false, "d"));

		all_vars.push_back(bdt_variable("3/3*(sqrt(1-reco_track_dirz[0]*reco_track_dirz[0])*"+reco_track_momentum+"+sqrt(1-reco_shower_dirz[0]*reco_shower_dirz[0])*"+reco_shower_momentum+")",
			    "(40,0,1.5)","Total Momentum Perpendicular to the z-direction [GeV]", false, "d"));
	    }
	    //4.general cuts
	    all_vars.push_back(bdt_variable("4/4*reco_shower_energy[0]/1000.0","(48,0,2)","Reconstructed Shower Energy [GeV]", false,"d"));  //3
	    all_vars.push_back(bdt_variable("5/5*reco_track_proton_kinetic_energy[0]","(48,0,2)","Reconstructed Track Kinetic Energy [GeV]","true","d"));

	    all_vars.push_back(bdt_variable("6/6*reco_shower_energy[0]/1000.0+reco_track_proton_kinetic_energy[0]","(48,0,2)","Reconstructed Nue Energy [GeV]","true","d"));

	    all_vars.push_back(bdt_variable("7/7*"+QEE_e +"-"+ E_range, "(20,0,2)", "QE_{#nu}^{QE}[e]-E^{range} [GeV]", true, "d"));

	    //>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>> Momentum <<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<
	    all_vars.push_back(bdt_variable("8/8*abs((reco_track_dirz[0]*"+reco_track_momentum+"+reco_shower_dirz[0]*"+reco_shower_momentum+"))","(30,0,2)","Total Momentum on beam-direction [GeV/c]", false, "d"));

	    all_vars.push_back(bdt_variable("9/9*((reco_track_dirz[0]*"+reco_track_momentum+"+reco_shower_dirz[0]*"+reco_shower_momentum+")/(sqrt(1-reco_track_dirz[0]*reco_track_dirz[0])*"+reco_track_momentum+"+sqrt(1-reco_shower_dirz[0]*reco_shower_dirz[0])*"+reco_shower_momentum+"))","(30,0,8)","Ratio of Momentum on z-direction versus on xy-plane", false, "d"));

	    //>>>>>>>>>>>>>>>>>>>>>>>>>>>>> Stopping Power <<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<
	    all_vars.push_back(bdt_variable("10/10*abs(reco_track_mean_trunc_dEdx[0]-reco_shower_dEdx_plane2_median[0])",
			"(30,0,10)", "Difference of dE/dx btw a Track and a Shower [MeV/cm]",false,"d"));

	    all_vars.push_back(bdt_variable("11/11*reco_track_trunc_PIDA[0]","(48,0,24)","Track PIDA",true,"d"));//25
	    //>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>> Length <<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<
	    all_vars.push_back(bdt_variable("12/12*abs(max(reco_track_displacement[0],reco_shower_length[0]))","(30,0,150)","Maximum Length of a Track or a Shower [cm]", true,"d"));//note: muon is to be killed by evaluating the distance btw shower and track

	    //>>>>>>>>>>>>>>>>>>>> Distance btw shower and track <<<<<<<<<<<<<<<<<<<<<<<<<<<<
	    all_vars.push_back(bdt_variable("13/13*min("+ min_disz + ", min("+ min_disy +"," + min_disx + "))","(50,-100,5)", "A Track Ahead of a Shower (all-direction)[cm]",true,"d"));
	
	    all_vars.push_back(bdt_variable("14/14*reco_shower_conversion_distance[0]","(52,0,20)","Shower Conversion Length from Reconstructed Vertex [cm]" ,false,"d"));
	    all_vars.push_back(bdt_variable("15/15*abs("+angle_track_shower+")","(15,0,1)","|Cosine Track-Shower Angle|",true,"d"));//for back-to-back cut
	
	    //>>>>>>>>>>>>>>>>>>>>>>>>>>>>>> Others <<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<
	    all_vars.push_back(bdt_variable("16/16*reco_shower_opening_angle[0]","(20,0,0.8)","Shower Opening Angle [rad]",false,"d"));
	    all_vars.push_back(bdt_variable("17/17*2*(reco_shower_length[0]*tan(reco_shower_opening_angle[0]/2))","(30,0,20)","Base Length of a Shower [cm]", false, "d"));
	
	
	}

	
	//== 1e2p
	if( analysis_tag.compare(8,1,"electron2",8,1)==0){//1e2p

	    all_precuts.push_back("reco_track_good_calo[0] > 0 && reco_track_good_calo[1]>0");

	}

    }

};

