static const char* PATTERN = "../Rootfiles/moller_stream_molleradcse05_%3d.root"; // File path pattern

void divide_canvas_algorithm(TCanvas &c, const int size)
{
	[[maybe_unused]]
		int row = 1, col = 1, col_counter = 0;
	while(row*col < size) {
		col++;
		if(row < col)
		{
			row++;
			col--;
		}

	}
	c.Divide(row,col);
}


// This is the Vout where the gain is -3db
double calc_bandwidth_v_out(double Vin)
{
	return Vin * TMath::Power(10.0, (-3.0/20.0) );
}


void bandwidth_test(std::vector<int> runlist, std::string outfile = "Bandwidth")
{
	auto *c1 = new TCanvas("c1", "ADC Bandwidth (sine fit)", 700, 400);
	divide_canvas_algorithm(*c1, 2*runlist.size());

	vector<double> amp ;
	vector<double> ampError ;

	vector<double> freq = {1e3, 1e3, 1e4, 1e4, 1e5, 1e5, 1e6, 1e6, 6e5, 6e5, 8e5, 8e5, 400e5, 400e5};
	// vector<double> freq = {400e5, 400e5};
	for (int j=0; j < runlist.size(); j++){
		std::string file_name = Form(PATTERN, runlist[j]);
		TFile *f = new TFile(file_name.c_str(), "READ");
		TTree *t = static_cast<TTree*>(f->Get("DataTree"));

		for (int i=0; i<2; i++){

			const char* ybranch = Form("ch%d_data", i);
			const char* time = "tStmp";
			c1->cd(2*j+i+1);

			// double cut =  freq[j] < 1e4 ? cut = 5.0: 1.0;
			double cut = 5000.0 / freq[2*j];
			int n = t->Draw(Form("%s:%s", ybranch, time), Form("tStmp < %f", cut), "goff"); 

			TGraph* gr = new TGraph(n,t->GetV2(), t->GetV1());
			gr->Draw("ap");
			gr->SetTitle  (Form("Freq: %3.0f kHz; time_in_msec; chan%d_data",freq[2*j+i]/1e3, i));

			TF1* fit =new TF1("fit_sin", "[0]*TMath::Sin(TMath::TwoPi()*[1]*(x*1e-3) + [2]) + [3]");
			fit->SetParameter(0,2);
			fit->FixParameter(1,freq[2*j+i]);
			fit->SetNpx(1000);

			gr->Fit(fit);
			amp.push_back(TMath::Abs(fit->GetParameter(0)));
			ampError.push_back(fit->GetParError(0));
		}
	}
	auto c2 = new TCanvas("bwidth");
	TGraphErrors* g = new TGraphErrors(amp.size(),freq.data(), amp.data(), 0, ampError.data());
	g->SetMarkerStyle(8);
	g->SetTitle("ADC Bandwith (V_{in} = 4 Vpp); Frequency [Hz]; V_{Meas.} [V]");
	g->Draw("ap");

	g->Fit("pol2");
	gStyle->SetOptFit(1);

	double bandwidth_vout = calc_bandwidth_v_out(2); // Vpp = 4;
	TLine *lBandwidth = new TLine(g->GetPointX(0), bandwidth_vout, g->GetPointX(g->GetN()-1), bandwidth_vout);
	lBandwidth->SetLineWidth(2);
	lBandwidth->Draw("SAME");

	std::cout << "Bandwidth: " << bandwidth_vout << std::endl;


}
