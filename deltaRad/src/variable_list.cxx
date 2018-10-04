#include "variable_list.h"

variable_list::variable_list(std::string analysis_tag_in): analysis_tag(analysis_tag_in){

    //*******************************************************************************//
    //*******************************************************************************//
    //****************   Single Photon ||   track  and ncdelta 1g0p ***********//
    //*******************************************************************************//
    //*******************************************************************************//

    //cosmic: tag+cosmic; bnb: tag+bnb. 

    if(analysis_tag == "track" || analysis_tag == "notrack"){

	std::string angle_track_shower ="(reco_track_dirx[0]*reco_shower_dirx[0]+reco_track_diry[0]*reco_shower_diry[0]+reco_track_dirz[0]*reco_shower_dirz[0])";
	std::string fiducial_vertex = "reco_nuvertx > 10 && reco_nuvertx < 246 && reco_nuverty > -107 && reco_nuverty < 107 && reco_nuvertz > 10 && reco_nuvertz < 1026 ";
	std::string fiducial_shower_end = "reco_shower_endx > 5 && reco_shower_endx < 245 && reco_shower_endy > -105 && reco_shower_endy < 95 && reco_shower_endz > 10 && reco_shower_endz < 1026 ";
	std::string fiducial_track_end = "reco_track_endx > 5 && reco_track_endx < 245 && reco_track_endy > -95 && reco_track_endy < 95 && reco_track_endz > 10 && reco_track_endz < 1026 ";
	std::string fiducial_cut = fiducial_vertex;//"&&"+fiducial_shower_end;
	std::string bump_region = " reco_shower_helper_energy[0]>0.130 && reco_shower_helper_energy[0]<0.2";

	std::string proton_mass = "0.938272";
	std::string reco_shower_momentum = "(-0.017962 + 1.3719*reco_shower_helper_energy[0])"; 
	std::string reco_track_energy = "(track_info.reco_track_kinetic_from_length[0]+"+proton_mass + ")";
	std::string reco_track_momentum  = "sqrt("+reco_track_energy+"*"+reco_track_energy+"-"+proton_mass +"*"+proton_mass+")";
	std::string invariant_mass = "("+proton_mass+"*"+proton_mass+"+2.0*("+reco_track_energy+"*"+reco_shower_momentum+"-"+reco_track_momentum+"*"+reco_shower_momentum+"*"+angle_track_shower+"))";

	std::string delta_transverse_momentum = "sqrt( pow("+reco_track_energy +"*(reco_track_dirx[0])+ "+reco_shower_momentum+"*reco_shower_dirx[0],2)+ pow("+reco_track_energy +"*(reco_track_diry[0])+"+reco_shower_momentum +"*reco_shower_diry[0],2))";

	std::string new_precuts;

	std::string good_calo_cut = "track_info.reco_track_good_calo[0]>0 && track_info.reco_track_mean_dEdx[0] > 1";
	std::string track_length_cut = "reco_track_displacement[0]<150";
	std::string min_shower_energy_cut = "reco_shower_helper_energy[0]>0.02 ";
	std::string min_conversion_cut = "shortest_asso_shower_to_vert_dist > 1";
	std::string track_direction_cut = "track_info.reco_track_start_mean_dEdx[0]/track_info.reco_track_end_mean_dEdx[0] >= 0.95";
	std::string back_to_back_cut = angle_track_shower+" > -0.975 &&"  + angle_track_shower + "< 0.975";
	std::string pe_cut = "totalpe_ibg_sum > 20";
	std::string flash_proximity_cut = "reco_shower_dist_to_closest_flashzcenter[0] < 80";
	std::string delta_like_cut = delta_transverse_momentum + "< 1.25";

	std::string dead_wire_track = "((reco_track_endz < 685 || reco_track_endz > 715) && shower_info.amalgamated_shower_dEdx[0] < 6.5 )";

	//************************************************* Shower Only Variables **********************************************//

	all_vars.push_back(bdt_variable("totalpe_ibg_sum","(48,0,5000)","Total in Beam-Gate PE",false,"d"));  //1
	all_vars.push_back(bdt_variable("shower_info.amalgamated_shower_dEdx[0]","(48,0,6.5)", "Amalgamated Shower dE/dx [MeV/cm]",false,"d")); //2
	all_vars.push_back(bdt_variable(reco_shower_momentum,"(48,0,0.8)","Reconstructed Shower Energy (Corrected) [GeV]", false,"d"));  //3
	all_vars.push_back(bdt_variable("reco_shower_length[0]","(48,0,100)","Shower Length [cm]",false,"d"));//4

	all_vars.push_back(bdt_variable("reco_shower_opening_angle[0]","(48,0,0.8)","Shower Opening Angle",false,"d"));//5
	all_vars.push_back(bdt_variable("reco_shower_dist_to_closest_flashzcenter[0]","(48,0,520)","Distance from Shower to Flashcenter [cm]",false,"d"));//6

	all_vars.push_back(bdt_variable("reco_nu_vtx_dist_to_closest_tpc_wall","(48,0,120)","Reconstructed Vertex to TPC Wall Distance [cm]",false,"d"));//7
	all_vars.push_back(bdt_variable("cos(atan2(reco_shower_diry[0],reco_shower_dirz[0]))","(48,-1,1)","Reconstructed Shower - Cosine Theta", false,"d"));//8
	all_vars.push_back(bdt_variable("cos(atan2(reco_shower_diry[0], reco_shower_dirx[0]))","(48,-1,1)","Reconstructed Shower - Cosine Phi", false,"d"));//9
	all_vars.push_back(bdt_variable("reco_shower_endx[0]","(48,0,250)"," Reconstructed Shower End X pos [cm]",false,"d"));//10
	all_vars.push_back(bdt_variable("reco_shower_endy[0]","(48,-110,+110)","Reconstructed Shower End Y pos [cm]",false,"d"));//11
	all_vars.push_back(bdt_variable("reco_shower_endz[0]","(48,0,1050)","Reconstructed Shower End Z pos [cm]",false,"d"));//12

	all_vars.push_back(bdt_variable( "fabs(atan2(reco_shower_diry[0],reco_shower_dirz[0]))/3.14159","(48,0,1.0)","Reconstucted Shower Angle Beamy-ness [rad]","false","d")); //13
	all_vars.push_back(bdt_variable( "min(fabs(atan2(reco_shower_diry[0],reco_shower_dirx[0])+3.14159/2.0),fabs(atan2(reco_shower_diry[0],reco_shower_dirx[0])-3.14159/2.0))/3.14159","(48,0,0.5)","Reconstucted Shower Angle Cosmicy-ness [rad]","false","d")); //14



	//************************************************* Track + Shower Only Variables **********************************************//
	if(analysis_tag == "track"){
	    //all_vars.push_back(bdt_variable( "-log((track_info.reco_track_principal_2[0]+track_info.reco_track_principal_1[0])/track_info.reco_track_principal_0[0])","(48,0,30)","Track PCA straightness","true","d"));//15
	    all_vars.push_back(bdt_variable( "fabs(atan2(reco_track_diry[0],reco_track_dirz[0]))/3.14159","(48,0,1.0)","Reconstucted Track Angle Beamy-ness [rad]","true","d")); //15
	    all_vars.push_back(bdt_variable( "min(fabs(atan2(reco_track_diry[0],reco_track_dirx[0])+3.14159/2.0),fabs(atan2(reco_track_diry[0],reco_track_dirx[0])-3.14159/2.0))/3.14159","(48,0,0.5)","Reconstucted Track Angle Cosmicy-ness [rad]","true","d")); //16
	    all_vars.push_back(bdt_variable("log(track_info.reco_track_straightline_chi2[0])","(48,6,16)"," Track Line chi^2",true,"d") );//17
	    all_vars.push_back(bdt_variable("log(track_info.reco_track_sum_angles[0])","(48,-30,20)"," Track Sum of all Angles",true,"d") );//18

	    all_vars.push_back(bdt_variable( reco_track_energy +"*(reco_track_dirx[0])+ "+reco_shower_momentum+"*reco_shower_dirx[0]","(48,-2,2)","Reconstructed Delta Momentum X [GeV]",true,"d"));//19
	    all_vars.push_back(bdt_variable( reco_track_energy +"*(reco_track_diry[0])+ "+reco_shower_momentum+"*reco_shower_diry[0]","(48,-2,2)","Reconstructed Delta Momentum Y [GeV]",true,"d"));//20
	    all_vars.push_back(bdt_variable( reco_track_energy +"*(reco_track_dirz[0])+ "+reco_shower_momentum+"*reco_shower_dirz[0]","(48,-1.5,2)","Reconstructed Delta Momentum Z [GeV]",true,"d"));//21
	    all_vars.push_back(bdt_variable( "sqrt( pow("+reco_track_energy +"*(reco_track_dirx[0])+ "+reco_shower_momentum+"*reco_shower_dirx[0],2)+ pow("+reco_track_energy +"*(reco_track_diry[0])+"+reco_shower_momentum +"*reco_shower_diry[0],2))","(48,0,2)","Reconstucted Delta Transverse (YX) Momentum [GeV]",true,"d"));//22
	    all_vars.push_back(bdt_variable("track_info.reco_track_mean_dEdx[0]", "(48,0,12)","Mean Track dE/dx [MeV/cm]", true,"d"));//23
	    all_vars.push_back(bdt_variable("track_info.reco_track_start_mean_dEdx[0]/track_info.reco_track_end_mean_dEdx[0]", "(48,0,3.5)","Ratio of Mean Start/End Track dE/dx", true,"d"));//24
	    all_vars.push_back(bdt_variable("track_info.reco_track_PIDA[0]","(48,0,24)","Track PIDA",true,"d"));//25
	    all_vars.push_back(bdt_variable("track_info.reco_track_braggA[0]","(48,0,25)","Track Bragg A",true,"d"));//26
	    all_vars.push_back(bdt_variable("shortest_asso_shower_to_vert_dist","(48,0,30)","Photon Conversion Length from Reconstructed Vertex [cm]" ,true,"d"));//37
	    all_vars.push_back(bdt_variable(invariant_mass,"(48,1,2.5)","Invariant Mass",true,"d"));//28
	    all_vars.push_back(bdt_variable("cos(atan2(reco_track_diry[0],reco_track_dirz[0]))","(48,-1,1)","Reconstructed Track - Cosine Theta", true,"d"));//29
	    all_vars.push_back(bdt_variable("cos(atan2(reco_track_diry[0],reco_track_dirx[0]))","(48,-1,1)","Reconstructed Track - Cosine Phi", true,"d"));//30
	    all_vars.push_back(bdt_variable(angle_track_shower,	"(48,-1,1)","Cosine Track-Shower Angle ",true,"d"));//31
	    all_vars.push_back(bdt_variable("reco_track_endx[0]","(48,0,250)"," Reconstructed Track End X pos [cm]",true,"d"));//32
	    all_vars.push_back(bdt_variable("reco_track_endy[0]","(48,-110,+110)","Reconstructed Track End Y pos [cm]",true,"d"));//33
	    all_vars.push_back(bdt_variable("reco_track_endz[0]","(48,0,1050)","Reconstructed Track End Z pos [cm]",true,"d"));//34
	    all_vars.push_back(bdt_variable("reco_track_energy_new_legacy[0]", "(48,0,1)","Track Energy (Best Plane)",true, "d"));//35
	    all_vars.push_back(bdt_variable("track_info.reco_track_kinetic[0]","(48,0,1)","Reconstructed Track Kinetic Energy (Calo) [cm]", true,"d"));//46
	    all_vars.push_back(bdt_variable("track_info.reco_track_kinetic_from_length[0]","(48,0,1)","Reconstructed Track Kinetic Energy (Length) [cm]", true,"d"));//37
	    all_vars.push_back(bdt_variable("track_info.reco_track_kinetic_from_length[0]/track_info.reco_track_kinetic[0]","(48,0,5)","Ratio of Reco Track Kinetic (Length/Calo)", true,"d"));//38

	};

	all_precuts.push_back(pe_cut);
	all_precuts.push_back(fiducial_cut);
	all_precuts.push_back(min_shower_energy_cut);

	if(analysis_tag == "track"){
	    all_precuts.push_back(track_length_cut);
	    all_precuts.push_back(min_conversion_cut);
	    all_precuts.push_back(good_calo_cut);
	    all_precuts.push_back(track_direction_cut);
	    all_precuts.push_back(back_to_back_cut);
	    all_precuts.push_back(dead_wire_track);
	}

//Above are for reference only

	//*******************************************************************************//
	//*******************************************************************************//
	//*************  LEENue "electron1" for 1e1p and "electron0" for 0p  ************//
	//*******************************************************************************//
	//*******************************************************************************//

    }else if( analysis_tag.compare(0,8,"electron",0,8) == 0){//if first 8 letters of tag matches electron
	   /* analysis_tag == "electron1" || analysis_tag == "electron0" ||
	    analysis_tag == "electron1bnb" || analysis_tag == "electron0bnb"  ||
	    analysis_tag == "electron1cosmic" || analysis_tag == "electron0cosmic"*/
	//prepare precuts:
	std::string angle_track_shower ="(reco_track_dirx[0]*reco_shower_dirx[0]+reco_track_diry[0]*reco_shower_diry[0]+reco_track_dirz[0]*reco_shower_dirz[0])";
	std::string fiducial_vertex = "reco_nuvertx > 10 && reco_nuvertx < 246 && reco_nuverty > -107 && reco_nuverty < 107.01 && reco_nuvertz > 10 && reco_nuvertz < 1026 ";
	std::string fiducial_shower_end ="reco_shower_endx > 10 && reco_shower_endx < 246 && reco_shower_endy > -107 && reco_shower_endy < 107.01 && reco_shower_endz > 10 &&       reco_shower_endz < 1026 ";
	std::string fiducial_track_end = "reco_track_endx > 10 && reco_track_endx < 246 && reco_track_endy > -107 && reco_track_endy < 107.01 && reco_track_endz > 10 && reco_track_endz <  1026 ";

	std::string pe_cut = "totalpe_ibg_sum > 20";
	std::string fiducial_cut = fiducial_vertex;//"&&"+fiducial_shower_end;
	std::string track_length_cut = "1";
//	std::string nu_energy_cut = "reco_shower_helper_energy[0]+reco_track_energy_new_legacy[0]<0.4";
	std::string min_conversion_cut = "1";
	std::string good_calo_cut = "track_info.reco_track_good_calo[0]>0 && track_info.reco_track_mean_dEdx[0] > 0.95";
	std::string back_to_back_cut = angle_track_shower+" > -0.95 &&"  + angle_track_shower +  "< 0.95";
//	std::string track_direction_cut = "track_info.reco_track_start_mean_dEdx[0]/track_info.reco_track_end_mean_dEdx[0] >= 0.95";
//	std::string dead_wire_track = "((reco_track_endz < 685 || reco_track_endz > 715) && shower_info.amalgamated_shower_dEdx[0] < 6.5 )";
	std::string shower_open_angle = "reco_shower_opening_angle[0]>0.04";

	//Push_back precuts
	all_precuts.push_back(pe_cut);//eliminate cosmic events
	all_precuts.push_back(fiducial_cut);//eliminate events close to the wall

//	all_precuts.push_back(nu_energy_cut);
//	all_precuts.push_back(track_length_cut);
//	all_precuts.push_back(min_conversion_cut);
	all_precuts.push_back(good_calo_cut);
//	all_precuts.push_back(track_direction_cut);
	all_precuts.push_back(shower_open_angle);
//	all_precuts.push_back(back_to_back_cut);
    

//BDT cuts:

/*

//----------------------------------------------------------->>Energy<<-------------------------------------------------------------

    std::string E_range = "reco_shower_helper_energy[0]+reco_track_energy_new_legacy[0]+0.   000511+0.9382723-(0.93957-0.014)";//0.004? not sure.
    std::string QEE_p = "0.5*(2*(0.93957-0.014)*(reco_track_energy_new_legacy[0]+0.9382723)- (pow(0.93957-0.014,2)+pow(0.9382723,2)-pow(0.000511,2)))/((0.93957-0.014)-                       (reco_track_energy_new_legacy[0]+0.9382723)+sqrt(pow(reco_track_energy_new_legacy[0],2)+2*       reco_track_energy_new_legacy[0]*0.9382723)*reco_track_dirz[0])";
    all_vars.push_back(bdt_variable(QEE_p +"-"+ E_range, "(100,-2,2)", "QE energy of proton with correction", true, "d"));

//---------------------------------------------------------->>Momentum<<-------------------------------------------------------------
	    all_vars.push_back(bdt_variable("4/4*(reco_track_dirz[0]*"+track_mom+"+reco_shower_dirz[0]*"+shower_mom+")/(sqrt(1-reco_track_dirz[0]*reco_track_dirz[0])*"+track_mom+"+sqrt(1-reco_shower_dirz[0]*reco_shower_dirz[0])*"+shower_mom+")","(52,0,8)","Ratio of Momentum on z-direction versus xy plane", false, "d"));

    //----------------------------------------------------------->>Orientation<<-------------------------------------------------------------
    all_vars.push_back(bdt_variable("7/7*reco_shower_dirz/abs(reco_shower_dirz)+2*reco_track_dirz/abs(reco_track_dirz)","(52,-10,10)","SemiAngle Orientation (shower and track     normalized/weighted)", false, "i"));

    //THIS IS RATIO!!
    all_vars.push_back(bdt_variable("8/8*((reco_shower_dirz+reco_track_dirz)/sqrt(pow(reco_shower_diry+reco_track_diry,2)+pow(reco_shower_dirx+reco_track_dirx,2)))","(52,-4, 20)","Track and Shower Aligned to z-direction", false, "d"));

    //----------------------------------------------------------->>Stopping Power<<-------------------------------------------------------------

    all_vars.push_back(bdt_variable("9/9*(reco_shower_dedx_plane0[0]+reco_shower_dedx_plane1[0]+ reco_shower_dedx_plane2[0])",
		"(52,0,30)", "dE/dx on Shower in All Planes [MeV/cm]",false,"d"));

    //-------------------------------------------------->>Distance btw shower and track<<------------------------------------------------------------

    //mainly to rule out long track
  all_vars.push_back(bdt_variable("10/10*min("+ min_disz + ", min("+ min_disy +"," + min_disx + "))","(52,-100,50)",
		"Distance of a Track Ahead of a Shower (all-direction)[cm]",true,"d"));

    //----------------------------------------------------------->>Others<<-------------------------------------------------------------
    //NC pi0 killer
    std::string reco_track_energy = "reco_track_energy_new_legacy[0]+0.938272"; //assuming a proton
    std::string invariant_mass = "11/11*0.938272*0.938272+2.0*("+reco_track_energy+"*        reco_shower_helper_energy[0]-"+track_mom+"*"+shower_mom+"*"+angle_track_shower+")";//of proton   photon pair
    all_vars.push_back(bdt_variable(invariant_mass,"(52,1,2.5)","Invariant Mass",true,"d"));

    //----------------------------------------`------------------>>Others (Track)<<-----------------------------------------------------
    //muon killer
    all_vars.push_back(bdt_variable("13/13*abs(track_info.reco_track_PIDA[0]-10)","(52,0,14)",   "Track PIDA diff respected to 10",true,"d"));

    //----------------------------------------------------------->>Others (Shower)<<-----------------------------------------------------
    all_vars.push_back(bdt_variable("14/14*reco_shower_dist_to_closest_flashzcenter[0]","(52,0,  520)","Distance from Shower to Flashcenter [cm]",false,"d"));
*/


	if(analysis_tag == "electron1" || analysis_tag == "electron0"){//Not for training

	}

	//distance of track ahead of a shower (this "distance" is negative, if the track is ahead of the shower)
	std::string min_disz = "min(reco_track_endz[0]-reco_shower_startz[0],(reco_track_startz[0]-reco_shower_startz[0]))";
	std::string min_disy = "min(-reco_track_endy[0]+reco_shower_starty[0],(-reco_track_starty[0]+reco_shower_starty[0]))";
	std::string min_disx = "min(reco_track_endx[0]-reco_shower_startx[0],(reco_track_startx[0]-reco_shower_startx[0]))";

	if(analysis_tag == "electron1bnb" || analysis_tag == "electron1" || analysis_tag == "electron0bnb"){//BNB variables

	    all_vars.push_back(bdt_variable("1/1*("+ min_disz + ")","(80,-100,40)", "Distance of a Track Ahead of a Shower (z-direction)[cm]",true,"d"));
	    
	    //The denomenator might be zero, but solve this problem later.
	    all_vars.push_back(bdt_variable("2/2*reco_shower_dedxnew_plane2[0]/reco_shower_dirz[0]", "(52,0,10)", "dE/dx of Shower in Collection Plane (Orientation Corrected) [MeV/cm]",false,"d"));
	}


	std::string shower_mom = "sqrt(pow(reco_shower_helpernew_energy[0],2)+2*0.000511*reco_shower_helpernew_energy[0])";//E=Ek+m
	std::string track_mom  = "sqrt(pow(reco_track_energy_new_legacy[0],2)+2*0.938272*reco_track_energy_new_legacy[0])";//reco_track_energy_new_legacy is actually KE

	if(analysis_tag == "electron1cosmic" ||analysis_tag == "electron1" || analysis_tag == "electron0cosmic"){//Cosmic variables

	    all_vars.push_back(bdt_variable("3/3*(abs(reco_shower_diry+reco_track_diry))","(52,0,2)","Total Momentum (Track and Shower, Normalized) Aligned to y-direction", false, "d"));

	    all_vars.push_back(bdt_variable("4/4*("+ min_disx + ")","(52,-100,50)", "Distance of a Track Ahead of a Shower (x-direction)[cm]",true,"d"));

	    all_vars.push_back(bdt_variable("5/5*(sqrt(1-reco_track_dirz[0]*reco_track_dirz[0])*"+track_mom+"+sqrt(1-reco_shower_dirz[0]*reco_shower_dirz[0])*"+shower_mom+")",
				"(52,0,1.2)","Total Momtum Perpendicular to z-direction", false, "d"));
	}


//----------------------------------------------------------->>Energy<<-------------------------------------------------------------
//	all_vars.push_back(bdt_variable("2/2*(min(abs(reco_track_energy_new_legacy[0]-reco_shower_helper_energy[0]+0.938272-0.000511),abs(reco_shower_helper_energy[0]-reco_track_energy_new_legacy[0]+0.938272-0.000511)))","(52,0,2)","Energy diff. on track and shower[GeV]", false,"d"));
//	all_vars.push_back(bdt_variable("3/3*reco_shower_helper_energy[0]","(52,0,0.5)","Reconstructed Shower Energy [GeV]", false,"d"));
//	all_vars.push_back(bdt_variable("4/4*reco_track_energy_new_legacy[0]", "(52,0,1)","Track Energy (Best Plane)",true, "d"));
	
	all_vars.push_back(bdt_variable("6/6*reco_shower_helpernew_energy[0]+reco_track_energy_new_legacy[0]","(52,0,1)","Nue Energy [GeV]", false,"d"));

	/* Particle Mass [GeV]
	 *  n	    0.93957
	 *  p	    0.93828
	 *  e	    0.000511
	 *  
	std::string shower_mom = "sqrt(pow(reco_shower_helpernew_energy[0],2)+2*0.000511*reco_shower_helpernew_energy[0])";//E=Ek+m
	std::string track_mom  = "sqrt(pow(reco_track_energy_new_legacy[0],2)+2*0.938272*reco_track_energy_new_legacy[0])";//reco_track_energy_new_legacy is actually KE
	 */

	std::string Mn_b = "(0.93957-0.04)";//neutron mass - binding energy	

	std::string QEE_p = "7/7*((0.5* (2*"+Mn_b+"* (reco_track_energy_new_legacy[0]+0.93827)-(pow("+Mn_b+",2)+(pow(0.93827,2)-pow(0.000511,2))))/("+Mn_b+"-(reco_track_energy_new_legacy[0]+0.93827)+"+track_mom+"* reco_track_dirz[0])))";
	std::string QEE_e = "8/8*(0.5* (2* "+Mn_b+"* (reco_shower_helpernew_energy[0]+0.000511)-(pow("+Mn_b+",2)+(pow(0.000511,2)-pow(0.93827,2))))/("+Mn_b+"-(reco_shower_helpernew_energy[0]+0.000511)+"+shower_mom+"* reco_shower_dirz[0]))";

	std::string E_range = "reco_shower_helpernew_energy[0]+reco_track_energy_new_legacy[0]+0.000511+0.9382723-(0.93957-0.04)";//0.004? not sure.
	//muon killer N/A
	all_vars.push_back(bdt_variable(QEE_p +"-"+ E_range, "(100,-2,2)", "QE_{#nu}^{QE}[p]-E^{range}", true, "d"));
	all_vars.push_back(bdt_variable(QEE_e +"-"+ E_range, "(100,-2,2)", "QE_{#nu}^{QE}[e]-E^{range}", true, "d"));
	
//----------------------------------------------------------->>Momentum<<-------------------------------------------------------------
	//this might work for BNB BDT, because low energy event tends to lost signal, hence, low ratio.
	all_vars.push_back(bdt_variable("9/9*(reco_track_dirz[0]*"+track_mom+"+reco_shower_dirz[0]*"+shower_mom+")","(52,-3,3)","Total Momentum on z-direction", false, "d"));
	
	all_vars.push_back(bdt_variable("10/10*1/((sqrt(1-reco_track_dirz[0]*reco_track_dirz[0])*"+track_mom+"+sqrt(1-reco_shower_dirz[0]*reco_shower_dirz[0])*"+shower_mom+")/(reco_track_dirz[0]*"+track_mom+"+reco_shower_dirz[0]*"+shower_mom+"))","(52,0,8)","Ratio of Momentum on z-direction versus on xy-plane", false, "d"));

//----------------------------------------------------------->>Orientation<<-------------------------------------------------------------
//	all_vars.push_back(bdt_variable("7/7*(abs(reco_track_dirz[0])+abs(reco_shower_dirz[0]))","(52,0,2)","Track and Shower Aligned(abs) to z-direction", false, "d"));
//	all_vars.push_back(bdt_variable("9/9*reco_shower_dirz/abs(reco_shower_dirz)+2*reco_track_dirz/abs(reco_track_dirz)","(52,-10,10)","SemiAngle Orientation (shower and track normalized/weighted)", false, "i"));
	
//The below one is not that bad..	
//	all_vars.push_back(bdt_variable("10/10*abs((reco_track_dirz[0])+(reco_shower_dirz[0]))","(52,0,2)","Total Momentum (Track and Shower, Normalized) Aligned to z-direction", false, "d"));

//	all_vars.push_back(bdt_variable("11/11*(reco_track_dirx[0]+reco_shower_dirx[0])","(52,-2,2)","Track and Shower Aligned to x-direction", false, "d"));//trash
//	all_vars.push_back(bdt_variable("12/12*(abs(reco_track_dirx[0])+abs(reco_shower_dirx[0]))","(52,-2,2)","Track and Shower absAligned to x-direction", false, "d"));//trash
	
    //Following two are FOR COSMIC ONLY, how close the angle related to pi/2
	
//	all_vars.push_back(bdt_variable("13/13*abs(abs(atan2(reco_shower_diry[0],reco_shower_dirz[0]))-1.5708)","(52,0,1.58)","Reconstructed Shower - |Theta_{yz} - pi/2|", false,"d"));
//	all_vars.push_back(bdt_variable("14/14*abs(abs(atan2(reco_track_dirx[0],reco_track_dirz[0]))-1.5708)","(52,0,1.58)","Reconstructed Track - |alpha_{xz} - pi/2|", true,"d"));
	
//----------------------------------------------------------->>Stopping Power<<-------------------------------------------------------------
    	all_vars.push_back(bdt_variable("11/11*abs(track_info.reco_track_mean_dEdx[0]/reco_track_dirz[0]-reco_shower_dedxnew_plane2[0]/reco_shower_dirz[0])",
	    "(52,0,10)", "Diff. of dE/dx on Track and Shower in Col. Plane (Directrion Corrected)[MeV/cm]",false,"d"));

    	all_vars.push_back(bdt_variable("12/12*track_info.reco_track_mean_dEdx[0]/reco_track_dirz[0]","(52,0,10)", "dE/dx of Track in Collection Plane (Direction Corrected) [MeV/cm]",false,"d"));
	    
//----------------------------------------------------------->>Length<<-------------------------------------------------------------
	all_vars.push_back(bdt_variable("13/13*max(reco_track_displacement[0],reco_shower_length[0])","(52,0,350)","Maximum Displacement of Track or Shower [cm]", true,"d"));//note: muon is to be killed by evaluating the distance btw shower and track

//-------------------------------------------------->>Distance btw shower and track<<------------------------------------------------------------

//	all_vars.push_back(bdt_variable("3/3*("+ min_disy + ")","(52,-100,50)", "Distance of a Track Ahead of a Shower (y-direction)[cm]",true,"d"));

	all_vars.push_back(bdt_variable("14/14*min("+ min_disz + ", min("+ min_disy +"," + min_disx + "))","(52,-100,50)",
		"Distance of a Track Ahead of a Shower (all-direction)[cm]",true,"d"));


//----------------------------------------------------------->>Others<<-------------------------------------------------------------
//	all_vars.push_back(bdt_variable("(totalpe_ibg_sum*reco_shower_helper_energy/(reco_shower_helper_energy+reco_track_energy))/10000","(52,0,2)","Shower PE number", true,"d"));
	//NC pi0 killer
	std::string reco_track_energy = "reco_track_energy_new_legacy[0]+0.938272"; //assuming a proton
	std::string invariant_mass = "24/24*0.938272*0.938272+2.0*("+reco_track_energy+"*reco_shower_helper_energy[0]-"+track_mom+"*"+shower_mom+"*"+angle_track_shower+")";//of proton photon pair
//	all_vars.push_back(bdt_variable(invariant_mass,"(52,1,2.5)","Invariant Mass",true,"d"));
//	all_vars.push_back(bdt_variable("25/25*pi0_info.num_reco_showers_within_10cm_vertex","(10,0,10)","Num Showers within 10cm",false,"i"));
	all_vars.push_back(bdt_variable("15/15*shortest_asso_shower_to_vert_dist","(52,0,20)","Photon Conversion Length from Reconstructed Vertex [cm]" ,false,"d"));
	all_vars.push_back(bdt_variable("16/16*"+angle_track_shower,	"(52,-1,1)","Cosine Track-Shower Angle ",true,"d"));//for back-to-back cut

	all_vars.push_back(bdt_variable("17/17*reco_shower_dist_to_closest_flashzcenter[0]","(52,0,520)","Distance from Shower to Flashcenter [cm]",false,"d"));
//----------------------------------------------------------->>Others (Track)<<-----------------------------------------------------
	//muon killer
	all_vars.push_back(bdt_variable("18/18*abs(track_info.reco_track_PIDA[0]-10)","(52,0,14)","|Track PIDA - 10|",true,"d"));

//	all_vars.push_back(bdt_variable("track_info.reco_track_mean_dEdx[0]","(0,2)", "Track Mean dEdx",true,"d"));
//	all_vars.push_back(bdt_variable("track_info.reco_track_start_mean_dEdx[0]/track_info.reco_track_end_mean_dEdx[0]","(0,5)", "Track direction",true,"d"));
//	std::string track_direction_cut = "track_info.reco_track_start_mean_dEdx[0]/track_info.reco_track_end_mean_dEdx[0] >= 0.9";
//
//----------------------------------------------------------->>Others (Shower)<<-----------------------------------------------------
	all_vars.push_back(bdt_variable("19/19*(reco_shower_length[0]*tan(reco_shower_opening_angle[0]/2))","(52,0,10)","Base Length of a Shower [cm]", false, "d"));
//	all_vars.push_back(bdt_variable("30/30*(reco_shower_length[0]*reco_shower_length[0]*tan(reco_shower_opening_angle[0]/2))","(52,0,200)","Thickness of Shower [#{cm^2}]", false, "d"));
	all_vars.push_back(bdt_variable("20/20*reco_shower_opening_angle[0]","(52,0,0.8)","Shower Opening Angle",false,"d"));
	all_vars.push_back(bdt_variable("21/21*reco_nu_vtx_dist_to_closest_tpc_wall","(52,0,120)","Reconstructed Vertex to TPC Wall Distance [cm]",false,"d"));
	
//	all_vars.push_back(bdt_variable("34/34*reco_shower_length[0]*pow(tan(reco_shower_opening_angle[0]/2),2)/reco_shower_helper_energy[0]","(52,0,20)","Area of Shower Length over shower energy [#{cm^2/GeV}]", false, "d"));
  
    }
};

