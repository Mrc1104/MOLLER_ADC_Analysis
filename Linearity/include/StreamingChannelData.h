#ifndef __STREAMING_CHANNEL_DATA_H__
#define __STREAMING_CHANNEL_DATA_H__
#include <memory>
#include <array>

#include <TGraph.h>
#include <TDirectory.h>
#include <TMultiGraph.h>
#include <TLegend.h>
#include <ROOT/RNTupleReader.hxx>

class StreamingChannelData
{
public:
	struct adc_channels_t
	{
		static constexpr size_t N_ADC_CHANNELS = 2;
		std::array<size_t ,N_ADC_CHANNELS> channels;
		adc_channels_t(std::array<size_t ,N_ADC_CHANNELS> chans) : channels(chans) { }
	};
public:
	StreamingChannelData(TDirectory* file, std::unique_ptr<ROOT::RNTupleReader> reader, adc_channels_t channels);
	void ReadChannelData();
	void SavePlots();
private:
	TDirectory* File;
	std::unique_ptr<ROOT::RNTupleReader> Reader;
	adc_channels_t Channels;
private:
	std::array<std::unique_ptr<TGraph>, adc_channels_t::N_ADC_CHANNELS> gChannels;
	std::array<std::unique_ptr<TGraph>, adc_channels_t::N_ADC_CHANNELS> gChannels_gated;
};


#endif

