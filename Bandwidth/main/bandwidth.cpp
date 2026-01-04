#include <memory>
#include <iostream>
#include <vector>
#include <string>

#include <TFile.h>
#include <TTree.h>
#include <TMath.h>
#include <TApplication.h>
#include <TCanvas.h>
#include <TGraph.h>
#include <TTreeReader.h>
#include <TTreeReaderArray.h>
#include <TH1F.h>

#include "FFT.h"
#include "Results.h"

constexpr double SAMPLE_RATE = 1470583.0; // Hz
constexpr double MSEC_TO_SEC = 1e-3;

struct Samples
{
	Samples(const double time, const size_t size = 1000) : end_time{time} { data.reserve(size); }
	bool push(const double time, const double sample) {
		bool flag = false;
		if( (time < end_time) || (data.size() < minimum_sample_size) ) {
			data.push_back(sample);
			flag = true;
		}
		return flag;
	}
	std::vector<double> data;
	double end_time;
private:
	size_t minimum_sample_size = 1000;
};

struct Bandwidth_Data
{
	Bandwidth_Data(const size_t size) {
		data.reserve(size);
		binwidth.reserve(size);
	}
	std::vector<double> data;
	std::vector<double> binwidth;
};


double compute_mean(const double *x, const size_t size) {
	double sum = 0;
	for(size_t index = 0; index < size; index++) {
		sum += x[index];
	}
	return sum / size;
}

double compute_variance(const double *x, const size_t size, const double mean) {
	double sum = 0;
	for(size_t index = 0; index < size; index++) {
		double distance = x[index] - mean;
		sum += distance * distance ;
	}
	return sum / (size-1);
}

const char *PATTERN = "Input/moller_stream_molleradcse05_%d.root";
void Obtain_Results(Bandwidth_Results &results, const unsigned runnumber, const unsigned chan = 0) {

	auto fInput = std::make_unique<TFile>(Form(PATTERN, runnumber));
	auto tInput = fInput->Get<TTree>("DataTree");
	auto Reader = TTreeReader(tInput);

	size_t nregions = 10;
	size_t region   = 0;
	double region_size_in_msec = 4;
	double time_offset = 0;
	bool end = false;
	TTreeReaderArray<double> time(Reader, "tStmp");
	TTreeReaderArray<double> data(Reader, Form("ch%d_data", chan));
	Samples samples(region_size_in_msec);
	Bandwidth_Data bandwidth(nregions);
	while( Reader.Next() ) {
		if(end) break;
		size_t size = time.GetSize();
		for(size_t index = 0; index < size; index++) {
			auto t = time[index];
			auto d = data[index];
			if(time_offset > t) {
				continue;
			}
			if(!samples.push(t, d)) {
				FFT fft( samples.data, SAMPLE_RATE, region_size_in_msec*MSEC_TO_SEC);
				bandwidth.data.push_back(fft.GetPower(fft.GetLargestFreq(), 2.0));
				bandwidth.binwidth.push_back(fft.GetBinWidth());

				time_offset = t + 1e3; // t + 1 sec
				size_t new_size = samples.data.size();
				double new_end_time = region_size_in_msec + time_offset;

				samples = Samples(new_end_time, new_size);
				region++;
				if(region >= nregions) {
					end = true;
					break;
				}
			}
		}
	}
	double mean_db = compute_mean(bandwidth.data.data(), bandwidth.data.size());
	double var_db  = compute_variance(bandwidth.data.data(), bandwidth.data.size(), mean_db);
	double std_db  = TMath::Sqrt(var_db);

	double mean_bin = compute_mean(bandwidth.binwidth.data(), bandwidth.binwidth.size());
	double var_bin  = compute_variance(bandwidth.binwidth.data(), bandwidth.binwidth.size(), mean_bin);
	double std_bin  = TMath::Sqrt(var_bin);

	std::cout << "Results for run " << runnumber << " (chan " << chan << ")\n";
	std::cout << "\tMean: " << mean_db << " [dB] \tstd: " << std_db << std::endl;
	std::cout << "\tMean: " << mean_bin << " [Hz] \tstd: " << std_bin << std::endl;

	results.channel = chan;
	results.dB  = mean_db;
	results.std = std_db;
	results.binWidth = mean_bin / 1000.0;
	results.iterations = region;
}


int main()
{

	auto fsave = std::make_unique<TFile>("./Output/Bandwidth.root", "RECREATE");
	auto tsave = std::make_unique<TTree>("Data", "Bandwidth Data");
	Bandwidth_Results Results[2] = {Bandwidth_Results(), Bandwidth_Results() };
	tsave->Branch("Chan0", &Results[0], 3000, 99);
	tsave->Branch("Chan1", &Results[1], 3000, 99);

	std::vector<unsigned> runs = {120, 121, 123,  124,  128, 129};
	std::vector<double>   freq = {  1,  10, 100, 1000, 600, 800};
	size_t size = runs.size();
	for(size_t i = 0; i < size; i++) {
		auto r = runs[i];
		auto f = freq[i];
		for(int i = 0; i < 2; i++) {
			Results[i].run       = r;
			Results[i].frequency = f;
			Obtain_Results(Results[i], r, i);
		}
		tsave->Fill();
	}
	fsave->cd();
	tsave->Write();
	return 0;
}
