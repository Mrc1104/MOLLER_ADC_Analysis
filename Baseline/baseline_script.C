#include <fstream>
#include <memory>

// Example: root 'baseline_script.C({16,17,18,26,29,21,22,24},{{1,2},{3,4},{5,6},{7,8},{9,10},{11,12},{13,14},{15,16}})'

static constexpr unsigned TOTAL_CHANNELS = 16;
static constexpr unsigned CHAN_PER_RUN   =  2;
static const char*        TTREE_NAME     = "DataTree";

enum GAUSS_FIT_PARAMS
{
	// TF1 Fit paramaters
	// for the "gaus" fit
	AMPLITUDE = 0,
	MEAN,
	RMS
};

inline
void divide_canvas_algorithm(TCanvas &c, const int size)
{
	int row = 1, col = 1;
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

void ConfigureTGraph(TGraph &g, std::string title)
{
	g.SetTitle(title.c_str());
	g.SetMarkerStyle(8);
}

static const char* PATTERN="Rootfiles/Int_Run_%03d.root";
void baseline_script(std::vector<int> runList, std::vector<std::pair<int, int>> adc_chan = {{0,1}}, std::string outfile="Baseline")
{

	ROOT::EnableImplicitMT();

	if(runList.size() != adc_chan.size())
		throw std::runtime_error("Mismatch adc_channel entries and run entries!");

	ofstream fcsv(outfile+".csv");
	fcsv << "ADC_Chan,Mean,Std\n";

	// ROOT::RDataFrame df(TTREE_NAME, Form(PATTERN, 17));
	std::vector<ROOT::RDataFrame> dataframes;
	std::vector< ROOT::RDF::RResultPtr<TH1D>> histograms;
	// std::vector< ROOT::RDF::RResultHandle > histograms;
	for(auto const run : runList) {
		auto df = dataframes.emplace(std::end(dataframes), TTREE_NAME, Form(PATTERN, run));
		for(int i = 0; i < 2; i++) {
			histograms.emplace_back( df->Histo1D(Form("SampleStream.ch%d_data",i)) );
		}
	}

	{  // anonymous scope
		std::vector< ROOT::RDF::RResultHandle > handlers;
		for( auto h : histograms ) {
			handlers.push_back( h );
		}
		ROOT::RDF::RunGraphs(  handlers );
	}

	auto chistograms = std::make_unique<TCanvas>();
	divide_canvas_algorithm(*chistograms, TOTAL_CHANNELS);
	auto gMean = std::make_unique<TGraphErrors>(TOTAL_CHANNELS);
	auto gRMS  = std::make_unique<TGraphErrors>(TOTAL_CHANNELS);
	unsigned entry = 0, chan = 0, chan_index = 0;
	for( auto h : histograms ) {
		auto pad = entry + 1;
		chistograms->cd(pad);
		h->Draw();
		auto fitresult = h->Fit("gaus", "QS");

		auto channel = (chan) ? adc_chan[chan_index].second : adc_chan[chan_index].first;

		// Construct TGraph
		gMean->SetPoint(   entry  , channel, fitresult->Parameter(MEAN));
		gMean->SetPointError(entry,       0, fitresult->ParError(MEAN) );
		gRMS ->SetPoint(   entry  , channel, fitresult->Parameter(RMS) );
		gRMS ->SetPointError(entry,       0, fitresult->ParError(RMS)  );

		std::string hname = Form("ADC Chan %d; Chan %d [soft. chan]; Cts", channel, chan);
		h->SetTitle(hname.c_str());
		if( (++chan) == 2) {
			chan = 0;
			chan_index++;
		}

		// Write to CSV
		fcsv << channel  << "," << fitresult->Parameter(MEAN) << "," << fitresult->Parameter(RMS) << std::endl;
		entry++;
	}

	// Draw TGraphs
	auto cStats = std::make_unique<TCanvas>("cStats");
	cStats->Divide(1,2);
	ConfigureTGraph(*gMean, std::string("Mean Vs ADC Chan; ADC Chan; Mean [V]"));
    ConfigureTGraph(*gRMS,  std::string("RMS Vs ADC Chan; ADC Chan; RMS [V]"  ));

	cStats->cd(1);
	gMean ->Draw("AP");
	cStats->cd(2);
	gRMS  ->Draw("AP");

	cStats->Print("stats.ps");
	chistograms->Print("hists.ps");

	auto fsave = std::make_unique<TFile>("Baseline.root", "RECREATE");
	gMean->Write("Mean");
	gRMS->Write("RMS");
	cStats->Write("cStats");
	chistograms->Write("cHistograms");

	auto dir_hists = fsave->mkdir("Histograms");
	dir_hists->cd();
	for( auto h : histograms ) h->Write(h->GetTitle());





	return;
}
