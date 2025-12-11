


static const char* PATTERN = "../Rootfiles/moller_stream_molleradcse05_%3d.root"; // File path pattern
void plot()
{
	int run_w_splitter  = 120;
	int run_wo_splitter = 126;

	TFile *file_w_splitter   = new TFile(Form(PATTERN, run_w_splitter ), "READ");
	TFile *file_wo_splitter  = new TFile(Form(PATTERN, run_wo_splitter), "READ");

	TTree *tree_w_splitter   = file_w_splitter->Get<TTree>( "DataTree");
	TTree *tree_wo_splitter  = file_wo_splitter->Get<TTree>("DataTree");
	const char* cut = "tStmp > 450 && tStmp < 460";

	TCanvas *c = new TCanvas();
	c->Divide(1,2);
	c->cd(1);
	int n = tree_w_splitter->Draw("ch1_data:tStmp", cut, "goff"); 
	TGraph* g_w_splitter = new TGraph(n,tree_w_splitter->GetV2(), tree_w_splitter->GetV1());
	g_w_splitter->SetTitle("With Splitter; tStmp; ch1_data");
	g_w_splitter->Draw("ap");

	c->cd(2);
	n = 0;
	n = tree_wo_splitter->Draw("ch1_data:tStmp", cut, "goff"); 
	TGraph* g_wo_splitter = new TGraph(n,tree_wo_splitter->GetV2(), tree_wo_splitter->GetV1());
	g_wo_splitter->SetTitle("Without Splitter; tStmp; ch1_data");
	g_wo_splitter->Draw("ap");




}
