/* The goal of this macro is: 
 *   Given a big covariance matrix, identify ONE block matrix within big matrix, and compare all sub-block matrices with it...
 *   Metric set as: [sqrt(A(i,j)) - sqrt(B(i,j))]/sqrt(B(i,j))*100, where B is the reference block matrix, and A is any block matrix 
 *
 *   The metric is selected to compare the uncertainty on the diagonals 
 *
 *   By Guanqun Ge, Jun 1st 2022
 */

void CovarMatrixComparison(){
    
    //-------- INPUT -------------
    std::string reference_bin_range = "0 0";
    std::string filename = "/uboone/app/users/gge/singlephoton/whipping_star/working_directory/SPmodule_test/FluxAssignmentTest/OrderedSample/NCDelta/1Bin.SBNcovar.root";
    std::string matrix_name = "frac_covariance";
    std::string output_pdf = "CovarMatrixComparison_NCCOH_vs_SourceNCDelta_1Bin.pdf";
    //--------- End of INPUT ---------


    
    TFile* f = new TFile(filename.c_str(), "read");
    TMatrixT<double>* tmatrix = (TMatrixT<double>*)f->Get(matrix_name.c_str());

    //Configure the input 
    std::istringstream iss(reference_bin_range);
    int ref_bin_start, ref_bin_end, ref_nbins; 
    iss >> ref_bin_start >> ref_bin_end;
    ref_nbins = ref_bin_end - ref_bin_start + 1;

    int nbin_total = tmatrix->GetNcols();
    int nblock = nbin_total/ref_nbins;
    if(nbin_total%ref_nbins){
	std::runtime_error("This matrix cannot be divided into integer block matrices..");
    }

    //calculate the divided matrix 
    TMatrixT<double> divided_matrix = *tmatrix; 
    for(int row = 0 ; row < nblock; ++row){
	for(int col = 0; col < nblock; ++col){
	   int base_row = ref_nbins*row, base_col = ref_nbins*col;

	   for(int i = 0; i < ref_nbins; ++i){
		for(int j = 0; j < ref_nbins; ++j){

		    //for diagonal element, compare the size of uncertainty 
		    if(i == j && base_row == base_col){
		    	if(abs((*tmatrix)(ref_bin_start+i,ref_bin_start+j)) != 0)
		    		divided_matrix(i+base_row,j+base_col) = (sqrt(divided_matrix(i+base_row,j+base_col)) - sqrt((*tmatrix)(ref_bin_start+i,ref_bin_start+j)))/sqrt((*tmatrix)(ref_bin_start+i,ref_bin_start+j))*100;
		        else if(abs(divided_matrix(i+base_row,j+base_col)) != 0 )
				divided_matrix(i+base_row,j+base_col) = 100;
		    }	
		    else{
		    //for non-diagonal element, compare the correlation coefficient
		    	double curr_coefficient = divided_matrix(i+base_row,j+base_col)/sqrt((*tmatrix)(i+base_row,i+base_row)*(*tmatrix)(j+base_col,j+base_col));
			double ref_coefficient = (*tmatrix)(ref_bin_start+i,ref_bin_start+j)/sqrt(((*tmatrix)(ref_bin_start+i,ref_bin_start+i))*((*tmatrix)(ref_bin_start+j,ref_bin_start+j)));
			divided_matrix(i+base_row,j+base_col) = (curr_coefficient- ref_coefficient)/ref_coefficient*100;
			std::cout << i << " " << j << " curr : " << curr_coefficient << " ref: " << ref_coefficient << std::endl;
		    }
		}
	   }
	}
    }

    //draw the matrix 
    TH2D h_tmatrix(*tmatrix);
    h_tmatrix.SetTitle("Fractional Covariance Matrix;Global Bin Number; Global Bin Number;");
    TH2D h_divided_matrix(divided_matrix);
    h_divided_matrix.SetTitle("Relative Difference in Uncertainty and Coefficient;Global Bin Number; Global Bin Number;");
    TLine* tline = new TLine(0,0,nbin_total, nbin_total);
    tline->SetLineStyle(2);
    tline->SetLineWidth(2);
    tline->SetLineColor(kRed);

    TCanvas* c= new TCanvas();
    gStyle->SetOptStat(0);
    c->Print((output_pdf+"[").c_str());
    gStyle->SetPaintTextFormat(".4f");
    c->cd();
    h_tmatrix.Draw("TEXT COLZ");
    tline->Draw();
    c->Print(output_pdf.c_str()); 
    c->Clear();
    gStyle->SetPaintTextFormat(".2f");
    h_divided_matrix.Draw("TEXT COLZ");
    tline->Draw();
    c->Print(output_pdf.c_str());
    c->Print((output_pdf+"]").c_str());
 
    return 0;
}
