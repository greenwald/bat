#ifndef __BCMODELHISTOGRAMRATIOFITTER__H
#define __BCMODELHISTOGRAMRATIOFITTER__H

/*!
 * \class BCHistogramRatioFitter
 * \brief A class for fitting histograms with functions
 * \author Daniel Kollar
 * \author Kevin Kr&ouml;ninger
 * \version 1.0
 * \date 11.2008
 * \detail This class allows fitting of a TH1D histogram using
 * a TF1 function.
 */

/*
 * Copyright (C) 2008, Daniel Kollar and Kevin Kroeninger.
 * All rights reserved.
 *
 * For the licensing terms see doc/COPYING.
 */

// ---------------------------------------------------------

#include <vector>

#include "BCModel.h"
#include "BCH1D.h"

#include <TGraphAsymmErrors.h>
#include <TH2D.h> 

// ROOT classes
class TH1D;
class TF1;
class TGraphAsymmErrors; 

// ---------------------------------------------------------

class BCHistogramRatioFitter : public BCModel
{
 public:

	/** \name Constructors and destructors */
	/* @{ */

	/**
	 * The default constructor. */
	BCHistogramRatioFitter();

	/**
	 * A constructor.
	 * @param hist1 The histogram with the larger numbers
	 * @param hist2 The histogram with the smaller numbers
	 * @param func The fit function. */
	BCHistogramRatioFitter(TH1D * hist1, TH1D * hist2, TF1 * func);

	/**
	 * The default destructor. */
	~BCHistogramRatioFitter();

	/* @} */

	/** \name Member functions (get) */
	/* @{ */

	/**
	 * @return The histogram 1 */
	TH1D * GetHistogram1()
	{ return fHistogram1; };
		
	/**
	 * @return The histogram 2 */
	TH1D * GetHistogram2()
	{ return fHistogram2; };

	/**
	 * @return The histogram ratio */ 
	TGraphAsymmErrors * GetHistogramRatio()
	{ return fHistogramRatio; }; 

	/**
	 * @return The fit function */
	TF1 * GetFitFunction()
	{ return fFitFunction; };

	/**
	 * @return pointer to the error band */
	TGraph * GetErrorBand()
	{ return fErrorBand; }; 

	/**
	 * @return pointer to a graph for the fit function */ 
	TGraph * GetGraphFitFunction()
	{ return fGraphFitFunction; };

	/**
	 * Calculates the lower and upper limits for a given probability. 
	 * @param n n for the binomial. 
	 * @param k k for the binomial. 
	 * @param p The central probability defining the limits.
	 * @param xmin The lower limit. 
	 * @param xmax The upper limit. 
	 * @return A flag (=1 plot point, !=1 do not plot point). 
	 */ 
	int GetUncertainties(int n, int k, double p, double &xmin, double &xmax); 

	/* @} */

	/** \name Member functions (set) */
	/* @{ */

	/**
	 * @param hist The histogram 1
	 * @param hist The histogram 2
	 * @ return An error code (1:pass, 0:fail).
	 */
	int SetHistograms(TH1D * hist1, TH1D * hist2);

	/**
	 * @param func The fit function
	 * @ return An error code (1:pass, 0:fail).
	 */
	int SetFitFunction(TF1 * func);

	/**
	 * Sets the flag for integration. \n
	 * true: use ROOT's TH1D::Integrate() \n
	 * false: use linear interpolation 
	 */ 
	void SetFlagIntegration(bool flag)
	{ fFlagIntegration = flag; }; 

	/* @} */
	/** \name Member functions (miscellaneous methods) */
	/* @{ */

	/**
	 * The log of the prior probability. Overloaded from BCModel.
	 * @param parameters A vector of doubles containing the parameter values. */
	virtual double LogAPrioriProbability(std::vector <double> parameters);

	/**
	 * The log of the conditional probability. Overloaded from BCModel.
	 * @param parameters A vector of doubles containing the parameter values. */
	virtual double LogLikelihood(std::vector <double> parameters);

	/**
	 * Returns the y-value of the 1-dimensional fit function at an x and
	 * for a set of parameters.
	 * @param x A vector with the x-value.
	 * @param parameters A set of parameters. */
	double FitFunction(std::vector <double> x, std::vector <double> parameters);

	/**
	 * Performs the fit.
	 * @return An error code. */
	int Fit()
	{ return this -> Fit(fHistogram1, fHistogram2, fFitFunction); };
		
	/**
	 * Performs the fit.
	 * @param hist1 The histogram with the larger number. 
	 * @param hist2 The histogram with the smaller number. 
	 * @param func The fit function.
	 * @return An error code. */
	int Fit(TH1D * hist1, TH1D * hist2, TF1 * func);
		
	/**
	 * Draw the fit in the current pad. */
	void DrawFit(const char * options = "", bool flaglegend = false);

	/**
	 * Print a summary of the fit to the screen
	 */
	void PrintFitSummary();

	/**
	 * Calculate the p-value using fast-MCMC.
	 * @param par A set of parameter values 
	 * @param  pvalue The pvalue
	 * @return An error code 
	 */ 
	//		int CalculatePValueFast(std::vector<double> par, double &pvalue); 

	/* @} */

 private:

	/**
	 * The histogram containing the larger numbers.
	 */
	TH1D * fHistogram1;

	/**
	 * The histogram containing the smaller numbers.
	 */
	TH1D * fHistogram2;

	/**
	 * The efficiency histogram. 
	 */
	TGraphAsymmErrors * fHistogramRatio; 

	/**
	 * The fit function */
	TF1 * fFitFunction;

	/** 
	 * Flag for using the ROOT TH1D::Integral method (true), or linear
	 * interpolation (false) */ 
	bool fFlagIntegration; 
		
	/**
	 * Pointer to the error band (for legend) */ 
	TGraph * fErrorBand; 

	/**
	 * Pointer to a graph for displaying the fit function */ 
	TGraph * fGraphFitFunction; 

	/**
	 * Temporary histogram for calculating the binomial qunatiles */
	TH1D * fHistogramBinomial; 
	
};

// ---------------------------------------------------------

#endif
