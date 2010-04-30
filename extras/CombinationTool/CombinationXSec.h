#ifndef __COMBINATIONXSEC__H
#define __COMBINATIONXSEC__H

/*!
 * \class CombinationXSec
 * \brief A class for combining different cross-section measurements.
 * \author Kevin Kr&ouml;ninger
 * \version 1.0
 * \date 04.2010
 * This class combines different cross-section measurements.
 */

#include "CombinationModel.h"
#include "ParameterSummary.h" 

class BCH1D; 

// ---------------------------------------------------------
class CombinationXSec : public CombinationModel
{
 public:
	
	/** \name Constructors and destructors */
	/* @{ */
	
	/**
	 * Default constructor. 
	 * @param name The name of the signal to be combined.
	 * @param xmin The minimum of the signal.
	 * @param xmax The maximum of the signal.
	 */ 
	CombinationXSec(const char * name, double xmin, double xmax);

	/**
	 * Default destructor. 
	 */ 
	~CombinationXSec();
	
	/* @} */

	/** \name Public member functions (get) */
	/* @{ */

	/**
	 * Return the parameter index for the channel efficiency.
	 * @param channelname The name of the channel.
	 */ 
	int GetParIndexChannelEfficiency(const char* channelname); 

	/**
	 * Return the parameter index for the channel luminosity.
	 * @param channelname The name of the channel.
	 */ 
	int GetParIndexChannelLuminosity(const char* channelname); 

	/* @} */

	/** \name Public member functions (set) */
	/* @{ */

	/**
	 * Set the efficiency prior for one channel. 
	 * @param channelname The name of the channel. 
	 * @param prior The prior TF1 function.
	 * @return An error code. 
	 */ 
	int SetChannelEfficiencyPrior(const char* channelname, TF1* prior); 

	/**
	 * Set a Gaussian efficiency prior for one channel. 
	 * @param channelname The name of the channel. 
	 * @param mean The mean value of the Gauss function.
	 * @param sigma The standard deviation of the Gauss function.
	 * @return An error code.
	 */ 
	int SetChannelEfficiencyPriorGauss(const char* channelname, double mean, double sigma = 0.);

	/**
	 * Set the luminosity prior for one channel. 
	 * @param channelname The name of the channel. 
	 * @param prior The prior TF1 function.
	 * @return An error code. 
	 */ 
	int SetChannelLuminosityPrior(const char* channelname, TF1* prior); 

	/**
	 * Set a Gaussian luminosity prior for one channel. 
	 * @param channelname The name of the channel. 
	 * @param mean The mean value of the Gauss function.
	 * @param sigma The standard deviation of the Gauss function.
	 * @return An error code.
	 */ 
	int SetChannelLuminosityPriorGauss(const char* channelname, double mean, double sigma = 0.); 

	/**
	 * Set the branching ratio for a channel. 
	 * @param channelname The name of the channel. 
	 * @param BR the branching ratio. 
	 * @return An error code. 
	 */ 
	int SetChannelBR(const char* channelname, double BR);

	/* @} */

	/** \name Public member functions (run) */
	/* @{ */

	/**
	 * Perform analysis on one channel.
	 * @param channelname The name of the channel
	 * @param flag_syst Flag: systematics on (1) or off (0).
	 * @return A summary object of the signal parameter.
	 */ 
	ParameterSummary PerformSingleChannelAnalysis(const char* channelname, bool flag_syst = true);

	/* @} */

	/** \name Public member functions (misc) */
	/* @{ */

	/**
	 * Add a channel to be combined. 
	 * @param channelname The name of the channel. 
	 * @return An error code. 
	 */ 
	int AddChannel(const char* channelname); 

	/* @} */

	/** \name Public member functions (overloaded from BCModel) */
	/* @{ */

	/**
	 * Calculate the log of the prior probability. 
	 * @param parameters A vector of parameters. 
	 * @return The log of the prior probability.
	 */
	double LogAPrioriProbability(std::vector <double> parameters);

	/**
	 * Calculate the log of the likelihood
	 * @param parameters A vector of parameters. 
	 * @return The log of the likelihood.
	 */
	double LogLikelihood(std::vector <double> parameters);

	/* @} */

 private:

	/**
	 * The container of the efficiency priors. The index is the channel
	 * container index.
	 */ 
	std::vector<TF1*> fChannelEfficiencyPriorContainer;

	/**
	 * The container of the luminosity priors. The index is the channel
	 * container index.
	 */ 
	std::vector<TF1*> fChannelLuminosityPriorContainer;

	/**
	 * The container of the efficiency parameter index. The index is the
	 * channel container index.
	 */ 
	std::vector<int> fParIndexChannelEfficiency;

	/**
	 * The container of the luminosity parameter index. The index is the
	 * channel container index.
	 */ 
	std::vector<int> fParIndexChannelLuminosity;

	/**
	 * The containe of branching ratio. The index ist he channel
	 * container index.
	 */ 	
	std::vector<double> fChannelBR; 

};
// ---------------------------------------------------------

#endif

