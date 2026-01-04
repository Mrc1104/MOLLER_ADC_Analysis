#ifndef __FFT_H__
#define __FFT_H__
#include <vector>
#include <TGraph.h>
#include <complex>
#include <memory>
#include <limits>

class FFT
{
public:
	enum class FFT_TYPE{
		IMAGINARY,
		REAL,
		MAGNITUDE,
		AMPLITUDE,
		POWER_SPECTRUM,
		POWER_SPECTRUM_DENSITY
	};
public:
	FFT(const double *data, const size_t size, const double sample_rate, const double total_sample_time);
	FFT(const std::vector<double> &data, const double sample_rate, const double total_sample_time);
	double GetLargestFreq() const;
	double GetLargestMag()  const;
	double GetBinWidth()    const;
	[[nodiscard]]
	std::unique_ptr<TGraph> GetPlot(const FFT_TYPE = FFT_TYPE::AMPLITUDE, const double options = 0) const;
private:
	double nyquist_freq;
	double total_time;
	size_t number_samples;
	std::vector<std::complex<double>> transform;
	double largest_freq{0};
	double largest_mag{0};
private:
	void compute_fft(const double *data, const size_t size);
	void ConfigureTGraph(TGraph *g, const char* title) const;
	std::unique_ptr<TGraph> make_real_plot() const;
	std::unique_ptr<TGraph> make_imaginary_plot() const;
	std::unique_ptr<TGraph> make_magnitude_plot() const;
	std::unique_ptr<TGraph> make_amplitude_plot() const;
	std::unique_ptr<TGraph> make_ps_plot(const double ref = 1) const;
	std::unique_ptr<TGraph> make_psd_plot(const double ref = 1) const;
private:
	double compute_frequency(const int unsigned k_index) const;
	double compute_amplitude(const std::complex<double> c, const size_t k_index, const double frequency) const;
	double compute_power_watts(const double amplitude) const;
	double convert_power_db(const double power_watts, const double ref = 1 /* Watt */) const;
	double compute_equivalent_noise_bandwidth() const;
public:
	double GetPower(const double frequency, const double ref) const;

};


#endif
