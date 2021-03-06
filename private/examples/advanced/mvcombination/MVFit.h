#ifndef __BAT__MVFIT__H
#define __BAT__MVFIT__H

#include "MVCombination.h"

// ---------------------------------------------------------
class MVFit : public MVCombination
{
 public:

  // Constructor
  MVFit();

  // Destructor
  ~MVFit();
	
  // Add a parameter
  // name: the name of the parameter
  // min:  the minimum value of the parameter
  // max:  the maximum value of the parameter
  void AddObservable(std::string name, double min, double max);
	
	// return a value for an observable
	// index: the index of the variable
	// parameters: the physics parameters
  virtual double CalculateObservable(int index, const std::vector<double> &parameters)
	{ return 0; }; 

  // BAT methods

	// the log of the likelihood
  double LogLikelihood(const std::vector<double> &parameters);

 private:

};
// ---------------------------------------------------------

#endif

