#include "variable_list.h"

variable_list::variable_list(std::string analysis_tag_in): analysis_tag(analysis_tag_in){

	//*******************************************************************************//
	//*******************************************************************************//
	//****************   Single Photon ||   track  and ncdelta 1g0p *****************//
    //**************** For NCpi0 variables/cuts, search "ncpi0_2g1p" ****************//
	//*******************************************************************************//
	//*******************************************************************************//


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

    }

		//*******************************************************************************//
		//*******************************************************************************//
		//****************   YOUR TAGE HERE				      ***********//
		//*******************************************************************************//
		//*******************************************************************************//

    else if (analysis_tag == "ncpi0_2g1p") {
        // Useful strings
		std::string angle_track_shower ="(reco_track_dirx[0]*reco_shower_dirx[0]+reco_track_diry[0]*reco_shower_diry[0]+reco_track_dirz[0]*reco_shower_dirz[0])";
		std::string fiducial_vertex = "reco_nuvertx > 10 && reco_nuvertx < 246 && reco_nuverty > -107 && reco_nuverty < 107 && reco_nuvertz > 10 && reco_nuvertz < 1026 ";
		std::string fiducial_shower_end = "reco_shower_endx > 5 && reco_shower_endx < 245 && reco_shower_endy > -105 && reco_shower_endy < 95 && reco_shower_endz > 10 && reco_shower_endz < 1026 ";
		std::string fiducial_track_end = "reco_track_endx > 5 && reco_track_endx < 245 && reco_track_endy > -95 && reco_track_endy < 95 && reco_track_endz > 10 && reco_track_endz < 1026 ";
		std::string fiducial_cut = fiducial_vertex;//"&&"+fiducial_shower_end;
		std::string bump_region = " reco_shower_helper_energy[0]>0.130 && reco_shower_helper_energy[0]<0.2";

		std::string proton_mass = "0.938272";
		std::string reco_track_energy = "(track_info.reco_track_kinetic_from_length[0]+"+proton_mass + ")";
		std::string reco_track_momentum  = "sqrt("+reco_track_energy+"*"+reco_track_energy+"-"+proton_mass +"*"+proton_mass+")";
        std::string angle_shower1_shower2 ="reco_shower_dirx[0]*reco_shower_dirx[1]+reco_shower_diry[0]*reco_shower_diry[1]+reco_shower_dirz[0]*reco_shower_dirz[1]";
        std::string E1 = "reco_shower_helper_energy[most_energetic_shower_index]"; 
        std::string E2 = "reco_shower_helper_energy[second_most_energetic_shower_index]"; 

        // Invariant mass for two massless particles (photons)
        std::string invMass = "sqrt(2.0*"+E1+"*"+E2+"*(1.0-"+angle_shower1_shower2+"))";

        // Pion momentum components
        std::string p_pi_x = E1+"*reco_shower_dirx[most_energetic_shower_index]"+"+"+E2+"*reco_shower_dirx[second_most_energetic_shower_index]";
        std::string p_pi_y = E1+"*reco_shower_diry[most_energetic_shower_index]"+"+"+E2+"*reco_shower_diry[second_most_energetic_shower_index]";
        std::string p_pi_z = E1+"*reco_shower_dirz[most_energetic_shower_index]"+"+"+E2+"*reco_shower_dirz[second_most_energetic_shower_index]";
        std::string p_pi_comp = "sqrt("+p_pi_x+"*"+p_pi_x+" + "+p_pi_y+"*"+p_pi_y+" + "+p_pi_z+"*"+p_pi_z+")";
        std::string p_pi_trans = "sqrt("+p_pi_y+"*"+p_pi_y+" + "+p_pi_z+"*"+p_pi_z+")";

        //********************************Pre-selection cuts*************************************************//
		std::string new_precuts;

		std::string good_calo_cut = "track_info.reco_track_good_calo[0]>0 && track_info.reco_track_mean_dEdx[0] > 1";
		std::string track_length_cut = "reco_track_displacement[0]<150";
		std::string min_shower_energy_cut = "reco_shower_helper_energy[second_most_energetic_shower_index]>0.03 ";
		std::string min_conversion_cut = "shortest_asso_shower_to_vert_dist > 1";
		std::string track_direction_cut = "track_info.reco_track_start_mean_dEdx[0]/track_info.reco_track_end_mean_dEdx[0] >= 0.95";
		std::string pe_cut = "totalpe_ibg_sum > 20";
		std::string flash_proximity_cut = "reco_shower_dist_to_closest_flashzcenter[0] < 80";
		std::string dead_wire_track = "((reco_track_endz < 685 || reco_track_endz > 715) && shower_info.amalgamated_shower_dEdx[0] < 6.5 )";


		//************************************************* Shower-only Variables **********************************************//

		all_vars.push_back(bdt_variable("totalpe_ibg_sum","(48,0,5000)","Total in Beam-Gate PE",false,"d"));  //1
        all_vars.push_back(bdt_variable("reco_shower_dedx_plane2[most_energetic_shower_index]","(48,0,15)", "Leading Shower dE/dx (Collection Plane) [MeV/cm]",false,"d")); //2
        all_vars.push_back(bdt_variable("reco_shower_dedx_plane2[second_most_energetic_shower_index]","(48,0,15)", "Subleading Shower dE/dx (Collection Plane) [MeV/cm]",false,"d")); //3
		all_vars.push_back(bdt_variable("reco_shower_length[most_energetic_shower_index]","(48,0,100)","Leading Shower Length [cm]",false,"d"));//4
		all_vars.push_back(bdt_variable("reco_shower_length[second_most_energetic_shower_index]","(48,0,100)","Subleading Shower Length [cm]",false,"d"));//5

		all_vars.push_back(bdt_variable(angle_shower1_shower2,"(48,0,0.8)","Shower Opening Angle (Lab)",false,"d"));//5
		all_vars.push_back(bdt_variable("reco_shower_dist_to_closest_flashzcenter[0]","(48,0,520)","Distance from Leading Shower to Flashcenter [cm]",false,"d"));//6

		all_vars.push_back(bdt_variable("reco_nu_vtx_dist_to_closest_tpc_wall","(48,0,120)","Reconstructed Vertex to TPC Wall Distance [cm]",false,"d"));//7
		all_vars.push_back(bdt_variable("cos(atan2(reco_shower_diry[most_energetic_shower_index],reco_shower_dirz[most_energetic_shower_index]))","(48,-1,1)","Reconstructed Leading Shower - Cosine Theta", false,"d"));//8
		all_vars.push_back(bdt_variable("cos(atan2(reco_shower_diry[most_energetic_shower_index], reco_shower_dirx[most_energetic_shower_index]))","(48,-1,1)","Reconstructed Leading Shower - Cosine Phi", false,"d"));//9

		all_vars.push_back(bdt_variable("cos(atan2(reco_shower_diry[second_most_energetic_shower_index],reco_shower_dirz[second_most_energetic_shower_index]))","(48,-second_most_energetic_shower_index,second_most_energetic_shower_index)","Reconstructed Subleading Shower - Cosine Theta", false,"d"));//10
		all_vars.push_back(bdt_variable("cos(atan2(reco_shower_diry[second_most_energetic_shower_index], reco_shower_dirx[second_most_energetic_shower_index]))","(48,-second_most_energetic_shower_index,second_most_energetic_shower_index)","Reconstructed Subleading Shower - Cosine Phi", false,"d"));//11

		all_vars.push_back(bdt_variable("reco_shower_endx[most_energetic_shower_index]","(48,0,250)"," Reconstructed Leading Shower End X pos [cm]",false,"d"));//12
		all_vars.push_back(bdt_variable("reco_shower_endy[most_energetic_shower_index]","(48,-110,+110)","Reconstructed Leading Shower End Y pos [cm]",false,"d"));//13
		all_vars.push_back(bdt_variable("reco_shower_endz[most_energetic_shower_index]","(48,0,1050)","Reconstructed Leading Shower End Z pos [cm]",false,"d"));//14
		all_vars.push_back(bdt_variable("reco_shower_endx[second_most_energetic_shower_index]","(48,0,250)"," Reconstructed Subleading Shower End X pos [cm]",false,"d"));//15
		all_vars.push_back(bdt_variable("reco_shower_endy[second_most_energetic_shower_index]","(48,-110,+110)","Reconstructed Subleading Shower End Y pos [cm]",false,"d"));//16
		all_vars.push_back(bdt_variable("reco_shower_endz[second_most_energetic_shower_index]","(48,0,1050)","Reconstructed Subleading Shower End Z pos [cm]",false,"d"));//17

		all_vars.push_back(bdt_variable( "fabs(atan2(reco_shower_diry[0],reco_shower_dirz[0]))/3.14159","(48,0,1.0)","Reconstucted Leading Shower Angle Beamy-ness [rad]","false","d")); //18
		all_vars.push_back(bdt_variable( "min(fabs(atan2(reco_shower_diry[0],reco_shower_dirx[0])+3.14159/2.0),fabs(atan2(reco_shower_diry[0],reco_shower_dirx[0])-3.14159/2.0))/3.14159","(48,0,0.5)","Reconstucted Leading Shower Angle Cosmicy-ness [rad]","false","d")); //19



		//************************************************* Track + Shower Only Variables **********************************************//
        all_vars.push_back(bdt_variable( "fabs(atan2(reco_track_diry[0],reco_track_dirz[0]))/3.14159","(48,0,1.0)","Reconstucted Track Angle Beamy-ness [rad]","true","d")); //20
        all_vars.push_back(bdt_variable( "min(fabs(atan2(reco_track_diry[0],reco_track_dirx[0])+3.14159/2.0),fabs(atan2(reco_track_diry[0],reco_track_dirx[0])-3.14159/2.0))/3.14159","(48,0,0.5)","Reconstucted Track Angle Cosmicy-ness [rad]","true","d")); //21
        //all_vars.push_back(bdt_variable("log(track_info.reco_track_straightline_chi2[0])","(48,6,16)"," Track Line chi^2",true,"d") );//22
        //all_vars.push_back(bdt_variable("log(track_info.reco_track_sum_angles[0])","(48,-30,20)"," Track Sum of all Angles",true,"d") );//23

        all_vars.push_back(bdt_variable("track_info.reco_track_mean_dEdx[0]", "(48,0,12)","Mean Track dE/dx [MeV/cm]", true,"d"));//24
        all_vars.push_back(bdt_variable("track_info.reco_track_start_mean_dEdx[0]/track_info.reco_track_end_mean_dEdx[0]", "(48,0,3.5)","Ratio of Mean Start/End Track dE/dx", true,"d"));//25
        all_vars.push_back(bdt_variable("track_info.reco_track_PIDA[0]","(48,0,24)","Track PIDA",true,"d"));//26
        all_vars.push_back(bdt_variable("track_info.reco_track_braggA[0]","(48,0,25)","Track Bragg A",true,"d"));//27
        all_vars.push_back(bdt_variable("shortest_asso_shower_to_vert_dist","(48,0,30)","Photon Conversion Length from Reconstructed Vertex [cm]" ,true,"d"));//28
        all_vars.push_back(bdt_variable("cos(atan2(reco_track_diry[0],reco_track_dirz[0]))","(48,-1,1)","Reconstructed Track - Cosine Theta", true,"d"));//29
        all_vars.push_back(bdt_variable("cos(atan2(reco_track_diry[0],reco_track_dirx[0]))","(48,-1,1)","Reconstructed Track - Cosine Phi", true,"d"));//30
        all_vars.push_back(bdt_variable(angle_track_shower,	"(48,-1,1)","Cosine Track-Shower Angle ",true,"d"));//31
        all_vars.push_back(bdt_variable("reco_track_endx[0]","(48,0,250)"," Reconstructed Track End X pos [cm]",true,"d"));//32
        all_vars.push_back(bdt_variable("reco_track_endy[0]","(48,-110,+110)","Reconstructed Track End Y pos [cm]",true,"d"));//33
        all_vars.push_back(bdt_variable("reco_track_endz[0]","(48,0,1050)","Reconstructed Track End Z pos [cm]",true,"d"));//34
        all_vars.push_back(bdt_variable("reco_track_energy_new_legacy[0]", "(48,0,1)","Track Energy (Best Plane)",true, "d"));//35
        all_vars.push_back(bdt_variable("track_info.reco_track_kinetic[0]","(48,0,1)","Reconstructed Track Kinetic Energy (Calo) [cm]", true,"d"));//36
        all_vars.push_back(bdt_variable("track_info.reco_track_kinetic_from_length[0]","(48,0,1)","Reconstructed Track Kinetic Energy (Length) [cm]", true,"d"));//37
        all_vars.push_back(bdt_variable("track_info.reco_track_kinetic_from_length[0]/track_info.reco_track_kinetic[0]","(48,0,5)","Ratio of Reco Track Kinetic (Length/Calo)", true,"d"));//38
        all_vars.push_back(bdt_variable("pi0Boost_info.gamma_pi_least_angle","(25, -0.1, 1.6)","Smallest #gamma #pi Angle (CM Frame)", true,"d"));//39
        all_vars.push_back(bdt_variable("pi0Boost_info.gamma_z_least_angle","(25, -0.1, 1.6)","Smallest #gamma z Angle (CM Frame)", true,"d"));//40

		all_precuts.push_back(pe_cut);
		all_precuts.push_back(fiducial_cut);
		all_precuts.push_back(min_shower_energy_cut);

        all_precuts.push_back(track_length_cut);
        all_precuts.push_back(min_conversion_cut);
        all_precuts.push_back(good_calo_cut);
        all_precuts.push_back(track_direction_cut);
        all_precuts.push_back(dead_wire_track);


	}else if( analysis_tag == "YOURTAGHERE"){

		//Fill out all_vars and all-precuts here!


	} 
    else {
        std::cout << "Invalid analysis tag when getting vars/cuts" << std::endl;
    }

};
