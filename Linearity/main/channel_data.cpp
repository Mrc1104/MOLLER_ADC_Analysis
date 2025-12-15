#include <TFile.h>
#include <TCanvas.h>
#include <TString.h>
#include <TGraph.h>
#include <TLegend.h>
#include <TMultiGraph.h>
#include <ROOT/RNTupleReader.hxx>

#include <memory>
#include <array>
#include <vector>

#include "DataSmpl.h"
#include "StreamingChannelData.h"

constexpr size_t NCHANS = 2;
const char *PATTERN = "./Input/molleradc_%d.root";

int main()
{
	struct rundata_t
	{
		int run;
		std::array<size_t, StreamingChannelData::adc_channels_t::N_ADC_CHANNELS> channels;
	};
	std::array<rundata_t, 8> runList= {
		//{runnumber,adc_chan_0, adc_chan_1}
		rundata_t{182,  1, 2},
		rundata_t{184,  3, 4},
		rundata_t{188,  5, 6},
		rundata_t{189,  7, 8},
		rundata_t{190,  9,10},
		rundata_t{194, 11,12},
		rundata_t{195, 13,14},
		rundata_t{196, 15,16}
	};

	auto fsave = std::make_shared<TFile>("./Output/Channel_Data.root", "RECREATE");
	for(const auto& rundata : runList) {
		auto run = rundata.run;
		auto chan= rundata.channels;

		std::cout << "Generating Plots for run: " << run << " {" << chan[0] << ", " << chan[1] << "}\n";
		StreamingChannelData data(fsave->mkdir( Form("run%d_%zu_%zu",run, chan[0], chan[1])  )
		                         , ROOT::RNTupleReader::Open("DataTree", Form(PATTERN, run))
								 , chan
								 );
		data.ReadChannelData();
		data.SavePlots();
	}


	return 0;
}
