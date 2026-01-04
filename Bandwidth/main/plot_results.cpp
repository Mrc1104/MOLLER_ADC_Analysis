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
#include <TGraphErrors.h>
#include <TTreeReader.h>
#include <TTreeReaderArray.h>
#include <TH1F.h>
#include <TF1.h>

#include "Results.h"
int main()
{

	auto file = std::make_unique<TFile>("Output/Bandwidth.root", "READ");
	auto tResults = file->Get<TTree>("Data");

	Bandwidth_Results *results[2] = {new Bandwidth_Results(), new Bandwidth_Results()};
	tResults->SetBranchAddress("Chan0", &results[0]);
	tResults->SetBranchAddress("Chan1", &results[1]);

	std::unique_ptr<TGraphErrors> graphs[2] = {
		std::make_unique<TGraphErrors>(),
		std::make_unique<TGraphErrors>()
	};

	size_t entries = tResults->GetEntries();

	for(size_t entry = 0; entry < entries; entry++) {
		tResults->GetEntry(entry);
		for(int i = 0; i < 2; i++) {
			double db = results[i]->dB;
			double freq = results[i]->frequency;
			double db_error   = results[i]->std / TMath::Sqrt(entries);
			double freq_error = results[i]->binWidth;
			graphs[i]->SetPoint(entry, freq, db);
			graphs[i]->SetPointError(entry, freq_error, db_error);
		}
	}
	
	TApplication *app = new TApplication("Bandwidth", 0, 0);
	auto canvas = std::make_unique<TCanvas>();
	canvas->Divide(1,2);
	for(int i = 0; i < 2; i++) {
		graphs[i]->SetMarkerStyle(8);
		graphs[i]->SetTitle(Form("Channel %d Bandwidth; Freq [kHz]; Power [dB]", i));
		canvas->cd(i+1);
		graphs[i]->Draw("AP");

	}
	app->Run();
};
