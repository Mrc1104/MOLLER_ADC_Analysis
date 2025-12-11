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
#include "TF1.h"
#include "TCanvas.h"
#include "TStyle.h"


const char* PATTERN = "./Input/molleradc_%d.root";
int main()
{
	auto Reader = ROOT::RNTupleReader::Open(KEY_NAME,Form(PATTERN, 145));
	Reader->PrintInfo();
	return 0;
}

