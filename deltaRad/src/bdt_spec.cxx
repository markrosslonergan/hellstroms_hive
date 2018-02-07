#include "bdt_spec.h"


THStack* bdt_stack::getStack(bdt_variable var, int level, double cut1, double cut2){

	THStack *stacked = new THStack((this->name+"_stack").c_str(), (this->name+"_stack").c_str());

	for(int t=0; t<stack.size(); t++){
		TH1* hist = (TH1*)stack.at(t)->getTH1(var, stack.at(t)->getStageCuts(level,cut1, cut2), "stack_"+stack.at(t)->tag+"_"+var.name, plot_pot);

		hist->SetTitle((this->name+"_"+var.name).c_str());
		hist->SetLineColor(kBlack);
		hist->SetStats(0);
		hist->SetLineWidth(1);
		//hist->SetMarkerStyle(20);
		hist->SetFillColor(stack.at(t)->col);

		hist->GetXaxis()->SetTitle(var.unit.c_str());
		hist->GetYaxis()->SetTitle("Verticies");



		stacked->Add(hist);
	}

	return stacked;	
		
}

