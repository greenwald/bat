/*
 * Copyright (C) 2009,
 * Daniel Kollar, Kevin Kroeninger and Jing Liu.
 * All rights reserved.
 *
 * For the licensing terms see doc/COPYING.
 */

//=============================================================================

//#include "BAT/BCAux.h"
#include "BAT/BCLog.h"

#include "BCBenchmarkMCMC.h"
#include "BCBenchmarkMCMC2D.h"
#include "BCBenchmarkMCMC3D.h"

//=============================================================================

// the main function for the test of mcmc.
// Markov chains are created based on some test functions
// and then compared to these functions
//
// the first parameter of the function should be a normalization factor
int main()
{
	//BCAux::SetStyle();

	BCLog::OpenLog("benchmark.log", BCLog::detail, BCLog::detail);

	// define the range for the test
	double xmin = -4.0, xmax = 16.0;


	// test function: gaus
	// =================================

	TF1 *testfunc = new TF1("Gaussian", "gaus", xmin, xmax);
	testfunc->SetParameters(2,3,4);

	BCBenchmarkMCMC *benchmark = new BCBenchmarkMCMC(testfunc,"mcmcgaus1d.root");

	benchmark -> MCMCSetNIterationsRun(100000);

	benchmark -> MCMCMetropolis();

	benchmark -> ProcessMCTrees();
	benchmark -> PerformLagsTest();
	benchmark -> PerformIterationsTest();

	benchmark -> WriteResults();

	delete testfunc;
	delete benchmark;


	// test function: exponential decay
	// ==================================

	testfunc = new TF1("Exponential decay", "[0]*exp(-[1]*x)", xmin, xmax);
	testfunc->SetParameters(1,0.2);

	benchmark = new BCBenchmarkMCMC(testfunc,"mcmcexpo1d.root");

	benchmark -> MCMCSetNIterationsRun(100000);

	benchmark -> MCMCMetropolis();

	benchmark -> ProcessMCTrees();
	benchmark -> PerformLagsTest();
	benchmark -> PerformIterationsTest();

	benchmark -> WriteResults();

	delete testfunc;
	delete benchmark;


	// test function: x^4 * sin^2(x)
	// ==================================

	testfunc = new TF1("x4sin2x", "[0]*x*x*x*x*sin(x)*sin(x)", xmin, xmax);
	testfunc->SetParameter(0,1.);

	benchmark = new BCBenchmarkMCMC(testfunc,"mcmcx4sin2x1d.root");

	benchmark -> MCMCSetNIterationsRun(100000);

	benchmark -> MCMCMetropolis();

	benchmark -> ProcessMCTrees();
	benchmark -> PerformLagsTest();
	benchmark -> PerformIterationsTest();

	benchmark -> WriteResults();

	delete testfunc;
	delete benchmark;


	// test function: Cauchy distribution
	// (probably interesting because it has no mean, variance or skwness
	// defined)
	// ==================================

	testfunc = new TF1("cauchy", "[0] / (3.14159 * ( (x-[1])**2 +[0]**2))",xmin, xmax);
	testfunc->SetParameters(1.,3.);

	benchmark = new BCBenchmarkMCMC(testfunc,"mcmccauchy.root");

	benchmark -> MCMCSetNIterationsRun(100000);

	benchmark -> MCMCMetropolis();

	benchmark -> ProcessMCTrees();
	benchmark -> PerformLagsTest();
	benchmark -> PerformIterationsTest();

	benchmark -> WriteResults();

	delete testfunc;
	delete benchmark;


	// define the range for the test
	double ymin = 0.0, ymax = 20.0;


	// test 2D function: gaus2
	// =================================

	TF2 *testfunc2 = new TF2("gaus2","xygausn",xmin,xmax,ymin,ymax);
	testfunc2->SetParameters(1,4,2,8,1);

	BCBenchmarkMCMC2D *benchmark2
		= new BCBenchmarkMCMC2D(testfunc2,"mcmcgaus2d.root");

	benchmark2 -> MCMCSetNIterationsRun(100000);

	benchmark2 -> MCMCMetropolis();

	benchmark2 -> ProcessMCTrees();
	benchmark2 -> PerformLagsTest();
	benchmark2 -> PerformIterationsTest();

	benchmark2 -> WriteResults();

	delete testfunc2;
	delete benchmark2;


	// define range for z dimension
	double zmin = 0.0, zmax = 20.0;

	// test 3D function: gaus3 (3-dimensional gaussian)
	// =================================

	TF3 *testfunc3 = new TF3("gaus3",
			"[0] * exp(-0.5*((x-[1])/[2])**2) * exp(-0.5*((y-[3])/[4])**2) * exp(-0.5*((z-[5])/[6])**2)",
			xmin, xmax, ymin, ymax, zmin, zmax);
	testfunc3->SetParameters(1., 4., 2., 8., 1., 8., 1.);

	BCBenchmarkMCMC3D *benchmark3
		= new BCBenchmarkMCMC3D(testfunc3,"mcmcgaus3d.root");

	benchmark3 -> MCMCSetNIterationsRun(100000);

	benchmark3 -> MCMCMetropolis();

	benchmark3 -> ProcessMCTrees();
	benchmark3 -> PerformLagsTest();
	benchmark3 -> PerformIterationsTest();

	benchmark3 -> WriteResults();

	delete testfunc3;
	delete benchmark3;


	return 0;
}

