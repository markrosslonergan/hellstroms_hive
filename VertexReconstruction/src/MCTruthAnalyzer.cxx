

#include "MCTruthAnalyzer.h"


MCTruthAnalyzer::MCTruthAnalyzer() :
  ftree(new TTree("MCTruthAnalyzer_tree", "")) {

  ftree->Branch("delta_pdg", &delta_pdg, "delta_pdg/I");
  ftree->Branch("proton_pdg", &proton_pdg, "proton_pdg/I");
  ftree->Branch("photon_pdg", &photon_pdg, "photon_pdg/I");

  ftree->Branch("exiting_particles", &exiting_particles);
  ftree->Branch("number_of_exiting_particles", &number_of_exiting_particles, "number_of_exiting_particles/I");

  ftree->Branch("mp", &mp, "mp/D");
  ftree->Branch("Ep", &Ep, "Ep/D");
  ftree->Branch("Pp", &Pp, "Pp/D");
  ftree->Branch("Eg", &Eg, "Eg/D");
  
  ftree->Branch("Pxp", &Pxp, "Pxp/D");
  ftree->Branch("Pyp", &Pyp, "Pyp/D");
  ftree->Branch("Pzp", &Pzp, "Pzp/D");
  
  ftree->Branch("Pxg", &Pxg, "Pxg/D");
  ftree->Branch("Pyg", &Pyg, "Pyg/D");
  ftree->Branch("Pzg", &Pzg, "Pzg/D");
  
  ftree->Branch("costerm", &costerm, "costerm/D");

  ftree->Branch("inv_delta_mass", &inv_delta_mass, "inv_delta_mass/D");
  
}


bool MCTruthAnalyzer::Run() {

  int const delta_mct_index = fstorage->fdelta_mct_index;
  if(delta_mct_index == -1) return false;
  int const delta_index = fstorage->fdelta_index->at(delta_mct_index);

  int const photon_index = fstorage->fdelta_photon_index->at(delta_mct_index);
  int const proton_index = fstorage->fdelta_proton_index->at(delta_mct_index);
  if(proton_index == -1) return false;

  delta_pdg = fstorage->fgenie_particle_PdgCode->at(delta_mct_index).at(delta_index);
  proton_pdg = fstorage->fgenie_particle_PdgCode->at(delta_mct_index).at(proton_index);
  photon_pdg = fstorage->fgenie_particle_PdgCode->at(delta_mct_index).at(photon_index);

  std::vector<int> const & pdg_codes = fstorage->fgenie_particle_PdgCode->at(delta_mct_index);
  std::vector<int> const & status_codes = fstorage->fgenie_particle_StatusCode->at(delta_mct_index);
  number_of_exiting_particles = 0;
  for(size_t i = 0; i < status_codes.size(); ++i) {
    if(status_codes.at(i) != 1) continue;
    exiting_particles.push_back(pdg_codes.at(i));
    ++number_of_exiting_particles;
  }
  
  mp = 0.9382720813;
  Ep = fstorage->fgenie_particle_E->at(delta_mct_index).at(proton_index);
  Eg = fstorage->fgenie_particle_E->at(delta_mct_index).at(photon_index);
  
  Pxp = fstorage->fgenie_particle_Px->at(delta_mct_index).at(proton_index);
  Pyp = fstorage->fgenie_particle_Py->at(delta_mct_index).at(proton_index);
  Pzp = fstorage->fgenie_particle_Pz->at(delta_mct_index).at(proton_index);

  Pp = sqrt(Pxp*Pxp + Pyp*Pyp + Pzp*Pzp);
  
  Pxg = fstorage->fgenie_particle_Px->at(delta_mct_index).at(photon_index);
  Pyg = fstorage->fgenie_particle_Py->at(delta_mct_index).at(photon_index);
  Pzg = fstorage->fgenie_particle_Pz->at(delta_mct_index).at(photon_index);
  
  costerm = Pxp*Pxg + Pyp*Pyg + Pzp*Pzg;

  inv_delta_mass = mp*mp + 2*(Ep*Eg-costerm);

  ftree->Fill();

  exiting_particles.clear();

  return true;

}


void MCTruthAnalyzer::Finalize() {

  if(fofile) {

    if(ftree) ftree->Write();

  }

}
