#include "Baseline.h"

#include <iostream>
#include <string>

std::ifstream& operator>>(std::ifstream& in, Baseline::baseline_data_structure &data)
{
	char delim[2];
	in >> data.adc_channel >> delim[0] >> data.mean >> delim[1] >> data.rms;
	return in;
}
std::ostream& operator<<(std::ostream& out, const Baseline::baseline_data_structure &data)
{
	out << "\tadc_channel = " << data.adc_channel << "\n";
	out << "\tmean = "        << data.mean        << "\n";
	out << "\trms = "         << data.rms         << "\n";
	return out;
}


Baseline::Baseline(std::ifstream &fcsv)
{
	// Remove First line comment '#'
	std::string comment;
	getline(fcsv, comment);

	baseline_data_structure tmp;
	while(fcsv >> tmp) {
		baselines.emplace_back( std::move(tmp) );
	}
}

double Baseline::GetMean(unsigned channel)
{
	for(const auto& baseline : baselines) {
		if(baseline.adc_channel == channel)
			return baseline.mean;
	}
	throw std::runtime_error("Selected Channel was not found!");
}

double Baseline::GetRMS(unsigned channel)
{
	for(const auto& baseline : baselines) {
		if(baseline.adc_channel == channel)
			return baseline.rms;
	}
	throw std::runtime_error("Selected Channel was not found!");
}
