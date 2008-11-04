/*!
 * \class BCH2D
 * \brief  A class for handling 2D distributions.
 * \author Daniel Kollar
 * \author Kevin Kr&ouml;ninger
 * \version 1.0
 * \date 08.2008
 * \detail This class contains a TH2D histogram and some additional
 * functions. It is used for marginalized distributions.
 */

/*
 * Copyright (C) 2008, Daniel Kollar and Kevin Kroeninger.
 * All rights reserved.
 *
 * For the licensing terms see doc/COPYING.
 */

// ---------------------------------------------------------

#ifndef __BCH2D__H
#define __BCH2D__H

#include <vector>

#include <TH1D.h>
#include <TH2D.h>
#include <TGraph.h>

// ---------------------------------------------------------

class BCH2D
{

	public:

		/** \name Constructors and destructors */
		/* @{ */

		/**
		 * The default constructor. */
		BCH2D();

		/**
		 * The default destructor. */
		~BCH2D();

		/* @} */
		/** \name Member functions (get)  */
		/* @{ */

		/**
		 * @return The 2D histogram. */
		TH2D * GetHistogram()
			{ return fHistogram; };

		/**
		 * @return The mean of the distribution. */
		void GetMean(double& mean);

		/**
		 * @return The mode of the distribution */
		void GetMode(double& mode);

		/* @} */
		/** \name Member functions (set)  */
		/* @{ */

		/**
		 * Set the 2D histogram. */
		void SetHistogram(TH2D * hist)
			{ fHistogram = hist; };

		/**
		 * Set global mode.
		 * @param The global mode. */
		void SetGlobalMode(double mode[2])
			{ fMode[0] = mode[0]; fMode[1] = mode[1]; fModeFlag =1; };

		/* @} */
		/** \name Member functions (miscellaneous methods) */
		/* @{ */

		/**
		 * Print 2-d histogram to file
		 * @param filename The filename
		 * @param ww canvas size in pixels along X
		 * @param ww canvas size in pixels along Y
		 * If ww and wh are set to 0, default ROOT canvas size is used.
		 * For explanation of the parameter options see the Draw() method. */
		void Print(const char * filename, int options=0, int ww=0, int wh=0);

		/**
		 * Draw 2-d distribution into the active canvas
		 * @param options explanation to come */
		void Draw(int options=0);

		/*
		 * Calculates the integral of the distribution as a function of the
		 * height. */
		void CalculateIntegratedHistogram();

		/*
		 * Calculates the height below which the integrated probability has
		 * a certain value.
		 * @param p The integrated probability in the region below the height to be estimated. */
		double GetLevel(double p);

		/**
		 * Returns the number of intervals as a function of x
		 * @param h The histogram.
		 * @param nfoundmax The maximum number of intervals.
		 * @return A vector containing the number of intervals for all bins in x. */
		std::vector <int> GetNIntervalsY(TH2D * h, int &nfoundmax);

		/**
		 *   */
		TGraph * GetLowestBandGraph(TH2D * h, std::vector<int> nint);
		TGraph * GetLowestBandGraph(TH2D * h);


		std::vector <double> GetLevelBoundary(TH2D * h, double level);
		TGraph * GetBandGraph(TH2D * h , double level1, double level2);

		TGraph ** GetBandGraphs(TH2D * h);
		TGraph ** GetBandGraphs(TH2D * h, int &n);

		/* @} */

	private:

		/**
		 * The 2D histogram */
		TH2D * fHistogram;

		/**
		 * The integrated 2D histogram */
		TH1D * fIntegratedHistogram;

		/**
		 * Global mode */
		double fMode[2];

		/**
		 * "Is there a global mode?" flag */
		int fModeFlag;

};

// ---------------------------------------------------------

#endif
