/*!
 * \class BCDataSet
 * \brief A class representing a set of data points. 
 * \author Daniel Kollar
 * \author Kevin Kr&ouml;ninger
 * \version 1.0
 * \date 08.2008
 * \detail This class represents a data set containing a set of data
 * points. The data points are organized in a vector. The class
 * provides functions to read in data from a file.
 *
 * Copyright (C) 2008, Daniel Kollar and Kevin Kroeninger. 
 * All rights reserved. 
 * 
 * For the licensing terms see doc/LICENSE. 
 */ 

// ---------------------------------------------------------

#ifndef __BCDATASET__H
#define __BCDATASET__H

#include <vector>

#include "BCDataPoint.h" 

// --------------------------------------------------------- 

typedef std::vector<BCDataPoint*> BCDataVector; 

// --------------------------------------------------------- 

class BCDataSet
{

 public: 

	/** \name Constructors and destructors */ 
	/* @{ */ 

	/* 
	 * Default constructor 
	 */ 
	BCDataSet(); 

	/* 
	 * Default destructor 
	 */ 
	virtual ~BCDataSet(); 

	/* @} */ 

	/** \name Member functions (get) */ 
	/* @{ */ 

	/* 
	 * @param The vector of data points. 
	 */ 
	BCDataVector * GetDataVector(); 

	/* 
	 * @return The number of data points. 
	 */
	int GetNDataPoints(); 

	/* 
	 * @param index The index of the data point to be returned. 
	 * @return The data point at the index. 
	 */ 
	BCDataPoint* GetDataPoint(int index); 

	/* @} */ 

	/** \name Member functions (miscellaneous methods) */ 
	/* @{ */

	/**
	 * Reads data from a file. For a description see the following
	 * member functions.
	 */ 
	int ReadDataFromFile(const char * filename, const char * treename, const char * branchnames)
	{ return this ->  ReadDataFromFileTree(filename, treename, branchnames); }; 
	int ReadDataFromFile(const char * filename, int nvariables)
	{ return this -> ReadDataFromFileTxt(filename, nvariables); }; 
	int ReadDataFromFile(const char * filename, std::vector<int> options_int, std::vector<double> options_double, const char * options_char)
	{ return this -> ReadDataFromFileUser(filename, options_int, options_double, options_char); }; 

	/** 
	 * Reads a TTree from a .root file.  Opens a .root file and
	 * gets a TTree. It creates data points containing the values
	 * read from the file.
	 * @param filename The name of the .root file. 
	 * @param treename The name of the TTree. 	 
	 * @param branchnames A list of the names of the branches
	 * separated by a comma	 
	 * @return An error code.  
	 * @see ReadDataFromFileTxt(char* filename, int nbranches); 
	 * @see ReadDataFromFileUser(const char * filename, std::vector<int> options_int, std::vector<double> options_double, const char * options_char);
	 */ 
	int ReadDataFromFileTree(const char * filename, const char * treename, const char * branchnames); 

	/** 
	 * Reads data from a .txt file.  Opens a .txt file and creates
	 * data objects containing the values read from the file.
	 * @param filename The name of the .txt file. 
	 * @param nvariables The number of variables. 
	 * @see ReadDataFromFileTree(char* filename, char* treename, std::vector<char*> branchnames)
	 * @see ReadDataFromFileUser(const char * filename, std::vector<int> options_int, std::vector<double> options_double, const char * options_char);
	 */
	int ReadDataFromFileTxt(const char * filename, int nvariables);

	/** 
	 * Reads data from a user specified file.  Opens a user
	 * specified file and creates data objects containing the
	 * values read from the file. This method needs to be
	 * overloaded.
	 * @param filename The name of the file 
	 * @param options_int A vector of options of type int 
	 * @param options_double A vector of options of type double 
	 * @param options_char A pointer of characters 
	 * @see ReadDataFromFileTree(char* filename, char* treename, std::vector<char*> branchnames)
	 * @see ReadDataFromFileTxt(char* filename, int nbranches); 
	 */   
	virtual int ReadDataFromFileUser(const char * filename, std::vector<int> options_int, std::vector<double> options_double, const char * options_char);

	/**
	 * Adds a data point to the data set. 
	 * @param datapoint The data point to be added 
	 */ 
	void AddDataPoint(BCDataPoint* datapoint); 

	/**
	 * Resets the content of the data set 
	 */ 
	void Reset(); 

	/* @} */ 

 private: 

	/* 
	 * A vector containing the data points 
	 */ 
	BCDataVector* fBCDataVector; 

}; 

// --------------------------------------------------------- 

#endif 

