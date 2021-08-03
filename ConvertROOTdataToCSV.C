/*
 convert EIC eval ROOT files to CSV
 
 execute:
 ---------------------
 root -l 'ConvertROOTdataToCSV.C("calin","singleElectron","ntp_cluster")'
 root -l ConvertROOTdataToCSV.C
 root -l 'ConvertROOTdataToCSV.C("calout","singleElectron","ntp_cluster")'
 
 */


#include <stdio.h>

TString     evalpath;
TString     datapath;
TString       GitTag;

// ToDo: (1) change data path in local PC to be same as on farm
// (2) operate code on files on farm
// (3) download one tracking file and one hcal file, print their structure
// (4) add valiables for tracking and hcal files
// (5) run localy to convert them to CSV
// (6) run on farm to convert all files to 4 CSV files per particle gun: calin calout hcal tracking
// (7) run the same on singlePion files
// (8) download 8 CSV files to local computer
// (9) plot distibutions
// (10) plot one purity vs. efficiency for E, one for x/y or \rho, and one for z
// (11) save results to keynote for presentation

// Oo.oOo.oOo.oOo.oOo.oOo.oOo.oOo.oOo.oOo.oOo.oOo.oOo.oOo.oOo.oOo.oOo.oOo.
void ConvertROOTdataToCSV(TString filelabel   = "calin",
                          TString particleGun = "singleElectron",
                          TString TChainName  = "ntp_cluster"){
        
    if (std::string(gSystem->pwd()).find("erezcohen/Desktop")!=std::string::npos){
        evalpath = "/Users/erezcohen/Desktop/data/EIC/ECCE/ElectronPionPID/";
    } else if (std::string(gSystem->pwd()).find("home/cohen")!=std::string::npos){
        evalpath = "/w/eic3-scshelf2104/users/ecohen/";
    }
    datapath = evalpath + "EvalFiles/";
    if (particleGun=="singleElectron") {
        GitTag = "fb01759";
    }
    else if (particleGun=="singlePion") {
        GitTag = "419662c";
    }
        
    TString ROOTdatapath = datapath + GitTag + "/" + particleGun + "/";
    TString  CSVdatapath = datapath + "/CSVfiles/";
    TString  csvfilename = CSVdatapath + "/" + particleGun + "_" + filelabel + "_" + TChainName + ".csv";
    
   
    
    Float_t eta, x, y, z, phi, e;
    TChain * chain = new TChain(TChainName);

    // load all similar files and combine them to a large TChain
    gSystem->cd( datapath + GitTag );
    TSystemDirectory dire( particleGun, particleGun  );
    std::cout << datapath + GitTag << "/" << dire.GetName() << std::endl;
    TList * files = dire.GetListOfFiles(); //Displays a list of all files
    for (auto file:*files) {
        std::string filename = (std::string)file->GetName();
        if (filename.find(filelabel) != std::string::npos) {
            std::cout << "adding " << filename << std::endl;
            chain->Add( ROOTdatapath + filename.c_str() );
        }
    }
    int Nevents = chain->GetEntries();
    
    // copy TChain variables to a csv
    std::cout << "processign " << Nevents <<  " entries" << std::endl;
    chain -> SetBranchAddress("eta",  &eta);
    chain -> SetBranchAddress("x",    &x);
    chain -> SetBranchAddress("y",    &y);
    chain -> SetBranchAddress("z",    &z);
    chain -> SetBranchAddress("phi",  &phi);
    chain -> SetBranchAddress("e",    &e);
    
    std::ofstream csvfile;
    csvfile.open( csvfilename );
    csvfile << "eta,x,y,z,e,phi," << std::endl;
    
    for (int event=0; event < Nevents ; event++){
        chain -> GetEntry(event);
        csvfile
        << eta  << ","
        << x    << ","
        << y    << ","
        << z    << ","
        << e    << ","
        << phi  << ","
        << std::endl;
        
        if (event%(Nevents/10)==0) {
            std::cout << 100.*event/Nevents << "%" << std::endl;
        }
    }
    csvfile.close();
    
    std::cout << "Done. Written variables from " << Nevents <<  " entries into a CSV " << csvfilename << std::endl;
    return;
}




/*
 ntuple variables:
 TNtuple ntp_cluster
 Br    2 :ntowers   : Float_t                                                *
 *Entries :      242 : Total  Size=       1540 bytes  File Size  =        225 *
 *Baskets :        1 : Basket Size=      32000 bytes  Compression=   4.66     *
 *............................................................................*
 *Br    3 :eta       : Float_t                                                *
 *Entries :      242 : Total  Size=       1520 bytes  File Size  =       1045 *
 *Baskets :        1 : Basket Size=      32000 bytes  Compression=   1.00     *
 *............................................................................*
 *Br    4 :x         : Float_t                                                *
 *Entries :      242 : Total  Size=       1510 bytes  File Size  =        619 *
 *Baskets :        1 : Basket Size=      32000 bytes  Compression=   1.68     *
 *............................................................................*
 *Br    5 :y         : Float_t                                                *
 *Entries :      242 : Total  Size=       1510 bytes  File Size  =        625 *
 *Baskets :        1 : Basket Size=      32000 bytes  Compression=   1.67     *
 *............................................................................*
 *Br    6 :z         : Float_t                                                *
 *Entries :      242 : Total  Size=       1510 bytes  File Size  =        600 *
 *Baskets :        1 : Basket Size=      32000 bytes  Compression=   1.74     *
 *............................................................................*
 *Br    7 :phi       : Float_t                                                *
 *Entries :      242 : Total  Size=       1520 bytes  File Size  =        667 *
 *Baskets :        1 : Basket Size=      32000 bytes  Compression=   1.57     *
 *............................................................................*
 *Br    8 :e         : Float_t                                                *
 *Entries :      242 : Total  Size=       1510 bytes  File Size  =        499 *
 *Baskets :        1 : Basket Size=      32000 bytes  Compression=   2.09     *
 *............................................................................*
 *Br    9 :gparticleID : Float_t                                              *
 *Entries :      242 : Total  Size=       1560 bytes  File Size  =        216 *
 *Baskets :        1 : Basket Size=      32000 bytes  Compression=   4.88     *
 *............................................................................*
 *Br   10 :gflavor   : Float_t                                                *
 *Entries :      242 : Total  Size=       1540 bytes  File Size  =        214 *
 *Baskets :        1 : Basket Size=      32000 bytes  Compression=   4.90     *
 *............................................................................*
 *Br   11 :gnhits    : Float_t                                                *
 *Entries :      242 : Total  Size=       1535 bytes  File Size  =        205 *
 *Baskets :        1 : Basket Size=      32000 bytes  Compression=   5.11     *
 *............................................................................*
 *Br   12 :geta      : Float_t                                                *
 *Entries :      242 : Total  Size=       1525 bytes  File Size  =        790 *
 *Baskets :        1 : Basket Size=      32000 bytes  Compression=   1.32     *
 *............................................................................*
 *Br   13 :gphi      : Float_t                                                *
 *Entries :      242 : Total  Size=       1525 bytes  File Size  =        791 *
 *Baskets :        1 : Basket Size=      32000 bytes  Compression=   1.32     *
 *............................................................................*
 *Br   14 :ge        : Float_t                                                *
 *Entries :      242 : Total  Size=       1515 bytes  File Size  =        760 *
 *Baskets :        1 : Basket Size=      32000 bytes  Compression=   1.37     *
 *............................................................................*
 *Br   15 :gpt       : Float_t                                                *
 *Entries :      242 : Total  Size=       1520 bytes  File Size  =        765 *
 *Baskets :        1 : Basket Size=      32000 bytes  Compression=   1.37     *
 *............................................................................*
 *Br   16 :gvx       : Float_t                                                *
 *Entries :      242 : Total  Size=       1520 bytes  File Size  =        202 *
 *Baskets :        1 : Basket Size=      32000 bytes  Compression=   5.17     *
 *............................................................................*
 *Br   17 :gvy       : Float_t                                                *
 *Entries :      242 : Total  Size=       1520 bytes  File Size  =        202 *
 *Baskets :        1 : Basket Size=      32000 bytes  Compression=   5.17     *
 *............................................................................*
 *Br   18 :gvz       : Float_t                                                *
 *Entries :      242 : Total  Size=       1520 bytes  File Size  =        792 *
 *Baskets :        1 : Basket Size=      32000 bytes  Compression=   1.32     *
 *............................................................................*
 *Br   19 :gembed    : Float_t                                                *
 *Entries :      242 : Total  Size=       1535 bytes  File Size  =        211 *
 *Baskets :        1 : Basket Size=      32000 bytes  Compression=   4.97     *
 *............................................................................*
 *Br   20 :gedep     : Float_t                                                *
 *Entries :      242 : Total  Size=       1530 bytes  File Size  =        771 *
 *Baskets :        1 : Basket Size=      32000 bytes  Compression=   1.36     *
 *............................................................................*
 *Br   21 :efromtruth : Float_t                                               *
 *Entries :      242 : Total  Size=       1555 bytes  File Size  =        921 *
 *Baskets :        1 : Basket Size=      32000 bytes  Compression=   1.14     *
 *............................................................................*
 */
