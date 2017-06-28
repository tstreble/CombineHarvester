#! /bin/csh -f

WriteDatacards_2lss_1tau_ttH_comb_incl_Hdecay -i /data_CMS/cms/strebler/ttH_prod_80X_01_2017/datacards_ttH_comb/datacard_MEM_ttH_vs_full_bkg_2lSS_nomiss_nofaketau_35.9fb_042017.root -o ttH_2lss_1tau_MEM_nomiss.root -b ttH_2lss_1tau_nomiss

WriteDatacards_2lss_1tau_ttH_comb_incl_Hdecay -i /data_CMS/cms/strebler/ttH_prod_80X_01_2017/datacards_ttH_comb/datacard_MEM_ttH_vs_full_bkg_2lSS_missing_nofaketau_35.9fb_042017.root -o ttH_2lss_1tau_MEM_missing.root -b ttH_2lss_1tau_missing

combineCards.py ttH_2lss_1tau_MEM_nomiss.txt ttH_2lss_1tau_MEM_missing.txt > ttH_2lss_1tau_2016_MEM_2cat.txt

combine -M MaxLikelihoodFit -m 125 ttH_2lss_1tau_2016_MEM_2cat.txt

combine -M Asymptotic -m 125 ttH_2lss_1tau_2016_MEM_2cat.txt
