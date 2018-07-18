

#include "Storage.h"


Storage::Storage(char const * pot_name,
		 char const * meta_name,
		 char const * event_name,
		 std::vector<char const *> const & files) :
  fpot_chain(nullptr),
  fmeta_chain(nullptr),
  fevent_chain(nullptr),
  fofile_filter(nullptr),
  ffilter_dir(nullptr),
  fevent_tree(nullptr) {

  if(files.empty()) {
    std::cout << "Empty file list given to Storage\n";
    exit(1);
  }

  if(!CheckFile(pot_name,
		meta_name,
		event_name,
		files.front())) {
    exit(1);
  }
  
  Initialize();

  for(char const * file : files) {
    if(fpot_chain) fpot_chain->Add(file);
    fmeta_chain->Add(file);
    fevent_chain->Add(file);
  }

  if(fmeta_chain->GetEntries() == 0) {
    std::cout << "ERROR: empty meta chain in storage\n";
    exit(1);
  }
  if(fevent_chain->GetEntries() == 0) {
    std::cout << "ERROR: empty event chain in storage\n";
    exit(1);
  }

  SetupChains();

}


Storage::~Storage() {

  if(fofile_filter) fofile_filter->Close();
  if(fpot_chain) delete fpot_chain;
  delete fmeta_chain;
  delete fevent_chain;

}


bool Storage::CheckFile(char const * pot_name,
			char const * meta_name,
			char const * event_name,
			char const * const file_name) {

  TFile * file = TFile::Open(file_name);

  if(!file) {
    std::cout << "File: " << file_name << " not found\n";
    return false;
  }

  TTree * pot_tree = dynamic_cast<TTree*>(file->Get(pot_name));
  TTree * meta_tree = dynamic_cast<TTree*>(file->Get(meta_name));
  TTree * event_tree = dynamic_cast<TTree*>(file->Get(event_name));

  bool result = true;

  if(pot_tree) {
    fpot_chain = new TChain(pot_name);
  }
  else {
    std::cout << "WARNING: first input file has not pot tree named " << pot_name << ", pot information will not be loaded\n";
  }
  if(meta_tree) { 
    fmeta_chain = new TChain(meta_name);
  }
  else {
    result = false;
    std::cout << "ERROR: first input file has no meta_tree named " << meta_name << "\n";
  }
  if(event_tree) {
    fevent_chain = new TChain(event_name);
  }
  else {
    result = false;
    std::cout << "ERROR: first input file has no event_tree named " << event_name << "\n";
  }

  file->Close();

  return result;

}


std::string Storage::GetTDirName(char const * oname) {

  std::string const str(oname);
  size_t const pos = str.rfind("/");
  if(pos == std::string::npos) return {};
  else return str.substr(0, pos);

}


void Storage::MDCD(char const * dir_name) {

  if(!gDirectory->FindKey(dir_name)) gDirectory->mkdir(dir_name);
  gDirectory->cd(dir_name);

}


void Storage::CloneChain(TChain * chain, int const entries) {
  
  TDirectory * start_dir = gDirectory;
  MDCD(GetTDirName(chain->GetName()).c_str());
  TTree * tree = chain->CloneTree(entries);
  tree->Write();
  delete tree;
  start_dir->cd();

} 


void Storage::SetOutputFilterFileName(char const * name) {

  TDirectory * start_dir = gDirectory;

  fofile_filter = TFile::Open(name, "recreate");

  if(fpot_chain) CloneChain(fpot_chain);
  CloneChain(fmeta_chain);
  MDCD(GetTDirName(fevent_chain->GetName()).c_str());
  ffilter_dir = gDirectory;
  fevent_tree = fevent_chain->CloneTree(0);
  
  start_dir->cd();

}


void Storage::Initialize() {

  fnumber_of_events = 0;
  fpot = 0;

  fcurrent_entry = -1;
  
  fpot_producer = nullptr;
  fswtrigger_product = nullptr;
  fopflash_producers = nullptr;
  fhit_producers = nullptr;
  ftrack_producers = nullptr;
  fshower_producers = nullptr;
  frmcmassociation_producers = nullptr;
  
  fopflash_producer_indices = nullptr;
  fhit_producer_indices = nullptr;
  ftrack_producer_indices = nullptr;
  fshower_producer_indices = nullptr;

  //event_chain
  fopflash_producer_indices = nullptr;
  fhit_producer_indices = nullptr;
  ftrack_producer_indices = nullptr;
  fshower_producer_indices = nullptr;

  //Software Trigger
  fswtrigger_getListOfAlgorithms = nullptr;
  fswtrigger_passedAlgo = nullptr;
  fswtrigger_passedPrescaleAlgo = nullptr;
  fswtrigger_vetoAlgo = nullptr;

  //Reco Flash
  freco_opflash_producer_index = nullptr;
  freco_opflash_Time = nullptr;
  freco_opflash_TimeWidth = nullptr;
  freco_opflash_AbsTime = nullptr;
  freco_opflash_Frame = nullptr;
  //std::vector<std::vector<double>> freco_opflash_PEs = nullptr;
  freco_opflash_YCenter = nullptr;
  freco_opflash_YWidth = nullptr;
  freco_opflash_ZCenter = nullptr;
  freco_opflash_ZWidth = nullptr;
  freco_opflash_InBeamFrame = nullptr;
  freco_opflash_OnBeamFrame = nullptr;
  freco_opflash_WireCenters = nullptr;
  freco_opflash_WireWidths = nullptr;
  freco_opflash_TotalPE = nullptr;
  freco_opflash_FastToTotal = nullptr;

  //Reco Hit
  freco_hit_producer_index = nullptr;
  freco_hit_StartTick = nullptr;
  freco_hit_EndTick = nullptr;
  freco_hit_PeakTime = nullptr;
  freco_hit_SigmaPeakTime = nullptr;
  freco_hit_RMS = nullptr;
  freco_hit_PeakAmplitude = nullptr;
  freco_hit_SigmaPeakAmplitude = nullptr;
  freco_hit_SummedADC = nullptr;
  freco_hit_Integral = nullptr;
  freco_hit_SigmaIntegral = nullptr;
  freco_hit_Multiplicity = nullptr;
  freco_hit_LocalIndex = nullptr;
  freco_hit_GoodnessOfFit = nullptr;
  freco_hit_DegreesOfFreedom = nullptr;
  freco_hit_View = nullptr;
  freco_hit_SignalType = nullptr;
  freco_hit_WireID_CryostatID = nullptr;
  freco_hit_WireID_TPCID = nullptr;
  freco_hit_WireID_PlaneID = nullptr;
  freco_hit_WireID_WireID = nullptr;
  //Reco - MC matching
  freco_hit_mc_type = nullptr;
  freco_hit_mc_index = nullptr;
  /*
    freco_hit_true_ideFraction = nullptr;
    freco_hit_true_isMaxIDE = nullptr;
    freco_hit_true_ideNFraction = nullptr;
    freco_hit_true_isMaxIDEN = nullptr;
  */
  freco_hit_true_numElectrons = nullptr;
  freco_hit_true_energy = nullptr;

  //Reco Track
  freco_track_producer_index = nullptr;
  freco_track_NumberTrajectoryPoints = nullptr;
  freco_track_NPoints = nullptr;
  freco_track_FirstPoint = nullptr;
  freco_track_LastPoint = nullptr;
  freco_track_FirstValidPoint = nullptr;
  freco_track_LastValidPoint = nullptr;
  freco_track_CountValidPoints = nullptr;
  freco_track_X = nullptr;
  freco_track_Y = nullptr;
  freco_track_Z = nullptr;
  freco_track_HasMomentum = nullptr;
  freco_track_Length = nullptr;
  freco_track_Chi2 = nullptr;
  freco_track_Chi2PerNdof = nullptr;
  freco_track_Ndof = nullptr;
  freco_track_ParticleId = nullptr;
  freco_track_Theta = nullptr;
  freco_track_Phi = nullptr;
  freco_track_ZenithAngle = nullptr;
  freco_track_AzimuthAngle = nullptr;
  freco_track_VertexDirection_X = nullptr;
  freco_track_VertexDirection_Y = nullptr;
  freco_track_VertexDirection_Z = nullptr;
  freco_track_to_reco_hit = nullptr;
  freco_track_EnergyHelper_resrange = nullptr;
  freco_track_EnergyHelper_dedx = nullptr;
  freco_track_EnergyHelper_energy = nullptr;
  freco_track_EnergyHelperNew_energy_legacy = nullptr;
  freco_track_EnergyHelperNew_energy = nullptr;
  freco_track_EnergyHelperNew_energy_from_dedx = nullptr;
  freco_track_EnergyHelperNew_dedx = nullptr;
  //Reco - MC matching
  freco_track_largest_mc_type = nullptr;
  freco_track_largest_mc_index = nullptr;
  freco_track_largest_ratio = nullptr;
  freco_track_mc_type = nullptr;
  freco_track_mc_index = nullptr;
  freco_track_charge_contribution = nullptr;
  freco_track_charge_total = nullptr;

  //Reco Shower
  freco_shower_producer_index = nullptr;
  freco_shower_Direction_X = nullptr;
  freco_shower_Direction_Y = nullptr;
  freco_shower_Direction_Z = nullptr;
  freco_shower_DirectionErr_X = nullptr;
  freco_shower_DirectionErr_Y = nullptr;
  freco_shower_DirectionErr_Z = nullptr;
  freco_shower_ShowerStart_X = nullptr;
  freco_shower_ShowerStart_Y = nullptr;
  freco_shower_ShowerStart_Z = nullptr;
  freco_shower_ShowerStartErr_X = nullptr;
  freco_shower_ShowerStartErr_Y = nullptr;
  freco_shower_ShowerStartErr_Z = nullptr;
  freco_shower_Energy = nullptr;
  freco_shower_EnergyErr = nullptr;
  freco_shower_MIPEnergy = nullptr;
  freco_shower_MIPEnergyErr = nullptr;
  freco_shower_best_plane = nullptr;
  freco_shower_Length = nullptr;
  freco_shower_OpenAngle = nullptr;
  freco_shower_dEdx = nullptr;
  freco_shower_dEdxErr = nullptr;
  freco_shower_has_open_angle = nullptr;
  freco_shower_has_length = nullptr;
  freco_shower_to_reco_hit = nullptr;
  freco_shower_EnergyHelper_energy_legacy = nullptr;
  freco_shower_EnergyHelper_energy = nullptr;
  freco_shower_EnergyHelper_dedx = nullptr;
  freco_shower_EnergyHelperNew_energy_legacy = nullptr;
  freco_shower_EnergyHelperNew_energy = nullptr;
  freco_shower_EnergyHelperNew_dedx = nullptr;
  //Reco - MC matching
  freco_shower_largest_mc_type = nullptr;
  freco_shower_largest_mc_index = nullptr;
  freco_shower_largest_ratio = nullptr;
  freco_shower_mc_type = nullptr;
  freco_shower_mc_index = nullptr;
  freco_shower_charge_contribution = nullptr;
  freco_shower_charge_total = nullptr;

  //Pandora
  fpfp_pdg = nullptr;
  fpfp_vertex_X = nullptr;
  fpfp_vertex_Y = nullptr;
  fpfp_vertex_Z = nullptr;
  fpfp_original_index = nullptr;
  fpfp_children = nullptr;

  //Truth
  fnu_pdg = nullptr;
  fnu_energy = nullptr;
  flep_pdg = nullptr;
  flep_energy = nullptr;
  fccnc = nullptr;
  fmode = nullptr;
  finteraction_type = nullptr;

  ftrue_nu_E = nullptr;
  ftrue_nuvertx = nullptr;
  ftrue_nuverty = nullptr;
  ftrue_nuvertz = nullptr;

  ftrue_nu_vtx_tpc_contained = nullptr;
  ftrue_nu_vtx_fid_contained = nullptr;

  //NCDelta indices
  fis_delta_rad = nullptr;
  fdelta_index = nullptr;
  fdelta_photon_index = nullptr;
  fdelta_mcshower_index = nullptr;
  fdelta_proton_index = nullptr;
  fdelta_mctrack_index = nullptr;

  //GENIE MCParticle
  fgenie_particle_TrackId = nullptr;
  fgenie_particle_StatusCode = nullptr;
  fgenie_particle_PdgCode = nullptr;
  fgenie_particle_Mother = nullptr;
  fgenie_particle_X = nullptr;
  fgenie_particle_Y = nullptr;
  fgenie_particle_Z = nullptr;
  fgenie_particle_T = nullptr;
  fgenie_particle_Px = nullptr;
  fgenie_particle_Py = nullptr;
  fgenie_particle_Pz = nullptr;
  fgenie_particle_E = nullptr;

  //MCParticle
  fmcparticle_TrackId = nullptr;
  fmcparticle_StatusCode = nullptr;
  fmcparticle_PdgCode = nullptr;
  fmcparticle_Mother = nullptr;  
  /* 
     fmcparticle_X = nullptr;
     fmcparticle_Y = nullptr;
     fmcparticle_Z = nullptr;
     fmcparticle_T = nullptr;
     fmcparticle_Px = nullptr;
     fmcparticle_Py = nullptr;
     fmcparticle_Pz = nullptr;
     fmcparticle_E = nullptr;
  */

  //MCTrack
  fmctrack_Origin = nullptr;
  fmctrack_PdgCode = nullptr;
  fmctrack_TrackID = nullptr;
  fmctrack_Process = nullptr;
  fmctrack_Start_X = nullptr;
  fmctrack_Start_Y = nullptr;
  fmctrack_Start_Z = nullptr;
  fmctrack_Start_T = nullptr;
  fmctrack_Start_Px = nullptr;
  fmctrack_Start_Py = nullptr;
  fmctrack_Start_Pz = nullptr;
  fmctrack_Start_E = nullptr;
  fmctrack_End_X = nullptr;
  fmctrack_End_Y = nullptr;
  fmctrack_End_Z = nullptr;
  fmctrack_End_T = nullptr;
  fmctrack_End_Px = nullptr;
  fmctrack_End_Py = nullptr;
  fmctrack_End_Pz = nullptr;
  fmctrack_End_E = nullptr;
  fmctrack_X = nullptr;
  fmctrack_Y = nullptr;
  fmctrack_Z = nullptr;
  fmctrack_T = nullptr;
  fmctrack_Px = nullptr;
  fmctrack_Py = nullptr;
  fmctrack_Pz = nullptr;
  fmctrack_E = nullptr;
  fmctrack_dQdx = nullptr;
  fmctrack_dEdx = nullptr;
  fmctrack_MotherPdgCode = nullptr;
  fmctrack_MotherTrackID = nullptr;
  fmctrack_MotherProcess = nullptr;
  fmctrack_AncestorPdgCode = nullptr;
  fmctrack_AncestorTrackID = nullptr;
  fmctrack_AncestorProcess = nullptr;
  fmctrack_contributed_charge = nullptr;

  //MCShower
  fmcshower_Origin = nullptr;
  fmcshower_PdgCode = nullptr;
  fmcshower_TrackID = nullptr;
  fmcshower_Process = nullptr;
  fmcshower_Start_X = nullptr;
  fmcshower_Start_Y = nullptr;
  fmcshower_Start_Z = nullptr;
  fmcshower_Start_T = nullptr;
  fmcshower_Start_Px = nullptr;
  fmcshower_Start_Py = nullptr;
  fmcshower_Start_Pz = nullptr;
  fmcshower_Start_E = nullptr;
  fmcshower_End_X = nullptr;
  fmcshower_End_Y = nullptr;
  fmcshower_End_Z = nullptr;
  fmcshower_End_T = nullptr;
  fmcshower_End_Px = nullptr;
  fmcshower_End_Py = nullptr;
  fmcshower_End_Pz = nullptr;
  fmcshower_End_E = nullptr;
  fmcshower_MotherPdgCode = nullptr;
  fmcshower_MotherTrackID = nullptr;
  fmcshower_MotherProcess = nullptr;
  fmcshower_AncestorPdgCode = nullptr;
  fmcshower_AncestorTrackID = nullptr;
  fmcshower_AncestorProcess = nullptr;
  fmcshower_DetProfile_X = nullptr;
  fmcshower_DetProfile_Y = nullptr;
  fmcshower_DetProfile_Z = nullptr;
  fmcshower_DetProfile_T = nullptr;
  fmcshower_DetProfile_Px = nullptr;
  fmcshower_DetProfile_Py = nullptr;
  fmcshower_DetProfile_Pz = nullptr;
  fmcshower_DetProfile_E = nullptr;
  fmcshower_DaughterTrackID = nullptr;
  fmcshower_Charge = nullptr;
  fmcshower_dQdx = nullptr;
  fmcshower_StartDir_X = nullptr;
  fmcshower_StartDir_Y = nullptr;
  fmcshower_StartDir_Z = nullptr;
  fmcshower_contributed_charge = nullptr;

}


void Storage::SetupChains() {

  ProcessMetaChain();
  ProcessEventChain();
  if(fpot_chain) ProcessPotChain();

}


void Storage::ProcessPotChain() {

  int total_number_of_events = 0;
  int number_of_events;
  double pot;

  fpot_chain->SetBranchAddress("number_of_events", &number_of_events);
  fpot_chain->SetBranchAddress("pot", &pot);   

  for(int i = 0; i < fpot_chain->GetEntries(); ++i) {
    fpot_chain->GetEntry(i);
    total_number_of_events += number_of_events;
    fpot += pot;
  }

  if(total_number_of_events != fnumber_of_events) std::cout << "WARNING: number of events in POT tree: " << total_number_of_events << " != number of events in event tree: " << fnumber_of_events << ", don't trust POT: " << fpot << "\n";

}


void Storage::ProcessMetaChain() {

  int is_heavy;
  int is_mc;

  fmeta_chain->SetBranchAddress("is_heavy", &is_heavy);
  fmeta_chain->SetBranchAddress("is_mc", &is_mc);
  fmeta_chain->SetBranchAddress("swtrigger_product", &fswtrigger_product);
  fmeta_chain->SetBranchAddress("opflash_producers", &fopflash_producers);
  fmeta_chain->SetBranchAddress("hit_producers", &fhit_producers);
  fmeta_chain->SetBranchAddress("track_producers", &ftrack_producers);
  fmeta_chain->SetBranchAddress("shower_producers", &fshower_producers);
  fmeta_chain->SetBranchAddress("rmcmassociation_producers", &frmcmassociation_producers);
  fmeta_chain->SetBranchAddress("DetHalfHeight", &fDetHalfHeight);
  fmeta_chain->SetBranchAddress("DetHalfWidth", &fDetHalfWidth);
  fmeta_chain->SetBranchAddress("DetLength", &fDetLength);
  fmeta_chain->SetBranchAddress("mc_type_shower", &fmc_type_shower);
  fmeta_chain->SetBranchAddress("mc_type_track", &fmc_type_track);
  fmeta_chain->SetBranchAddress("mc_type_particle", &fmc_type_particle);

  fmeta_chain->GetEntry(0);

  if(is_heavy) fheavy = true;
  else fheavy = false;
  if(is_mc) fmc = true;
  else fmc = false;

  if(frmcmassociation_producers->size() > 0) frmcm_bool = true;

}


void Storage::ProcessEventChain() {

  fnumber_of_events = fevent_chain->GetEntries();

  fevent_chain->SetBranchAddress("opflash_producer_indices", &fopflash_producer_indices);
  fevent_chain->SetBranchAddress("hit_producer_indices", &fhit_producer_indices);
  fevent_chain->SetBranchAddress("track_producer_indices", &ftrack_producer_indices);
  fevent_chain->SetBranchAddress("shower_producer_indices", &fshower_producer_indices);

  fevent_chain->SetBranchAddress("run_number", &frun_number);
  fevent_chain->SetBranchAddress("subrun_number", &fsubrun_number); 
  fevent_chain->SetBranchAddress("event_number", &fevent_number);

  //fevent_chain->SetBranchAddress("swtrigger_producer_index", &fswtrigger_producer_index);
  fevent_chain->SetBranchAddress("swtrigger_getListOfAlgorithms", &fswtrigger_getListOfAlgorithms);
  fevent_chain->SetBranchAddress("swtrigger_passedAlgo", &fswtrigger_passedAlgo);
  fevent_chain->SetBranchAddress("swtrigger_passedPrescaleAlgo", &fswtrigger_passedPrescaleAlgo);
  fevent_chain->SetBranchAddress("swtrigger_vetoAlgo", &fswtrigger_vetoAlgo);
  fevent_chain->SetBranchAddress("swtrigger_passedAlgos", &fswtrigger_passedAlgos);
  fevent_chain->SetBranchAddress("swtrigger_vetoAlgos", &fswtrigger_vetoAlgos);
  fevent_chain->SetBranchAddress("swtrigger_passedPrescaleAlgos", &fswtrigger_passedPrescaleAlgos);
  fevent_chain->SetBranchAddress("swtrigger_passed_swtrigger", &fswtrigger_passed_swtrigger);

  fevent_chain->SetBranchAddress("reco_opflash_producer_index", &freco_opflash_producer_index);
  fevent_chain->SetBranchAddress("reco_opflash_Time", &freco_opflash_Time);
  if(fheavy) {
    fevent_chain->SetBranchAddress("reco_opflash_TimeWidth", &freco_opflash_TimeWidth);
    fevent_chain->SetBranchAddress("reco_opflash_AbsTime", &freco_opflash_AbsTime);
    fevent_chain->SetBranchAddress("reco_opflash_Frame", &freco_opflash_Frame);
    //fevent_chain->SetBranchAddress("reco_opflash_PEs", &freco_opflash_PEs);
  }
  fevent_chain->SetBranchAddress("reco_opflash_YCenter", &freco_opflash_YCenter);
  fevent_chain->SetBranchAddress("reco_opflash_YWidth", &freco_opflash_YWidth);
  fevent_chain->SetBranchAddress("reco_opflash_ZCenter", &freco_opflash_ZCenter);
  fevent_chain->SetBranchAddress("reco_opflash_ZWidth", &freco_opflash_ZWidth);
  fevent_chain->SetBranchAddress("reco_opflash_InBeamFrame", &freco_opflash_InBeamFrame);
  fevent_chain->SetBranchAddress("reco_opflash_OnBeamFrame", &freco_opflash_OnBeamFrame);
  fevent_chain->SetBranchAddress("reco_opflash_WireCenters", &freco_opflash_WireCenters);
  fevent_chain->SetBranchAddress("reco_opflash_WireWidths", &freco_opflash_WireWidths);
  fevent_chain->SetBranchAddress("reco_opflash_TotalPE", &freco_opflash_TotalPE);
  if(fheavy) fevent_chain->SetBranchAddress("reco_opflash_FastToTotal", &freco_opflash_FastToTotal);

  if(fheavy) {
    fevent_chain->SetBranchAddress("reco_hit_producer_index", &freco_hit_producer_index);
    fevent_chain->SetBranchAddress("reco_hit_StartTick", &freco_hit_StartTick);
    fevent_chain->SetBranchAddress("reco_hit_EndTick", &freco_hit_EndTick);
    fevent_chain->SetBranchAddress("reco_hit_PeakTime", &freco_hit_PeakTime);
    if(fheavy) {
      fevent_chain->SetBranchAddress("reco_hit_SigmaPeakTime", &freco_hit_SigmaPeakTime);
      fevent_chain->SetBranchAddress("reco_hit_RMS", &freco_hit_RMS);
      fevent_chain->SetBranchAddress("reco_hit_PeakAmplitude", &freco_hit_PeakAmplitude);
      fevent_chain->SetBranchAddress("reco_hit_SigmaPeakAmplitude", &freco_hit_SigmaPeakAmplitude);
    }
    fevent_chain->SetBranchAddress("reco_hit_SummedADC", &freco_hit_SummedADC);
    fevent_chain->SetBranchAddress("reco_hit_Integral", &freco_hit_Integral);
    if(fheavy) {
      fevent_chain->SetBranchAddress("reco_hit_SigmaIntegral", &freco_hit_SigmaIntegral);
      fevent_chain->SetBranchAddress("reco_hit_Multiplicity", &freco_hit_Multiplicity);
      fevent_chain->SetBranchAddress("reco_hit_LocalIndex", &freco_hit_LocalIndex);
      fevent_chain->SetBranchAddress("reco_hit_GoodnessOfFit", &freco_hit_GoodnessOfFit);
      fevent_chain->SetBranchAddress("reco_hit_DegreesOfFreedom", &freco_hit_DegreesOfFreedom);
    }
    fevent_chain->SetBranchAddress("reco_hit_View", &freco_hit_View);
    fevent_chain->SetBranchAddress("reco_hit_SignalType", &freco_hit_SignalType);
    fevent_chain->SetBranchAddress("reco_hit_WireID_CryostatID", &freco_hit_WireID_CryostatID);
    fevent_chain->SetBranchAddress("reco_hit_WireID_TPCID", &freco_hit_WireID_TPCID);
    fevent_chain->SetBranchAddress("reco_hit_WireID_PlaneID", &freco_hit_WireID_PlaneID);
    fevent_chain->SetBranchAddress("reco_hit_WireID_WireID", &freco_hit_WireID_WireID);
    if(fmc && frmcm_bool) {
      fevent_chain->SetBranchAddress("reco_hit_mc_type", &freco_hit_mc_type);
      fevent_chain->SetBranchAddress("reco_hit_mc_index", &freco_hit_mc_index);
      /*
	fevent_chain->SetBranchAddress("reco_hit_true_ideFraction", &freco_hit_true_ideFraction);
	fevent_chain->SetBranchAddress("reco_hit_true_isMaxIDE", &freco_hit_true_isMaxIDE);
	fevent_chain->SetBranchAddress("reco_hit_true_ideNFraction", &freco_hit_true_ideNFraction);
	fevent_chain->SetBranchAddress("reco_hit_true_isMaxIDEN", &freco_hit_true_isMaxIDEN);
      */
      fevent_chain->SetBranchAddress("reco_hit_true_numElectrons", &freco_hit_true_numElectrons);
      if(fheavy) fevent_chain->SetBranchAddress("reco_hit_true_energy", &freco_hit_true_energy);
    }
  }

  fevent_chain->SetBranchAddress("reco_track_producer_index", &freco_track_producer_index);
  if(fheavy) {
    fevent_chain->SetBranchAddress("reco_track_NumberTrajectoryPoints", &freco_track_NumberTrajectoryPoints);
    fevent_chain->SetBranchAddress("reco_track_NPoints", &freco_track_NPoints);
    fevent_chain->SetBranchAddress("reco_track_FirstPoint", &freco_track_FirstPoint);
    fevent_chain->SetBranchAddress("reco_track_LastPoint", &freco_track_LastPoint);
    fevent_chain->SetBranchAddress("reco_track_FirstValidPoint", &freco_track_FirstValidPoint);
    fevent_chain->SetBranchAddress("reco_track_LastValidPoint", &freco_track_LastValidPoint);
    fevent_chain->SetBranchAddress("reco_track_CountValidPoints", &freco_track_CountValidPoints);
  }
  fevent_chain->SetBranchAddress("reco_track_X", &freco_track_X);
  fevent_chain->SetBranchAddress("reco_track_Y", &freco_track_Y);
  fevent_chain->SetBranchAddress("reco_track_Z", &freco_track_Z);
  if(fheavy) {
    fevent_chain->SetBranchAddress("reco_track_HasMomentum", &freco_track_HasMomentum);
    fevent_chain->SetBranchAddress("reco_track_Length", &freco_track_Length);
    fevent_chain->SetBranchAddress("reco_track_Chi2", &freco_track_Chi2);
    fevent_chain->SetBranchAddress("reco_track_Chi2PerNdof", &freco_track_Chi2PerNdof);
    fevent_chain->SetBranchAddress("reco_track_Ndof", &freco_track_Ndof);
    fevent_chain->SetBranchAddress("reco_track_ParticleId", &freco_track_ParticleId);
  }
  fevent_chain->SetBranchAddress("reco_track_Theta", &freco_track_Theta);
  fevent_chain->SetBranchAddress("reco_track_Phi", &freco_track_Phi);
  fevent_chain->SetBranchAddress("reco_track_ZenithAngle", &freco_track_ZenithAngle);
  fevent_chain->SetBranchAddress("reco_track_AzimuthAngle", &freco_track_AzimuthAngle);
  fevent_chain->SetBranchAddress("reco_track_VertexDirection_X", &freco_track_VertexDirection_X);
  fevent_chain->SetBranchAddress("reco_track_VertexDirection_Y", &freco_track_VertexDirection_Y);
  fevent_chain->SetBranchAddress("reco_track_VertexDirection_Z", &freco_track_VertexDirection_Z);
  fevent_chain->SetBranchAddress("reco_track_to_reco_hit", &freco_track_to_reco_hit);
  fevent_chain->SetBranchAddress("reco_track_EnergyHelper_resrange", &freco_track_EnergyHelper_resrange);
  fevent_chain->SetBranchAddress("reco_track_EnergyHelper_dedx", &freco_track_EnergyHelper_dedx);
  fevent_chain->SetBranchAddress("reco_track_EnergyHelper_energy", &freco_track_EnergyHelper_energy);
  fevent_chain->SetBranchAddress("reco_track_EnergyHelperNew_energy_legacy", &freco_track_EnergyHelperNew_energy_legacy);
  fevent_chain->SetBranchAddress("reco_track_EnergyHelperNew_energy", &freco_track_EnergyHelperNew_energy);
  fevent_chain->SetBranchAddress("reco_track_EnergyHelperNew_energy_from_dedx", &freco_track_EnergyHelperNew_energy_from_dedx);
  fevent_chain->SetBranchAddress("reco_track_EnergyHelperNew_dedx", &freco_track_EnergyHelperNew_dedx);

  if(fmc && frmcm_bool) {
    fevent_chain->SetBranchAddress("reco_track_largest_mc_type", &freco_track_largest_mc_type);
    fevent_chain->SetBranchAddress("reco_track_largest_mc_index", &freco_track_largest_mc_index);
    fevent_chain->SetBranchAddress("reco_track_largest_ratio", &freco_track_largest_ratio);
    //if(fheavy) {
      fevent_chain->SetBranchAddress("reco_track_mc_type", &freco_track_mc_type);
      fevent_chain->SetBranchAddress("reco_track_mc_index", &freco_track_mc_index);
      fevent_chain->SetBranchAddress("reco_track_charge_contribution", &freco_track_charge_contribution);
      //}
    fevent_chain->SetBranchAddress("reco_track_charge_total", &freco_track_charge_total);
  }

  fevent_chain->SetBranchAddress("reco_shower_producer_index", &freco_shower_producer_index);
  fevent_chain->SetBranchAddress("reco_shower_Direction_x", &freco_shower_Direction_X);
  fevent_chain->SetBranchAddress("reco_shower_Direction_y", &freco_shower_Direction_Y);
  fevent_chain->SetBranchAddress("reco_shower_Direction_z", &freco_shower_Direction_Z);
  if(fheavy) {
    fevent_chain->SetBranchAddress("reco_shower_DirectionErr_x", &freco_shower_DirectionErr_X);
    fevent_chain->SetBranchAddress("reco_shower_DirectionErr_y", &freco_shower_DirectionErr_Y);
    fevent_chain->SetBranchAddress("reco_shower_DirectionErr_z", &freco_shower_DirectionErr_Z);
  }
  fevent_chain->SetBranchAddress("reco_shower_ShowerStart_x", &freco_shower_ShowerStart_X);
  fevent_chain->SetBranchAddress("reco_shower_ShowerStart_y", &freco_shower_ShowerStart_Y);
  fevent_chain->SetBranchAddress("reco_shower_ShowerStart_z", &freco_shower_ShowerStart_Z);
  if(fheavy) {
    fevent_chain->SetBranchAddress("reco_shower_ShowerStartErr_x", &freco_shower_ShowerStartErr_X);
    fevent_chain->SetBranchAddress("reco_shower_ShowerStartErr_y", &freco_shower_ShowerStartErr_Y);
    fevent_chain->SetBranchAddress("reco_shower_ShowerStartErr_z", &freco_shower_ShowerStartErr_Z);
    fevent_chain->SetBranchAddress("reco_shower_Energy", &freco_shower_Energy);
    fevent_chain->SetBranchAddress("reco_shower_EnergyErr", &freco_shower_EnergyErr);
    fevent_chain->SetBranchAddress("reco_shower_MIPEnergy", &freco_shower_MIPEnergy);
    fevent_chain->SetBranchAddress("reco_shower_MIPEnergyErr", &freco_shower_MIPEnergyErr);
  }
  fevent_chain->SetBranchAddress("reco_shower_best_plane", &freco_shower_best_plane);
  fevent_chain->SetBranchAddress("reco_shower_Length", &freco_shower_Length);
  fevent_chain->SetBranchAddress("reco_shower_OpenAngle", &freco_shower_OpenAngle);
  if(fheavy) {
    fevent_chain->SetBranchAddress("reco_shower_dEdx", &freco_shower_dEdx);
    fevent_chain->SetBranchAddress("reco_shower_dEdxErr", &freco_shower_dEdxErr);
    fevent_chain->SetBranchAddress("reco_shower_has_open_angle", &freco_shower_has_open_angle);
    fevent_chain->SetBranchAddress("reco_shower_has_length", &freco_shower_has_length);
  }
  fevent_chain->SetBranchAddress("reco_shower_to_reco_hit", &freco_shower_to_reco_hit);
  fevent_chain->SetBranchAddress("reco_shower_EnergyHelper_energy_legacy", &freco_shower_EnergyHelper_energy_legacy);
  fevent_chain->SetBranchAddress("reco_shower_EnergyHelper_energy", &freco_shower_EnergyHelper_energy);
  fevent_chain->SetBranchAddress("reco_shower_EnergyHelper_dedx", &freco_shower_EnergyHelper_dedx);
  fevent_chain->SetBranchAddress("reco_shower_EnergyHelperNew_energy_legacy", &freco_shower_EnergyHelperNew_energy_legacy);
  fevent_chain->SetBranchAddress("reco_shower_EnergyHelperNew_energy", &freco_shower_EnergyHelperNew_energy);
  fevent_chain->SetBranchAddress("reco_shower_EnergyHelperNew_dedx", &freco_shower_EnergyHelperNew_dedx);
  if(fmc && frmcm_bool) { 
    fevent_chain->SetBranchAddress("reco_shower_largest_mc_type", &freco_shower_largest_mc_type);
    fevent_chain->SetBranchAddress("reco_shower_largest_mc_index", &freco_shower_largest_mc_index);
    fevent_chain->SetBranchAddress("reco_shower_largest_ratio", &freco_shower_largest_ratio);
    //if(fheavy) {
      fevent_chain->SetBranchAddress("reco_shower_mc_type", &freco_shower_mc_type);
      fevent_chain->SetBranchAddress("reco_shower_mc_index", &freco_shower_mc_index);
      fevent_chain->SetBranchAddress("reco_shower_charge_contribution", &freco_shower_charge_contribution);
      //}
    fevent_chain->SetBranchAddress("reco_shower_charge_total", &freco_shower_charge_total);
  }

  fevent_chain->SetBranchAddress("pfp_pdg", &fpfp_pdg);
  fevent_chain->SetBranchAddress("pfp_vertex_X", &fpfp_vertex_X);
  fevent_chain->SetBranchAddress("pfp_vertex_Y", &fpfp_vertex_Y);
  fevent_chain->SetBranchAddress("pfp_vertex_Z", &fpfp_vertex_Z);
  fevent_chain->SetBranchAddress("pfp_original_index", &fpfp_original_index);
  fevent_chain->SetBranchAddress("pfp_children", &fpfp_children);

  if(fmc) {

    fevent_chain->SetBranchAddress("nu_pdg", &fnu_pdg);
    fevent_chain->SetBranchAddress("nu_energy", &fnu_energy);
    fevent_chain->SetBranchAddress("lep_pdg", &flep_pdg);
    fevent_chain->SetBranchAddress("lep_energy", &flep_energy);
    fevent_chain->SetBranchAddress("ccnc", &fccnc);
    fevent_chain->SetBranchAddress("mode", &fmode);
    fevent_chain->SetBranchAddress("interaction_type", &finteraction_type);

    fevent_chain->SetBranchAddress("true_nu_E", &ftrue_nu_E);

    fevent_chain->SetBranchAddress("true_nuvertx", &ftrue_nuvertx);
    fevent_chain->SetBranchAddress("true_nuverty", &ftrue_nuverty);
    fevent_chain->SetBranchAddress("true_nuvertz", &ftrue_nuvertz);

    fevent_chain->SetBranchAddress("true_nu_vtx_tpc_contained", &ftrue_nu_vtx_tpc_contained); 
    fevent_chain->SetBranchAddress("true_nu_vtx_fid_contained", &ftrue_nu_vtx_fid_contained); 
    
    fevent_chain->SetBranchAddress("delta_mct_index", &fdelta_mct_index);
    fevent_chain->SetBranchAddress("is_delta_rad", &fis_delta_rad);
    fevent_chain->SetBranchAddress("delta_index", &fdelta_index);
    fevent_chain->SetBranchAddress("delta_photon_index", &fdelta_photon_index);
    fevent_chain->SetBranchAddress("delta_mcshower_index", &fdelta_mcshower_index);
    fevent_chain->SetBranchAddress("delta_proton_index", &fdelta_proton_index);
    fevent_chain->SetBranchAddress("delta_mctrack_index", &fdelta_mctrack_index);

    fevent_chain->SetBranchAddress("genie_particle_TrackId", &fgenie_particle_TrackId);
    fevent_chain->SetBranchAddress("genie_particle_StatusCode", &fgenie_particle_StatusCode);
    fevent_chain->SetBranchAddress("genie_particle_PdgCode", &fgenie_particle_PdgCode);
    fevent_chain->SetBranchAddress("genie_particle_Mother", &fgenie_particle_Mother);
    fevent_chain->SetBranchAddress("genie_particle_X", &fgenie_particle_X);
    fevent_chain->SetBranchAddress("genie_particle_Y", &fgenie_particle_Y);
    fevent_chain->SetBranchAddress("genie_particle_Z", &fgenie_particle_Z);
    fevent_chain->SetBranchAddress("genie_particle_Px", &fgenie_particle_Px);
    fevent_chain->SetBranchAddress("genie_particle_Py", &fgenie_particle_Py);
    fevent_chain->SetBranchAddress("genie_particle_Pz", &fgenie_particle_Pz);
    fevent_chain->SetBranchAddress("genie_particle_E", &fgenie_particle_E);

    fevent_chain->SetBranchAddress("mcparticle_TrackId", &fmcparticle_TrackId);
    fevent_chain->SetBranchAddress("mcparticle_StatusCode", &fmcparticle_StatusCode);
    fevent_chain->SetBranchAddress("mcparticle_PdgCode", &fmcparticle_PdgCode);
    fevent_chain->SetBranchAddress("mcparticle_Mother", &fmcparticle_Mother);
    /*
    fevent_chain->SetBranchAddress("mcparticle_X", &fmcparticle_X);
    fevent_chain->SetBranchAddress("mcparticle_Y", &fmcparticle_Y);
    fevent_chain->SetBranchAddress("mcparticle_Z", &fmcparticle_Z);
    fevent_chain->SetBranchAddress("mcparticle_Px", &fmcparticle_Px);
    fevent_chain->SetBranchAddress("mcparticle_Py", &fmcparticle_Py);
    fevent_chain->SetBranchAddress("mcparticle_Pz", &fmcparticle_Pz);
    fevent_chain->SetBranchAddress("mcparticle_E", &fmcparticle_E);
    */

    fevent_chain->SetBranchAddress("mctrack_Origin", &fmctrack_Origin);
    fevent_chain->SetBranchAddress("mctrack_PdgCode", &fmctrack_PdgCode);
    fevent_chain->SetBranchAddress("mctrack_TrackID", &fmctrack_TrackID);
    fevent_chain->SetBranchAddress("mctrack_Process", &fmctrack_Process);
    fevent_chain->SetBranchAddress("mctrack_Start_X", &fmctrack_Start_X);
    fevent_chain->SetBranchAddress("mctrack_Start_Y", &fmctrack_Start_Y);
    fevent_chain->SetBranchAddress("mctrack_Start_Z", &fmctrack_Start_Z);
    fevent_chain->SetBranchAddress("mctrack_Start_T", &fmctrack_Start_T);
    fevent_chain->SetBranchAddress("mctrack_Start_Px", &fmctrack_Start_Px);
    fevent_chain->SetBranchAddress("mctrack_Start_Py", &fmctrack_Start_Py);
    fevent_chain->SetBranchAddress("mctrack_Start_Pz", &fmctrack_Start_Pz);
    fevent_chain->SetBranchAddress("mctrack_Start_E", &fmctrack_Start_E);
    fevent_chain->SetBranchAddress("mctrack_End_X", &fmctrack_End_X);
    fevent_chain->SetBranchAddress("mctrack_End_Y", &fmctrack_End_Y);
    fevent_chain->SetBranchAddress("mctrack_End_Z", &fmctrack_End_Z);
    fevent_chain->SetBranchAddress("mctrack_End_T", &fmctrack_End_T);
    fevent_chain->SetBranchAddress("mctrack_End_Px", &fmctrack_End_Px);
    fevent_chain->SetBranchAddress("mctrack_End_Py", &fmctrack_End_Py);
    fevent_chain->SetBranchAddress("mctrack_End_Pz", &fmctrack_End_Pz);
    fevent_chain->SetBranchAddress("mctrack_End_E", &fmctrack_End_E);
    fevent_chain->SetBranchAddress("mctrack_X", &fmctrack_X);
    fevent_chain->SetBranchAddress("mctrack_Y", &fmctrack_Y);
    fevent_chain->SetBranchAddress("mctrack_Z", &fmctrack_Z);
    fevent_chain->SetBranchAddress("mctrack_T", &fmctrack_T);
    if(fheavy) {
      fevent_chain->SetBranchAddress("mctrack_Px", &fmctrack_Px);
      fevent_chain->SetBranchAddress("mctrack_Py", &fmctrack_Py);
      fevent_chain->SetBranchAddress("mctrack_Pz", &fmctrack_Pz);
      fevent_chain->SetBranchAddress("mctrack_E", &fmctrack_E);
      fevent_chain->SetBranchAddress("mctrack_dQdx", &fmctrack_dQdx);
      fevent_chain->SetBranchAddress("mctrack_dEdx", &fmctrack_dEdx);
    }
    fevent_chain->SetBranchAddress("mctrack_MotherTrackID", &fmctrack_MotherTrackID);
    fevent_chain->SetBranchAddress("mctrack_MotherPdgCode", &fmctrack_MotherPdgCode);
    fevent_chain->SetBranchAddress("mctrack_MotherProcess", &fmctrack_MotherProcess);
    fevent_chain->SetBranchAddress("mctrack_AncestorTrackID", &fmctrack_AncestorTrackID);
    fevent_chain->SetBranchAddress("mctrack_AncestorPdgCode", &fmctrack_AncestorPdgCode);
    fevent_chain->SetBranchAddress("mctrack_AncestorProcess", &fmctrack_AncestorProcess);
    if(fmc && frmcm_bool) fevent_chain->SetBranchAddress("mctrack_contributed_charge", &fmctrack_contributed_charge);

    fevent_chain->SetBranchAddress("mcshower_Origin", &fmcshower_Origin);
    fevent_chain->SetBranchAddress("mcshower_PdgCode", &fmcshower_PdgCode);
    fevent_chain->SetBranchAddress("mcshower_TrackID", &fmcshower_TrackID);
    fevent_chain->SetBranchAddress("mcshower_Process", &fmcshower_Process);
    fevent_chain->SetBranchAddress("mcshower_Start_X", &fmcshower_Start_X);
    fevent_chain->SetBranchAddress("mcshower_Start_Y", &fmcshower_Start_Y);
    fevent_chain->SetBranchAddress("mcshower_Start_Z", &fmcshower_Start_Z);
    fevent_chain->SetBranchAddress("mcshower_Start_T", &fmcshower_Start_T);
    fevent_chain->SetBranchAddress("mcshower_Start_Px", &fmcshower_Start_Px);
    fevent_chain->SetBranchAddress("mcshower_Start_Py", &fmcshower_Start_Py);
    fevent_chain->SetBranchAddress("mcshower_Start_Pz", &fmcshower_Start_Pz);
    fevent_chain->SetBranchAddress("mcshower_Start_E", &fmcshower_Start_E);
    fevent_chain->SetBranchAddress("mcshower_End_X", &fmcshower_End_X);
    fevent_chain->SetBranchAddress("mcshower_End_Y", &fmcshower_End_Y);
    fevent_chain->SetBranchAddress("mcshower_End_Z", &fmcshower_End_Z);
    fevent_chain->SetBranchAddress("mcshower_End_T", &fmcshower_End_T);
    fevent_chain->SetBranchAddress("mcshower_End_Px", &fmcshower_End_Px);
    fevent_chain->SetBranchAddress("mcshower_End_Py", &fmcshower_End_Py);
    fevent_chain->SetBranchAddress("mcshower_End_Pz", &fmcshower_End_Pz);
    fevent_chain->SetBranchAddress("mcshower_End_E", &fmcshower_End_E);
    fevent_chain->SetBranchAddress("mcshower_MotherTrackID", &fmcshower_MotherTrackID);
    fevent_chain->SetBranchAddress("mcshower_MotherPdgCode", &fmcshower_MotherPdgCode);
    fevent_chain->SetBranchAddress("mcshower_MotherProcess", &fmcshower_MotherProcess);
    fevent_chain->SetBranchAddress("mcshower_AncestorTrackID", &fmcshower_AncestorTrackID);
    fevent_chain->SetBranchAddress("mcshower_AncestorPdgCode", &fmcshower_AncestorPdgCode);
    fevent_chain->SetBranchAddress("mcshower_AncestorProcess", &fmcshower_AncestorProcess);
    fevent_chain->SetBranchAddress("mcshower_DetProfile_X", &fmcshower_DetProfile_X);
    fevent_chain->SetBranchAddress("mcshower_DetProfile_Y", &fmcshower_DetProfile_Y);
    fevent_chain->SetBranchAddress("mcshower_DetProfile_Z", &fmcshower_DetProfile_Z);
    fevent_chain->SetBranchAddress("mcshower_DetProfile_T", &fmcshower_DetProfile_T);
    fevent_chain->SetBranchAddress("mcshower_DetProfile_Px", &fmcshower_DetProfile_Px);
    fevent_chain->SetBranchAddress("mcshower_DetProfile_Py", &fmcshower_DetProfile_Py);
    fevent_chain->SetBranchAddress("mcshower_DetProfile_Pz", &fmcshower_DetProfile_Pz);
    fevent_chain->SetBranchAddress("mcshower_DetProfile_E", &fmcshower_DetProfile_E);
    fevent_chain->SetBranchAddress("mcshower_DaughterTrackID", &fmcshower_DaughterTrackID);
    if(fheavy) {
      fevent_chain->SetBranchAddress("mcshower_Charge", &fmcshower_Charge);
      fevent_chain->SetBranchAddress("mcshower_dQdx", &fmcshower_dQdx);
    }
    fevent_chain->SetBranchAddress("mcshower_StartDir_X", &fmcshower_StartDir_X);
    fevent_chain->SetBranchAddress("mcshower_StartDir_Y", &fmcshower_StartDir_Y);
    fevent_chain->SetBranchAddress("mcshower_StartDir_Z", &fmcshower_StartDir_Z);
    if(fmc && frmcm_bool) fevent_chain->SetBranchAddress("mcshower_contributed_charge", &fmcshower_contributed_charge);

    if(fheavy) {

      fevent_chain->SetBranchAddress("fweight_genie_ncelaxial_p1sigma", &fweight_genie_ncelaxial_p1sigma);
      fevent_chain->SetBranchAddress("fweight_genie_ncelaxial_m1sigma", &fweight_genie_ncelaxial_m1sigma);
      fweight_branch_map.emplace("genie_NCELaxial_Genie", std::vector<double *>{&fweight_genie_ncelaxial_p1sigma, &fweight_genie_ncelaxial_m1sigma});

      fevent_chain->SetBranchAddress("fweight_genie_nceleta_p1sigma", &fweight_genie_nceleta_p1sigma);
      fevent_chain->SetBranchAddress("fweight_genie_nceleta_m1sigma", &fweight_genie_nceleta_m1sigma);
      fweight_branch_map.emplace("genie_NCELeta_Genie", std::vector<double *>{&fweight_genie_nceleta_p1sigma, &fweight_genie_nceleta_m1sigma});

      fevent_chain->SetBranchAddress("fweight_genie_qema_p1sigma", &fweight_genie_qema_p1sigma);
      fevent_chain->SetBranchAddress("fweight_genie_qema_m1sigma", &fweight_genie_qema_m1sigma);
      fweight_branch_map.emplace("genie_QEMA_Genie", std::vector<double *>{&fweight_genie_qema_p1sigma, &fweight_genie_qema_m1sigma});

      fevent_chain->SetBranchAddress("fweight_genie_qevec_p1sigma", &fweight_genie_qevec_p1sigma);
      fevent_chain->SetBranchAddress("fweight_genie_qevec_m1sigma", &fweight_genie_qevec_m1sigma);
      fweight_branch_map.emplace("genie_QEVec_Genie", std::vector<double *>{&fweight_genie_qevec_p1sigma, &fweight_genie_qevec_m1sigma});

      fevent_chain->SetBranchAddress("fweight_genie_ccresaxial_p1sigma", &fweight_genie_ccresaxial_p1sigma);
      fevent_chain->SetBranchAddress("fweight_genie_ccresaxial_m1sigma", &fweight_genie_ccresaxial_m1sigma);
      fweight_branch_map.emplace("genie_CCResAxial_Genie", std::vector<double *>{&fweight_genie_ccresaxial_p1sigma, &fweight_genie_ccresaxial_m1sigma});

      fevent_chain->SetBranchAddress("fweight_genie_ccresvector_p1sigma", &fweight_genie_ccresvector_p1sigma);
      fevent_chain->SetBranchAddress("fweight_genie_ccresvector_m1sigma", &fweight_genie_ccresvector_m1sigma);
      fweight_branch_map.emplace("genie_CCResVector_Genie", std::vector<double *>{&fweight_genie_ccresvector_p1sigma, &fweight_genie_ccresvector_m1sigma});

      fevent_chain->SetBranchAddress("fweight_genie_resganged_p1sigma", &fweight_genie_resganged_p1sigma);
      fevent_chain->SetBranchAddress("fweight_genie_resganged_m1sigma", &fweight_genie_resganged_m1sigma);
      fweight_branch_map.emplace("genie_ResGanged_Genie", std::vector<double *>{&fweight_genie_resganged_p1sigma, &fweight_genie_resganged_m1sigma});

      fevent_chain->SetBranchAddress("fweight_genie_ncresaxial_p1sigma", &fweight_genie_ncresaxial_p1sigma);
      fevent_chain->SetBranchAddress("fweight_genie_ncresaxial_m1sigma", &fweight_genie_ncresaxial_m1sigma);
      fweight_branch_map.emplace("genie_NCResAxial_Genie", std::vector<double *>{&fweight_genie_ncresaxial_p1sigma, &fweight_genie_ncresaxial_m1sigma});

      fevent_chain->SetBranchAddress("fweight_genie_ncresvector_p1sigma", &fweight_genie_ncresvector_p1sigma);
      fevent_chain->SetBranchAddress("fweight_genie_ncresvector_m1sigma", &fweight_genie_ncresvector_m1sigma);
      fweight_branch_map.emplace("genie_NCResVector_Genie", std::vector<double *>{&fweight_genie_ncresvector_p1sigma, &fweight_genie_ncresvector_m1sigma});

      fevent_chain->SetBranchAddress("fweight_genie_cohma_p1sigma", &fweight_genie_cohma_p1sigma);
      fevent_chain->SetBranchAddress("fweight_genie_cohma_m1sigma", &fweight_genie_cohma_m1sigma);
      fweight_branch_map.emplace("genie_CohMA_Genie", std::vector<double *>{&fweight_genie_cohma_p1sigma, &fweight_genie_cohma_m1sigma});

      fevent_chain->SetBranchAddress("fweight_genie_cohr0_p1sigma", &fweight_genie_cohr0_p1sigma);
      fevent_chain->SetBranchAddress("fweight_genie_cohr0_m1sigma", &fweight_genie_cohr0_m1sigma);
      fweight_branch_map.emplace("genie_CohR0_Genie", std::vector<double *>{&fweight_genie_cohr0_p1sigma, &fweight_genie_cohr0_m1sigma});

      fevent_chain->SetBranchAddress("fweight_genie_nonresrvp1pi_p1sigma", &fweight_genie_nonresrvp1pi_p1sigma);
      fevent_chain->SetBranchAddress("fweight_genie_nonresrvp1pi_m1sigma", &fweight_genie_nonresrvp1pi_m1sigma);
      fweight_branch_map.emplace("genie_NonResRvp1pi_Genie", std::vector<double *>{&fweight_genie_nonresrvp1pi_p1sigma, &fweight_genie_nonresrvp1pi_m1sigma});

      fevent_chain->SetBranchAddress("fweight_genie_nonresrvbarp1pi_p1sigma", &fweight_genie_nonresrvbarp1pi_p1sigma);
      fevent_chain->SetBranchAddress("fweight_genie_nonresrvbarp1pi_m1sigma", &fweight_genie_nonresrvbarp1pi_m1sigma);
      fweight_branch_map.emplace("genie_NonResRvbarp1pi_Genie", std::vector<double *>{&fweight_genie_nonresrvbarp1pi_p1sigma, &fweight_genie_nonresrvbarp1pi_m1sigma});

      fevent_chain->SetBranchAddress("fweight_genie_nonresrvp2pi_p1sigma", &fweight_genie_nonresrvp2pi_p1sigma);
      fevent_chain->SetBranchAddress("fweight_genie_nonresrvp2pi_m1sigma", &fweight_genie_nonresrvp2pi_m1sigma);
      fweight_branch_map.emplace("genie_NonResRvp2pi_Genie", std::vector<double *>{&fweight_genie_nonresrvp2pi_p1sigma, &fweight_genie_nonresrvp2pi_m1sigma});

      fevent_chain->SetBranchAddress("fweight_genie_nonresrvbarp2pi_p1sigma", &fweight_genie_nonresrvbarp2pi_p1sigma);
      fevent_chain->SetBranchAddress("fweight_genie_nonresrvbarp2pi_m1sigma", &fweight_genie_nonresrvbarp2pi_m1sigma);
      fweight_branch_map.emplace("genie_NonResRvbarp2pi_Genie", std::vector<double *>{&fweight_genie_nonresrvbarp2pi_p1sigma, &fweight_genie_nonresrvbarp2pi_m1sigma});

      fevent_chain->SetBranchAddress("fweight_genie_resdecaygamma_p1sigma", &fweight_genie_resdecaygamma_p1sigma);
      fevent_chain->SetBranchAddress("fweight_genie_resdecaygamma_m1sigma", &fweight_genie_resdecaygamma_m1sigma);
      fweight_branch_map.emplace("genie_ResDecayGamma_Genie", std::vector<double *>{&fweight_genie_resdecaygamma_p1sigma, &fweight_genie_resdecaygamma_m1sigma});

      fevent_chain->SetBranchAddress("fweight_genie_resdecayeta_p1sigma", &fweight_genie_resdecayeta_p1sigma);
      fevent_chain->SetBranchAddress("fweight_genie_resdecayeta_m1sigma", &fweight_genie_resdecayeta_m1sigma);
      fweight_branch_map.emplace("genie_ResDecayEta_Genie", std::vector<double *>{&fweight_genie_resdecayeta_p1sigma, &fweight_genie_resdecayeta_m1sigma});

      fevent_chain->SetBranchAddress("fweight_genie_resdecaytheta_p1sigma", &fweight_genie_resdecaytheta_p1sigma);
      fevent_chain->SetBranchAddress("fweight_genie_resdecaytheta_m1sigma", &fweight_genie_resdecaytheta_m1sigma);
      fweight_branch_map.emplace("genie_ResDecayTheta_Genie", std::vector<double *>{&fweight_genie_resdecaytheta_p1sigma, &fweight_genie_resdecaytheta_m1sigma});

      fevent_chain->SetBranchAddress("fweight_genie_nc_p1sigma", &fweight_genie_nc_p1sigma);
      fevent_chain->SetBranchAddress("fweight_genie_nc_m1sigma", &fweight_genie_nc_m1sigma);
      fweight_branch_map.emplace("genie_NC_Genie", std::vector<double *>{&fweight_genie_nc_p1sigma, &fweight_genie_nc_m1sigma});

      fevent_chain->SetBranchAddress("fweight_genie_disath_p1sigma", &fweight_genie_disath_p1sigma);
      fevent_chain->SetBranchAddress("fweight_genie_disath_m1sigma", &fweight_genie_disath_m1sigma);
      fweight_branch_map.emplace("genie_DISAth_Genie", std::vector<double *>{&fweight_genie_disath_p1sigma, &fweight_genie_disath_m1sigma});

      fevent_chain->SetBranchAddress("fweight_genie_disbth_p1sigma", &fweight_genie_disbth_p1sigma);
      fevent_chain->SetBranchAddress("fweight_genie_disbth_m1sigma", &fweight_genie_disbth_m1sigma);
      fweight_branch_map.emplace("genie_DISBth_Genie", std::vector<double *>{&fweight_genie_disbth_p1sigma, &fweight_genie_disbth_m1sigma});

      fevent_chain->SetBranchAddress("fweight_genie_discv1u_p1sigma", &fweight_genie_discv1u_p1sigma);
      fevent_chain->SetBranchAddress("fweight_genie_discv1u_m1sigma", &fweight_genie_discv1u_m1sigma);
      fweight_branch_map.emplace("genie_DISCv1u_Genie", std::vector<double *>{&fweight_genie_discv1u_p1sigma, &fweight_genie_discv1u_m1sigma});

      fevent_chain->SetBranchAddress("fweight_genie_discv2u_p1sigma", &fweight_genie_discv2u_p1sigma);
      fevent_chain->SetBranchAddress("fweight_genie_discv2u_m1sigma", &fweight_genie_discv2u_m1sigma);
      fweight_branch_map.emplace("genie_DISCv2u_Genie", std::vector<double *>{&fweight_genie_discv2u_p1sigma, &fweight_genie_discv2u_m1sigma});

      fevent_chain->SetBranchAddress("fweight_genie_disnucl_p1sigma", &fweight_genie_disnucl_p1sigma);
      fevent_chain->SetBranchAddress("fweight_genie_disnucl_m1sigma", &fweight_genie_disnucl_m1sigma);
      fweight_branch_map.emplace("genie_DISnucl_Genie", std::vector<double *>{&fweight_genie_disnucl_p1sigma, &fweight_genie_disnucl_m1sigma});

      fevent_chain->SetBranchAddress("fweight_genie_agkyxf_p1sigma", &fweight_genie_agkyxf_p1sigma);
      fevent_chain->SetBranchAddress("fweight_genie_agkyxf_m1sigma", &fweight_genie_agkyxf_m1sigma);
      fweight_branch_map.emplace("genie_AGKYxF_Genie", std::vector<double *>{&fweight_genie_agkyxf_p1sigma, &fweight_genie_agkyxf_m1sigma});

      fevent_chain->SetBranchAddress("fweight_genie_agkypt_p1sigma", &fweight_genie_agkypt_p1sigma);
      fevent_chain->SetBranchAddress("fweight_genie_agkypt_m1sigma", &fweight_genie_agkypt_m1sigma);
      fweight_branch_map.emplace("genie_AGKYpT_Genie", std::vector<double *>{&fweight_genie_agkypt_p1sigma, &fweight_genie_agkypt_m1sigma});

      fevent_chain->SetBranchAddress("fweight_genie_formzone_p1sigma", &fweight_genie_formzone_p1sigma);
      fevent_chain->SetBranchAddress("fweight_genie_formzone_m1sigma", &fweight_genie_formzone_m1sigma);
      fweight_branch_map.emplace("genie_FormZone_Genie", std::vector<double *>{&fweight_genie_formzone_p1sigma, &fweight_genie_formzone_m1sigma});

      fevent_chain->SetBranchAddress("fweight_genie_fermigasmodelkf_p1sigma", &fweight_genie_fermigasmodelkf_p1sigma);
      fevent_chain->SetBranchAddress("fweight_genie_fermigasmodelkf_m1sigma", &fweight_genie_fermigasmodelkf_m1sigma);
      fweight_branch_map.emplace("genie_FermiGasModelKf_Genie", std::vector<double *>{&fweight_genie_fermigasmodelkf_p1sigma, &fweight_genie_fermigasmodelkf_m1sigma});

      fevent_chain->SetBranchAddress("fweight_genie_fermigasmodelsf_p1sigma", &fweight_genie_fermigasmodelsf_p1sigma);
      fevent_chain->SetBranchAddress("fweight_genie_fermigasmodelsf_m1sigma", &fweight_genie_fermigasmodelsf_m1sigma);
      fweight_branch_map.emplace("genie_FermiGasModelSf_Genie", std::vector<double *>{&fweight_genie_fermigasmodelsf_p1sigma, &fweight_genie_fermigasmodelsf_m1sigma});

      fevent_chain->SetBranchAddress("fweight_genie_intranukenmfp_p1sigma", &fweight_genie_intranukenmfp_p1sigma);
      fevent_chain->SetBranchAddress("fweight_genie_intranukenmfp_m1sigma", &fweight_genie_intranukenmfp_m1sigma);
      fweight_branch_map.emplace("genie_IntraNukeNmfp_Genie", std::vector<double *>{&fweight_genie_intranukenmfp_p1sigma, &fweight_genie_intranukenmfp_m1sigma});

      fevent_chain->SetBranchAddress("fweight_genie_intranukencex_p1sigma", &fweight_genie_intranukencex_p1sigma);
      fevent_chain->SetBranchAddress("fweight_genie_intranukencex_m1sigma", &fweight_genie_intranukencex_m1sigma);
      fweight_branch_map.emplace("genie_IntraNukeNcex_Genie", std::vector<double *>{&fweight_genie_intranukencex_p1sigma, &fweight_genie_intranukencex_m1sigma});

      fevent_chain->SetBranchAddress("fweight_genie_intranukenel_p1sigma", &fweight_genie_intranukenel_p1sigma);
      fevent_chain->SetBranchAddress("fweight_genie_intranukenel_m1sigma", &fweight_genie_intranukenel_m1sigma);
      fweight_branch_map.emplace("genie_IntraNukeNel_Genie", std::vector<double *>{&fweight_genie_intranukenel_p1sigma, &fweight_genie_intranukenel_m1sigma});

      fevent_chain->SetBranchAddress("fweight_genie_intranukeninel_p1sigma", &fweight_genie_intranukeninel_p1sigma);
      fevent_chain->SetBranchAddress("fweight_genie_intranukeninel_m1sigma", &fweight_genie_intranukeninel_m1sigma);
      fweight_branch_map.emplace("genie_IntraNukeNinel_Genie", std::vector<double *>{&fweight_genie_intranukeninel_p1sigma, &fweight_genie_intranukeninel_m1sigma});

      fevent_chain->SetBranchAddress("fweight_genie_intranukenabs_p1sigma", &fweight_genie_intranukenabs_p1sigma);
      fevent_chain->SetBranchAddress("fweight_genie_intranukenabs_m1sigma", &fweight_genie_intranukenabs_m1sigma);
      fweight_branch_map.emplace("genie_IntraNukeNabs_Genie", std::vector<double *>{&fweight_genie_intranukenabs_p1sigma, &fweight_genie_intranukenabs_m1sigma});

      fevent_chain->SetBranchAddress("fweight_genie_intranukenpi_p1sigma", &fweight_genie_intranukenpi_p1sigma);
      fevent_chain->SetBranchAddress("fweight_genie_intranukenpi_m1sigma", &fweight_genie_intranukenpi_m1sigma);
      fweight_branch_map.emplace("genie_IntraNukeNpi_Genie", std::vector<double *>{&fweight_genie_intranukenpi_p1sigma, &fweight_genie_intranukenpi_m1sigma});

      fevent_chain->SetBranchAddress("fweight_genie_intranukepimfp_p1sigma", &fweight_genie_intranukepimfp_p1sigma);
      fevent_chain->SetBranchAddress("fweight_genie_intranukepimfp_m1sigma", &fweight_genie_intranukepimfp_m1sigma);
      fweight_branch_map.emplace("genie_IntraNukePImfp_Genie", std::vector<double *>{&fweight_genie_intranukepimfp_p1sigma, &fweight_genie_intranukepimfp_m1sigma});

      fevent_chain->SetBranchAddress("fweight_genie_intranukepicex_p1sigma", &fweight_genie_intranukepicex_p1sigma);
      fevent_chain->SetBranchAddress("fweight_genie_intranukepicex_m1sigma", &fweight_genie_intranukepicex_m1sigma);
      fweight_branch_map.emplace("genie_IntraNukePIcex_Genie", std::vector<double *>{&fweight_genie_intranukepicex_p1sigma, &fweight_genie_intranukepicex_m1sigma});

      fevent_chain->SetBranchAddress("fweight_genie_intranukepiel_p1sigma", &fweight_genie_intranukepiel_p1sigma);
      fevent_chain->SetBranchAddress("fweight_genie_intranukepiel_m1sigma", &fweight_genie_intranukepiel_m1sigma);
      fweight_branch_map.emplace("genie_IntraNukePIel_Genie", std::vector<double *>{&fweight_genie_intranukepiel_p1sigma, &fweight_genie_intranukepiel_m1sigma});

      fevent_chain->SetBranchAddress("fweight_genie_intranukepiinel_p1sigma", &fweight_genie_intranukepiinel_p1sigma);
      fevent_chain->SetBranchAddress("fweight_genie_intranukepiinel_m1sigma", &fweight_genie_intranukepiinel_m1sigma);
      fweight_branch_map.emplace("genie_IntraNukePIinel_Genie", std::vector<double *>{&fweight_genie_intranukepiinel_p1sigma, &fweight_genie_intranukepiinel_m1sigma});

      fevent_chain->SetBranchAddress("fweight_genie_intranukepiabs_p1sigma", &fweight_genie_intranukepiabs_p1sigma);
      fevent_chain->SetBranchAddress("fweight_genie_intranukepiabs_m1sigma", &fweight_genie_intranukepiabs_m1sigma);
      fweight_branch_map.emplace("genie_IntraNukePIabs_Genie", std::vector<double *>{&fweight_genie_intranukepiabs_p1sigma, &fweight_genie_intranukepiabs_m1sigma});

      fevent_chain->SetBranchAddress("fweight_genie_intranukepipi_p1sigma", &fweight_genie_intranukepipi_p1sigma);
      fevent_chain->SetBranchAddress("fweight_genie_intranukepipi_m1sigma", &fweight_genie_intranukepipi_m1sigma);
      fweight_branch_map.emplace("genie_IntraNukePIpi_Genie", std::vector<double *>{&fweight_genie_intranukepipi_p1sigma, &fweight_genie_intranukepipi_m1sigma});

    }

  }    
    
}


void Storage::GetProducerMap(std::vector<std::string> const & producers,
			     std::vector<int> const & producer_indices,
			     std::unordered_map<std::string, std::pair<int, int>> & producer_map) const {

  producer_map.clear();
  int starting_index = 0;
  for(size_t i = 0; i < producers.size(); ++i) {
    std::string const & producer_name = producers.at(i);
    int const ending_index = producer_indices.at(i);
    producer_map[producer_name] = std::make_pair(starting_index, ending_index);
    starting_index = ending_index;
  }

}


void Storage::GetEvent(int const i) {

  fcurrent_entry = i;

  fevent_chain->GetEntry(i);

  GetProducerMap(*fopflash_producers,
		 *fopflash_producer_indices,
		 fopflash_producer_map);

  GetProducerMap(*fhit_producers,
		 *fhit_producer_indices,
		 fhit_producer_map);

  GetProducerMap(*ftrack_producers,
		 *ftrack_producer_indices,
		 ftrack_producer_map);

  GetProducerMap(*fshower_producers,
		 *fshower_producer_indices,
		 fshower_producer_map);

}


std::pair<int, int> const & Storage::GetIndices(std::string const & producer,
						std::unordered_map<std::string, std::pair<int, int>> const & producer_map) const {
  
  auto const it = producer_map.find(producer);

  if(it == producer_map.end()) {
    std::cout << "Could not find producer : " << producer << "\n";
    exit(1);
  }

  return it->second;

}


std::pair<int, int> const & Storage::GetOpFlashIndices(std::string const & producer) const {

  return GetIndices(producer, fopflash_producer_map);

}

std::pair<int, int> const & Storage::GetHitIndices(std::string const & producer) const {

  return GetIndices(producer, fhit_producer_map);

}

std::pair<int, int> const & Storage::GetTrackIndices(std::string const & producer) const {

  return GetIndices(producer, ftrack_producer_map);

}

std::pair<int, int> const & Storage::GetShowerIndices(std::string const & producer) const {

  return GetIndices(producer, fshower_producer_map);

}


void Storage::Write() {

  TDirectory * start_dir = gDirectory;
  ffilter_dir->cd();
  fevent_tree->Write();
  start_dir->cd();

}
