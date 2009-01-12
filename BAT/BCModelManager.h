#ifndef __BCMODELMANAGER__H
#define __BCMODELMANAGER__H

/*!
 * \class BCModelManager
 * \brief A class representing a set of BCModels.
 * \author Daniel Kollar
 * \author Kevin Kr&ouml;ninger
 * \version 1.0
 * \date 08.2008
 * \detail This class represents a manager for BCModels. It handles
 * common data sets and performs operations on BCModels
 * simultaneously. Model comparsion in terms of a posteriori
 * probabilities is only possible with this class.
 */

/*
 * Copyright (C) 2008, Daniel Kollar and Kevin Kroeninger.
 * All rights reserved.
 *
 * For the licensing terms see doc/COPYING.
 */

// ---------------------------------------------------------

#include "BAT/BCModel.h"
#include "BAT/BCDataSet.h"

// BAT classes
class BCDataPoint;

// ---------------------------------------------------------

class BCModelManager
{
	public:

		/** \name Constructors and destructors */
		/* @{ */

		/**
		 * The default constructor. */
		BCModelManager();

		/**
		 * The default copy constructor. */
		BCModelManager(const BCModelManager & modelmanager);

		/**
		 * The default destructor. */
		virtual ~BCModelManager();

		/* @} */

		/** \name Assignment operators */
		/* @{ */

		/**
		 * The defaut assignment operator */
		BCModelManager & operator = (const BCModelManager & modelmanager);

		/* @} */

		/** \name Member functions (get) */
		/* @{ */

		/**
		 * @return The number of models. */
		unsigned int GetNModels()
			{ return fModelContainer -> size(); };

		/**
		 * Returns the BCModel at a certain index of this BCModelManager.
		 * @param index The index of the model in the BCModelManager.
		 * @return The BCModel at the index. */
		BCModel * GetModel(int index)
			{ return fModelContainer -> at(index); };

		/**
		 * Returns the number of entries in the common data set.
		 * @return The number of entries. */
		int GetNDataPoints()
			{ return (fDataSet) ? fDataSet -> GetNDataPoints() : 0; };

		/**
		 * Returns a data point of the common data set at an index.
		 * @param index The index of the data point in the data set.
		 * @return The data point. */
		BCDataPoint * GetDataPoint(int index)
			{ return fDataSet -> GetDataPoint(index); };

		/**
		 * Returns the common data set.
		 * @return The data set. */
		BCDataSet * GetDataSet()
			{ return fDataSet; };

		/* @} */

		/** \name Member functions (set) */
		/* @{ */

		/**
		 * Sets the data set common to all BCModels in this
		 * BCModelManager.
		 * @param dataset A data set */
		void SetDataSet(BCDataSet * dataset);

		/**
		 * Sets a single data point as a common data set.
		 * @param datapoint A data point
		 * @see SetSingleDataPoint(BCDataSet * dataset, int index)
		 * @see SetDataSet(BCDataSet * dataset) */
		void SetSingleDataPoint(BCDataPoint * datapoint);

		/**
		 * Sets a single data point as a common data set.
		 * @param dataset A data set.
		 * @param index The index of the data point in the data set specified.
		 * @see SetSingleDataPoint(BCDataPoint * datapoint)
		 * @see SetDataSet(BCDataSet * dataset) */
		void SetSingleDataPoint(BCDataSet * dataset, unsigned int index);

		// DEBUG DELETE?
		/**
		 * Sets the maximum number of iterations for the Monte Carlo
		 * integration for all BCModels in this BCModelManager.
		 * @param niterations */
		//	void SetNIterationsMax(int niterations);

		/**
		 * @param method The marginalization method */
		void SetMarginalizationMethod(BCIntegrate::BCMarginalizationMethod method);

		/**
		 * @param method The integration method */
		void SetIntegrationMethod(BCIntegrate::BCIntegrationMethod method);

		/**
		 * @param method The mode finding method */
		void SetOptimizationMethod(BCIntegrate::BCOptimizationMethod method);

		/**
		 * @param niterations Number of iterations per dimension for Monte
		 * Carlo integration. */
		void SetNiterationsPerDimension(unsigned int niterations);

		/**
		 * @param n Number of samples per 2D bin per variable in the
		 * Metropolis marginalization.  Default is 100. */
		void SetNSamplesPer2DBin(unsigned int n);

		/**
		 * @param relprecision The relative precision envisioned for Monte
		 * Carlo integration */
		void SetRelativePrecision(double relprecision);

		/**
		 * @param n Number of bins per dimension for the marginalized
		 * distributions.  Default is 100. Minimum number allowed is 2. */
		void SetNbins(unsigned int n);

		/**
		 * Turn on or off the filling of the error band during the MCMC run
		 * for all models added to the model manager before calling this method.
		 * @param flag set to true for turning on the filling */
		void SetFillErrorBand(bool flag = true);

		/**
		 * Turn off the filling of the error band during the MCMC run
		 * for all models added to the model manager before calling this method. */
		void UnetFillErrorBand()
			{ SetFillErrorBand(false); };

		/**
		 * Sets index of the x values in function fits.
		 * @param index Index of the x values */
		void SetFitFunctionIndexX(int index);

		/**
		 * Sets index of the y values in function fits.
		 * @param index Index of the y values */
		void SetFitFunctionIndexY(int index);

		void SetFitFunctionIndices(int indexx, int indexy);

		/**
		 * Sets the data point containing the lower boundaries of possible
		 * data values */
		void SetDataPointLowerBoundaries(BCDataPoint * datasetlowerboundaries);

		/**
		 * Sets the data point containing the upper boundaries of possible
		 * data values */
		void SetDataPointUpperBoundaries(BCDataPoint* datasetupperboundaries);

		/**
		 * Sets the lower boundary of possible data values for a particular
		 * variable */
		void SetDataPointLowerBoundary(int index, double lowerboundary);

		/**
		 * Sets the upper boundary of possible data values for a particular
		 * variable */
		void SetDataPointUpperBoundary(int index, double upperboundary);

		/**
		 * Set the lower and upper boundaries for possible data values for a
		 * particular variable */
		void SetDataBoundaries(int index, double lowerboundary, double upperboundary);

		/*
		 * Fixes an axis */
		void FixDataAxis(int index, bool fixed);

		/*
		 * Sets the number of Markov chains */
		void SetNChains(unsigned int n);

		/*
		 * Sets the flag for PCA */
		void SetFlagPCA(bool flag);

		/* @} */

		/** \name Member functions (miscellaneous methods) */
		/* @{ */

		/**
		 * Adds a model to the container
		 * @param model The model
		 * @param probability The a priori probability
		 * @see AddModel(BCModel * model)
		 * @see SetModelPrior(BCModel * model, double probability) */
		void AddModel(BCModel * model, double probability=0.);

		/**
		 * Adds a data point to the data container.
		 * @param datapoint The data point */
		void AddDataPoint(BCDataPoint * datapoint)
			{ fDataSet -> AddDataPoint(datapoint); };

		/**
		 * Reads data from a file. For a description see the following
		 * member functions. */
		int ReadDataFromFile(const char * filename, const char * treename, const char * branchnames)
		{ return this ->  ReadDataFromFileTree(filename, treename, branchnames); };
		
		int ReadDataFromFile(const char * filename, int nvariables)
		{ return this -> ReadDataFromFileTxt(filename, nvariables); };
		
		/**
		 * Reads tree data from a ROOT file.
		 * Opens a ROOT file and gets a ROOT tree. It creates data set
		 * containing the values read from the file.
		 * @param filename The filename of the ROOT file
		 * @param treename The name of the ROOT tree
		 * @param branchnames A vector of the names of the branches
		 * @see ReadDataFromFileHist(char * filename, char * histname, const char*  branchnames);
		 * @see ReadDataFromFileTxt(char * filename, int nbranches); */
		int ReadDataFromFileTree(const char * filename, const char * treename, const char * branchnames);

		/**
		 * Reads data from a txt file.
		 * Opens a txt file and creates data set
		 * containing the values read from the file.
		 * @param filename The filename of the ROOT file
		 * @param nbranches The number of variables
		 * @see ReadDataFromFileTree(char * filename, char * treename, std::vector<char*> branchnames)
		 * @see ReadDataFromFileHist(char * filename, char * histname, const char * branchnames); */
		int ReadDataFromFileTxt(const char * filename, int nbranches);

		/**
		 * Calculates the normalization of the likelihood for each model in
		 * the container. */
		void Normalize();

		/**
		 * Does the mode finding */
		void FindMode();

		/**
		 * Marginalize all probabilities wrt. single parameters and all
		 * combinations of two parameters for all models. */
		void MarginalizeAll();

		/*
		 * Flag for writing Markov chain to file */
		void WriteMarkovChain(bool flag);

		/**
		 * Resets the data set */
		void ResetDataSet()
			{ fDataSet -> Reset(); };

		/**
		 * Prints a summary of model comparison into a file.
		 * If filename is omitted the summary will be printed onto the screen
		 * @param filename name of the file to write into. */
		void PrintModelComparisonSummary(const char * filename=0);

		/**
		 * Prints a summary into a file. If filename is omitted the summary
		 * will be printed onto the screen.
		 * This method is obsolete. Use PrintResults() instead.
		 * @param filename name of the file to write into. */
		void PrintSummary(const char * filename=0);

		/*
		 * Prints summaries of all files */
		void PrintResults();

		/*
		 * Calculates the p-value for all models. */
		void CalculatePValue(bool flag_histogram=false);

		/* @} */

	private:

		/*
		 * Copies this BCModelManager into another one */
		void Copy(BCModelManager & modelmanager) const;

		/**
		 * The BCModelContainer containing all BCModels. */
		BCModelContainer * fModelContainer;

		/**
		 * The data set common to all models. */
		BCDataSet * fDataSet;

};

// ---------------------------------------------------------

#endif
