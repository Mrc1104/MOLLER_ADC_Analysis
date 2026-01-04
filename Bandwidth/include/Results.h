#ifndef RESULTS_H
#define RESULTS_H

class Bandwidth_Results {
public:
	int run;
	double frequency; // kHz
	unsigned channel;
	double dB;
	double std;
	double binWidth;
	double iterations;
public:
	virtual ~Bandwidth_Results() = default;

	ClassDef(Bandwidth_Results, 1);
};

#endif
