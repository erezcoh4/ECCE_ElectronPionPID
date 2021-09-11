

# Analysis tools for e/$\pi$ discrimination in ECCE proposal
using EVAL ROOT files from Fun4All simulations with a single particle gun

root -l 'ConvertROOTdataToCSV.C("cemc","singlePion","ntp_cluster")'


## copy data to my computer

scp cohen@ftp.jlab.org:/w/eic3-scshelf2104/users/ecohen/EvalFiles//CSVfiles//* ~/Desktop/data/EIC/ECCE/ElectronPionPID/EvalFiles/CSVfiles/
