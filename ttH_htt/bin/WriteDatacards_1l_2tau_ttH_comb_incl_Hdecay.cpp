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
  double lumi = -1.;
  bool add_shape_sys = true;
  po::variables_map vm;
  po::options_description config("configuration");
  config.add_options()
    ("input_file,i", po::value<string>(&input_file)->default_value("Tallinn/ttH_1l_2tau_2016Jul08_vTight.input.root"))
    ("output_file,o", po::value<string>(&output_file)->default_value("ttH_1l_2tau.root"))
    ("lumi,l", po::value<double>(&lumi)->default_value(lumi))
    ("add_shape_sys,s", po::value<bool>(&add_shape_sys)->default_value(true));
  po::store(po::command_line_parser(argc, argv).options(config).run(), vm);
  po::notify(vm);

  //! [part1]
  // First define the location of the "auxiliaries" directory where we can
  // source the input files containing the datacard shapes
  //string aux_shapes = string(getenv("CMSSW_BASE")) + "/src/CombineHarvester/HIG15008/shapes/";
  //string aux_shapes = "/afs/cern.ch/user/v/veelken/public/HIG15008_datacards/";
  string aux_shapes = "/home/veelken/public/HIG15008_datacards/";
  if ( input_file.find_first_of("/") == 0 ) aux_shapes = ""; // set aux_shapes directory to zero in case full path to input file is given on command line

  // Create an empty CombineHarvester instance that will hold all of the
  // datacard configuration and histograms etc.
  ch::CombineHarvester cb;
  // Uncomment this next line to see a *lot* of debug information
  // cb.SetVerbosity(3);

  // Here we will just define two categories for an 8TeV analysis. Each entry in
  // the vector below specifies a bin name and corresponding bin_id.
  ch::Categories cats = {
      {1, "ttH_1l_2tau"}
    };
  // ch::Categories is just a typedef of vector<pair<int, string>>
  //! [part1]


  //! [part2]
  vector<string> masses = {"*"};
  //! [part2]

  //! [part3]
  cb.AddObservations({"*"}, {"ttHl"}, {"13TeV"}, {"*"}, cats);
  //! [part3]

  //! [part4]
  //std::string proc_fakes = "fakes_mc";
  std::string proc_fakes = "data_fakes";

  vector<string> bkg_procs = {"TTW", "TTZ", "EWK", "Rares", "TTWW", "tHq", "tHW", proc_fakes};
  //vector<string> bkg_procs = {"TTW", "TTZ", "EWK", "Rares", "TTWW", proc_fakes};
  cb.AddProcesses({"*"}, {"*"}, {"13TeV"}, {"*"}, bkg_procs, cats, false);

  //vector<string> sig_procs = {"ttH_hww", "ttH_hzz", "ttH_htt"};
  vector<string> sig_procs = {"ttH"};
  //vector<string> sig_procs = {"ttH","tHq","tHW"};
  cb.AddProcesses(masses, {"*"}, {"13TeV"}, {"*"}, sig_procs, cats, true);
  //! [part4]

  //Some of the code for this is in a nested namespace, so
  // we'll make some using declarations first to simplify things a bit.
  using ch::syst::SystMap;
  using ch::syst::SystMapAsymm;
  using ch::syst::era;
  using ch::syst::bin_id;
  using ch::syst::process;

  //! [part5]
  cb.cp().process(ch::JoinStr({sig_procs, {"TTW", "TTZ", "Rares", "TTWW", "tHq", "tHW"}}))
      .AddSyst(cb, "lumi_13TeV_2016", "lnN", SystMap<>::init(1.025));
  //! [part5]

  //! [part6]
  cb.cp().process({"ttH"})
      .AddSyst(cb, "BR_hbb", "lnN", SystMap<>::init(1.0126));
  cb.cp().process({"ttH"})
      .AddSyst(cb, "pdf_Higgs_ttH", "lnN", SystMap<>::init(1.036));
  cb.cp().process({"ttH"})
    .AddSyst(cb, "QCDscale_ttH", "lnN", SystMapAsymm<>::init(0.915, 1.058));
  if ( add_shape_sys ) {
    cb.cp().process({"ttH"})
    .AddSyst(cb, "CMS_ttHl_thu_shape_ttH1", "shape", SystMap<>::init(1.0));
  }


  cb.cp().process({"TTW"})
      .AddSyst(cb, "pdf_qqbar", "lnN", SystMap<>::init(1.04));
  cb.cp().process({"TTW"})
      .AddSyst(cb, "QCDscale_ttW", "lnN", SystMap<>::init(1.12));
  if ( add_shape_sys ) {
    cb.cp().process({"TTW"})
      .AddSyst(cb, "CMS_ttHl_thu_shape_ttW1", "shape", SystMap<>::init(1.0));
  }

  cb.cp().process({"TTZ"})
      .AddSyst(cb, "pdf_gg", "lnN", SystMap<>::init(0.966));
  cb.cp().process({"TTZ"})
      .AddSyst(cb, "QCDscale_ttZ", "lnN", SystMap<>::init(1.11));
  if ( add_shape_sys ) {   
    cb.cp().process({"TTZ"})
      .AddSyst(cb, "CMS_ttHl_thu_shape_ttZ1", "shape", SystMap<>::init(1.0));
  }

  cb.cp().process({"TTWW"})
      .AddSyst(cb, "pdf_TTWW", "lnN", SystMap<>::init(1.03));
  cb.cp().process({"TTWW"})
    .AddSyst(cb, "QCDscale_ttWW", "lnN", SystMapAsymm<>::init(0.891, 1.081));

  cb.cp().process({"tHq"})
      .AddSyst(cb, "pdf_qg", "lnN", SystMap<>::init(1.01));
  cb.cp().process({"tHq"})
    .AddSyst(cb, "QCDscale_tHq", "lnN", SystMapAsymm<>::init(0.933, 1.041));

  cb.cp().process({"tHW"})
      .AddSyst(cb, "pdf_qg", "lnN", SystMap<>::init(1.027));
  cb.cp().process({"tHW"})
    .AddSyst(cb, "QCDscale_tHW", "lnN", SystMapAsymm<>::init(0.939, 1.046));


  cb.cp().process({"EWK"})
      .AddSyst(cb, "CMS_ttHl_EWK_4j", "lnN", SystMap<>::init(2.0));
  
  cb.cp().process({"Rares"})
      .AddSyst(cb, "CMS_ttHl_Rares", "lnN", SystMap<>::init(1.5));

  cb.cp().process({proc_fakes})
      .AddSyst(cb, "CMS_ttHl_fakes", "lnN", SystMap<>::init(1.3));
  if ( add_shape_sys ) {
    cb.cp().process({proc_fakes})
      .AddSyst(cb, "CMS_ttHl16_FRjt_norm", "shape", SystMap<>::init(1.0));
    cb.cp().process({proc_fakes})
      .AddSyst(cb, "CMS_ttHl16_FRjt_shape", "shape", SystMap<>::init(1.0));
  }    


  cb.cp().process(ch::JoinStr({sig_procs, {"TTW", "TTZ", "Rares", "TTWW", "tHq", "tHW"}}))
      .AddSyst(cb, "CMS_ttHl16_trigger_leptau", "lnN", SystMap<>::init(1.03));
  cb.cp().process(ch::JoinStr({sig_procs, {"TTW", "TTZ", "Rares", "TTWW", "tHq", "tHW"}}))
      .AddSyst(cb, "CMS_ttHl16_lepEff_elloose", "shape", SystMap<>::init(1.0));
  cb.cp().process(ch::JoinStr({sig_procs, {"TTW", "TTZ", "Rares", "TTWW", "tHq", "tHW"}}))
      .AddSyst(cb, "CMS_ttHl16_lepEff_muloose", "shape", SystMap<>::init(1.0));
  cb.cp().process(ch::JoinStr({sig_procs, {"TTW", "TTZ", "Rares", "TTWW", "tHq", "tHW"}}))
      .AddSyst(cb, "CMS_ttHl16_lepEff_eltight", "shape", SystMap<>::init(1.0));
  cb.cp().process(ch::JoinStr({sig_procs, {"TTW", "TTZ", "Rares", "TTWW", "tHq", "tHW"}}))
      .AddSyst(cb, "CMS_ttHl16_lepEff_mutight", "shape", SystMap<>::init(1.0));
  
  cb.cp().process(ch::JoinStr({sig_procs, {"TTW", "TTZ", "Rares", "TTWW", "tHq", "tHW"}}))
    .AddSyst(cb, "CMS_ttHl16_tauID", "lnN", SystMap<>::init(1.10));

  if ( add_shape_sys ) {
    cb.cp().process(ch::JoinStr({sig_procs, {"TTW", "TTZ", "Rares", "TTWW", "tHq", "tHW"}}))
      .AddSyst(cb, "CMS_ttHl16_JES", "shape", SystMap<>::init(1.0));
  
    cb.cp().process(ch::JoinStr({sig_procs, {"TTW", "TTZ", "Rares", "TTWW", "tHq", "tHW"}}))
      .AddSyst(cb, "CMS_ttHl16_tauES", "shape", SystMap<>::init(1.0));
  }


  if ( add_shape_sys ) {
    for ( auto s : {"HF", "LF", "cErr1", "cErr2"} ) {
      cb.cp().process(ch::JoinStr({sig_procs, {"TTW", "TTZ", "Rares", "TTWW", "tHq", "tHW"}}))
	.AddSyst(cb, Form("CMS_ttHl16_btag_%s", s), "shape", SystMap<>::init(1.0));
    }
    for ( auto s : {"HFStats1", "HFStats2", "LFStats1", "LFStats2"} ) {
      cb.cp().process(ch::JoinStr({sig_procs, {"TTW", "TTZ", "Rares" , "TTWW", "tHq", "tHW"}}))
          .AddSyst(cb, Form("CMS_ttHl16_btag_%s", s), "shape", SystMap<>::init(1.0));
    }
  }
  //! [part6]

  //! [part7]
  cb.cp().backgrounds().ExtractShapes(
      aux_shapes + input_file,
      "x_$PROCESS",
      "x_$PROCESS_$SYSTEMATIC");
  cb.cp().signals().ExtractShapes(
      aux_shapes + input_file,
      "x_$PROCESS",
      "x_$PROCESS_$SYSTEMATIC");
  //! [part7]

  // CV: scale yield of all signal and background processes by lumi/2.3,
  //     with 2.3 corresponding to integrated luminosity of 2015 dataset
  if ( lumi > 0. ) {  
    std::cout << "scaling signal and background yields to L=" << lumi << "fb^-1 @ 13 TeV." << std::endl;
    cb.cp().process(ch::JoinStr({sig_procs, {"TTW_gentau", "TTZ_gentau", "EWK_gentau", "Rares_gentau", proc_fakes}})).ForEachProc([&](ch::Process* proc) {
      proc->set_rate(proc->rate()*lumi/2.3);
    });
  }

  //! [part8]
  auto bbb = ch::BinByBinFactory()
    .SetAddThreshold(0.1)
    .SetFixNorm(true);
  bbb.SetPattern("CMS_$BIN_$PROCESS_bin_$#");
  bbb.AddBinByBin(cb.cp().backgrounds(), cb);
  bbb.AddBinByBin(cb.cp().signals(), cb);

  // This function modifies every entry to have a standardised bin name of
  // the form: {analysis}_{channel}_{bin_id}_{era}
  // which is commonly used in the htt analyses
  //ch::SetStandardBinNames(cb);
  //! [part8]

  //! [part9]
  // First we generate a set of bin names:
  set<string> bins = cb.bin_set();
  // This method will produce a set of unique bin names by considering all
  // Observation, Process and Systematic entries in the CombineHarvester
  // instance.

  // We create the output root file that will contain all the shapes.
  TFile output(output_file.data(), "RECREATE");

  // Finally we iterate through bins and write a
  // datacard.
  for (auto b : bins) {
    cout << ">> Writing datacard for bin: " << b
	 << "\n";
      // We need to filter on both the mass and the mass hypothesis,
      // where we must remember to include the "*" mass entry to get
      // all the data and backgrounds.
    //cb.cp().bin({b}).mass({"*"}).WriteDatacard(
    //	b + ".txt", output);
    cb.cp().bin({b}).mass({"*"}).WriteDatacard(
      TString(output_file.data()).ReplaceAll(".root", ".txt").Data(), output);	
  }
  //! [part9]

}
