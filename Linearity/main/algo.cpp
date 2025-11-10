#include "TCanvas.h"
#include "TH1F.h"
#include "TGraph.h"
#include "TBox.h"
#include "TFile.h"
#include "DataSmpl.h"
#include <ROOT/RNTupleReader.hxx>
#include <TStyle.h>
#include <TF1.h>
#include <TPaveStats.h>
#include <TPaveText.h>
#include <iostream>
#include <iomanip>
#include <memory>
#include <vector>
#include <algorithm>
#include <limits>
#include <fstream>
#include <utility>


int main()
{
	std::string file_name = "../Rootfiles/moller_stream_molleradcse05_110.root";
	std::unique_ptr<ROOT::RNTupleReader> Reader = ROOT::RNTupleReader::Open("DataTree", file_name);
	ROOT::RNTupleView<tDataSamples> data = Reader->GetView<tDataSamples>("SampleStream");
	const int CHAN = 0; // CH0, CH1


	// Fast forward to second cycle
	bool first_cycle_found          = false;
	bool end_of_first_cycle_found   = false;
	double end_of_first_cycle_tstmp = 0;
	for( auto entry : Reader->GetEntryRange() ) {
		if(end_of_first_cycle_found==true) break;
		const auto& gate1    = data(entry).gate1;
		const auto& tStmp    = data(entry).tStmp;
		for(size_t index = 0; index < gate1.size(); index++) {
			auto g = gate1[index];
			if(first_cycle_found == false) {
				if(g != 0) {
					first_cycle_found = true;
				}
			} else {
				if(g == 0) {
					end_of_first_cycle_found = true;
					end_of_first_cycle_tstmp = tStmp[index];
					break;
				}
			}
		}
	}

	double max = std::numeric_limits<double>::min(); unsigned MaxTimeStamp = 0;
	double min = std::numeric_limits<double>::max(); unsigned MinTimeStamp = 0;
	bool gate_found = false;
	bool range_found= false;
	for( auto entry : Reader->GetEntryRange() ) {
		const auto& gate1    = data(entry).gate1;
		const auto& ch_data  = (CHAN == 1) ? data(entry).ch1_data : data(entry).ch0_data;
		const auto& tStmp    = data(entry).tStmp;
		if(tStmp[tStmp.size()-1] < end_of_first_cycle_tstmp) continue;
		if( range_found == true ) break;
		for(size_t index = 0; index < ch_data.size(); index++) {
			if(tStmp[index] < end_of_first_cycle_tstmp) continue;
			if( gate1[index] != 0 ) {
				gate_found = true;
				if(ch_data[index] > max) {
					max = ch_data[index];
					MaxTimeStamp = tStmp[index];
				}
				if(ch_data[index] < min) {
					min = ch_data[index];
					MinTimeStamp = tStmp[index];
				}
			} else if(gate_found == true) {
				range_found = true;
				break;
			} else {
				continue;
			}
		}
	}
	std::cout << "Max Found to be " << max << " at tStmp: " << MaxTimeStamp << "\n";
	std::cout << "Min Found to be " << min << " at tStmp: " << MinTimeStamp << "\n";

	auto canvas = std::make_unique<TCanvas>();
	auto graph  = std::make_unique<TGraph>();
	for( auto entry : Reader->GetEntryRange() ) {
		const auto& ch_data  = (CHAN == 1) ? data(entry).ch1_data : data(entry).ch0_data;
		const auto& tStmp    = data(entry).tStmp;
		if(tStmp[tStmp.size()-1] < MinTimeStamp) continue;
		for(size_t index = 0; index < ch_data.size(); index++) {
			if(tStmp[index] >= 0.9*MinTimeStamp && tStmp[index] <= 1.1*MaxTimeStamp) {
				// std::cout <<tStmp[index] << "\t" <<  ch_data[index] << "\n";
				graph->AddPoint(tStmp[index], ch_data[index]);
			}
		}
	}
	TFile *f = new TFile("algo.root","RECREATE");
	graph->Draw("AP");
	canvas->Print("algo.ps");
	graph->Write("g");
	delete f;


	return 0;
}
