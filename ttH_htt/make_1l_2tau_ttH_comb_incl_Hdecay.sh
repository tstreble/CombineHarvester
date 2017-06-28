#! /bin/csh -f

WriteDatacards_1l_2tau_ttH_comb_incl_Hdecay -i /data_CMS/cms/strebler/ttH_prod_80X_01_2017/datacards_ttH_comb/datacard_BDT_1l2tau_syst_35.9fb_042017.root

combine -M MaxLikelihoodFit -m 125 ttH_1l_2tau.txt --rMin -5 --rMax 5

combine -M Asymptotic -m 125 ttH_1l_2tau.txt
