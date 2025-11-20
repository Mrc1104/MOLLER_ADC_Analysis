#include "Baseline.h"

std::ifstream& operator>>(std::ifstream& in, Baseline::baseline_data_structure &data)
{
	if(in.peek() != '#')
		in >> data.adc_channel >> data.mean >> data.rms;
	return in;
}
std::ofstream& operator<<(std::ofstream& out, const Baseline::baseline_data_structure &data)
{
	out << "\tadc_channel = " << data.adc_channel << "\n";
	out << "\tmean = "        << data.mean        << "\n";
	out << "\trms = "         << data.rms         << "\n";
	return out;
}


Baseline::Baseline(std::ifstream &fcsv)
{
	baseline_data_structure tmp;
	while(fcsv >> tmp) {
		baselines.push_back( std::move(tmp) );
	}
}
