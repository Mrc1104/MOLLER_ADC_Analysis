#include "FFT.h"
#include <complex>
#include <cmath>
#include <TMath.h>
#include <TGraph.h>
#include <memory>
#include <iostream>

// This is where std::span (c++20) would come in handy
FFT::FFT(const double *data, const size_t size, const double sample_rate, const double total_sample_time)
: nyquist_freq( sample_rate / 2.0 )
, total_time( total_sample_time )
, number_samples( size )
{
	compute_fft(data, size);
}

FFT::FFT(const std::vector<double> &data, const double sample_rate, const double total_sample_time)
: nyquist_freq( sample_rate / 2.0 )
, total_time( total_sample_time )
, number_samples( data.size() )
{
	compute_fft(data.data(), data.size());
}

void FFT::compute_fft(const double* data, const size_t size)
{
	using namespace std::complex_literals;

	for(int k = 0; k < number_samples/2.0 + 1; k++) {
		int i =0;
		std::complex<double> sum(0,0);
		for(size_t index = 0; index < size; index++) {
			double y = data[index];
			std::complex<double> yc(y);
			std::complex<double> arg(0, (-2.0f*k*TMath::Pi()*i)/number_samples);
			i++;
			sum+=std::exp(arg)*yc;
		};
		transform.push_back( sum );
		if(k) {
			double freq = (k/total_time);
			if( (std::abs(sum) > largest_mag) && (freq <= nyquist_freq) ) {
				largest_mag = std::abs(sum);
				largest_freq= freq;
			}
		}
	}
}

double FFT::GetLargestFreq() const
{
	return largest_freq;
}
double FFT::GetLargestMag()  const
{
	return largest_mag;
}

double FFT::GetBinWidth() const
{
	return 2*nyquist_freq / number_samples;
}


std::unique_ptr<TGraph> FFT::make_magnitude_plot() const
{
	auto gfft = std::make_unique<TGraph>(transform.size());
	ConfigureTGraph(gfft.get(), "FFT; freq [Hz]; |C_{k}|");
	size_t k_index = 0;
	for( auto const &c : transform ) {
		gfft->SetPoint(k_index, compute_frequency(k_index), std::abs(c));
		k_index++;
	}
	return gfft;
}

std::unique_ptr<TGraph> FFT::make_real_plot() const
{
	auto gfft = std::make_unique<TGraph>(transform.size());
	ConfigureTGraph(gfft.get(), "FFT; freq [Hz]; Re[C_{k}]");
	size_t k_index = 0;
	for( auto const &c : transform ) {
		gfft->SetPoint(k_index, k_index, c.real());
		k_index++;
	}
	return gfft;
}

std::unique_ptr<TGraph> FFT::make_imaginary_plot() const
{
	auto gfft = std::make_unique<TGraph>(transform.size());
	ConfigureTGraph(gfft.get(), "FFT; freq [Hz]; Im[C_{k}]");
	size_t k_index = 0;
	for( auto const &c : transform ) {
		gfft->SetPoint(k_index, k_index, c.imag());
		k_index++;
	}
	return gfft;
}

std::unique_ptr<TGraph> FFT::make_amplitude_plot() const
{
	auto gfft = std::make_unique<TGraph>(transform.size());
	ConfigureTGraph(gfft.get(), "FFT; freq [Hz]; Amplitude");
	size_t k_index = 0;
	for( auto const &c : transform ) {
		double frequency = compute_frequency(k_index);
		double amplitude = compute_amplitude(c, k_index, frequency);
		gfft->SetPoint(k_index, frequency, amplitude);
		k_index++;
	}
	return gfft;
}


// Power Spectrum of a sinusoidal
std::unique_ptr<TGraph> FFT::make_psd_plot(const double ref) const
{
	auto gfft = std::make_unique<TGraph>(transform.size());
	ConfigureTGraph(gfft.get(), Form("FFT: Power Spectrum Density; freq [Hz]; Power [dB / Hz] [ref = %.3f W]", ref));
	size_t k_index = 0;
	for( auto const &c : transform ) {
		double frequency = compute_frequency(k_index);
		double amplitude = compute_amplitude(c, k_index, frequency);
		double power_watts = compute_power_watts(amplitude);
		double power_dB    = convert_power_db(power_watts, ref);
		double enbw        = compute_equivalent_noise_bandwidth();
		double power_density  = power_dB / enbw;

		gfft->SetPoint(k_index, frequency, power_density);
		k_index++;
	}
	return gfft;
}

// Power Spectrum of a sinusoidal
std::unique_ptr<TGraph> FFT::make_ps_plot(const double ref) const
{
	auto gfft = std::make_unique<TGraph>(transform.size());
	ConfigureTGraph(gfft.get(), Form("FFT: Power Spectrum; freq [Hz]; Power [dB] [ref = %.3f W]", ref));
	size_t k_index = 0;
	for( auto const &c : transform ) {
		double frequency = compute_frequency(k_index);
		double amplitude = compute_amplitude(c, k_index, frequency);
		double power_watts = amplitude * amplitude / 2;
		double power_dB    = convert_power_db(power_watts, ref);

		gfft->SetPoint(k_index, frequency, power_dB);
		k_index++;
	}
	return gfft;
}

// if more options are required other than a lone double, consider a union
std::unique_ptr<TGraph> FFT::GetPlot(FFT_TYPE type, const double options) const
{
	std::unique_ptr<TGraph> plot{nullptr};
	switch( type ) {
		case FFT_TYPE::IMAGINARY:
			plot = make_imaginary_plot();
			break;
		case FFT_TYPE::REAL:
			plot = make_real_plot();
			break;
		case FFT_TYPE::MAGNITUDE:
			plot = make_magnitude_plot();
			break;
		case FFT_TYPE::AMPLITUDE:
			plot = make_amplitude_plot();
			break;
		case FFT_TYPE::POWER_SPECTRUM:
			plot = make_ps_plot(options);
			break;
		case FFT_TYPE::POWER_SPECTRUM_DENSITY:
			plot = make_psd_plot(options);
			break;
		default:
			break;
	};
	return plot;
}

void FFT::ConfigureTGraph(TGraph *g, const char *title) const
{
	g->SetMarkerStyle(8);
	g->SetTitle(title);
}

double FFT::compute_frequency(const int unsigned k_index) const
{
	return k_index / total_time;
}

double FFT::compute_amplitude(const std::complex<double> c, const size_t k_index, const double frequency) const
{
	double amplitude;
	if(k_index == 0 || (std::abs(frequency - nyquist_freq) < 1e-6)) {
		amplitude = std::abs(c) / number_samples;
	} else {
		amplitude = 2*std::abs(c) / number_samples;
	}
	return amplitude;
}

double FFT::compute_equivalent_noise_bandwidth() const
{
	// ENWB for a rectangular window {w_i = 1}
	double sample_rate = 2*nyquist_freq;
	return sample_rate / number_samples;
}

double FFT::compute_power_watts(const double amplitude) const
{
	// Power of sine(x) = A^2 / 2
	return amplitude * amplitude / 2.0;
}

double FFT::convert_power_db(const double power_watts, const double ref) const
{
	double power_dB = 10.0*TMath::Log10(power_watts / ref);
	return power_dB;
}

double FFT::GetPower(const double frequency, const double ref) const
{
	size_t k_index = std::lround( frequency * total_time );
	double amplitude = compute_amplitude(transform.at(k_index), k_index, frequency);
	double power_watts = compute_power_watts(amplitude);
	double power_dB    = convert_power_db(power_watts, ref);
	return power_dB;
}

