


void draw_hist(TTree * tree,
	       std::string const & cname,
	       std::string const & draw,
	       std::string const & binning,
	       std::string const & we,
	       std::string const & title,
	       std::string const & xtitle,
	       std::string const & ytitle) {

  TCanvas * canvas = new TCanvas(cname.c_str());
  tree->Draw((draw+">>h"+binning).c_str(), we.c_str());
  TH1F * h = (TH1F*)gDirectory->Get("h");
  h->SetTitle(title.c_str());
  h->GetXaxis()->SetTitle(xtitle.c_str());
  h->GetXaxis()->CenterTitle();
  h->GetYaxis()->SetTitle(ytitle.c_str());
  h->GetYaxis()->CenterTitle();

  canvas->SaveAs(("pdfs/"+cname+".pdf").c_str());
  delete canvas;

}



void pandora_comparison() {

  TFile * vbfile = TFile::Open("new/runmv_sp.root");
  TFile * pfile = TFile::Open("pandora/runmv_sp.root");

  TTree * vbtree = (TTree*)vbfile->Get("LEEPhoton/vertex_tree");
  TTree * ptree = (TTree*)pfile->Get("LEEPhoton/vertex_tree");

  std::string binning = "(100, 0, 100)";
  std::string cuts = "passed_swtrigger == 1 && shower_dist_to_flashzcenter <= 38 && totalpe_ibg_sum > 139 && reco_nu_vtx_fid_contained == 1 && summed_associated_reco_shower_energy > 0.07 && longest_asso_track_displacement <= 32";
  
  draw_hist(vbtree,
	    "vbdist",
	    "reco_true_nuvert_dist",
	    binning,
	    cuts,
	    "Vertex Builder",
	    "Reco - True Nu Vtx Distance [cm]",
	    "Number of Vertices");
  
  draw_hist(vbtree,
	    "vbdist0track",
	    "reco_true_nuvert_dist",
	    binning,	  
	    cuts+" && reco_asso_tracks == 0",
	    "Vertex Builder (associated tracks == 0)",
	    "Reco - True Nu Vtx Distance [cm]",
	    "Number of Vertices");

  draw_hist(vbtree,
	    "vbdistg0track",
	    "reco_true_nuvert_dist",
	    binning,
	    cuts+" && reco_asso_tracks > 0",
	    "Vertex Builder (associated tracks > 0)",
	    "Reco - True Nu Vtx Distance [cm]",
	    "Number of Vertices");

  draw_hist(ptree,
	    "pdist",
	    "reco_true_nuvert_dist",
	    binning,	 
	    cuts,
	    "Pandora",
	    "Reco - True Nu Vtx Distance [cm]",
	    "Number of Vertices");
  
  draw_hist(ptree,
	    "pdist0track",
	    "reco_true_nuvert_dist",
	    binning,	  
	    cuts+" && reco_asso_tracks == 0",
	    "Pandora (associated tracks == 0)",
	    "Reco - True Nu Vtx Distance [cm]",
	    "Number of Vertices");

  draw_hist(ptree,
	    "pdistg0track",
	    "reco_true_nuvert_dist",
	    binning,
	    cuts+" && reco_asso_tracks > 0",
	    "Pandora (associated tracks > 0)",
	    "Reco - True Nu Vtx Distance [cm]",
	    "Number of Vertices");

}
