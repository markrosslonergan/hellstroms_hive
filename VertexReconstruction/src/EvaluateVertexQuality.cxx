

#include "EvaluateVertexQuality.h"


EvaluateVertexQuality::EvaluateVertexQuality(char const * vq_name,
					     std::vector<char const *> const & files) {
  
  if(files.empty()) {
    std::cout << "Empty file list given to Storage\n";
    exit(1);
  }
      
  if(!CheckFile(vq_name, files)) exit(1);
  Initialize();

  fvq_chain = new TChain(vq_name);

  AddFiles(files);
  SetupChain();

}


bool EvaluateVertexQuality::CheckFile(char const * vq_name,
			std::vector<char const *> const & files) const {

  TFile * file = TFile::Open(files.front());

  if(!file) {
    std::cout << "File: " << files.front() << " not found\n";
    exit(1);
  }

  TTree * vq_tree = dynamic_cast<TTree*>(file->Get(vq_name));

  if(!vq_tree) {
    std::cout << __PRETTY_FUNCTION__ << "\nCould not find a tree\n";
    return false;
  }

  file->Close();

  return true;

}


void EvaluateVertexQuality::Initialize() {

  ftrack_matching_ratio_v = nullptr;
  ftrack_true_pdg_v = nullptr;
  ftrack_true_origin_v = nullptr;

  fshower_matching_ratio_v = nullptr;
  fshower_true_pdg_v = nullptr;
  fshower_true_origin_v = nullptr;

}


void EvaluateVertexQuality::AddFiles(std::vector<char const *> const & files) {

  for(char const * file : files) {
    fvq_chain->Add(file);
  }

}


void EvaluateVertexQuality::SetupChain() {

  fvq_chain->SetBranchAddress("start_prox", &fstart_prox);
  fvq_chain->SetBranchAddress("shower_prox", &fshower_prox);
  fvq_chain->SetBranchAddress("max_bp_dist", &fmax_bp_dist);
  fvq_chain->SetBranchAddress("cpoa_vert_prox", &fcpoa_vert_prox);
  fvq_chain->SetBranchAddress("cpoa_trackend_prox", &fcpoa_trackend_prox);

  fvq_chain->SetBranchAddress("reco_vertex_present", &freco_vertex_present);
  fvq_chain->SetBranchAddress("tpc_volume_contained", &ftpc_volume_contained);

  fvq_chain->SetBranchAddress("dist", &fdist);
  fvq_chain->SetBranchAddress("distx", &fdistx);
  fvq_chain->SetBranchAddress("disty", &fdisty);
  fvq_chain->SetBranchAddress("distz", &fdistz);

  fvq_chain->SetBranchAddress("true_track_total", &ftrue_track_total);
  fvq_chain->SetBranchAddress("true_shower_total", &ftrue_shower_total);
  fvq_chain->SetBranchAddress("reco_track_total", &freco_track_total);
  fvq_chain->SetBranchAddress("reco_shower_total", &freco_shower_total);
  fvq_chain->SetBranchAddress("correct_track_total", &fcorrect_track_total);
  fvq_chain->SetBranchAddress("correct_shower_total", &fcorrect_shower_total);

  fvq_chain->SetBranchAddress("track_matching_ratio_v", &ftrack_matching_ratio_v);
  fvq_chain->SetBranchAddress("track_true_pdg_v", &ftrack_true_pdg_v);
  fvq_chain->SetBranchAddress("track_true_origin_v", &ftrack_true_origin_v);

  fvq_chain->SetBranchAddress("shower_matching_ratio_v", &fshower_matching_ratio_v);
  fvq_chain->SetBranchAddress("shower_true_pdg_v", &fshower_true_pdg_v);
  fvq_chain->SetBranchAddress("shower_true_origin_v", &fshower_true_origin_v);

}


void EvaluateVertexQuality::Run() {

  for(int i = 0; i < fvq_chain->GetEntries(); ++i) {

    fvq_chain->GetEntry(i);

    std::cout << fstart_prox << " " << fshower_prox << " " << fmax_bp_dist << " " << fcpoa_vert_prox << " " << fcpoa_trackend_prox << "\n";

  }

}
