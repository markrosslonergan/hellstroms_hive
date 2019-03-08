#include "variable_list.h"

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

		//all_vars.push_back(bdt_variable("sqrt( pow(mctruth_nu_vertex_x-reco_vertex_x,2)+ pow(mctruth_nu_vertex_y-reco_vertex_y,2)+ pow(mctruth_nu_vertex_z-reco_vertex_z,2))","(24,0,100)"," Reco-True Vetrex Dist [cm]",false,"d"));  //1

		all_vars.push_back(bdt_variable("reco_flash_total_pe_in_beamgate[0]","(24,0,5000)","Total in Beam-Gate PE",false,"d"));  //1
		all_vars.push_back(bdt_variable("reco_shower_dEdx_plane2_median[0]","(24,0,6.5)", "Median Shower dE/dx plane 2 [MeV/cm]",false,"d")); //2
		all_vars.push_back(bdt_variable("reco_shower_energy[0]/1000.0","(24,0,0.8)","Reconstructed Shower Energy [GeV]", false,"d"));  //3
		all_vars.push_back(bdt_variable("reco_shower_length[0]","(24,0,100)","Shower Length [cm]",false,"d"));//4

		all_vars.push_back(bdt_variable("reco_shower_opening_angle[0]","(24,0,0.8)","Shower Opening Angle",false,"d"));//5
		all_vars.push_back(bdt_variable("reco_shower_flash_shortest_distz[0]","(24,0,520)","Distance from Shower to Flashcenter [cm]",false,"d"));//6

		all_vars.push_back(bdt_variable("cos(reco_shower_theta_yz[0])","(24,-1,1)","Reconstructed Shower - Cosine Theta", false,"d"));//8
		all_vars.push_back(bdt_variable("cos(reco_shower_phi_yx[0])","(24,-1,1)","Reconstructed Shower - Cosine Phi", false,"d"));//9
		all_vars.push_back(bdt_variable("reco_shower_startx[0]","(24,0,250)"," Reconstructed Shower start X pos [cm]",false,"d"));//10
		all_vars.push_back(bdt_variable("reco_shower_starty[0]","(24,-110,+110)","Reconstructed Shower start Y pos [cm]",false,"d"));//11
		all_vars.push_back(bdt_variable("reco_shower_startz[0]","(24,0,1050)","Reconstructed Shower start Z pos [cm]",false,"d"));//12
        all_vars.push_back(bdt_variable("log10(reco_shower_conversion_distance[0])","(24,-2,4)","Log Reconstructed Shower conversion distance","false","d"));
      //  all_vars.push_back(bdt_variable("reco_shower_delaunay_area_plane2[0]","(24,0,200e3)","Reconstructed Shower delaunay area plane 2","false","d"));
        
       // all_vars.push_back(bdt_variable("reco_shower_start_to_nearest_dead_wire_plane0[0]","(24,0,250)"," Reconstructed Shower dead wire dist plane 0 pos [cm]",false,"d"));//10
		//all_vars.push_back(bdt_variable("reco_shower_start_to_nearest_dead_wire_plane1[0]","(24,-110,+110)","Reconstructed Shower dead wire dist plane 1 pos [cm]",false,"d"));//11
		//all_vars.push_back(bdt_variable("reco_shower_start_to_nearest_dead_wire_plane2[0]","(24,0,1050)","Reconstructed Shower dead wire dist plane 2 pos [cm]",false,"d"));//12


		//************************************************* Track + Shower Only Variables **********************************************//
		if(analysis_tag == "track"){
			//all_vars.push_back(bdt_variable( "-log((track_info.reco_track_principal_2[0]+track_info.reco_track_principal_1[0])/track_info.reco_track_principal_0[0])","(24,0,30)","Track PCA straightness","true","d"));//15

            all_vars.push_back(bdt_variable("reco_track_proton_kinetic_energy[0]","(24,0,2)","Reconstructed Track Kinetic Energy [GeV]","true","d"));
			all_vars.push_back(bdt_variable( reco_track_energy +"*(reco_track_dirx[0])+ "+reco_shower_momentum+"*reco_shower_dirx[0]","(24,-2,2)","Reconstructed Delta Momentum X [GeV]",true,"d"));//19
			all_vars.push_back(bdt_variable( reco_track_energy +"*(reco_track_diry[0])+ "+reco_shower_momentum+"*reco_shower_diry[0]","(24,-2,2)","Reconstructed Delta Momentum Y [GeV]",true,"d"));//20
			all_vars.push_back(bdt_variable( reco_track_energy +"*(reco_track_dirz[0])+ "+reco_shower_momentum+"*reco_shower_dirz[0]","(24,-1.5,2)","Reconstructed Delta Momentum Z [GeV]",true,"d"));//21
			all_vars.push_back(bdt_variable( "sqrt( pow("+reco_track_energy +"*(reco_track_dirx[0])+ "+reco_shower_momentum+"*reco_shower_dirx[0],2)+ pow("+reco_track_energy +"*(reco_track_diry[0])+"+reco_shower_momentum +"*reco_shower_diry[0],2))","(24,0,2)","Reconstucted Delta Transverse (YX) Momentum [GeV]",true,"d"));//22
			all_vars.push_back(bdt_variable("reco_track_mean_trunc_dEdx[0]", "(24,0,12)","Mean Track dE/dx [MeV/cm]", true,"d"));//23
			all_vars.push_back(bdt_variable("reco_track_mean_trunc_dEdx_start_half[0]/reco_track_mean_trunc_dEdx_end_half[0]", "(24,0,3.5)","Ratio of Mean Start/End Track dE/dx", true,"d"));//24
			all_vars.push_back(bdt_variable("reco_track_trunc_PIDA[0]","(24,0,24)","Track PIDA",true,"d"));//25
			all_vars.push_back(bdt_variable(invariant_mass,"(24,1,2.5)","Invariant Mass",true,"d"));//28
			all_vars.push_back(bdt_variable("cos(reco_track_theta_yz[0])","(24,-1,1)","Reconstructed Track - Cosine Theta", true,"d"));//29
			all_vars.push_back(bdt_variable("cos(reco_track_phi_yx[0])","(24,-1,1)","Reconstructed Track - Cosine Phi", true,"d"));//30
			all_vars.push_back(bdt_variable(angle_track_shower,	"(24,-1,1)","Cosine Track-Shower Angle ",true,"d"));//31
			all_vars.push_back(bdt_variable("reco_track_endx[0]","(24,0,250)"," Reconstructed Track End X pos [cm]",true,"d"));//32
			all_vars.push_back(bdt_variable("reco_track_endy[0]","(24,-110,+110)","Reconstructed Track End Y pos [cm]",true,"d"));//33
			all_vars.push_back(bdt_variable("reco_track_endz[0]","(24,0,1050)","Reconstructed Track End Z pos [cm]",true,"d"));//34
      all_vars.push_back(bdt_variable("reco_track_startx[0]","(24,0,250)"," Reconstructed Track Start X pos [cm]",true,"d"));//32
			all_vars.push_back(bdt_variable("reco_track_starty[0]","(24,-110,+110)","Reconstructed Track Start Y pos [cm]",true,"d"));//33
			all_vars.push_back(bdt_variable("reco_track_startz[0]","(24,0,1050)","Reconstructed Track Start Z pos [cm]",true,"d"));//34
			all_vars.push_back(bdt_variable("reco_track_spacepoint_principal0[0]","(24,0,1)","Recontstructed Track Spacepoint Principal0",true,"d"));//34

		}else if(analysis_tag == "1g2p"){

            all_vars.push_back(bdt_variable("reco_track_proton_kinetic_energy[reco_track_ordered_energy_index[0]]","(24,0,2)","Reconstructed Track Kinetic Energy 1st [GeV]","true","d"));
            all_vars.push_back(bdt_variable("reco_track_proton_kinetic_energy[reco_track_ordered_energy_index[1]]","(24,0,2)","Reconstructed Track Kinetic Energy 2nd [GeV]","true","d"));

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
		//****************   YOUR TAGE HERE				      ***********//
		//*******************************************************************************//
		//*******************************************************************************//


	}

  else if(analysis_tag == "2g1p" || analysis_tag == "2g0p"){

    // Define a bunch of useful strings for variables and cuts

		//std::string fiducial_vertex_truth = "mctruth_nu_vertex_x > 10 && mctruth_nu_vertex_x < 246 && mctruth_nu_vertex_y > -107 && mctruth_nu_vertex_y < 107 && mctruth_nu_vertex_z > 10 && mctruth_nu_vertex_z < 1026 ";
        //
		std::string angle_track_shower ="(reco_track_dirx[0]*reco_shower_dirx[0]+reco_track_diry[0]*reco_shower_diry[0]+reco_track_dirz[0]*reco_shower_dirz[0])";
		std::string fiducial_vertex = "reco_vertex_x > 10 && reco_vertex_x < 246 && reco_vertex_y > -107 && reco_vertex_y < 107 && reco_vertex_z > 10 && reco_vertex_z < 1026 ";
		std::string fiducial_shower_end = "reco_shower_endx > 5 && reco_shower_endx < 245 && reco_shower_endy > -105 && reco_shower_endy < 95 && reco_shower_endz > 10 && reco_shower_endz < 1026 ";
		std::string fiducial_track_end = "reco_track_endx > 5 && reco_track_endx < 245 && reco_track_endy > -95 && reco_track_endy < 95 && reco_track_endz > 10 && reco_track_endz < 1026 ";
		std::string fiducial_cut = fiducial_vertex;//"&&"+fiducial_shower_end;

    std::string shower_index1 = "(reco_shower_ordered_energy_index[0])";
    std::string shower_index2 = "(reco_shower_ordered_energy_index[1])";

		std::string proton_mass = "0.938272";
		std::string reco_shower_momentum = "(reco_shower_energy[0]*0.001)"; 
		std::string reco_track_energy = "(reco_track_proton_kinetic_energy[0]+"+proton_mass + ")";
		std::string reco_track_momentum  = "sqrt("+reco_track_energy+"*"+reco_track_energy+"-"+proton_mass +"*"+proton_mass+")";
    std::string E1 = "reco_shower_energy["+shower_index1+"]";
    std::string E2 = "reco_shower_energy["+shower_index2+"]";
    std::string two_shower_opening_angle = "(acos(reco_shower_dirx[0]*reco_shower_dirx[1] + reco_shower_diry[0]*reco_shower_diry[1] + reco_shower_dirz[0]*reco_shower_dirz[1]))";
		std::string invariant_mass = "sqrt(2.0*"+E1+"*"+E2+"*(1.0-"+two_shower_opening_angle+"))/1000";

    std::string p_pi_x = "(reco_shower_energy["+shower_index1+"]*reco_shower_dirx["+shower_index1+"] + reco_shower_energy["+shower_index2+"]*reco_shower_dirx["+shower_index2+"])";
    std::string p_pi_y = "(reco_shower_energy["+shower_index1+"]*reco_shower_diry["+shower_index1+"] + reco_shower_energy["+shower_index2+"]*reco_shower_diry["+shower_index2+"])";
    std::string p_pi_z = "(reco_shower_energy["+shower_index1+"]*reco_shower_dirz["+shower_index1+"] + reco_shower_energy["+shower_index2+"]*reco_shower_dirz["+shower_index2+"])";
    std::string p_pi = "sqrt("+p_pi_x+"*"+p_pi_x+" + "+p_pi_y+"*"+p_pi_y+" + "+p_pi_z+"*"+p_pi_z+")";

    std::string cm_angle = "(("+E1+" - "+E2+")/("+p_pi+"))";


		std::string new_precuts;

		std::string good_calo_cut = "1";
		//std::string good_calo_cut = "reco_track_good_calo[0] > 0 &&  reco_shower_dEdx_plane2_nhits[0]>1 && reco_shower_dEdx_plane2_nhits[1]>1";
		std::string track_length_cut = "reco_track_displacement[0]<200";
		std::string min_shower_energy_cut = "reco_shower_energy[0]>20.0 ";
		std::string min_conversion_cut = "reco_shower_conversion_distance["+shower_index1+"] > 1 && reco_shower_conversion_distance["+shower_index1+"]<100 && reco_shower_conversion_distance["+shower_index2+"]<100";
		std::string back_to_back_cut = "1";//("+angle_track_shower+" > -1 &&"  + angle_track_shower + "< 1)";
		std::string pe_cut = "reco_num_flashes_in_beamgate == 1 && reco_flash_total_pe_in_beamgate[0] > 20";
    std::string shower_dedx_cut = "1";
    //std::string shower_dedx_cut = "reco_shower_dEdx_plane2_nhits[0]>1 && reco_shower_dEdx_plane2_nhits[1]>1";


		//************************************************* Shower Only Variables **********************************************//

		train_vars.push_back(bdt_variable("reco_flash_total_pe_in_beamgate[0]","(24,0,5000)","Total in Beam-Gate PE",false,"d"));  //1
		plot_vars.push_back(bdt_variable("reco_shower_dEdx_plane2_median["+shower_index1+"]","(24,0,6.5)", "Median Leading Shower dE/dx plane 2 [MeV/cm]",false,"d")); //2
		plot_vars.push_back(bdt_variable("reco_shower_dEdx_plane2_median["+shower_index2+"]","(24,0,6.5)", "Median Subleading Shower dE/dx plane 2 [MeV/cm]",false,"d")); //2
		train_vars.push_back(bdt_variable("reco_shower_energy["+shower_index1+"]/1000.0","(24,0,0.5)","Reco. Leading Shower Energy (Uncalibrated) [GeV]", false,"d"));  //3
		train_vars.push_back(bdt_variable("reco_shower_energy["+shower_index2+"]/1000.0","(24,0,0.5)","Reco. Subleading Shower Energy (Corrected) [GeV]", false,"d"));  //3
		train_vars.push_back(bdt_variable(two_shower_opening_angle,"(24,-3.15,3.15)","Two-Shower Opening Angle",false,"d"));//5
		train_vars.push_back(bdt_variable("reco_shower_flash_shortest_distz["+shower_index1+"]","(24,0,520)","Distance from Leading Shower to Flashcenter [cm]",false,"d"));//6
		train_vars.push_back(bdt_variable("reco_shower_theta_yz["+shower_index1+"]","(24,-1,1)","Reco. Leading Shower Theta", false,"d"));//8
		train_vars.push_back(bdt_variable("reco_shower_phi_yx["+shower_index1+"]","(24,-1,1)","Reco. Leading Shower Phi", false,"d"));//9
    train_vars.push_back(bdt_variable("reco_shower_conversion_distance["+shower_index1+"]","(24,1,100)","Reco. Leading Shower conversion distance","false","d"));
    train_vars.push_back(bdt_variable("reco_shower_conversion_distance["+shower_index2+"]","(24,0,100)","Reco. Subleading Shower conversion distance","false","d"));
		plot_vars.push_back(bdt_variable("sqrt( pow(mctruth_nu_vertex_x-reco_vertex_x,2)+ pow(mctruth_nu_vertex_y-reco_vertex_y,2)+ pow(mctruth_nu_vertex_z-reco_vertex_z,2))","(48,0,100)"," Reco-True Vetrex Dist [cm]",false,"d"));  //1

    plot_vars.push_back(bdt_variable(p_pi_x+"/1000", "(24, 0, 0.5)", "Reco. #pi^{0} x-Momentum [GeV]", false, "d"));
    plot_vars.push_back(bdt_variable(p_pi_y+"/1000", "(24, 0, 0.5)", "Reco. #pi^{0} y-Momentum [GeV]", false, "d"));
    plot_vars.push_back(bdt_variable(p_pi_z+"/1000", "(24, 0, 0.5)", "Reco. #pi^{0} z-Momentum [GeV]", false, "d"));
    plot_vars.push_back(bdt_variable(invariant_mass, "(24, 0, 0.5)", "Reco. #pi^{0} Invariant Mass (uncalibrated) [GeV]", false, "d"));
    plot_vars.push_back(bdt_variable(cm_angle, "(24, 0., 1.0)", "Reco. cos(#theta_{CM}) ", false, "d"));

		plot_vars.push_back(bdt_variable("reco_shower_length["+shower_index1+"]","(24,0,100)","Leading Shower Length [cm]",false,"d"));//4
		plot_vars.push_back(bdt_variable("reco_shower_length["+shower_index2+"]","(24,0,100)","Subleading Shower Length [cm]",false,"d"));//4

		plot_vars.push_back(bdt_variable("reco_shower_opening_angle["+shower_index1+"]","(24,0,0.8)","Leading Shower Opening Angle",false,"d"));//5
		plot_vars.push_back(bdt_variable("reco_shower_opening_angle["+shower_index2+"]","(24,0,0.8)","Subleading Shower Opening Angle",false,"d"));//5
		plot_vars.push_back(bdt_variable("reco_shower_flash_shortest_distz["+shower_index2+"]","(24,0,520)","Distance from Subleading Shower to Flashcenter [cm]",false,"d"));//6

		plot_vars.push_back(bdt_variable("reco_shower_theta_yz["+shower_index2+"]","(24,-3.15, 3.15)","Reco. Subleading Shower #theta", false,"d"));//8
		plot_vars.push_back(bdt_variable("reco_shower_phi_yx["+shower_index2+"]","(24,-3.15, 3.15)","Reco. Subleading #Phi", false,"d"));//9
		plot_vars.push_back(bdt_variable("reco_shower_startx["+shower_index1+"]","(24,0,250)"," Reco. Leading Shower start X pos [cm]",false,"d"));//10
		plot_vars.push_back(bdt_variable("reco_shower_startx["+shower_index2+"]","(24,0,250)"," Reco. Subleading Shower start X pos [cm]",false,"d"));//10
		plot_vars.push_back(bdt_variable("reco_shower_starty["+shower_index1+"]","(24,0,120)"," Reco. Leading Shower start Y pos [cm]",false,"d"));//10
		plot_vars.push_back(bdt_variable("reco_shower_starty["+shower_index2+"]","(24,0,120)"," Reco. Subleading Shower start Y pos [cm]",false,"d"));//10
		plot_vars.push_back(bdt_variable("reco_shower_startz["+shower_index1+"]","(24,0,1050)"," Reco. Leading Shower start Z pos [cm]",false,"d"));//10
		plot_vars.push_back(bdt_variable("reco_shower_startz["+shower_index2+"]","(24,0,1050)"," Reco. Subleading Shower start Z pos [cm]",false,"d"));//10
    plot_vars.push_back(bdt_variable("reco_shower_delaunay_area_plane2["+shower_index1+"]","(48,0,200e3)","Reconstructed Shower delaunay area plane 2","false","d"));
  
    //remove dead wire stuff from training for now.
    /*
        all_vars.push_back(bdt_variable("reco_shower_start_to_nearest_dead_wire_plane0["+shower_index1+"]","(48,0,250)"," Reconstructed Leading Shower dead wire dist plane 0 pos [cm]",false,"d"));//10
        all_vars.push_back(bdt_variable("reco_shower_start_to_nearest_dead_wire_plane0["+shower_index2+"]","(48,0,250)"," Reconstructed Subleading Shower dead wire dist plane 0 pos [cm]",false,"d"));//10
		all_vars.push_back(bdt_variable("reco_shower_start_to_nearest_dead_wire_plane1["+shower_index1+"]","(48,-110,+110)","Reconstructed Leading Shower dead wire dist plane 1 pos [cm]",false,"d"));//11
		all_vars.push_back(bdt_variable("reco_shower_start_to_nearest_dead_wire_plane1["+shower_index2+"]","(48,-110,+110)","Reconstructed Subleading Shower dead wire dist plane 1 pos [cm]",false,"d"));//11
		all_vars.push_back(bdt_variable("reco_shower_start_to_nearest_dead_wire_plane2["+shower_index1+"]","(48,0,1050)","Reconstructed Leading Shower dead wire dist plane 2 pos [cm]",false,"d"));//12
		all_vars.push_back(bdt_variable("reco_shower_start_to_nearest_dead_wire_plane2["+shower_index2+"]","(48,0,1050)","Reconstructed Subleading Shower dead wire dist plane 2 pos [cm]",false,"d"));//12
*/
		//************************************************* Track + Shower Only Variables **********************************************//
		if(analysis_tag == "2g1p"){

      train_vars.push_back(bdt_variable("reco_track_proton_kinetic_energy[0]","(24,0,2)","Reconstructed Track Kinetic Energy [GeV]","true","d"));
			plot_vars.push_back(bdt_variable("reco_track_mean_trunc_dEdx[0]", "(48,0,12)","Mean Track dE/dx [MeV/cm]", true,"d"));//23
			train_vars.push_back(bdt_variable("reco_track_theta_yz[0]","(24,-3.15,3.15)","Reconstructed Track #theta", true,"d"));//29
			train_vars.push_back(bdt_variable("reco_track_phi_yx[0]","(24,-3.15,3.15)","Reconstructed Track #Phi", true,"d"));//30
			plot_vars.push_back(bdt_variable("reco_track_mean_trunc_dEdx_start_half[0]/reco_track_mean_trunc_dEdx_end_half[0]", "(48,0,3.5)","Ratio of Mean Start/End Track dE/dx", true,"d"));//24
			plot_vars.push_back(bdt_variable("reco_track_trunc_PIDA[0]","(24,0,24)","Track PIDA",true,"d"));//25
			plot_vars.push_back(bdt_variable(angle_track_shower,	"(24,-1,1)","Cosine Track-Shower Angle ",true,"d"));//31
			plot_vars.push_back(bdt_variable("reco_track_endx[0]","(24,0,250)"," Reconstructed Track End X pos [cm]",true,"d"));//32
			plot_vars.push_back(bdt_variable("reco_track_endy[0]","(24,-110,+110)","Reconstructed Track End Y pos [cm]",true,"d"));//33
			plot_vars.push_back(bdt_variable("reco_track_endz[0]","(24,0,1050)","Reconstructed Track End Z pos [cm]",true,"d"));//34

			all_precuts.push_back(track_length_cut);
			all_precuts.push_back(min_conversion_cut);
			//all_precuts.push_back(good_calo_cut);
			all_precuts.push_back(back_to_back_cut);
		};

    all_precuts.push_back(min_shower_energy_cut); 
		all_precuts.push_back(pe_cut); 
		all_precuts.push_back(fiducial_cut);
    //all_precuts.push_back(shower_dedx_cut);

    // Now put all variables into a single vector for non-training stages
    all_vars.reserve(train_vars.size() + plot_vars.size() );
    all_vars.insert(all_vars.end(), train_vars.begin(), train_vars.end() );
    all_vars.insert(all_vars.end(), plot_vars.begin(), plot_vars.end() );


	}

  else {
    std::cout << "Invalid analysis tag" << std::endl;
    return;
  }






};

