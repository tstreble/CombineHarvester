#include <string>
#include <map>
#include <set>
#include <iostream>
#include <utility>
#include <vector>
#include <cstdlib>
#include "boost/program_options.hpp"
#include <TString.h>
#include "CombineHarvester/CombineTools/interface/CombineHarvester.h"
#include "CombineHarvester/CombineTools/interface/Observation.h"
#include "CombineHarvester/CombineTools/interface/Process.h"
#include "CombineHarvester/CombineTools/interface/Utilities.h"
#include "CombineHarvester/CombineTools/interface/Systematics.h"
#include "CombineHarvester/CombineTools/interface/BinByBin.h"

using namespace std;
using boost::starts_with;
namespace po = boost::program_options;

int main(int argc, char** argv) {

  std::string input_file, output_file;
  bool add_shape_sys = true;
  int ctau, mchi, mgno;
  po::variables_map vm;
  po::options_description config("configuration");
  config.add_options()
    ("input_file,i", po::value<string>(&input_file)->default_value("/vols/cms/tstreble/LL_ntuples/datacards/datacard_histos.root"))
    ("output_file,o", po::value<string>(&output_file)->default_value("datacard.root"))
    ("ctau", po::value<int>(&ctau)->default_value(1))
    ("mchi", po::value<int>(&mchi)->default_value(0))
    ("mgno", po::value<int>(&mgno)->default_value(1000))    
    ("add_shape_sys,s", po::value<bool>(&add_shape_sys)->default_value(true));
  po::store(po::command_line_parser(argc, argv).options(config).run(), vm);
  po::notify(vm);


  // Create an empty CombineHarvester instance that will hold all of the
  // datacard configuration and histograms etc.
  ch::CombineHarvester cb;
  // Uncomment this next line to see a *lot* of debug information
  // cb.SetVerbosity(3);

  
   // Here we will just define one categories. Each entry in
  // the vector below specifies a bin name and corresponding bin_id.
  ch::Categories cats = {
      {1, "cat1"}
    };

  cb.AddObservations({"*"}, {"LLP"}, {"13TeV"}, {"*"}, cats);

  vector<string> bkg_procs = {"data_fakes"};
  cb.AddProcesses({"*"}, {"LLP"}, {"13TeV"}, {"*"}, bkg_procs, cats, false);

  vector<string> sig_procs = {Form("T1qqqq_ctau%i_mgno%i_mchi%i",ctau,mgno,mchi)};
  cb.AddProcesses({"*"}, {"*"}, {"13TeV"}, {"*"}, sig_procs, cats, true);


  //Some of the code for this is in a nested namespace, so
  // we'll make some using declarations first to simplify things a bit.
  using ch::syst::SystMap;
  using ch::syst::SystMapAsymm;
  using ch::syst::era;
  using ch::syst::bin_id;
  using ch::syst::process;

  cb.cp().signals()
    .AddSyst(cb, "lumi_$ERA", "lnN", SystMap<era>::init
	     ({"13TeV"}, 1.026));
  cb.cp().signals()
    .AddSyst(cb, "CMS_scale_j", "shape", SystMap<>::init(1.0));

  cb.cp().backgrounds()
    .AddSyst(cb, "Clos_shape", "shape", SystMap<>::init(1.0));
  cb.cp().backgrounds()
    .AddSyst(cb, "fake_norm_b", "shape", SystMap<>::init(1.0));
  cb.cp().backgrounds()
    .AddSyst(cb, "fake_norm_q", "shape", SystMap<>::init(1.0));
  cb.cp().backgrounds()
    .AddSyst(cb, "fake_norm_g", "shape", SystMap<>::init(1.0));  
  cb.cp().backgrounds()
    .AddSyst(cb, "fake_shape_b_pt", "shape", SystMap<>::init(1.0));
  cb.cp().backgrounds()
    .AddSyst(cb, "fake_shape_q_pt", "shape", SystMap<>::init(1.0));
  cb.cp().backgrounds()
    .AddSyst(cb, "fake_shape_g_pt", "shape", SystMap<>::init(1.0));  

  cb.cp().backgrounds().ExtractShapes(
       input_file,
      "x_$PROCESS",
      "x_$PROCESS_$SYSTEMATIC");
  cb.cp().signals().ExtractShapes(
      input_file,
      "x_$PROCESS",
      "x_$PROCESS_$SYSTEMATIC");

  auto bbb = ch::BinByBinFactory()
    .SetAddThreshold(0.1)
    .SetFixNorm(true);
  bbb.SetPattern("CMS_$BIN_$PROCESS_bin_$#");
  bbb.AddBinByBin(cb.cp().backgrounds(), cb);

  set<string> bins = cb.bin_set();

  TFile output(output_file.data(), "RECREATE");

  for (auto b : bins) {
    cout << ">> Writing datacard for bin: " << b
	 << "\n";
    cb.cp().bin({b}).mass({"*"}).WriteDatacard(
	TString(output_file.data()).ReplaceAll(".root", ".txt").Data(), output);
  }


}
