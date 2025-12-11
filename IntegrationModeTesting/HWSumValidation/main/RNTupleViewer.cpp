#include <vector>
#include <iostream>
#include <memory>
#include <fstream>
#include <array>
#include <limits>

#include "ROOT/RNTupleReader.hxx"
#include "ROOT/RNTupleView.hxx"
#include "DataSmpl.h"
#include "TString.h"
#include "TFile.h"
#include "TGraph.h"
#include "TH1F.h"
#include "TMath.h"
#include "TColor.h"
#include "Rtypes.h"
#include "TMultiGraph.h"
#include "TF1.h"
#include "TCanvas.h"
#include "TStyle.h"


const char* PATTERN = "./Input/molleradc_%d.root";
int main()
{
	auto Reader = ROOT::RNTupleReader::Open(KEY_NAME,Form(PATTERN, 176));
	Reader->EnableMetrics();


	auto ch_NSamples =  Reader->GetView<std::array<double,16>>(Form("%s.%s",INTEGRATION_BRANCH_NAME, "ch_NSamples"));
	auto ch_WindowNSamples =  Reader->GetView<std::array<double,16>>(Form("%s.%s",INTEGRATION_BRANCH_NAME, "ch_WindowNSamples"));
	
	for( auto entry : Reader->GetEntryRange() ) {
		const auto& window_samples = ch_WindowNSamples(entry);
		const auto& ch_n_samples   = ch_NSamples(entry);
		for(size_t i = 0; i < ch_n_samples[index].size(); i++) {
			std::cout << "\nCh: " << i;
			std::cout << "\n\t\tWindowNSamples = " << window_samples[index][i];
			std::cout << "\n\t\tch_n_Samples = " << ch_n_samples[index][i];
		}
	}
	/*
	std::array<double, 16> ch_min{std::numeric_limits<double>::max()};
	std::array<double, 16> ch_max{std::numeric_limits<double>::min()};
	auto ch_Sum = Reader->GetView<std::array<std::vector<double>,16>>(Form("%s.%s",INTEGRATION_BRANCH_NAME, "ch_Sum"));
	for( auto entry : Reader->GetEntryRange() ) {
		const auto& sum = ch_Sum(entry);	
		for( size_t ch = 0; ch < sum.size(); ch++ ) {
			for( auto s : sum[ch]) {
				if(s > ch_max[ch]) ch_max[ch] = s;
				if(s < ch_min[ch]) ch_min[ch] = s;
			}
		}
	}
	for(int i = 0; i < 16; i++) {
		std::cout << "{min, max} Found (" << i << "):\t";
		std::cout << "{" << ch_min[i] << "," << ch_max[i] << "}\n";
	}

	*/
	return 0;
}
