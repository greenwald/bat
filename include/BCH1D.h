/*! \class BCH1D
 *  \brief A class for handling 1D distributions
 *
 * A class which contains a TH1D histogram and can be used for \n 
 * marginalized probabilties 
 *
 * --------------------------------------------------------- 
 *
 * AUTHOR:  D. Kollar, K. Kroeninger 
 *
 * CONTACT: dkollar *at* mppmu *dot* mppmu *dot* de
 *          kevin.kroeninger *at* phys *dot* uni *minus* goettingen *dot* de 
 *
 * CREATED: 12.06.2007 
 * 
 * REVISION:
 *
 * 03.08.2007  Dano  * increase printout level of ROOT routines like Print()\n
 * 06.08.2007  Dano  * change Print "if" structure to "case" structure\n
 * 14.08.2007  Kevin * adding smallest interval printing\n
 * 14.08.2007  Dano  * rewriting the Print() method, adding new methods for printing\n
 *
 * --------------------------------------------------------- 
 */ 

// --------------------------------------------------------- 

#ifndef __BCH1D__H
#define __BCH1D__H

#include <vector>

#include <TH1.h>

#include "BCLog.h"

// --------------------------------------------------------- 

class BCH1D
{
  
 public:
  
	// constructors and destructor 

	/**
	 * The default constructor. 
	 */ 
	BCH1D(); 

	/** 
	 * The default destructor. 
	 */ 
	~BCH1D(); 

	// methods (get) 

	/**
	 * Returns the one-dimensional histogram. 
	 */ 
	TH1D* GetHistogram()
	{ return fHistogram; }; 

	/** 
	 * Returns the mean of the distribution. 
	 */ 
	double GetMean()
	{ return fHistogram -> GetMean(); }; 

	/** 
	 * Returns the mode of the distribution. 
	 */ 
	double GetMode(); 

	/** 
	 * Returns the median of the distribution 
	 */ 
	double GetMedian()
	{ return this -> GetQuantile(0.5); }; 

	/** 
	 * Returns the quantile of the distribution. 
	 * @param probabilitysum The probability sum 
	 * @return The quantile of the distribution for the sum  
	 */ 
	double GetQuantile(double probabilitysum); 

	/** 
	 * Returns the integral of the between two values. 
	 * @param valuemin The value from which the intergration is done 
	 * @param valuemax The value up to which the intergration is done 
	 * @return The integral  
	 */ 
	double GetIntegral(double valuemin, double valuemax); 

	/** 
	 * Returns the value below which 90% of the marginalized probability lie. 
	 * @return The 90% limit 
	 */ 
	double GetLimit90()
	{ return this -> GetQuantile(0.9); }; 

	/** 
	 * Returns the value below which 95% of the marginalized probability lie. 
	 * @return The 95% limit 
	 */ 
	double GetLimit95()
	{ return this -> GetQuantile(0.95); }; 

	/** 
	 * Returns the value below which 99% of the marginalized probability lie. 
	 * @return The 99% limit 
	 */ 
	double GetLimit99()
	{ return this -> GetQuantile(0.99); }; 

	/** 
	 * Returns the positive uncertainty from the mode to the 84% value. 
	 * Value at which the probability integral reaches 84% minus the mode. 
	 * @return The positive uncertainty. 
	 */ 
	double GetUncertaintyPlusFromMode()
	{ return this -> GetQuantile(0.84) - this -> GetMode(); }; 

	/** 
	 * Returns the negative uncertainty from the mode to the 16% probability 
	 * The mode - the value at which the probability integral reaches 16%. 
	 * @return The negative uncertainty. 
	 */ 
	double GetUncertaintyMinusFromMode()
	{ return this -> GetMode() - this -> GetQuantile(0.16); }; 

	/** 
	 * Returns the positive uncertainty from the mean to the 84% value. 
	 * Value at which the probability integral reaches 84% minus the mean. 
	 * @return The positive uncertainty. 
	 */ 
	double GetUncertaintyPlusFromMean()
	{ return this -> GetQuantile(0.84) - this -> GetMean(); }; 

	/** 
	 * Returns the negative uncertainty from the mean to the 16% probability 
	 * The mean - the value at which the probability integral reaches 16%. 
	 * @return The negative uncertainty. 
	 */ 
	double GetUncertaintyMinusFromMean()
	{ return this -> GetMean() - this -> GetQuantile(0.16); }; 

	/** 
	 * Returns the p-value. 
	 * Returns the intergral from probability to the maximum. 
	 * @param probability Lower limit of integration 
	 * @return The p-value 
	 */ 
	double GetPValue(double probability);     

	// methods (set) 

	/**
	 * Sets the histogram. 
	 */ 
	void SetHistogram(TH1D * hist)
	{ fHistogram = hist; }; 

	/**
	 * Set default probability limits. Allowed values are between 68%
	 * and 100%. The default value is 95%.
	 */
	void SetDefaultCLLimit(double limit);

	/**
	 * Set global mode
	 */
	void SetGlobalMode(double mode)
		{ fMode=mode; fModeFlag=1; };

	// methods

	/**
	 * Print distribution into a PostScript file.
	 * @param filename Output filename
	 * For explanation of parameters options and ovalue look at BCH1D::Draw()
	 * method.
	 */
	void Print(const char * filename, int options=0, double ovalue=0.);

	/**
	 * Draw distribution into the active canvas.
	 * @param options Drawing options: \n 0 = band mode [default], \n
	 *                1 = draw vertical line, \n
	 *                2 = band mode with minimal interval
	 * @param ovalue Option specific value. For option 0, if ovalue is nonzero
	 *    a limit is to be drawn rather than central band with ovalue being the
	 *    per cent value of the limit. If negative, limit is drawn from minimum,
	 *    if positive limit is drawn from maximum. Allowed values are
	 *    68 < |limit| < 100. If mode is outside the band, the limit is
	 *    drawn automatically. The default limit can be changed by
	 *    BCH1D::SetDefaultCLLimit(int limit). \n
	 *    For option 1 the ovalue defines
	 *    where the line is drawn. \n
	 *    For option 2 the ovalue sets the content of
	 *    the minimal interval in per cent. If omitted a 68% minimal interval
	 *    will be drawn.
	 */
	void Draw(int options=0, double ovalue=0.);

	/**
	 * Draw distribution with band between min and max and with marker at the mode.
	 * Write the location of the mode with uncertainties. If limit is specified,
	 * draw CL limit. Allowed values are 68 < |limit| < 100.
	 */
	void DrawShadedLimits(double mode, double min, double max, double limit=0);

	/**
	 * Draw distribution with bands so that the total shaded area is the
	 * smallest possible containing and integral of "prob". 
	*/ 
	void BCH1D::DrawSmallest(double prob); 

	/**
	 * Calculate the minimal interval of the distribution containing a given content.
	 * @param min calculated minimum of the interval
	 * @param max calculated maximum of the interval
	 * @param content content of the interval [default is .68]
	 */
	void GetSmallestInterval(double & min, double & max, double content=.68);

	TH1D * GetSmallestIntervalHistogram(double level);

	/**
	 * Calculate integral of the distribution between min and max.
	 * @param min lower boundary of the integrated interval
	 * @param max upper boundary of the integrated interval
	 * @return integral calculated as sum of BinContent*BinWidth
	 */
	double IntegralWidth(double min, double max);

	/**
	 * Get histogram with bins outside min, max band being zero. The
	 * new histogram can have 2 more bins than the original one as the
	 * bins where min and max fall into will be split in two (except for the
	 * case when min and/or max are equal to some of the original bin
	 * boundaries.
	 * @param min lower boundary of the non-zero interval
	 * @param max upper boundary of the non-zero interval
	 * @return new histogram which is nonzero only between min and max
	 */
	TH1D * GetSubHisto(double min, double max, const char * name);

 private: 

	/** 
	 * The histogram
	 */ 
	TH1D * fHistogram; 

	/**
	 * Default Confidence Level limit
	 */
	double fDefaultCLLimit;

	/**
	 * Global mode
	 */
	double fMode;

	/**
	 * "Is there a global mode?" flag
	 */
	int fModeFlag;

};

// ---------------------------------------------------------

#endif
