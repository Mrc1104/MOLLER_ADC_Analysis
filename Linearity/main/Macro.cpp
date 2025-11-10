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
	double second_moment = 0.0;
	for( auto d : v ) {
		second_moment = d * d;
	}
	second_moment /= v.size();
	return TMath::Sqrt( second_moment - mean*mean );
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
	return Signal_tStmp{MinTimeStamp, MaxTimeStamp};
}


int main(int argc, char** argv)
{
	auto cResidualMeans      = std::make_unique<TCanvas>("ResidualMeans");
	auto gResidualMeans = std::make_unique<TGraphErrors>();

	auto cResidual = std::make_unique<TCanvas>("Residual");
	cResidual->Divide(2);
	std::unique_ptr<TGraph> gResidual[2];
	for(int i = 0; i < 2; i++) {
		gResidual[i] = std::make_unique<TGraph>();
	}

	auto cIntercept = std::make_unique<TCanvas>("intercept");
	auto gIntercept = std::make_unique<TGraphErrors>();

	auto cSlope = std::make_unique<TCanvas>("slope");
	auto gSlope = std::make_unique<TGraphErrors>();


	auto cRange =  std::make_unique<TCanvas>("cRange");
	cRange->Divide(2);
	std::unique_ptr<TGraph> gRange[2];
	for(int i = 0; i < 2; i++) {
  		gRange[i] = std::make_unique<TGraph>();
	}


	auto fsave = std::make_unique<TFile>("LinearityStats.root", "RECREATE");

	for(int i = 0; i < 2; i++) {
		SOFTWARE_CHANNEL chan = (SOFTWARE_CHANNEL)i;

		// Open File
		std::string file_name = "../Rootfiles/moller_stream_molleradcse05_110.root";
		std::unique_ptr<ROOT::RNTupleReader> Reader = ROOT::RNTupleReader::Open("DataTree",file_name.c_str());
		const Signal_tStmp extrema = Find_Valid_Signal_Range(Reader.get(), CHAN_0);

		// Draw +-10% to check
		cRange->cd(chan+1);
		std::vector<double> voltage;
		std::vector<double> timestamps;
		ROOT::RNTupleView<tDataSamples> data = Reader->GetView<tDataSamples>("SampleStream");
		for( auto entry : Reader->GetEntryRange() ) {
			const auto& ch_data  = (chan == CHAN_1) ? data(entry).ch1_data : data(entry).ch0_data;
			const auto& tStmp    = data(entry).tStmp;
			if(tStmp[tStmp.size()-1] < extrema.min) continue;
			for(size_t index = 0; index < ch_data.size(); index++) {
				if(tStmp[index] >= extrema.min-10 && tStmp[index] <= extrema.max+10) {
					gRange[chan]->AddPoint(tStmp[index], ch_data[index]);
					if(tStmp[index] >= extrema.min && tStmp[index] <= extrema.max) {
						voltage.push_back( ch_data[index] );
						timestamps.push_back( tStmp[index] );
					}
				}
			}
		}
		gRange[chan]->SetTitle(Form("Soft. Chan %d vs Time; tStmp [ms]; ch%d_data",chan, chan));
		gRange[chan]->Draw("AP");

		// Fit with the exact range we care about
		TF1* fit = new TF1(Form("fit_soft_chan%d", chan), "pol1", extrema.min, extrema.max);
		gRange[chan]->Fit(fit, "R");

		// Draw slope vs Chan
		cSlope->cd();
		gSlope->AddPointError(chan, fit->GetParameter(1), 0, fit->GetParError(1));
		gSlope->SetTitle("Slope Vs Chan; Chan; Slope");
		gSlope->SetMarkerStyle(8);
		gSlope->Draw("AP");

		// Draw Intercept vs Chan
		cIntercept->cd();
		gIntercept->AddPointError(chan, fit->GetParameter(0), 0, fit->GetParError(0));
		gIntercept->SetTitle("Intercept Vs Chan; Chan; Intercept");
		gIntercept->SetMarkerStyle(8);
		gIntercept->Draw("AP");


		// Compute Residual
		cResidual->cd(chan+1);
		double average_residual = 0;
		size_t residual_counter = 0;
		for( size_t index = 0; index < voltage.size(); index++ ) {
			double time   = timestamps[index];
			double actual = voltage[index];
			double eval   = fit->Eval(time);
			double residual = eval - actual;
			average_residual += residual;
			residual_counter++;
			// std::cout << residual_counter << "\t" << residual << "\n";
			gResidual[chan]->AddPoint(time,residual); 
		}
		average_residual /= residual_counter;
		double rms_residual = GetRMS(voltage, average_residual);

		gResidual[chan]->SetTitle(Form("Residual Vs Chan%d;Chan %d; Residual", chan, chan));
		gResidual[chan]->Draw("AP");	
		std::cout << "Avg Residual: " << average_residual << "\n";
		std::cout << "RMS: " << rms_residual << "\n";

		cResidualMeans->cd();
		gResidualMeans->AddPointError(chan, average_residual, 0, rms_residual);
		gResidualMeans->SetTitle("Mean vs Chan; Chan; #mu_{residual}");
		gResidualMeans->SetMarkerStyle(8);
		gResidualMeans->Draw("AP");
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
