#include <TFile.h>
#include <TCanvas.h>
#include <TString.h>
#include <TGraph.h>
#include <TLegend.h>
#include <TMultiGraph.h>
#include <ROOT/RNTupleReader.hxx>

#include <memory>
#include <array>

#include "DataSmpl.h"

constexpr size_t NCHANS = 2;
const char *PATTERN = "./Input/molleradc_%d.root";
int main()
{
	
	auto Reader = ROOT::RNTupleReader::Open("DataTree", Form(PATTERN, 182));


	auto canvas = std::make_unique<TCanvas>("Channel Data");
	std::array<std::unique_ptr<TGraph>, NCHANS> graphs = {
		std::make_unique<TGraph>(),
		std::make_unique<TGraph>()
	};
	std::array<std::unique_ptr<TGraph>, NCHANS> graphs_gated = {
		std::make_unique<TGraph>(),
		std::make_unique<TGraph>()
	};
	canvas->Divide(1,NCHANS);

	auto stream_data = Reader->GetView<tDataSamples>(SAMPLE_BRANCH_NAME);
	for( auto entry : Reader->GetEntryRange() ) {
		const auto& chan0 = stream_data(entry).ch0_data;
		const auto& chan1 = stream_data(entry).ch1_data;
		const auto& tStmp = stream_data(entry).tStmp;
		const auto& gate1 = stream_data(entry).gate1;
	
		for(size_t index = 0; index < tStmp.size(); index++) {
			const auto& time = tStmp[index];
			const auto& ch0  = chan0[index];
			const auto& ch1  = chan1[index];
			const auto& gate = gate1[index];

			graphs[0]->AddPoint(time, ch0);
			graphs[1]->AddPoint(time, ch1);
			if(gate) {
				graphs_gated[0]->AddPoint(time, ch0);
				graphs_gated[1]->AddPoint(time, ch1);
			}
		}
	}

	std::array<std::unique_ptr<TMultiGraph>, NCHANS> multi_graphs = {
		std::make_unique<TMultiGraph>(),
		std::make_unique<TMultiGraph>()
	};
	std::array<std::unique_ptr<TLegend>, NCHANS> legends = {
		std::make_unique<TLegend>(0.6, 0.2, 0.8, 0.4),
		std::make_unique<TLegend>(0.6, 0.2, 0.8, 0.4)
	};

	auto fsave = std::make_unique<TFile>("./Output/Channel_Data.root", "RECREATE");
	
	for( size_t ch = 0 ; ch < NCHANS; ch++ ) {
		int pad = ch + 1;
		canvas->cd(pad);

		graphs[ch]->SetLineColor(kBlue);
		graphs[ch]->SetMarkerColor(kBlue);
		graphs_gated[ch]->SetLineColor(kRed);
		graphs_gated[ch]->SetMarkerColor(kRed);

		legends[ch]->AddEntry(graphs[ch].get()      , Form("Chan %zu", ch), "LP");
		legends[ch]->AddEntry(graphs_gated[ch].get(),        "gate1"     ,  "LP");

		multi_graphs[ch]->Add(graphs[ch].release()      , "AP");
		multi_graphs[ch]->Add(graphs_gated[ch].release(), "AP");
		multi_graphs[ch]->Draw("A");
		legends[ch]->Draw("SAME");
	
		multi_graphs[ch]->Write("Channel_Data");
	}
	canvas->Write("cChannel_Data");

	fsave->Close(0);
	return 0;
}
