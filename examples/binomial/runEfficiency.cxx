#include <BAT/BCLog.h>
#include <BAT/BCAux.h>
#include <BAT/BCSummaryTool.h>

#include "BinomialModel.h"

int main()
{

  // set nicer style for drawing than the ROOT default
  BCAux::SetStyle();

  // open log file
  BCLog::OpenLog("log.txt");
  BCLog::SetLogLevel(BCLog::detail);

  // create new BinomialModel object
  BinomialModel * m = new BinomialModel();

  // set precision
  m->MCMCSetPrecision(BCEngineMCMC::kHigh);

  // set number of events
  m->SetNTotal(20);
  m->SetNSelected(3);

  // marginalize
  m->MarginalizeAll();

  // find mode
  m->FindMode( m->GetBestFitParameters() );

  // draw all marginalized distributions into a PostScript file
  m->PrintAllMarginalized("BinomialModel_plots.ps");

  // print results of the analysis into a text file
  m->PrintResults("BinomialModel_results.txt");

  // close log file
  BCLog::CloseLog();

  delete m;

  return 0;

}

