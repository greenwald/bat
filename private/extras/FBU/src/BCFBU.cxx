// ***************************************************************
// This file was created using the CreateProject.sh script.
// CreateProject.sh is part of Bayesian Analysis Toolkit (BAT).
// BAT can be downloaded from http://mpp.mpg.de/bat
// ***************************************************************

#include "BCFBU.h"
#include "BCFBUNormSystematic.h"

#include <BAT/BCMath.h>
#include <BAT/BCLog.h>

#include <TFile.h>
#include <TVector.h>
#include <TMatrix.h>
#include <TCanvas.h>
#include <TH1D.h>
#include <TH2D.h>

#include <iostream>
#include <sstream>
#include <cstdlib>
#include <map>
#include <string>


// ---------------------------------------------------------
BCFBU::BCFBU()
	: BCModel("BFU")
	, fHistTruth(0)
	, fNBinsTruth(0)
	, fNBinsTruth1(0)
	, fNBinsTruth2(0)
	, fTruthMin(0)
	, fTruthMax(0)
	, fNBinsReco(0)
	, fNBinsReco1(0)
	, fNBinsReco2(0)
	, fResponseMatrix(0)
	, fMigrationMatrix(0)
	, fHistEfficiency(0)
	, fNDim(-1)
	, fBackgroundProcesses(0)
	, fNBackgroundProcesses(0)
	, fNSyst(0)
	, fNSamples(0)
	, fSystNames(0)
	, fSampleNames(0)
	, fNominalHisto(0)
	, fNNormSyst(0)
	, fInterpolationType(1)
{
};

// ---------------------------------------------------------
BCFBU::BCFBU(const char * name)
	: BCModel(name)
	, fHistTruth(0)
	, fNBinsTruth(0)
	, fNBinsTruth1(0)
	, fNBinsTruth2(0)
	, fTruthMin(0)
	, fTruthMax(0)
	, fNBinsReco(0)
	, fNBinsReco1(0)
	, fNBinsReco2(0)
	, fResponseMatrix(0)
	, fMigrationMatrix(0)
	, fHistEfficiency(0)
	, fNDim(-1)
	, fBackgroundProcesses(0)
	, fNSyst(0)
	, fNSamples(0)
	, fSystNames(0)
	, fSampleNames(0)
	, fNominalHisto(0)
	, fNNormSyst(0)
	, fInterpolationType(1)
{
};

// ---------------------------------------------------------
BCFBU::~BCFBU()
{
	if (fHistTruth)
		delete fHistTruth;

	if (fHistEfficiency)
		delete fHistEfficiency;

	// debugKK
	// delete background processes
};

// ---------------------------------------------------------
int BCFBU::RebinHistograms(int rebin, TH2* h_migration, TH1 *h_truth, TH1 *h_background)
{
	// check if migration matrix histogram exists
  if (!h_migration) {
		BCLog::OutWarning("BCFBU::RebinHistograms: migration matrix histogram not found. Exit.");
		exit(1);
	}

	// check if truth histogram exists
  if (!h_truth){
		BCLog::OutWarning("BCFBU::RebinHistograms: truth histogram not found. Exit.");
		exit(1);
	}

	// do not check if background histogram exists since it might not be
	// necessary to have
  if (rebin!=1) {
		h_migration->Rebin2D(rebin,rebin);
		h_truth->Rebin(rebin);
		if (h_background)
			h_background->Rebin(rebin);
	}

	// no error
	return 1;
}

// ---------------------------------------------------------
int BCFBU::PrepareResponseMatrix(TH2* h_migration, TH1* h_truth, TH1* h_background, std::vector<double> parmin, std::vector<double> parmax)
{

  // check if migration matrix histogram exists
  if (!h_migration) {
		BCLog::OutWarning("BCFBU::PrepareResponseMatrix: migration matrix histogram not found.");
		return 0;
	}

  // check if truth histogram exists
  if (!h_truth){
		BCLog::OutWarning("BCFBU::PrepareResponseMatrix: truth histogram not found.");
		return 0;
	}

	// check dimension
	if( (fNDim > 0) && (fNDim != h_truth->GetDimension()) ) {
		BCLog::OutWarning("BCBFU::PrepareResponseMatrix. Dimensions of truth histogram does not match previously defined dimension. Truth histogram rejected.");
		return 0;
	}
	else if (fNDim <= 0) {
		// set dimension
		fNDim = h_truth->GetDimension();
	}

	// check number of reco bins
	if ( (fNBinsReco > 0) && (fNBinsReco != h_migration->GetYaxis()->GetNbins()) ) {
		BCLog::OutWarning("BCBFU::SetDataHistogram. Number of reco bins doens't match number of bins in the migration matrix. Migration matrix rejected.");
		return 0;
		}

	// clone truth distribution
	if (fNDim==1) {
		fHistTruth = (TH1D*) h_truth->Clone();
		//		fHistTruth->Scale(1./fHistTruth->Integral());
		int nbins = fHistTruth->GetNbinsX();
		fRecoMin = fHistTruth->GetXaxis()->GetBinLowEdge(1);
		fRecoMax = fHistTruth->GetXaxis()->GetBinUpEdge(nbins);
	}
	else if (fNDim==2)
	  {
		fHistTruth = (TH2D*) h_truth->Clone();

		 std::cout << "dimensions of h_migration x = " << h_migration->GetXaxis()->GetNbins() << " y = " << h_migration->GetYaxis()->GetNbins() << " response(3,3) " << h_migration->GetBinContent(3,3) << std::endl;

		 std::cout << "dimensions of h_migration, after rebinning x = " << h_migration->GetXaxis()->GetNbins() << " y = " << h_migration->GetYaxis()->GetNbins() << " reponse(3,3) " << h_migration->GetBinContent(3,3) << std::endl;

		 fNBinsTruth1 = fHistTruth->GetXaxis()->GetNbins();
		 fNBinsTruth2 = fHistTruth->GetYaxis()->GetNbins();

		 std::cout << "dimensions of h_truth " << h_truth->GetXaxis()->GetNbins() << " " <<  h_truth->GetYaxis()->GetNbins() << std::endl;

		 fNBinsReco1 = h_background->GetXaxis()->GetNbins();
		 fNBinsReco2 = h_background->GetYaxis()->GetNbins();

		 fMatrixTruth.clear();
		 fMatrixReco.clear();


		 std::vector<double> tempTruth;
		 tempTruth.assign(fNBinsTruth2,0);
		 fMatrixTruth.assign(fNBinsTruth1,tempTruth);

		 std::vector<double> tempReco;
		 tempReco.assign(fNBinsReco2,0);
		 fMatrixReco.assign(fNBinsReco1,tempReco);

	  }

	// get number of bins
	fNBinsTruth = h_migration->GetXaxis()->GetNbins();
	fNBinsReco  = h_migration->GetYaxis()->GetNbins();

	// prepare helper varible for log likelihood calculation
	fVectorTruth.clear();
	fVectorTruth.assign(fNBinsTruth, 0);

	fVectorReco.clear();
	fVectorReco.assign(fNBinsReco, 0);




	// print statements
  BCLog::OutDetail(Form("Dimension of problem : %i", fNDim));
  BCLog::OutDetail(Form("Number of truth bins : %i", fNBinsTruth));
  BCLog::OutDetail(Form("Number of reco bins  : %i", fNBinsReco));

	// bring response matrix into matrix form (TMatrixD)
  fResponseMatrix = new TMatrixD(fNBinsTruth, fNBinsReco);

	// calculate migration matrix
  fMigrationMatrix = new TMatrixD(fNBinsTruth, fNBinsReco);

	// bring unnormalized migration matrix into matrix form (TMatrixD)
	// and switch axes
	// debugKK: remove this once 2D case is settled
  TMatrix m = TMatrixD(fNBinsTruth, fNBinsReco);

  for (int i_tru = 0; i_tru < fNBinsTruth; ++i_tru)
    for (int i_rec = 0; i_rec < fNBinsReco; ++i_rec)
			m(i_tru,i_rec) = h_migration->GetBinContent(i_tru+1, i_rec+1);

	// normalization factor of h_migration
	//	double norm = 0;

	// define migration and response matrices
  for (int i_tru=0;i_tru<fNBinsTruth;++i_tru) {

    // define migration matrix
    for (int i_rec=0;i_rec<fNBinsReco;++i_rec)
      (*fMigrationMatrix)(i_tru,i_rec) = h_migration->GetBinContent(i_tru+1, i_rec+1);


  }

  // calculate efficiency for 1D case
  if (fNDim==1) {

    // define response matrix
    for (int i_tru=0;i_tru<fNBinsTruth;++i_tru)
       for (int i_rec=0;i_rec<fNBinsReco;++i_rec)
	 (*fResponseMatrix)(i_tru,i_rec) = (*fMigrationMatrix)(i_tru, i_rec)/fHistTruth->GetBinContent(i_tru+1);

                // create new efficiency histogram based on the same binning as
		// the truth distribution
                fHistEfficiency = (TH1D*) fHistTruth->Clone();
		fHistEfficiency->GetYaxis()->SetTitle("efficiency");

		// loop over all truth bins
		for (int i_tru=0;i_tru<fNBinsTruth;++i_tru) {
			double sum = 0;

			// sum all reco bins
			for (int i_rec=0;i_rec<fNBinsReco;++i_rec)
   			       sum += (*fMigrationMatrix)(i_tru, i_rec);

			// calculate efficiency
			double eff = sum/fHistTruth->GetBinContent(i_tru+1);  // correct if truth histogram not normalised

			std::cout << " eff " << eff << " truth " << fHistTruth->GetBinContent(i_tru+1) << std::endl;

			// set efficiency
			fHistEfficiency->SetBinContent(i_tru+1, eff);

			// check parameter ranges

			double mini = 0.;
			double maxi = 20e4;

			if ( (parmin.size()) > 0 && (parmin.size() == parmax.size()) ) {
				mini = parmin[i_tru];
				maxi = parmax[i_tru];
			}
			else {
			  //			  maxi = fHistTruth->GetBinContent(i_tru+1)*5.0;
			  //			  mini = fHistTruth->GetBinContent(i_tru+1)/5.0;
			  mini = 0.;
			  maxi = 20e4;
			}

			// add parameter
			AddParameter(("T"+IntToString(i_tru+1)).c_str(), mini, maxi);
		}
	}

  // debugKK: need to work on 2D part
  if (fNDim==2)
    {

      // to obtain response matrix, normalise



      for (int i_tru = 0; i_tru < fNBinsTruth; ++i_tru)
	{
	  double totalrec = 0;

	  for (int i_rec=0;i_rec<fNBinsReco;i_rec++)
	    {
	      totalrec += (*fMigrationMatrix)(i_tru,i_rec);
	    }


	  for (int i_rec=0;i_rec<fNBinsReco;++i_rec)
	    (*fResponseMatrix)(i_tru,i_rec) = (*fMigrationMatrix)(i_tru, i_rec)/totalrec;

	  // this assumes fMigrationMatrix is NOT normalised; if it's normalised, can possibly use truth histo instead

	    //  (*fResponseMatrix)(i_tru,i_rec) = (*fMigrationMatrix)(i_tru, i_rec)/fHistTruth->GetBinContent(i_tru+1);
	}

      fHistEfficiency = (TH2D*) fHistTruth->Clone();



      std::cout << " truth bins X " << fNBinsTruth1 << " Y " << fNBinsTruth2 << std::endl;
      std::cout << " reco bins X " << fNBinsReco1 << " Y " << fNBinsReco2 << std::endl;

      for (int i_tru1=0;i_tru1<fNBinsTruth1;i_tru1++)
	for (int i_tru2=0;i_tru2<fNBinsTruth2;i_tru2++)
	  {
						double sum = 0;

						for (int i_rec1=0;i_rec1<fNBinsReco1;i_rec1++)
							for (int i_rec2=0;i_rec2<fNBinsReco2;i_rec2++)
								{
									std::cout << "index x y " << i_tru1 << " " << i_tru2 << " " << i_rec1 << " " << i_rec2 << " " << Get2DIndex(i_tru1,i_tru2,i_rec1, i_rec2).first << " " << 	      Get2DIndex(i_tru1,i_tru2,i_rec1, i_rec2).second << std::endl;

									sum += m(Get2DIndex(i_tru1,i_tru2,i_rec1, i_rec2).first,
													 Get2DIndex(i_tru1,i_tru2,i_rec1, i_rec2).second);

								}

						fHistEfficiency->SetBinContent(i_tru1+1,i_tru2+1, sum/fHistTruth->GetBinContent(i_tru1+1,i_tru2+1));

						std::cout << "total truth " << fHistTruth->GetBinContent(i_tru1+1,i_tru2+1) << " efficiency bin " << i_tru1 << " " << i_tru2 << " " << fHistEfficiency->GetBinContent(i_tru1+1,i_tru2+1) << std::endl;

						// add parameter

						AddParameter(("X"+IntToString(i_tru1+1)+"Y"+IntToString(i_tru2+1)).c_str(), 0, 20e4);

					}


      for (int i_tru1=0;i_tru1<fNBinsTruth1;i_tru1++)
				for (int i_tru2=0;i_tru2<fNBinsTruth2;i_tru2++)
					{
						std::cout << "truth bin " << i_tru1 << " " << i_tru2 << " " << fHistTruth->GetBinContent(i_tru1+1,i_tru2+1) << std::endl;
					}

    }

  // add one parameter per source of systematic normalization - common to 1D and 2D

  std::cout << " number of normalisation systematics " << fNormSystematicContainer.size() << " current nparameters " << GetNParameters() << std::endl;


  for (unsigned int i=0;i<fNormSystematicContainer.size();++i) {
    fNormSystParIndexContainer[fNormSystematicContainer[i].GetName()] = GetNParameters();
    AddParameter(fNormSystematicContainer[i].GetName().c_str(), -5, 5);
  }

  for (unsigned int i=0;i<fSystematicContainer.size();++i) {
    fSystParIndexContainer[fSystematicContainer[i].GetName()] = GetNParameters();
    AddParameter(fSystematicContainer[i].GetName().c_str(), -5, 5);
  }

  //----

  // no error
  return 1;
}

// ---------------------------------------------------------
/*
void BCFBU::DefineParameters(int mode, double min, double max) // (default) mode = 0: take (0.1*truth, 10*truth); mode = 1: take (min,max) - default range is (0,1.0e6)
{
        // add parameters for 1D case
  if (fNDim==1) {
                // add one parameter per truth bin
                for (int i=1; i<=fNBinsTruth; ++i) {
                        // debugKK: how to set the upper range?
                  //                    AddParameter(("T"+IntToString(i)).c_str(), 0, 20e4);

                  if (mode==0)
                    AddParameter(("T"+IntToString(i)).c_str(), fHistTruth->GetBinContent(i)/0.5, fHistTruth->GetBinContent(i)*1.5);
                  else if (mode==1)
                    AddParameter(("T"+IntToString(i)).c_str(), min, max);

                  BCLog::OutDetail(Form("Upper limit of param %i is %f", i, fHistTruth->GetBinContent(i)+100));
                }

                // add one parameter per source of systematic shape uncertainty
                for (int i=0; i<fNSyst; ++i) {
                        AddParameter(fSystNames[i].c_str(), -5, 5);
                        // debugKK: what is this?
                        fSystParamMap[fSystNames[i]] = GetNBinsTruth() + i;
                }


        }


	// debugKK: did not check the 2D case yet




  std::cout << " fNDim " << fNDim << " nbinsx " << GetNBinsTruthX() << std::endl;
  exit(1);

  if (fNDim==2)
    {
      for (int i=1;i<=GetNBinsTruthX();++i) // parameters corresponding to bins of sought after truth histogram
				for (int j=1;j<=GetNBinsTruthY();++j)
					{
						AddParameter(("X"+IntToString(i)+"Y"+IntToString(j)).c_str(), 0, 20e4);
					}


      for (int i=0;i<fNSyst;++i)  // shape systematics
				{
					AddParameter(fSystNames[i].c_str(), -5, 5);
					fSystParamMap[fSystNames[i]] = GetNBinsTruthX()*GetNBinsTruthY() + i;
				}

				}
     }

*/

// ---------------------------------------------------------
double BCFBU::LinearInterpolate(double alpha, double nominal, double up, double down)
{
  double res = 0;

  if (alpha>=0)
    res = alpha*(up-nominal);
  else
    res = alpha*(nominal-down);

  return res;
}

// ---------------------------------------------------------
double BCFBU::ExponentialInterpolate(double alpha, double nominal, double up, double down)
{
  double res = 0;

  if (alpha>=0)
    res = exp(alpha*log(up/nominal));
  else
    res = exp(-alpha*log(down/nominal));

  return res;
}

// ---------------------------------------------------------
double BCFBU::NoInterpolation(double alpha, double nominal, double up, double down)
{
  double res = 0;

  if (alpha>=1)
    res = up;
  else if (alpha<=-1)
    res = down;
  else
    res = nominal;

  return res;
}

// ---------------------------------------------------------
double BCFBU::LogLikelihood(const std::vector<double> & parameters)
{
  // This methods returns the logarithm of the conditional probability
  // p(data|parameters). This is where you have to define your model.

  double logprob = 0.;

  if (fNDim==2)
    {
      for (int i=0;i<fNBinsTruth1;++i)
	for (int j=0;j<fNBinsTruth2;++j)
	    fMatrixTruth[i][j] = parameters.at(Get1DIndex(i,j));




      for (int i_rec1=0;i_rec1<fNBinsReco1;i_rec1++) // loop over all reco bins
	for (int i_rec2=0;i_rec2<fNBinsReco2;i_rec2++)
	  {
	    double expectation = 0;

	    fMatrixReco[i_rec1][i_rec2] = 0;

	    for (int i_tru1=0;i_tru1<fNBinsTruth1;i_tru1++) // loop over truth bins
	      for (int i_tru2=0;i_tru2<fNBinsTruth2;i_tru2++)
		{

		  double nominalresp = (*fResponseMatrix)(Get2DIndex(i_tru1,i_tru2,i_rec1,i_rec2).first, Get2DIndex
							  (i_tru1,i_tru2,i_rec1,i_rec2).second ); // find appropriate entry of response matrix

		  double varresp = nominalresp;


		  // loop through the systematics, compute shifts in response matrix, add them to nominal response

		  for(unsigned int syst_no=0; syst_no<fSystematicContainer.size(); syst_no++)
		    {
		      //	std::cout << "Now doing systematic with name " << (*it_map2).first << std::endl;

		      std::string curr_syst = fSystematicContainer[syst_no].GetName();

		      int syst_param_num = fSystParIndexContainer[curr_syst];

		      BCFBUBkgSystematic Systematic = fSystematicContainer[syst_no];

		      double responseup = Systematic.GetResponseUp()->GetBinContent(Get2DIndex(i_tru1,i_tru2,i_rec1,i_rec2).first+1, Get2DIndex
										    (i_tru1,i_tru2,i_rec1,i_rec2).second+1); // find shifted up response for current syst


		      double responsedown = Systematic.GetResponseDown()->GetBinContent(Get2DIndex(i_tru1,i_tru2,i_rec1,i_rec2).first+1, Get2DIndex
											(i_tru1,i_tru2,i_rec1,i_rec2).second+1); // find shifted down response for current syst

		      //  std::cout << "nominal response " << nominalresp << " up " << responseup << " down " << responsedown << std::endl;

		      // find interpolated response matrix

		      if (fInterpolationType==0)
			varresp += LinearInterpolate(parameters.at(syst_param_num),nominalresp,responseup,responsedown);
		      else if (fInterpolationType==1)
			varresp *= ExponentialInterpolate(parameters.at(syst_param_num),nominalresp,responseup,responsedown);
		      else if (fInterpolationType==2)
			varresp = NoInterpolation(parameters.at(syst_param_num),nominalresp,responseup,responsedown);
		      else
			{
			  std::cout << "Unknown extrapolation type, exiting" << std::endl;
			  exit(1);
			}

		    }

		  // std::cout << " current sum " << sum << " reco bins " << i_rec1 << " " << i_rec2 << " truth bins " << i_tru1 << " " << i_tru2 << " truth val " << t(i_tru1,i_tru2) << " eff " << fHistEfficiency->GetBinContent(i_tru1+1,i_tru2+1) << " response " << varresp << " data " << d(i_rec1,i_rec2) << std::endl;

		  expectation += fMatrixTruth[i_tru1][i_tru2]*fHistEfficiency->GetBinContent(i_tru1+1,i_tru2+1)*varresp; // this is the contribution to reco bin (i_rec1,i_rec2) from truth bin (i_tru1,i_tru2)

		}

	    for (int i_bkg = 0; i_bkg < fNBackgroundProcesses; ++i_bkg) {

	      TH2D* hist_background = (TH2D*) fBackgroundProcesses[i_bkg]->GetHistogram();

	      double nominal = hist_background->GetBinContent(i_rec1+1,i_rec2+1);

	      // std::cout << fBackgroundProcesses[i_bkg]->GetName() << " " << nominal << " bin " << i_rec1 << " " << i_rec2 << std::endl;

	      // check if affected by normalisation systematic

	      double norm = 1.0;

	      std::string curr_sample = fBackgroundProcesses[i_bkg]->GetName();

	      for (unsigned int i_normsyst=0;i_normsyst<fNormSystematicContainer.size();i_normsyst++)
		  if (fNormSystematicContainer[i_normsyst].ApplySystematic(curr_sample))
		    {

		      int paramnumber = fNormSystParIndexContainer[fNormSystematicContainer[i_normsyst].GetName()];

		      //  std::cout << " normsyst for sample " << curr_sample << " parameter number " << paramnumber << " norm value " << 1 + parameters.at(paramnumber) << std::endl;

		      norm *= 1 + parameters.at(paramnumber);
		    }


	      //  std::cout << curr_sample << " nominal " << nominal << " nominal*norm " << nominal*norm << std::endl;

	      expectation += norm * nominal;


	      for (unsigned int i_syst=0;i_syst<fSystematicContainer.size();i_syst++)
		{
		  if (fSystematicContainer[i_syst].ApplySystematic(curr_sample))
		    {
		      double up = fSystematicContainer[i_syst].GetHistoUp(curr_sample)->GetBinContent(i_rec1+1,i_rec2+1);
		      double down = fSystematicContainer[i_syst].GetHistoDown(curr_sample)->GetBinContent(i_rec1+1,i_rec2+1);

		      int syst_param_num = fSystParIndexContainer[fSystematicContainer[i_syst].GetName()];

		      // std::cout << "Now doing systematic with name " << fSystematicContainer[i_syst].GetName() << " for sample " << curr_sample << " param number " << syst_param_num << " param value " << parameters.at(syst_param_num) << " nominal " << nominal << " up " << up << " down " << down << std::endl;

		      if (fInterpolationType==0)
			 expectation += norm * LinearInterpolate(parameters.at(syst_param_num),nominal,up,down);
		      else if (fInterpolationType==1)
			{
			  expectation *= ExponentialInterpolate(parameters.at(syst_param_num),nominal,up,down);
			}
		      else if (fInterpolationType==2)
			{
			  expectation += NoInterpolation(parameters.at(syst_param_num),nominal,up,down);
			}
		    }
		}


	    }

	    // add to expectation

	    fMatrixReco[i_rec1][i_rec2] = expectation;

	  }

      // calculate product of poisson probabilities
      for (int i=0;i<fNBinsReco1;++i)
	for (int j=0;j<fNBinsReco2;++j)
	  {
	    double r = fMatrixReco[i][j];

	    // check if expectation value is greater than 0
	    if (r>=0) {
	      double d = fHistData->GetBinContent(i+1,j+1);
	      logprob += BCMath::LogPoisson(d, r);
	    }
	    else
	      logprob = -1e50;
	  }
    }


  if (fNDim==1) {

    // copy truth parameters into a vector for CPU reasons
    for (int i=0;i<fNBinsTruth;++i)
      fVectorTruth[i] = parameters[i];

    // loop over all reco bins
    for (int i_rec=0; i_rec < fNBinsReco; ++i_rec) {
      // define expectation value
      double expectation = 0;

      // set reco vector to 0
      fVectorReco[i_rec] = 0;

      // loop over all truth bins
      for (int i_tru=0; i_tru < fNBinsTruth; ++i_tru) {
	// get migration matrix entry
	double nominalresp = (*fResponseMatrix)(i_tru, i_rec);

	// copy into another variable
	double varresp = nominalresp;

	// loop through the systematics, compute shifts in response matrix, add them to nominal response

	for(unsigned int syst_no=0; syst_no<fSystematicContainer.size(); syst_no++)
	  {
	    //	std::cout << "Now doing systematic with name " << (*it_map2).first << std::endl;

	    std::string curr_syst = fSystematicContainer[syst_no].GetName();

	    int syst_param_num = fSystParIndexContainer[curr_syst];

	    BCFBUBkgSystematic Systematic = fSystematicContainer[syst_no];

	    double responseup = Systematic.GetResponseUp()->GetBinContent(i_tru+1, i_rec+1); // find shifted up response for current syst

	    double responsedown = Systematic.GetResponseDown()->GetBinContent(i_tru+1, i_rec+1); // find shifted down response for current syst

	    // find interpolated response matrix
	    if (fInterpolationType==0)
	      varresp += LinearInterpolate(parameters.at(syst_param_num),nominalresp,responseup,responsedown);
	    else if (fInterpolationType==1)
	      varresp *= ExponentialInterpolate(parameters.at(syst_param_num),nominalresp,responseup,responsedown);
	    else if (fInterpolationType==2)
	      varresp = NoInterpolation(parameters.at(syst_param_num),nominalresp,responseup,responsedown);
	    else {
	      std::cout << "Unknown extrapolation type, exiting" << std::endl;
	      exit(1);
	    }
	  }

	// calculate expectation value
	expectation += fVectorTruth[i_tru] * varresp;
      }

      // loop over all background samples
      for (int i_bkg = 0; i_bkg < fNBackgroundProcesses; ++i_bkg) {
	TH1D* hist_background = (TH1D*) fBackgroundProcesses[i_bkg]->GetHistogram();

	double nominal = hist_background->GetBinContent(i_rec+1);

	//	fVectorReco[i_rec] += nominal;

	//  std::cout << " bin " << i_rec << " background " << fVectorReco[i_rec] << std::endl;

	 // check if affected by normalisation systematic

	double norm = 1.0;

	std::string curr_sample = fBackgroundProcesses[i_bkg]->GetName();

	for (unsigned int i_normsyst=0;i_normsyst<fNormSystematicContainer.size();i_normsyst++)
	  if (fNormSystematicContainer[i_normsyst].ApplySystematic(curr_sample))
	    {

	      int paramnumber = fNormSystParIndexContainer[fNormSystematicContainer[i_normsyst].GetName()];

	      //							      std::cout << " normsyst for sample " << curr_sample << " parameter number " << paramnumber << " norm value " << 1 + parameters.at(paramnumber) << std::endl;

	      norm *= 1 + parameters.at(paramnumber);
	    }

	//	r(i_rec1,i_rec2) += norm * nominal;

	expectation += norm * nominal;


	for (unsigned int i_syst=0;i_syst<fSystematicContainer.size();i_syst++)
	  {
	    if (fSystematicContainer[i_syst].ApplySystematic(curr_sample))
	      {
		double up = fSystematicContainer[i_syst].GetHistoUp(curr_sample)->GetBinContent(i_rec+1);
		double down = fSystematicContainer[i_syst].GetHistoDown(curr_sample)->GetBinContent(i_rec+1);

		int syst_param_num = fSystParIndexContainer[fSystematicContainer[i_syst].GetName()];

		// std::cout << "Now doing systematic with name " << fSystematicContainer[i_syst].GetName() << " for sample " << curr_sample << " param number " << syst_param_num << " param value " << parameters.at(syst_param_num) << " nominal " << nominal << " up " << up << " down " << down << std::endl;

		if (fInterpolationType==0)
		  expectation += norm * LinearInterpolate(parameters.at(syst_param_num),nominal,up,down);
		else if (fInterpolationType==1)
		  expectation *= ExponentialInterpolate(parameters.at(syst_param_num),nominal,up,down);
		else if (fInterpolationType==2)
		  expectation += NoInterpolation(parameters.at(syst_param_num),nominal,up,down);
	      }
	  }
      }

      // add to expectation
      fVectorReco[i_rec] = expectation;
    }


    // calculate product of poisson probabilities
    for (int i=0;i<fNBinsReco;++i) {
      double r = fVectorReco[i];

      // check if expectation value is greater than 0
      if (r>=0) {
	double d = fHistData->GetBinContent(i+1);
	logprob += BCMath::LogPoisson(d, r);
      }
      else
	logprob = -1e50;
    }
  }

  // return log likelihood
  return logprob;
}

// ---------------------------------------------------------
double BCFBU::LogAPrioriProbability(const std::vector <double> & parameters)
{
  // This method returns the logarithm of the prior probability for the
  // parameters p(parameters).

  double logprob = 0.;

  if (fNDim==2)
    {
      for (int i = 0; i < GetNBinsTruthX()*GetNBinsTruthY(); ++i)
	logprob -= log(GetParameter(i)->GetRangeWidth());

      // gaussian profile for systematics

      for (unsigned int i=0;i<fNormSystematicContainer.size();i++)
	{
	  int paramind = fNormSystParIndexContainer[fNormSystematicContainer[i].GetName()];

	  if ((1+parameters.at(paramind))>0)
	    logprob += BCMath::LogGaus(parameters.at(paramind), 0., fNormSystematicContainer[i].GetUncertainty());
	  else
	    logprob = -9999.0;
	}

      for (unsigned int i=0;i<fSystematicContainer.size();i++)
	{
	  int paramind = fSystParIndexContainer[fSystematicContainer[i].GetName()];

	  logprob += BCMath::LogGaus(parameters.at(paramind), 0., 1.0);

	}


    }


  if (fNDim==1)
    {
      // debugKK
      // does the prior really be normalized?
      //			for (int i = 0; i < GetNBinsTruth(); ++i)
      //				logprob -= log(GetParameter(i)->GetRangeWidth());

      // gaussian priors for systematics
      for (int i = GetNBinsTruth(); i < GetNBinsTruth() + fNSyst; ++i)
	logprob += BCMath::LogGaus(parameters.at(i), 0., 1.0);

      /*

      for (unsigned int i = GetNBinsTruth() + fNSyst; i < GetNParameters(); ++i)
      {
      if ((1+parameters.at(i))>0)
      logprob += BCMath::LogGaus(parameters.at(i), 0., fNormSystSizes[i - (GetNBinsTruth() + fNSyst)]);
      else
      logprob = -9999.0;
      }
      */

    }


  return logprob;
}
// ---------------------------------------------------------
void BCFBU::MCMCIterationInterface()
{
  // get number of chains
	//  int nchains = MCMCGetNChains();

  // get number of parameters
	//  int npar = GetNParameters();

  // loop over all chains and fill histogram

}

// ---------------------------------------------------------
void BCFBU::PrintHistogram()
{
  // print the BAT histogram to an eps file

}

// ---------------------------------------------------------
Double_t BCFBU::GetCurvature(const TVectorD& vec, const TMatrixD& curv)
{
	// Compute curvature of vector
	return vec*(curv*vec);
}

// ---------------------------------------------------------
void BCFBU::FillCurvatureMatrix( TMatrixD& tCurv, TMatrixD& tC, int fDdim )
{
	Double_t eps = 0.00001;

	Int_t ndim = tCurv.GetNrows();

	// Init
	tCurv *= 0;
	tC    *= 0;

	if (fDdim == 0)
		for (Int_t i=0; i<ndim; ++i) tC(i,i) = 1;
	else if (fDdim == 1) {
		for (Int_t i=0; i<ndim; ++i) {
			if (i < ndim-1) tC(i,i+1) = 1.0;
			tC(i,i) = 1.0;
		}
	}
	else if (fDdim == 2) {
		for (Int_t i=0; i<ndim; ++i) {
			if (i > 0)      tC(i,i-1) = 1.0;
			if (i < ndim-1) tC(i,i+1) = 1.0;
			tC(i,i) = -2.0;
		}
		tC(0,0) = -1.0;
		tC(ndim-1,ndim-1) = -1.0;
	}

	// Add epsilon to avoid singularities
	for (Int_t i=0; i<ndim; ++i) tC(i,i) = tC(i,i) + eps;

	//Get curvature matrix
	for (Int_t i=0; i<ndim; ++i) {
		for (Int_t j=0; j<ndim; ++j) {
			for (Int_t k=0; k<ndim; k++) {
				tCurv(i,j) = tCurv(i,j) + tC(k,i)*tC(k,j);
			}
		}
	}


}

// ---------------------------------------------------------
void BCFBU::AddBackgroundProcess(std::string backgroundname, TH1 *h_background, int nevents)
{
	// create backgroudn object
        BCFBUBackground* background = new BCFBUBackground(backgroundname);

	// clone histogram
	TH1D* hist_background = (TH1D*) h_background->Clone();

	// get normalization
	double norm = hist_background->Integral();

	// normalize histogram
	if (nevents > 0)
	  hist_background->Scale(double(nevents)/norm);

	// set histogram
	background->SetHistogram(hist_background);

	// add background object to container
	fBackgroundProcesses.push_back(background);

	// increase number of background processes
	fNBackgroundProcesses++;

	// debugKK: can be removed later on
	fNSamples++;
	fSampleNames.push_back(backgroundname);
	fNominalHisto.push_back(h_background);
}

// ---------------------------------------------------------
void BCFBU::DefineSystematic(std::string samplename, std::string systName, TH1 *up, TH1 *down)
{

  // has this systematic appeared before?

  bool found = false;

  bool ind = -1;

  for (unsigned int i=0;i<fSystematicContainer.size();++i)
    if (fSystematicContainer[i].GetName()==systName)
      {
	found = true;
	ind = i;
      }

  // if not, add it to the list of systematics

  if (!found)
    {
      fNSyst++;
      BCFBUBkgSystematic syst = BCFBUBkgSystematic(systName);
      syst.SetHistoUp(samplename, up);
      syst.SetHistoDown(samplename, down);
      syst.AddBackground(samplename);
      fSystematicContainer.push_back(syst);
    }
  else
    {
      fSystematicContainer[ind].AddBackground(samplename); // ought to check whether the systematic has already been defined for this sample and throw an expection if so
      fSystematicContainer[ind].SetHistoUp(samplename,up);
      fSystematicContainer[ind].SetHistoDown(samplename,down);
    }


  /*
  std::cout << " systematic " << parname << " UP: " << std::endl;

  for (int i_rec1=0;i_rec1<fNBinsReco1;i_rec1++)
    for (int i_rec2=0;i_rec2<fNBinsReco2;i_rec2++)
      {
				std::cout << " bin " << i_rec1 << " " << i_rec2 << " " << up->GetBinContent(i_rec1+1,i_rec2+1) << std::endl;
			}

  std::cout << " systematic " << parname << " DOWN: " << std::endl;

  for (int i_rec1=0;i_rec1<fNBinsReco1;i_rec1++)
    for (int i_rec2=0;i_rec2<fNBinsReco2;i_rec2++)
      {
	std::cout << " bin " << i_rec1 << " " << i_rec2 << " " << down->GetBinContent(i_rec1+1,i_rec2+1) << std::endl;
      }
  */

}

// ---------------------------------------------------------
int BCFBU::GetSystParamNumber(std::string systname)
{
  int result = -1;

  for( std::map<std::string, int>::iterator it_map2=fSystParamMap.begin(); it_map2!=fSystParamMap.end(); it_map2++)
    {
      if ( (*it_map2).first == systname)
				result = (*it_map2).second;
    }

  if (result==-1)
    {
      std::cout << " Systematic with name " << systname << " not found - exiting. " << std::endl;
      exit(1);
    }

  return result;
}

// ---------------------------------------------------------
void BCFBU::DefineSystematicResponse(std::string systName, TH2 *responseup, TH2 *responsedown)
{

  int fNBinsTruth = responseup->GetYaxis()->GetNbins();
  int fNBinsReco = responseup->GetXaxis()->GetNbins();

  TH2D *cl_responseup = (TH2D*) responseup->Clone();
  TH2D *cl_responsedown = (TH2D*) responsedown->Clone();

  TH2D *norm_responseup = (TH2D*) responseup->Clone();
  TH2D *norm_responsedown = (TH2D*) responsedown->Clone();

  for (int i_tru=0;i_tru<fNBinsTruth;++i_tru)
    for (int i_rec=0;i_rec<fNBinsReco;++i_rec)
      {
				cl_responseup->SetBinContent(i_tru+1,i_rec+1, responseup->GetBinContent(i_rec+1,i_tru+1));
				cl_responsedown->SetBinContent(i_tru+1,i_rec+1, responsedown->GetBinContent(i_rec+1,i_tru+1)) ;
      }

  for (int i_tru=0;i_tru<fNBinsTruth;++i_tru)
    {
      double sum = 0;

      for (int i_rec=0;i_rec<fNBinsReco;++i_rec)
				sum += cl_responseup->GetBinContent(i_tru+1,i_rec+1);

      for (int i_rec=0;i_rec<fNBinsReco;++i_rec)
				norm_responseup->SetBinContent(i_tru+1,i_rec+1, cl_responseup->GetBinContent(i_tru+1,i_rec+1)/sum);
    }

  for (int i_tru=0;i_tru<fNBinsTruth;++i_tru)
    {
      double sum = 0;

      for (int i_rec=0;i_rec<fNBinsReco;++i_rec)
				sum += cl_responsedown->GetBinContent(i_tru+1,i_rec+1);

      for (int i_rec=0;i_rec<fNBinsReco;++i_rec)
				norm_responsedown->SetBinContent(i_tru+1,i_rec+1, cl_responsedown->GetBinContent(i_tru+1,i_rec+1)/sum);
    }

  std::cout << "Response up " << std::endl;

  Dump(norm_responseup);

  std::cout << "Response down " << std::endl;

  Dump(norm_responsedown);

  // try to find systematic in container


  bool found = false;

  bool ind = -1;

  for (unsigned int i=0;i<fSystematicContainer.size();++i)
    if (fSystematicContainer[i].GetName()==systName)
      {
	found = true;
	ind = i;
      }

  if (!found)
    {
      fNSyst++;
      BCFBUBkgSystematic syst = BCFBUBkgSystematic(systName);
      syst.SetResponseUp(norm_responseup);
      syst.SetResponseDown(norm_responsedown);
      fSystematicContainer.push_back(syst);
    }
  else
    {
      fSystematicContainer[ind].SetResponseUp(norm_responseup);
      fSystematicContainer[ind].SetResponseDown(norm_responsedown);
    }

  //  exit(1);

  //  fSystResponseUpMap[parname]=norm_responseup;
  //  fSystResponseDownMap[parname]=norm_responsedown;
}

// ---------------------------------------------------------
void BCFBU::Dump(TH2D *bla)
{
  for (int i=1; i<bla->GetYaxis()->GetNbins();++i)
    {
      for (int j=1; j<bla->GetXaxis()->GetNbins();++j)
				{
					std::cout << bla->GetBinContent(i,j) << " ";
				}
      std::cout << std::endl;
    }

}


// ---------------------------------------------------------
void BCFBU::DefineNormalisationSystematic(std::string sampleName, std::string systName, double uncertainty)
{
  fNNormSyst++;
  //  fNormSystNames.push_back(parname);
  //  fNormSystSizes.push_back(uncertainty);

  bool found = false;

  bool ind = -1;

  for (unsigned int i=0;i<fNormSystematicContainer.size();++i)
    if (fNormSystematicContainer[i].GetName()==systName)
      {
	found = true;
	ind = i;
      }

  if (!found)
    {
      BCFBUNormSystematic syst(systName,uncertainty);
      syst.AddBackground(sampleName);
      fNormSystematicContainer.push_back(syst);
    }
  else
    {
      fNormSystematicContainer[ind].SetUncertainty(uncertainty);
      fNormSystematicContainer[ind].AddBackground(sampleName);
    }

  std::cout << " number of normalisation systematics " << fNormSystematicContainer.size() << " current nparameters " << GetNParameters() << std::endl;

}

// ---------------------------------------------------------
std::string BCFBU::IntToString(int x)
{
  std::stringstream ss;// create stream
  ss << x;//add number to the stream

  return ss.str();

}

// ---------------------------------------------------------
TH1* BCFBU::GetUnfoldedResult()
{
  TH1* unfolded = (TH1D*) fHistTruth->Clone();

  if (fNDim==1)
    {
      for (int i=1;i<=GetNBinsTruth();++i)
				{
					std::stringstream ss;//create a std::stringstream
					ss << i;//add number to the stream

					BCH1D *temph = GetMarginalized(("T"+ss.str()).c_str());

					temph->Print(("marginalised"+ss.str()+".pdf").c_str());

					double lowbound = 0;
					double upbound = 0;

					temph->GetSmallestInterval (lowbound, upbound);
					std::cout << " bin " << i << " mode " << temph->GetMode() << " mean " << temph->GetMean() << " median " << temph->GetMedian() << " low " << lowbound << " up " << upbound << std::endl;

					unfolded->SetBinContent(i,(upbound+lowbound)/2);

					unfolded->SetBinError(i,(upbound-lowbound)/2);
				}
    }


  if (fNDim==2)
    {
      for (int i=1;i<=GetNBinsTruthX();++i)
				for (int j=1;j<=GetNBinsTruthY();++j)
					{

					        BCH1D *temph = GetMarginalized(("X"+IntToString(i)+"Y"+IntToString(j)).c_str());

						double lowbound = 0;
						double upbound = 0;

						temph->Print(("marginalised"+IntToString(i)+"_"+IntToString(j)+".pdf").c_str());

						temph->GetSmallestInterval (lowbound, upbound);

						std::cout << " unfolded result: truth bin " << i << " " << j << " mode " << temph->GetMode() << " mean " << temph->GetMean() << " median " << temph->GetMedian() << " low " << lowbound << " up " << upbound << std::endl;

						unfolded->SetBinContent(i,j,(upbound+lowbound)/2);
						unfolded->SetBinError(i,j,(upbound-lowbound)/2);

					}

      for (int j=1;j<=GetNBinsTruthY();++j)
				{
					TCanvas *bla = new TCanvas("bla","bla",600,600);
					bla->SetLogy();

					TH1D *truthX = new TH1D("","",2,-3,3);

					TH1D *unfoldedX = new TH1D("","",2,-3,3);

					for (int i=1;i<=GetNBinsTruthX();++i)
						{

							truthX->SetBinContent(i,fHistTruth->GetBinContent(i,j));

							unfoldedX->SetBinContent(i,unfolded->GetBinContent(i,j));

							unfoldedX->SetBinError(i,unfolded->GetBinError(i,j));

							std::cout << "Y bin " << j << " X bin " << i << " truth value = " << fHistTruth->GetBinContent(i,j) << " unfolded value " << unfolded->GetBinContent(i,j) << std::endl;

						}

					truthX->Draw();

					unfoldedX->SetMarkerColor(kBlue);
					unfoldedX->SetLineColor(kBlue);
					unfoldedX->Draw("E same");


					bla->SaveAs(("unfolded_Ybin"+IntToString(j)+".pdf").c_str());
					bla->SaveAs(("unfolded_Ybin"+IntToString(j)+".png").c_str());

				}
    }

  return unfolded;

}

// ---------------------------------------------------------
void BCFBU::PrintSystematicsPosteriors()
{

  // save plots of posteriors for shape systematics

  for (int i=0;i<GetNSyst();++i)
    {

      BCH1D *temph = GetMarginalized((GetSystName(i)).c_str());
      temph->Print(("marginalised_"+GetSystName(i)+".pdf").c_str());
    }

  // save plots of posteriors for normalisation systematics

  for (unsigned int i=0;i<fNormSystematicContainer.size();++i)
    {

      BCH1D *temph = GetMarginalized((fNormSystematicContainer[i].GetName()).c_str());
      temph->Print(("marginalised_"+fNormSystematicContainer[i].GetName()+".pdf").c_str());
    }
}

// ---------------------------------------------------------
int BCFBU::SetDataHistogram(TH1 *h_data)
{
	// check dimension
	if( (fNDim > 0) && fNDim != h_data->GetDimension()) {
	        BCLog::OutWarning("BCBFU::SetDataHistogram. Dimensions of data histogram does not match previously defined dimension. Data histogram rejected.");
		return 0;
	}
	else if (fNDim <= 0) {
		// set dimension
		fNDim = h_data->GetDimension();
	}

	// handle 1-D case
  if (fNDim==1) {
		// check number of bins
		if ( (fNBinsReco > 0) && (fNBinsReco != h_data->GetNbinsX()) ) {
			BCLog::OutWarning("BCBFU::SetDataHistogram. Number of reco bins doens't match number of bins in the data histogram. Data histogram rejected.");
			return 0;
		}

		// clone histogram
		fHistData = (TH1D*) h_data->Clone();

		// (re)set number of reco bins
		fNBinsReco = fHistData->GetNbinsX();
	}

	// handle 2-D case
  if (fNDim==2) {

        // check number of bins

                   if (fNBinsReco != (h_data->GetNbinsX() * h_data->GetNbinsY()) ) {
			BCLog::OutWarning("BCBFU::SetDataHistogram. Number of reco bins doens't match number of bins in the data histogram. Data histogram rejected.");
			return 0;
		}

		fHistData = (TH2D*) h_data->Clone();

		// (re)set number of reco bins
		fNBinsReco = fHistData->GetNbinsX() * h_data->GetNbinsY();
	}

	// no error
	return 1;
}

// ---------------------------------------------------------
void BCFBU::PrintResponseMatrix(const char * filename)
{
	// create new canvas
	TCanvas * c1 = new TCanvas();
	c1->cd();

	// create histogram
	TH2D* hist = new TH2D("hist", ";truth;reco",
												fNBinsTruth, fHistTruth->GetXaxis()->GetBinLowEdge(1), fHistTruth->GetXaxis()->GetBinUpEdge(fNBinsTruth),
												fNBinsReco, fRecoMin, fRecoMax);
	hist->SetStats(kFALSE);

	// copy bins
  for (int i_tru = 0; i_tru < fNBinsTruth; ++i_tru)
    for (int i_rec = 0; i_rec < fNBinsReco; ++i_rec)
      hist->SetBinContent(i_tru+1,i_rec+1, (*fResponseMatrix)(i_tru,i_rec));

	// print
	hist->Draw("COLZ");
	c1->Print(filename);

	// free memory
	delete hist;
	delete c1;
}

// ---------------------------------------------------------
void BCFBU::PrintMigrationMatrix(const char * filename)
{
	// create new canvas
	TCanvas * c1 = new TCanvas();
	c1->cd();

	// create histogram
	TH2D* hist = new TH2D("hist", ";truth;reco",
												fNBinsTruth, fHistTruth->GetXaxis()->GetBinLowEdge(1), fHistTruth->GetXaxis()->GetBinUpEdge(fNBinsTruth),
												fNBinsReco, fRecoMin, fRecoMax);
	hist->SetStats(kFALSE);

	// copy bins
  for (int i_tru = 0; i_tru < fNBinsTruth; ++i_tru)
    for (int i_rec = 0; i_rec < fNBinsReco; ++i_rec)
      hist->SetBinContent(i_tru+1,i_rec+1, (*fMigrationMatrix)(i_tru,i_rec));

	// print
	hist->Draw("COLZ");
	c1->Print(filename);

	// free memory
	delete hist;
	delete c1;
}

// ---------------------------------------------------------
void BCFBU::PrintEfficiencyHistogram(const char * filename)
{
	// check if efficiency histogram exists
  if (!fHistEfficiency) {
		BCLog::OutWarning("BCFBU::PrintEfficiency: efficiency histogram not found. Exit.");
		exit(1);
	}

	// create new canvas
	TCanvas * c1 = new TCanvas();
	c1->cd();
	fHistEfficiency->Draw();
	c1->Print(filename);

	// free memory
	delete c1;
}

// ---------------------------------------------------------
void BCFBU::PrintTruthHistogram(const char * filename)
{
	// check if truth histogram exists
  if (!fHistTruth) {
		BCLog::OutWarning("BCFBU::PrintTruth: truth histogram not found. Exit.");
		exit(1);
	}

	// create new canvas
	TCanvas * c1 = new TCanvas();
	c1->cd();
	fHistTruth->Draw();
	c1->Print(filename);

	// free memory
	delete c1;
}

// ---------------------------------------------------------
void BCFBU::PrintDataHistogram(const char * filename)
{
	// check if data histogram exists
  if (!fHistData) {
		BCLog::OutWarning("BCFBU::PrintData: data histogram not found. Exit.");
		exit(1);
	}

	// create new canvas
	TCanvas * c1 = new TCanvas();
	c1->cd();
	fHistData->Draw();
	c1->Print(filename);

	// free memory
	delete c1;
}

// ---------------------------------------------------------
