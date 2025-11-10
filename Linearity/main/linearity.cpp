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
#include <limits>
#include <fstream>
#include <utility>

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


constexpr double   VOLTAGE_REF = 4.096;  // [V]
constexpr double   VOLTAGE_MAX =  (VOLTAGE_REF / 2);
constexpr double   VOLTAGE_MIN = -(VOLTAGE_REF / 2);
constexpr unsigned ADC_BITS    = 18;
constexpr double   LSB         = VOLTAGE_REF / power(2,ADC_BITS);
constexpr size_t   NBINS       = power(2,ADC_BITS)+1;
constexpr double   LOWER_BIN   = -power(2,ADC_BITS-1)-0.5;
constexpr double   UPPER_BIN   =  power(2,ADC_BITS-1)+0.5;

enum class SOFTWARE_CHANNEL : int
{
	CHAN_0 = 0,
	CHAN_1 = 1
};

// Returns {Min, Max} TimeStamp Values
std::pair<double, double> GetExtremaTimeStamps(ROOT::RNTupleReader* Reader, const SOFTWARE_CHANNEL CHAN, const size_t tStmp_limit = 6000)
{

	ROOT::RNTupleView<tDataSamples> data = Reader->GetView<tDataSamples>("SampleStream");

	double max = std::numeric_limits<double>::min(); unsigned MaxTimeStamp = 0;
	double min = std::numeric_limits<double>::max(); unsigned MinTimeStamp = 0;
	for( auto entry : Reader->GetEntryRange() ) {
		const auto& ch_data  = (CHAN == SOFTWARE_CHANNEL::CHAN_1) ? data(entry).ch1_data : data(entry).ch0_data;
		const auto& tStmp    = data(entry).tStmp;
		if(tStmp[0] > tStmp_limit) break;
		for(size_t index = 0; index < ch_data.size(); index++) {
			if(ch_data[index] > max) {
				max = ch_data[index];
				MaxTimeStamp = tStmp[index];
			}
			if(ch_data[index] < min) {
				min = ch_data[index];
				MinTimeStamp = tStmp[index];
			}
		}
	}
	std::cout << "Max Found to be " << max << " at tStmp: " << MaxTimeStamp << "\n";
	std::cout << "Min Found to be " << min << " at tStmp: " << MinTimeStamp << "\n";
	return std::pair{MinTimeStamp, MaxTimeStamp};
}

void FillTObject(ROOT::RNTupleReader* Reader, const SOFTWARE_CHANNEL CHAN, TH1F* h, TGraph* g, const std::pair<double, double> &TimeStampExtrema)
{
	ROOT::RNTupleView<tDataSamples> data = Reader->GetView<tDataSamples>("SampleStream");

	auto MinTimeStamp = TimeStampExtrema.first;
	auto MaxTimeStamp = TimeStampExtrema.second;
	double tol = 0.1; // 10% tolerance
	for( auto entry : Reader->GetEntryRange() ) {
		const auto& ch_data  = (CHAN == SOFTWARE_CHANNEL::CHAN_1) ? data(entry).ch1_data : data(entry).ch0_data;
		const auto& tStmp    = data(entry).tStmp;
		if(tStmp[tStmp.size()-1] < MinTimeStamp) continue;
		if(tStmp[0] > MaxTimeStamp) break;
		for(size_t index = 0; index < ch_data.size(); index++) {
			if((tStmp[index] > (1.0-tol)*MinTimeStamp) && (tStmp[index]< (1.0+tol)*MaxTimeStamp)) {
				g->AddPoint(tStmp[index], ch_data[index]);
				h->Fill( (ch_data[index]/LSB) );
			}
		}
	}

}


int main()
{
	std::unique_ptr<ROOT::RNTupleReader> Reader = ROOT::RNTupleReader::Open("DataTree", "../Rootfiles/moller_stream_molleradcse05_96.root");

	auto RampHist = std::make_unique<TH1F>("Ramp", "Ramp Histogram; LSB; Cts", NBINS, LOWER_BIN, UPPER_BIN);
	auto RampGraph= std::make_unique<TGraph>();
	auto TimeStampExtrema = GetExtremaTimeStamps(Reader.get(), SOFTWARE_CHANNEL::CHAN_0);
	FillTObject(Reader.get(), SOFTWARE_CHANNEL::CHAN_0, RampHist.get(), RampGraph.get(), TimeStampExtrema);

	auto canvas    = std::make_unique<TCanvas>();
	canvas->Divide(1,2);
	canvas->cd(1);
	RampGraph->Draw();
	RampGraph->SetTitle("Linearity; tStmp [ms]; ch0_data");
	gStyle->SetOptFit();
	auto fit = std::make_unique<TF1>("fit", "pol1", TimeStampExtrema.first, TimeStampExtrema.second);
	RampGraph->Fit(fit.get(), "R");
	canvas->Modified();
	canvas->Update();
	// Move Stats Box
	auto ps = (TPaveStats *)RampGraph->GetListOfFunctions()->FindObject("stats");
	ps->SetX1NDC(0.15);
	ps->SetX2NDC(0.55);
	ps->SetY1NDC(0.65);
	ps->SetY2NDC(0.95);

	canvas->cd(2);
	RampHist->Draw();
	canvas->Print();


	// Loop over and Compute Residual
	size_t NPOINTS = RampGraph->GetN();
	auto gResidual = std::make_unique<TGraph>();
	for(size_t point = 0; point < NPOINTS; point++) {
		double time = RampGraph->GetPointX(point);
		if(time >= TimeStampExtrema.first/0.9 && time <= TimeStampExtrema.second/1.1) {
			double data = RampGraph->GetPointY(point);
			double fit_val  = fit->Eval(time);
			double residual = fit_val - data;
			gResidual->AddPoint(time, residual);
			// std::cout << point << "\t" << time << "\t" << residual << std::endl;
		}
	}

	NPOINTS = gResidual->GetN();
	auto hResidual = std::make_unique<TH1F>("hResidual", "Residual Distribution", 1000, 0.9*gResidual->GetMinimum(), gResidual->GetMaximum());
	for(size_t point = 0; point < NPOINTS; point++) {
		double data = gResidual->GetPointY(point);
		hResidual->Fill(data);
	}


	auto fsave = std::make_unique<TFile>("./save.root","RECREATE");
	auto cResidual = std::make_unique<TCanvas>();
	cResidual->Divide(1,2);
	cResidual->cd(1);
	gResidual->Draw("AP");
	cResidual->Print("cRes.ps");

	cResidual->cd(2);
	hResidual->Draw();


	gResidual->Write("gResidual");
	hResidual->Write("hResidual");
	cResidual->Write("cResidual");
	canvas->Write("canvas");

	double bin_average = 0; size_t counter = 0;
	for(size_t bin = 0.2*NBINS; bin < 0.8*NBINS; bin++)
	{
		bin_average += RampHist->GetBinContent(bin);
		counter++;
	}
	bin_average /= counter;
	std::cout << "bin_average = " << bin_average << std::endl;

	auto ResidualGraph = std::make_unique<TGraph>(counter);
	for(size_t count = 0; count < counter; count++) {
		size_t bin         = count + 0.2*NBINS;
		double bin_content = RampHist->GetBinContent(bin);
		double residual    = bin_average - bin_content;
		// std::cout << "Average: " << bin_average << "\tbin: " << bin << "\tContent: " << bin_content << "\tResidual: " << residual << std::endl;
		ResidualGraph->SetPoint(count, bin, residual);
	}
	auto cResidual2 = std::make_unique<TCanvas>();
	ResidualGraph->SetMarkerStyle(8);
	ResidualGraph->Draw("AP");
	cResidual2->Print("res.ps");

	return 0;
}
