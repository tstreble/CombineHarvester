#! /bin/csh -f

WriteDatacards_3l_1tau_ttH_comb_incl_Hdecay  -i /data_CMS/cms/strebler/ttH_prod_80X_01_2017/datacards_ttH_comb/datacard_MVA_3l_syst_nofaketau_35.9fb_042017.root

combine -M MaxLikelihoodFit -m 125 ttH_3l_1tau.txt

combine -M Asymptotic -m 125 ttH_3l_1tau.txt
