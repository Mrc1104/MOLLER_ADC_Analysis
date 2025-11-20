#ifndef __BASELINE__H_
#define __BASELINE__H_

#include <vector>
#include <fstream>
class Baseline
{
private:
	struct baseline_data_structure
	{
		unsigned adc_channel;
		double   mean;
		double   rms;
	};
	friend std::ifstream& operator>>(std::ifstream& in ,       Baseline::baseline_data_structure &data);
	friend std::ofstream& operator<<(std::ofstream& out, const Baseline::baseline_data_structure &data);
	std::vector<baseline_data_structure> baselines;
public:
	// \brief Get baseline from a csv file (see baseline_script.C)
	Baseline(std::ifstream &fcsv);


};

#endif
