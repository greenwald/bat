/*
 * Copyright (C) 2013, Daniel Greenwald and Frederik Beaujean
 * All rights reserved.
 *
 * For the licensing terms see doc/COPYING.
 */
#include <config.h>

#include "IntegrationModel.h"
#include "GaussModel.h"
#include "test.h"

#include <BAT/BCMath.h>
#include <BAT/BCParameter.h>

using namespace test;

class BCIntegrateTest :
    public TestCase
{
public:

  struct Precision
  {
    double monteCarlo, importance, vegas, suave, divonne, cuhre;

    // Turn off checks by default
    Precision(double value = -1) :
      importance(-1)
    {
      Set(value);
    }

    void Set(double value)
    {
      monteCarlo = vegas = suave = divonne = cuhre = value;
    }

  };

  BCIntegrateTest() :
    TestCase("BCIntegrate test")
  {
  }

  static IntegrationModel Factory(unsigned dim, unsigned modality, unsigned complexity)
  {
    IntegrationModel m;
    m.SetDimensionality(dim);
    m.SetModality(modality);
    m.SetComplexity(complexity);
    m.PopulatePolynomialDegrees();
    m.MCMCSetRandomSeed(1346);

    return m;
  }

#if HAVE_CUBA_H
  static void IntegrateCuba(IntegrationModel & m, BCIntegrate::BCCubaMethod method, const double & eps)
  {
    // aim for higher precision, as naively 1/3 of tests would lie outside interval
    // if eps were used
    m.SetRelativePrecision(eps / 2.);
    TEST_CHECK_RELATIVE_ERROR(m.IntegrateCuba(method), m.Integral(), eps);
  }
#endif
  static void IntegrateBat(IntegrationModel & m, BCIntegrate::BCIntegrationMethod method, const double & eps)
  {
    // aim for higher precision, as naively 1/3 of tests would lie outside interval
    // if eps were used
    m.SetRelativePrecision(eps / 2.);
    TEST_CHECK_RELATIVE_ERROR(m.Integrate(method), m.Integral(), eps);
  }

  static void IntegrateAllMethods(unsigned dim, unsigned modality, unsigned complexity, const Precision & p)
  {
    IntegrationModel m = Factory(dim, modality, complexity);

    std::cout << "Computing single integration test in " << dim
        << " dimensions with modality = " << modality
        << " and complexity = " << complexity << ".\n";

    if (p.monteCarlo > 0)
      IntegrateBat(m, BCIntegrate::kIntMonteCarlo, p.monteCarlo);
    if (p.importance > 0)
      IntegrateBat(m, BCIntegrate::kIntImportance, p.importance);
#if HAVE_CUBA_H
    if (p.vegas > 0)
      IntegrateCuba(m, BCIntegrate::kCubaVegas, p.vegas);
    if (p.suave > 0)
      IntegrateCuba(m, BCIntegrate::kCubaSuave, p.suave);
    if (p.divonne> 0)
      IntegrateCuba(m, BCIntegrate::kCubaDivonne, p.divonne);
    if (p.cuhre > 0)
      IntegrateCuba(m, BCIntegrate::kCubaCuhre, p.cuhre);
#endif
  }

  void Optimization() const
  {
    // two modes at 0 and 1
    IntegrationModel m = Factory(1,1,1);
    m.MCMCSetRandomSeed(1346);
    TEST_CHECK_NEARLY_EQUAL(m.FindMode(std::vector<double>(1, 0.1)).front(), 0, 1e-3);
    TEST_CHECK_NEARLY_EQUAL(m.FindMode(BCIntegrate::kOptMinuit, std::vector<double>(1, 0.1)).front(), 0, 1e-10);
    TEST_CHECK_NEARLY_EQUAL(m.FindMode(BCIntegrate::kOptMinuit, std::vector<double>(1, 0.9)).front(), 1, 1e-10);
    TEST_CHECK_NEARLY_EQUAL(m.FindMode(BCIntegrate::kOptSA, std::vector<double>(1, 0.1)).front(), 1, 1e-3);
    TEST_CHECK_NEARLY_EQUAL(m.FindMode(BCIntegrate::kOptSA, std::vector<double>(1, 0.9)).front(), 1, 1e-3);
    TEST_CHECK_NEARLY_EQUAL(m.FindMode(BCIntegrate::kOptMetropolis).front(), 1, 1e-3);

    m.PrintSummary();
    m.PrintShortFitSummary();
  }

  void FixedParameters() const
  {
    static const unsigned ndim = 4;
    GaussModel m("Fixed parameter example", ndim);
    m.MCMCSetRandomSeed(613);
    m.GetParameter(3)->Fix(0.5);

    // integrate over normalized Gaussian likelihood
    // evidence = 1 / parameter volume * N(mu | \theta_3)
    double evidence = 1;
    for (unsigned i = 0 ; i < m.GetNParameters() ; ++i) {
      const BCParameter * p = m.GetParameter(i);
      if (p->Fixed())
        evidence *= exp(BCMath::LogGaus(p->GetFixedValue(), 0.0, 2.0, true));
      else
        evidence /= p->GetRangeWidth();
    }

    std::cout << "Correct evidence: " << evidence << std::endl;

    static const double eps = 3e-2;
    m.SetRelativePrecision(eps);
    m.SetAbsolutePrecision(1e-12);

    TEST_CHECK_RELATIVE_ERROR(m.Integrate(BCIntegrate::kIntMonteCarlo), evidence, eps);
#if HAVE_CUBA_H
    TEST_CHECK_RELATIVE_ERROR(m.IntegrateCuba(BCIntegrate::kCubaVegas),   evidence, eps);
    TEST_CHECK_RELATIVE_ERROR(m.IntegrateCuba(BCIntegrate::kCubaSuave),   evidence, eps);
    TEST_CHECK_RELATIVE_ERROR(m.IntegrateCuba(BCIntegrate::kCubaDivonne), evidence, eps);
    TEST_CHECK_RELATIVE_ERROR(m.IntegrateCuba(BCIntegrate::kCubaCuhre),   evidence, eps);
#endif
  }

  void Integration() const
  {
    // top down: normalization should work automatically
    {
      IntegrationModel m = Factory(2, 1, 1);
      static const double eps = 1e-2;
      m.SetRelativePrecision(eps);
      m.Normalize();
      TEST_CHECK_RELATIVE_ERROR(m.GetIntegral(), m.Integral(), eps * 1.3);
    }

    {
      Precision p;
      p.monteCarlo = 1e-2;
      p.vegas = 2e-2;
      p.suave = 1e-2;
      p.divonne = 2; // fails in 1D
      p.cuhre = 2; // fails in 1D
      IntegrateAllMethods(1, 1, 1, p);
    }

    {
      Precision p(1e-2);
      p.divonne = p.cuhre = -2e-3;
      IntegrateAllMethods(2, 1, 1, p);
    }

    {
      Precision p(5e-3);
      p.suave = -1;
      IntegrateAllMethods(5, 1, 1, p);
    }

    // tough case: all cuba methods far off, but sample mean surprisingly good
    // todo bug if modality is >3, results not reproducible, probably accessing random memory locations in likelihood
    {
      Precision p(5e-2);
      p.monteCarlo = 0.3;
      p.vegas = p.suave = p.divonne = p.cuhre = -1;
      IntegrateAllMethods(15, 1, 1, p);
    }
  }

  virtual void run() const
  {
    Integration();
    Optimization();
    FixedParameters();
  }
} bcIntegrateTest;
