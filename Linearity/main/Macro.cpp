#include "TApplication.h"
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
#include <numeric>
#include <limits>
#include <fstream>
#include <utility>
#include "TRootCanvas.h"
#include "TGraphErrors.h"

#define OFFSET 20
enum SOFTWARE_CHANNEL
{
	CHAN_0 = 0,
	CHAN_1 = 1
};

struct Signal_tStmp
{
	unsigned min;
	unsigned max;
};

double GetMean(const std::vector<double> &v)
{
	double first_moment = std::accumulate(std::begin(v), std::end(v), 0.0);
	return first_moment / v.size();
}
double GetRMS(const std::vector<double> &v, const double mean)
{
	double sum = 0.0;
	for( auto d : v ) {
		sum += (d - mean)*(d - mean);
	}
	sum /= v.size();
	return TMath::Sqrt( sum );
}

Signal_tStmp Find_Valid_Signal_Range(ROOT::RNTupleReader* Reader, SOFTWARE_CHANNEL chan)
{
	ROOT::RNTupleView<tDataSamples> data = Reader->GetView<tDataSamples>("SampleStream");

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
		const auto& ch_data  = (chan == CHAN_1) ? data(entry).ch1_data : data(entry).ch0_data;
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
				if(ch_data[index] <= min) {
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
	return (MinTimeStamp < MaxTimeStamp) ? Signal_tStmp{MinTimeStamp, MaxTimeStamp} : Signal_tStmp{MaxTimeStamp, MinTimeStamp};
}

void divide_canvas_algorithm(TCanvas &c, const int size)
{
	[[maybe_unused]]
	int row = 1, col = 1, col_counter = 0;
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


int main(int argc, char** argv)
{
	
	// Open File
	std::vector<std::string> vFiles = {
		"../Rootfiles/moller_stream_molleradcse05_110.root",
		"../Rootfiles/moller_stream_molleradcse05_111.root",
		"../Rootfiles/moller_stream_molleradcse05_112.root",
		"../Rootfiles/moller_stream_molleradcse05_113.root",
		"../Rootfiles/moller_stream_molleradcse05_116.root",
		"../Rootfiles/moller_stream_molleradcse05_117.root",
		"../Rootfiles/moller_stream_molleradcse05_118.root",
		"../Rootfiles/moller_stream_molleradcse05_119.root",
	};

	const size_t N_ADC_CHAN = 2*vFiles.size();
	auto cResidualMeans = std::make_unique<TCanvas>("ResidualMeans");
	auto gResidualMeans = std::make_unique<TGraphErrors>();

	auto cResidual = std::make_unique<TCanvas>("Residual");
	divide_canvas_algorithm(*cResidual, N_ADC_CHAN);
	std::unique_ptr<TGraph> gResidual[N_ADC_CHAN];
	for(size_t i = 0; i < N_ADC_CHAN; i++) {
		gResidual[i] = std::make_unique<TGraph>();
	}

	auto cIntercept = std::make_unique<TCanvas>("intercept");
	auto gIntercept = std::make_unique<TGraphErrors>();

	auto cSlope = std::make_unique<TCanvas>("slope");
	auto gSlope = std::make_unique<TGraphErrors>();


	auto cRange =  std::make_unique<TCanvas>("cRange");
	divide_canvas_algorithm(*cRange, N_ADC_CHAN);
	std::unique_ptr<TGraph> gRange[N_ADC_CHAN];
	for(int i = 0; i < N_ADC_CHAN; i++) {
  		gRange[i] = std::make_unique<TGraph>();
	}


	auto fsave = std::make_unique<TFile>("LinearityStats.root", "RECREATE");

	int adc_channel = -2;
	for( auto const &file_name : vFiles )
	{
		std::unique_ptr<ROOT::RNTupleReader> Reader = ROOT::RNTupleReader::Open("DataTree",file_name.c_str());
		adc_channel += 3;
		for(int i = 0; i < 2; i++) {
			SOFTWARE_CHANNEL chan = (SOFTWARE_CHANNEL)i;
			adc_channel -= chan;
			std::cout << "adc_channel: " << adc_channel << std::endl;

			const Signal_tStmp extrema = Find_Valid_Signal_Range(Reader.get(), chan);

			// Draw +-10% to check
			cRange->cd(adc_channel+1);
			std::vector<double> voltage;
			std::vector<double> timestamps;
			ROOT::RNTupleView<tDataSamples> data = Reader->GetView<tDataSamples>("SampleStream");
			for( auto entry : Reader->GetEntryRange() ) {
				const auto& ch_data  = (chan == CHAN_1) ? data(entry).ch1_data : data(entry).ch0_data;
				const auto& tStmp    = data(entry).tStmp;
				if(tStmp[tStmp.size()-1] < extrema.min) continue;
				for(size_t index = 0; index < ch_data.size(); index++) {
					if(tStmp[index] >= extrema.min-10 && tStmp[index] <= extrema.max+10) {
						gRange[adc_channel]->AddPoint(tStmp[index], ch_data[index]);
						if(tStmp[index] >= extrema.min+OFFSET && tStmp[index] <= extrema.max-OFFSET) {
							voltage.push_back( ch_data[index] );
							timestamps.push_back( tStmp[index] );
							// std::cout << ch_data[index] << "\t" << tStmp[index] << std::endl;
						}
					}
				}
			}
			gRange[adc_channel]->SetTitle(Form("Soft. Chan %d vs Time; tStmp [ms]; ch%d_data",chan, chan));
			gRange[adc_channel]->Draw("AP");

			// Fit with the exact range we care about
			TF1* fit = new TF1(Form("fit_adc_chan%d", adc_channel), "pol1", extrema.min+OFFSET, extrema.max-OFFSET);
			gRange[adc_channel]->Fit(fit, "R");

			// Draw slope vs Chan
			cSlope->cd();
			gSlope->AddPointError(adc_channel, fit->GetParameter(1), 0, fit->GetParError(1));
			gSlope->SetTitle("Slope Vs ADC Chan; ADC Chan; Slope");
			gSlope->SetMarkerStyle(8);
			gSlope->Draw("AP");

			// Draw Intercept vs Chan
			cIntercept->cd();
			gIntercept->AddPointError(adc_channel, fit->GetParameter(0), 0, fit->GetParError(0));
			gIntercept->SetTitle("Intercept Vs ADC Chan; ADC Chan; Intercept");
			gIntercept->SetMarkerStyle(8);
			gIntercept->Draw("AP");


			// Compute Residual
			cResidual->cd(adc_channel+1);
			std::vector<double> residual;
			for( size_t index = 0; index < voltage.size(); index++ ) {
				double time   = timestamps[index];
				double actual = voltage[index];
				double eval   = fit->Eval(time);
				auto res = residual.insert(std::end(residual), eval - actual);
				// std::cout << residual_counter << "\t" << residual << "\n";
				gResidual[adc_channel]->AddPoint(time,*res);
			}
			double avg_residual = GetMean(residual);
			double rms_residual = GetRMS(residual, avg_residual);

			gResidual[adc_channel]->SetTitle(Form("Residual Vs ADC Chan%d; ADC Chan %d; Residual", adc_channel,adc_channel));
			gResidual[adc_channel]->Draw("AP");	
			std::cout << "Avg Residual: " << avg_residual << "\n";
			std::cout << "RMS: " << rms_residual << "\n";

			cResidualMeans->cd();
			gResidualMeans->AddPointError(adc_channel, avg_residual, 0, rms_residual);
			gResidualMeans->SetTitle("Mean vs ADC Chan; ADC Chan; #mu_{residual}");
			gResidualMeans->SetMarkerStyle(8);
			gResidualMeans->Draw("AP");
		}
	}

	cResidualMeans->Write("cResidual");
	// gResidualMeans->Write();

	cResidual->Write();
	for(int i = 0; i < 2; i++) {
		// gResidual[i]->Write();
	}

	cIntercept->Write("cIntercept");
	// gIntercept->Write();

	cSlope->Write("cSlope");
	// gSlope->Write();


	cRange->Write("cRange");
	for(int i = 0; i < 2; i++) {
  		// gRange[i]->Write();
	}
}
