#include <vector>
#include <iostream>
#include <memory>
#include <fstream>
#include <array>

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

#include "Baseline.h"


// std::pow is constexpr in c++26
// TMath::Power is a wrapper around std::pow
template<typename T>
constexpr T power(T base, unsigned exp)
{
      T ret = 1;
      for(unsigned i = 1; i <= exp; i++) {
          ret *= base;
     }
     return ret;
}

constexpr double   VOLTAGE_REF =  8.192;            // [V]
constexpr double   VOLTAGE_MAX =  (VOLTAGE_REF / 2);
constexpr double   VOLTAGE_MIN = -(VOLTAGE_REF / 2);
constexpr unsigned ADC_BITS    =  18;
constexpr double   LSB         =  VOLTAGE_REF / power(2,ADC_BITS);
constexpr size_t   NBINS       =  power(2,ADC_BITS)+1;
constexpr double   LOWER_BIN   = -power(2,ADC_BITS-1)-0.5;
constexpr double   UPPER_BIN   =  power(2,ADC_BITS-1)+0.5;

constexpr double   PRESCALED_SAMPLING_TIME = 1.0 / 1470583.0; // Prescale = 10

constexpr double  FCN_GEN_VPP        =             8.0           ; // [ V ]
constexpr double  FCN_GEN_MAX        =   FCN_GEN_VPP /    2.0    ; // [ V ]
constexpr double  FCN_GEN_HZ         =           2.00025         ; // [ Hz]
constexpr double  FCN_GEN_CYCLE      =       1.0     / FCN_GEN_HZ; // [ s ]
constexpr double  FCN_GEN_HALF_CYCLE = FCN_GEN_CYCLE /    2.0    ; // [ s ]
constexpr double  FCN_GEN_SEC_TO_V   = FCN_GEN_VPP   / FCN_GEN_HALF_CYCLE; // [V/ s]
constexpr double  FCN_GEN_MSEC_TO_V  = FCN_GEN_SEC_TO_V / 1000   ;         // [V/ms]



const char* PATTERN = "./Input/moller_stream_molleradcse05_%d.root";
int main()
{
	std::cout << "LSB: " << LSB << std::endl;
	std::cout << "PRESCALED_SAMPLING_TIME = " << PRESCALED_SAMPLING_TIME << std::endl;
	std::cout << "FCN_GEN_SEC_TO_V       = " << FCN_GEN_SEC_TO_V         << std::endl;
	std::cout << "FCN_GEN_MSEC_TO_V       = " << FCN_GEN_MSEC_TO_V         << std::endl;
	std::cout << "FCN_GEN_HALF_CYCLE      = " << FCN_GEN_HALF_CYCLE      << std::endl;


	auto Reader = ROOT::RNTupleReader::Open(KEY_NAME,Form(PATTERN, 141));
	Reader->EnableMetrics();

	ROOT::RNTupleView<tDataSamples> Data = Reader->GetView<tDataSamples>(BRANCH_NAME);

	EColor colors[] = {kRed, kBlue, kGreen, kOrange, kBlack, kRed, kBlue, kGreen, kOrange, kBlack, kRed, kBlue, kGreen, kOrange, kBlack, kRed, kBlue, kGreen, kOrange, kBlack, kRed, kBlue, kGreen, kOrange, kBlack, kRed, kBlue, kGreen, kOrange, kBlack};
	auto const *c = colors;
	std::vector<TGraph*> plots;
	auto g = plots.emplace(std::end(plots), new TGraph());
	(*g)->SetMarkerStyle(8);

	auto RampHist = std::make_unique<TH1F>("Ramp", "Ramp Histogram; LSB; Cts", NBINS, LOWER_BIN, UPPER_BIN);

	int sign = -1;
	double start_time = 196.24 + 0.11 - 0.035;
	double ref_time   = start_time;
	double time_incr  = FCN_GEN_HALF_CYCLE * 1000; // [ms]
	for( auto entry : Reader->GetEntryRange() ) {
		bool end_loop = false;
		const auto& tStmp = Data(entry).tStmp;
		const auto& chdata= Data(entry).ch0_data;
		for(size_t index = 0; index < tStmp.size(); index++) {
			if( tStmp[index] > 50000) {
				end_loop = true;
				break;
			}
			if( tStmp[index] >= start_time && tStmp[index] <= 23*time_incr+start_time ) {
				// I want time_shift to be bounded between 0.0ms and 250ms
				if( tStmp[index] >= (ref_time+time_incr) ) {
					ref_time = tStmp[index];
					sign *= -1;
					g = plots.emplace(std::end(plots), new TGraph());
					(*g)->SetMarkerStyle(8);
					(*g)->SetMarkerColor(*c);
					c++;
				}
				(*g)->AddPoint(tStmp[index], chdata[index]);
				RampHist->Fill(chdata[index]/LSB);
			 }

		}
		if( end_loop ) break;
	}
	auto gPlot = std::make_unique<TMultiGraph>();
	for( auto const g : plots ) gPlot->Add(g, "AP");
	gPlot->SetTitle("ch0_data; tStmp [ms]; ch_data [V]");
	auto file = std::make_unique<TFile>("./Output/Cycles.root", "RECREATE");
	gPlot->Write("Plot");
	RampHist->Write("Ramp");

	return 0;
}
