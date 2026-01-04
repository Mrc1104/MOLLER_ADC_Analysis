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
constexpr double SAMPLE_RATE = 1470583.0; // Hz
constexpr double MSEC_TO_SEC = 1e-3;
const char *PATTERN = "Input/moller_stream_molleradcse05_%d.root";
int main(int argc, char** argv)
{
	if(argc != 2) {
		std::cout << "Usage:\n\t./plot_4msec <runnumber>\n";
		return 0;
	}
	const unsigned runnumber = std::stoi(argv[1]);
	auto fInput = std::make_unique<TFile>(Form(PATTERN, runnumber));
	auto tInput = fInput->Get<TTree>("DataTree");

	TApplication *app = new TApplication("Bandwidth", 0, 0);
	auto canvas = std::make_unique<TCanvas>();

	canvas->Divide(3,2);
	canvas->cd(1);
	auto entries = tInput->Draw("ch0_data:tStmp", "tStmp<4");
	
	const double *samples = tInput->GetV1();
	const double *time    = tInput->GetV2();
	std::cout  << "entries = " << entries << '\n';
	
	FFT fft( samples, entries, SAMPLE_RATE, (time[entries-1] - time[0])*MSEC_TO_SEC);
	canvas->cd(2);
	auto gMag = fft.GetPlot(FFT::FFT_TYPE::MAGNITUDE);
	gMag->Draw("AP");


	canvas->cd(3);
	auto gAmp = fft.GetPlot(FFT::FFT_TYPE::AMPLITUDE);
	gAmp->Draw("AP");

	canvas->cd(4);
	auto gPS = fft.GetPlot(FFT::FFT_TYPE::POWER_SPECTRUM, 2.0);
	gPS->Draw("AP");

	canvas->cd(5);
	auto gPSD = fft.GetPlot(FFT::FFT_TYPE::POWER_SPECTRUM_DENSITY, 2.0);
	gPSD->Draw("AP");

	std::cout << fft.GetPower(fft.GetLargestFreq(), 2.0) << std::endl;

	app->Run();
	return 0;

}
