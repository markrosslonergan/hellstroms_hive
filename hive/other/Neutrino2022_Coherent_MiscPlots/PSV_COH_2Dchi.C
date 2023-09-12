/*  Evaluate sensitivity at different PSV cut positions
 */
std::vector<TGraph> FindContour(TH2D hin, int n, std::string intag){

   //std::vector<double> full_contour{2.30, 6.18,11.83, 19.35, 28.23}; // chi2 value for 2 dof with 1, 2, 3, 4, 5 sigma confidence level
   std::vector<double> full_contour{1.0, 4.0, 9.0, 16.01}; // chi2 value for 2 dof with 1, 2, 3, 4, 5 sigma confidence level
   std::vector<std::string> full_ctour_string{"1sigma","2sigma", "3sigma", "4sigma", "5sigma"};
   std::vector<std::string> full_name_string{"1#sigma", "2#sigma", "3#sigma", "4#sigma", "5#sigma"};

   std::vector<int> full_color{kGreen-7, kCyan-7, kMagenta-7, kRed-7, kBlue-7};//color of the contour: kBlue, kMagenta, kRed
   std::vector<int> full_style{kSolid, kDashed, kDotted, kDashDotted, 5};
   std::vector<int> full_marker_style{24,25,26,27,28};

   std::vector<double> contour(full_contour.begin(), full_contour.begin()+n); // chi2 value for 2 dof with 1 sigma, 90%, 2 sigma and 3 sigma confidence level
   std::vector<std::string> CL_string(full_ctour_string.begin(), full_ctour_string.begin()+n);
   std::vector<std::string> Name_string(full_name_string.begin(), full_name_string.begin()+n);
   std::vector<int> color_vec(full_color.begin(), full_color.begin()+n);
   std::vector<int> style_vec(full_style.begin(), full_style.begin()+n);
   std::vector<int> mstyle_vec(full_marker_style.begin(), full_marker_style.begin()+n);

   std::vector<TGraph> out_graph(n);
   //draw contour
   TCanvas c((intag+"c").c_str(), (intag+"c").c_str());
   c.cd();
   hin.SetContour((int)contour.size(), &contour[0]);
   hin.Draw("CONT Z LIST");//"LIST" generates a list of TGraph for each contour
   //grab contour object
   c.Update();
   gROOT->GetListOfSpecials()->Print();
   TObjArray *conts = (TObjArray*)gROOT->GetListOfSpecials()->FindObject("contours");

   for(int i=0; i<conts->GetSize(); i++){
        //create a new TList pointer inside a loop, it will get deleted everytime during the loop --> no memory leak here.
        TList* con_list = (TList*)conts->At(i);   //a list of TGraph for i'th contour.
        out_graph[i].SetName(Form("graph_%s", CL_string[i].c_str()));
        out_graph[i].SetTitle(Form("%s", Name_string[i].c_str()));
        out_graph[i].SetLineStyle(style_vec[i]);
        out_graph[i].SetLineWidth(2);
        out_graph[i].SetMarkerColor(color_vec[i]);
        out_graph[i].SetMarkerStyle(mstyle_vec[i]);
        out_graph[i].SetMarkerSize(0.1);
        TGraph * c_graph = (TGraph*)con_list->First();  //grab the TGraph
        for(int j=0; j< con_list->GetSize() ; j++){
            TGraph * temp_graph= (TGraph*)c_graph->Clone();
            double x,y;
            for(int k =0; k< temp_graph->GetN(); k++){
                temp_graph->GetPoint(k, x, y);
                out_graph[i].SetPoint(out_graph[i].GetN(), x, y);
            }
            c_graph=(TGraph*)con_list->After(c_graph);
        }

        std::cout << "Contour " << CL_string[i] << ": " << contour[i] << std::endl;
    }
    return out_graph;
}
     
void DrawContour(TH2D* h, std::vector<TGraph>& v_graph, std::string intag){
   TCanvas *c_canvas = new TCanvas((intag+"_pretty_contour").c_str(), (intag+"_pretty_contour").c_str());
   gStyle->SetOptStat(0);
   c_canvas->cd();
   TLegend *leg = new TLegend(0.68, 0.7, 0.9,0.9);
   leg->SetFillStyle(0);
   leg->SetBorderSize(0);

   h->Draw("colz");
   for(auto& g:v_graph){
        g.SetLineColor(kWhite);
        g.SetMarkerColor(kWhite);
        g.Draw("same l");
        leg->AddEntry(&g, g.GetTitle(), "LP");
  }
  leg->SetTextSize(0.043);
  leg->Draw();
  c_canvas->SaveAs((intag+"_pretty_chi_contours.pdf").c_str(),"pdf");

  return;
}

void PSV_COH_2Dchi(){

    int pres = 2;
    double PSV_max = 1, PSV_min = 0, PSV_step = 0.01;
    double COH_max = 50, COH_min = 0, COH_step = 0.2;
    std::string inputdir = "/uboone/app/users/gge/singlephoton/whipping_star/working_directory/CoherentSinglePhoton/PSVcut";
    std::string tag_prefix = "PSVcut_";
    std::string file_suffix = "_fit_output.root";
    std::string output_name = "PSVcut_COHscale_";

    int nstep_PSV = (PSV_max - PSV_min) / PSV_step;
    int nstep_COH = (COH_max - COH_min )/COH_step;
    TH2D* hchi = new TH2D("hchi", "hchi", nstep_COH, COH_min, COH_max, nstep_PSV, PSV_min, PSV_max);
    hchi->SetTitle("#chi^2 surface; Coherent 1#gamma Scaling; Cut on Max PSV score of Clusters on P0 and P2");
    

    for(int i = 0; i < nstep_PSV; ++i){
	double PSV_cut = PSV_min + i * PSV_step;

	std::ostringstream oss;
 	oss.precision(pres);
	oss << std::fixed << PSV_cut; 
 
	std::string filename = inputdir + "/" + tag_prefix + oss.str() + "/" + tag_prefix + oss.str() + file_suffix;
	std::cout << "PSV cut: " << PSV_cut << ", on file: " << filename << std::endl;

	TFile* f = new TFile(filename.c_str(), "read");
	std::vector<double>* pvec = nullptr; 
	f->GetObject("vector_chi", pvec);
	std::cout << pvec->size() << std::endl;
	for(int j = 0; j != pvec->size(); ++j){
	   //std::cout << i << " " << j << " " << pvec->at(j) << std::endl;
	   hchi->SetBinContent(j+1, i+1, pvec->at(j));
	}

	f->Close();
    }

    //plotting
    auto vec_graph = FindContour(*hchi, 3, output_name);
    DrawContour(hchi, vec_graph, output_name);
    return; 
}
