#include "StreamingChannelData.h"
#include "DataSmpl.h"

#include <TCanvas.h>

#include <string>

StreamingChannelData::StreamingChannelData(TDirectory* file, std::unique_ptr<ROOT::RNTupleReader> reader, adc_channels_t channels)
: File(file)
, Reader(std::move(reader))
, Channels(channels)
{
	for(size_t chan = 0; chan < adc_channels_t::N_ADC_CHANNELS; chan++) {
		gChannels[chan] = std::make_unique<TGraph>();
		gChannels_gated[chan] = std::make_unique<TGraph>();
	}
}


void StreamingChannelData::ReadChannelData()
{
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

			gChannels[0]->AddPoint(time, ch0);
			gChannels[1]->AddPoint(time, ch1);
			if(gate) {
				gChannels_gated[0]->AddPoint(time, ch0);
				gChannels_gated[1]->AddPoint(time, ch1);
			}
		}
	}
}

void StreamingChannelData::SavePlots()
{
	File->cd();
	auto canvas = std::make_unique<TCanvas>();
	for( size_t ch = 0 ; ch < adc_channels_t::N_ADC_CHANNELS; ch++ ) {
		gChannels[ch]->SetLineColor(kBlue);
		gChannels[ch]->SetMarkerColor(kBlue);
		gChannels_gated[ch]->SetLineColor(kRed);
		gChannels_gated[ch]->SetMarkerColor(kRed);

		auto lg = std::make_unique<TLegend>(0.65,0.35,0.8,0.5);
		lg->AddEntry(gChannels[ch].get()      , Form("Chan %zu", ch), "LP");
		lg->AddEntry(gChannels_gated[ch].get(),        "gate1"     ,  "LP");

		auto mg = std::make_unique<TMultiGraph>();
		std::string title = Form("Adc_Chan_%zu", Channels.channels[ch]);
		mg->Add(gChannels[ch].release()      , "AP");
		mg->Add(gChannels_gated[ch].release(), "AP");
		mg->SetTitle(Form("%s; tStmp [ms]; soft. ch %zu [V]", title.c_str(), ch));
		
		mg->Draw("A");
		lg->Draw("SAME");
		mg->Write(title.c_str());
		canvas->Write(Form("c%s", title.c_str()));
	}
	return;
}
