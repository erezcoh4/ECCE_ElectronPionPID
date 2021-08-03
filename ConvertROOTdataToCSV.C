/*
 convert EIC eval ROOT files to CSV
 
 execute:
 ---------------------
 root -l ConvertROOTdataToCSV.C(filelabel,particleGun,TChainName)
 root -l ConvertROOTdataToCSV.C
 root -l 'ConvertROOTdataToCSV.C("hcalin","singleElectron","ntp_cluster")'
 root -l 'ConvertROOTdataToCSV.C("hcalout","singleElectron","ntp_cluster")'
 
 
 supported file-labels:
 
    hcalin hcalout fhcal tracking
 
 */


#include <stdio.h>

TString      evalpath;
TString      datapath;
TString        GitTag;
std::ofstream csvfile;

void StreamToCSVfile (std::vector<Double_t> observables );

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
void ConvertROOTdataToCSV(TString filelabel   = "hcalin",
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
    
    // for calorimeters
    Float_t eta, x, y, z, phi, e;
    
    // for tracking
    Int_t   trackID, charge, nhits;
    Float_t px, py, pz, pcax, pcay, pcaz, dca2d;
    
    if (filelabel == "tracking") {
        TChainName  = "tracks";
    }
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
    if (filelabel == "tracking") {
        // trackID,charge,nhits,px,py,pz,pcax,pcay,pcaz,dca2d
        chain -> SetBranchAddress("trackID",    &trackID);
        chain -> SetBranchAddress("charge",     &charge );
        chain -> SetBranchAddress("nhits",      &nhits  );
        chain -> SetBranchAddress("px",         &px     );
        chain -> SetBranchAddress("py",         &py     );
        chain -> SetBranchAddress("pz",         &pz     );
        chain -> SetBranchAddress("pcax",       &pcax   );
        chain -> SetBranchAddress("pcay",       &pcay   );
        chain -> SetBranchAddress("pcaz",       &pcaz   );
        chain -> SetBranchAddress("dca2d",      &dca2d  );
    } else {
        chain -> SetBranchAddress("eta",        &eta    );
        chain -> SetBranchAddress("x",          &x      );
        chain -> SetBranchAddress("y",          &y      );
        chain -> SetBranchAddress("z",          &z      );
        chain -> SetBranchAddress("phi",        &phi    );
        chain -> SetBranchAddress("e",          &e      );
    }
    csvfile.open( csvfilename );
    
    if (filelabel == "tracking") {
        csvfile << "trackID,charge,nhits,px,py,pz,pcax,pcay,pcaz,dca2d," << std::endl;
    } else {
        csvfile << "eta,x,y,z,e,phi," << std::endl;
    }
    for (int event=0; event < Nevents ; event++){
        chain -> GetEntry(event);
        
        if (filelabel == "tracking") {
            StreamToCSVfile( {trackID,charge,nhits,px,py,pz,pcax,pcay,pcaz,dca2d} );
        } else {
            StreamToCSVfile( {eta,x,y,z,e,phi} );
        }
        if (event%(Nevents/10)==0) {
            std::cout << 100.*event/Nevents << "%" << std::endl;
        }
    }
    csvfile.close();
    
    std::cout << "Done. Written variables from " << Nevents <<  " entries into a CSV " << csvfilename << std::endl;
    return;
}



// Oo.oOo.oOo.oOo.oOo.oOo.oOo.oOo.oOo.oOo.oOo.oOo.oOo.oOo.oOo.oOo.oOo.oOo.
void StreamToCSVfile ( std::vector<Double_t> observables ){
    for (auto v:observables) {
        csvfile << v << ",";
    }
    csvfile << std::endl;
}




/*
 ----------
 calin
 /
 calout
 ----------
 
 
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
 

 
 
 ----------
 fhcal
 ----------




******************************************************************************
*Tree    :ntp_cluster: cluster => max truth primary                           *
*Entries :       72 : Total =           19146 bytes  File  Size =       7061 *
*        :          : Tree compression factor =   1.48                       *
******************************************************************************
*Br    0 :event     : Float_t                                                *
*Entries :       72 : Total  Size=        850 bytes  File Size  =        257 *
*Baskets :        1 : Basket Size=      32000 bytes  Compression=   1.43     *
*............................................................................*
*Br    1 :clusterID : Float_t                                                *
*Entries :       72 : Total  Size=        870 bytes  File Size  =        150 *
*Baskets :        1 : Basket Size=      32000 bytes  Compression=   2.47     *
*............................................................................*
*Br    2 :ntowers   : Float_t                                                *
*Entries :       72 : Total  Size=        860 bytes  File Size  =        169 *
*Baskets :        1 : Basket Size=      32000 bytes  Compression=   2.18     *
*............................................................................*
*Br    3 :eta       : Float_t                                                *
*Entries :       72 : Total  Size=        840 bytes  File Size  =        365 *
*Baskets :        1 : Basket Size=      32000 bytes  Compression=   1.00     *
*............................................................................*
*Br    4 :x         : Float_t                                                *
*Entries :       72 : Total  Size=        830 bytes  File Size  =        306 *
*Baskets :        1 : Basket Size=      32000 bytes  Compression=   1.19     *
*............................................................................*
*Br    5 :y         : Float_t                                                *
*Entries :       72 : Total  Size=        830 bytes  File Size  =        293 *
*Baskets :        1 : Basket Size=      32000 bytes  Compression=   1.24     *
*............................................................................*
*Br    6 :z         : Float_t                                                *
*Entries :       72 : Total  Size=        830 bytes  File Size  =        101 *
*Baskets :        1 : Basket Size=      32000 bytes  Compression=   3.59     *
*............................................................................*
*Br    7 :phi       : Float_t                                                *
*Entries :       72 : Total  Size=        840 bytes  File Size  =        348 *
*Baskets :        1 : Basket Size=      32000 bytes  Compression=   1.05     *
*............................................................................*
*Br    8 :e         : Float_t                                                *
*Entries :       72 : Total  Size=        830 bytes  File Size  =        363 *
*Baskets :        1 : Basket Size=      32000 bytes  Compression=   1.00     *
*............................................................................*
*Br    9 :gparticleID : Float_t                                              *
*Entries :       72 : Total  Size=        880 bytes  File Size  =        111 *
*Baskets :        1 : Basket Size=      32000 bytes  Compression=   3.36     *
*............................................................................*
*Br   10 :gflavor   : Float_t                                                *
*Entries :       72 : Total  Size=        860 bytes  File Size  =        107 *
*Baskets :        1 : Basket Size=      32000 bytes  Compression=   3.45     *
*............................................................................*
*Br   11 :gnhits    : Float_t                                                *
*Entries :       72 : Total  Size=        855 bytes  File Size  =        103 *
*Baskets :        1 : Basket Size=      32000 bytes  Compression=   3.57     *
*............................................................................*
*Br   12 :geta      : Float_t                                                *
*Entries :       72 : Total  Size=        845 bytes  File Size  =        343 *
*Baskets :        1 : Basket Size=      32000 bytes  Compression=   1.07     *
*............................................................................*
*Br   13 :gphi      : Float_t                                                *
*Entries :       72 : Total  Size=        845 bytes  File Size  =        354 *
*Baskets :        1 : Basket Size=      32000 bytes  Compression=   1.03     *
*............................................................................*
*Br   14 :ge        : Float_t                                                *
*Entries :       72 : Total  Size=        835 bytes  File Size  =        343 *
*Baskets :        1 : Basket Size=      32000 bytes  Compression=   1.06     *
*............................................................................*
*Br   15 :gpt       : Float_t                                                *
*Entries :       72 : Total  Size=        840 bytes  File Size  =        347 *
*Baskets :        1 : Basket Size=      32000 bytes  Compression=   1.05     *
*............................................................................*
*Br   16 :gvx       : Float_t                                                *
*Entries :       72 : Total  Size=        840 bytes  File Size  =        100 *
*Baskets :        1 : Basket Size=      32000 bytes  Compression=   3.65     *
*............................................................................*
*Br   17 :gvy       : Float_t                                                *
*Entries :       72 : Total  Size=        840 bytes  File Size  =        100 *
*Baskets :        1 : Basket Size=      32000 bytes  Compression=   3.65     *
*............................................................................*
*Br   18 :gvz       : Float_t                                                *
*Entries :       72 : Total  Size=        840 bytes  File Size  =        347 *
*Baskets :        1 : Basket Size=      32000 bytes  Compression=   1.05     *
*............................................................................*
*Br   19 :gembed    : Float_t                                                *
*Entries :       72 : Total  Size=        855 bytes  File Size  =        106 *
*Baskets :        1 : Basket Size=      32000 bytes  Compression=   3.47     *
*............................................................................*
*Br   20 :gedep     : Float_t                                                *
*Entries :       72 : Total  Size=        850 bytes  File Size  =        347 *
*Baskets :        1 : Basket Size=      32000 bytes  Compression=   1.06     *
*............................................................................*
*Br   21 :efromtruth : Float_t                                               *
*Entries :       72 : Total  Size=        875 bytes  File Size  =        372 *
*Baskets :        1 : Basket Size=      32000 bytes  Compression=   1.00     *
*............................................................................*



 
 ------------
 g4tracking
 ------------
 
  KEY: TTree	tracks;1	FastSim Eval => tracks
  KEY: TTree	vertex;1	FastSim Eval => vertces
  KEY: TH2D	DeltaMomVsTruthEta;1	#frac{#Delta p}{truth p} vs. truth #eta
  KEY: TH2D	DeltaMomVsTruthMom;1	#frac{#Delta p}{truth p} vs. truth p
 
 
 tracks->Print()
 ******************************************************************************
 *Tree    :tracks    : FastSim Eval => tracks                                 *
 *Entries :     1000 : Total =          184009 bytes  File  Size =      57798 *
 *        :          : Tree compression factor =   2.95                       *
 ******************************************************************************
 *Br    0 :event     : event/I                                                *
 *Entries :     1000 : Total  Size=       4559 bytes  File Size  =       1518 *
 *Baskets :        1 : Basket Size=      32000 bytes  Compression=   2.68     *
 *............................................................................*
 *Br    1 :gtrackID  : gtrackID/I                                             *
 *Entries :     1000 : Total  Size=       4574 bytes  File Size  =        132 *
 *Baskets :        1 : Basket Size=      32000 bytes  Compression=  30.89     *
 *............................................................................*
 *Br    2 :gflavor   : gflavor/I                                              *
 *Entries :     1000 : Total  Size=       4569 bytes  File Size  =        132 *
 *Baskets :        1 : Basket Size=      32000 bytes  Compression=  30.88     *
 *............................................................................*
 *Br    3 :gpx       : gpx/F                                                  *
 *Entries :     1000 : Total  Size=       4549 bytes  File Size  =       3892 *
 *Baskets :        1 : Basket Size=      32000 bytes  Compression=   1.05     *
 *............................................................................*
 *Br    4 :gpy       : gpy/F                                                  *
 *Entries :     1000 : Total  Size=       4549 bytes  File Size  =       3893 *
 *Baskets :        1 : Basket Size=      32000 bytes  Compression=   1.05     *
 *............................................................................*
 *Br    5 :gpz       : gpz/F                                                  *
 *Entries :     1000 : Total  Size=       4549 bytes  File Size  =       3842 *
 *Baskets :        1 : Basket Size=      32000 bytes  Compression=   1.06     *
 *............................................................................*
 *Br    6 :gvx       : gvx/F                                                  *
 *Entries :     1000 : Total  Size=       4549 bytes  File Size  =        122 *
 *Baskets :        1 : Basket Size=      32000 bytes  Compression=  33.38     *
 *............................................................................*
 *Br    7 :gvy       : gvy/F                                                  *
 *Entries :     1000 : Total  Size=       4549 bytes  File Size  =        122 *
 *Baskets :        1 : Basket Size=      32000 bytes  Compression=  33.38     *
 *............................................................................*
 *Br    8 :gvz       : gvz/F                                                  *
 *Entries :     1000 : Total  Size=       4549 bytes  File Size  =       3801 *
 *Baskets :        1 : Basket Size=      32000 bytes  Compression=   1.07     *
 *............................................................................*
 *Br    9 :gvt       : gvt/F                                                  *
 *Entries :     1000 : Total  Size=       4549 bytes  File Size  =        122 *
 *Baskets :        1 : Basket Size=      32000 bytes  Compression=  33.38     *
 *............................................................................*
 *Br   10 :trackID   : trackID/I                                              *
 *Entries :     1000 : Total  Size=       4569 bytes  File Size  =        279 *
 *Baskets :        1 : Basket Size=      32000 bytes  Compression=  14.61     *
 *............................................................................*
 *Br   11 :charge    : charge/I                                               *
 *Entries :     1000 : Total  Size=       4564 bytes  File Size  =        304 *
 *Baskets :        1 : Basket Size=      32000 bytes  Compression=  13.40     *
 *............................................................................*
 *Br   12 :nhits     : nhits/I                                                *
 *Entries :     1000 : Total  Size=       4559 bytes  File Size  =        277 *
 *Baskets :        1 : Basket Size=      32000 bytes  Compression=  14.71     *
 *............................................................................*
 *Br   13 :px        : px/F                                                   *
 *Entries :     1000 : Total  Size=       4544 bytes  File Size  =       3803 *
 *Baskets :        1 : Basket Size=      32000 bytes  Compression=   1.07     *
 *............................................................................*
 *Br   14 :py        : py/F                                                   *
 *Entries :     1000 : Total  Size=       4544 bytes  File Size  =       3812 *
 *Baskets :        1 : Basket Size=      32000 bytes  Compression=   1.07     *
 *............................................................................*
 *Br   15 :pz        : pz/F                                                   *
 *Entries :     1000 : Total  Size=       4544 bytes  File Size  =       3745 *
 *Baskets :        1 : Basket Size=      32000 bytes  Compression=   1.09     *
 *............................................................................*
 *Br   16 :pcax      : pcax/F                                                 *
 *Entries :     1000 : Total  Size=       4554 bytes  File Size  =       3782 *
 *Baskets :        1 : Basket Size=      32000 bytes  Compression=   1.08     *
 *............................................................................*
 *Br   17 :pcay      : pcay/F                                                 *
 *Entries :     1000 : Total  Size=       4554 bytes  File Size  =       3774 *
 *Baskets :        1 : Basket Size=      32000 bytes  Compression=   1.08     *
 *............................................................................*
 *Br   18 :pcaz      : pcaz/F                                                 *
 *Entries :     1000 : Total  Size=       4554 bytes  File Size  =       3739 *
 *Baskets :        1 : Basket Size=      32000 bytes  Compression=   1.09     *
 *............................................................................*
 *Br   19 :dca2d     : dca2d/F                                                *
 *Entries :     1000 : Total  Size=       4559 bytes  File Size  =       3808 *
 *Baskets :        1 : Basket Size=      32000 bytes  Compression=   1.07     *
 *............................................................................*
 *Br   20 :nHit_G4HIT_BARR : nHit_G4HIT_BARR/I                                *
 *Entries :     1000 : Total  Size=       4609 bytes  File Size  =        543 *
 *Baskets :        1 : Basket Size=      32000 bytes  Compression=   7.52     *
 *............................................................................*
 *Br   21 :nHit_G4HIT_CTTL_0 : nHit_G4HIT_CTTL_0/I                            *
 *Entries :     1000 : Total  Size=       4619 bytes  File Size  =        564 *
 *Baskets :        1 : Basket Size=      32000 bytes  Compression=   7.24     *
 *............................................................................*
 *Br   22 :nHit_G4HIT_EFST_0 : nHit_G4HIT_EFST_0/I                            *
 *Entries :     1000 : Total  Size=       4619 bytes  File Size  =        477 *
 *Baskets :        1 : Basket Size=      32000 bytes  Compression=   8.57     *
 *............................................................................*
 *Br   23 :nHit_G4HIT_EFST_1 : nHit_G4HIT_EFST_1/I                            *
 *Entries :     1000 : Total  Size=       4619 bytes  File Size  =        520 *
 *Baskets :        1 : Basket Size=      32000 bytes  Compression=   7.86     *
 *............................................................................*
 *Br   24 :nHit_G4HIT_EFST_2 : nHit_G4HIT_EFST_2/I                            *
 *Entries :     1000 : Total  Size=       4619 bytes  File Size  =        514 *
 *Baskets :        1 : Basket Size=      32000 bytes  Compression=   7.95     *
 *............................................................................*
 *Br   25 :nHit_G4HIT_EFST_3 : nHit_G4HIT_EFST_3/I                            *
 *Entries :     1000 : Total  Size=       4619 bytes  File Size  =        493 *
 *Baskets :        1 : Basket Size=      32000 bytes  Compression=   8.29     *
 *............................................................................*
 *Br   26 :nHit_G4HIT_EFST_4 : nHit_G4HIT_EFST_4/I                            *
 *Entries :     1000 : Total  Size=       4619 bytes  File Size  =        490 *
 *Baskets :        1 : Basket Size=      32000 bytes  Compression=   8.34     *
 *............................................................................*
 *Br   27 :nHit_G4HIT_EGEM_0 : nHit_G4HIT_EGEM_0/I                            *
 *Entries :     1000 : Total  Size=       4619 bytes  File Size  =        515 *
 *Baskets :        1 : Basket Size=      32000 bytes  Compression=   7.93     *
 *............................................................................*
 *Br   28 :nHit_G4HIT_ETTL_0 : nHit_G4HIT_ETTL_0/I                            *
 *Entries :     1000 : Total  Size=       4619 bytes  File Size  =        510 *
 *Baskets :        1 : Basket Size=      32000 bytes  Compression=   8.01     *
 *............................................................................*
 *Br   29 :nHit_G4HIT_ETTL_1 : nHit_G4HIT_ETTL_1/I                            *
 *Entries :     1000 : Total  Size=       4619 bytes  File Size  =        510 *
 *Baskets :        1 : Basket Size=      32000 bytes  Compression=   8.01     *
 *............................................................................*
 *Br   30 :nHit_G4HIT_FGEM_0 : nHit_G4HIT_FGEM_0/I                            *
 *Entries :     1000 : Total  Size=       4619 bytes  File Size  =        506 *
 *Baskets :        1 : Basket Size=      32000 bytes  Compression=   8.08     *
 *............................................................................*
 *Br   31 :nHit_G4HIT_FST_0 : nHit_G4HIT_FST_0/I                              *
 *Entries :     1000 : Total  Size=       4614 bytes  File Size  =        487 *
 *Baskets :        1 : Basket Size=      32000 bytes  Compression=   8.39     *
 *............................................................................*
 *Br   32 :nHit_G4HIT_FST_1 : nHit_G4HIT_FST_1/I                              *
 *Entries :     1000 : Total  Size=       4614 bytes  File Size  =        526 *
 *Baskets :        1 : Basket Size=      32000 bytes  Compression=   7.77     *
 *............................................................................*
 *Br   33 :nHit_G4HIT_FST_2 : nHit_G4HIT_FST_2/I                              *
 *Entries :     1000 : Total  Size=       4614 bytes  File Size  =        510 *
 *Baskets :        1 : Basket Size=      32000 bytes  Compression=   8.01     *
 *............................................................................*
 *Br   34 :nHit_G4HIT_FST_3 : nHit_G4HIT_FST_3/I                              *
 *Entries :     1000 : Total  Size=       4614 bytes  File Size  =        480 *
 *Baskets :        1 : Basket Size=      32000 bytes  Compression=   8.51     *
 *............................................................................*
 *Br   35 :nHit_G4HIT_FST_4 : nHit_G4HIT_FST_4/I                              *
 *Entries :     1000 : Total  Size=       4614 bytes  File Size  =        487 *
 *Baskets :        1 : Basket Size=      32000 bytes  Compression=   8.39     *
 *............................................................................*
 *Br   36 :nHit_G4HIT_FTTL_0 : nHit_G4HIT_FTTL_0/I                            *
 *Entries :     1000 : Total  Size=       4619 bytes  File Size  =        528 *
 *Baskets :        1 : Basket Size=      32000 bytes  Compression=   7.74     *
 *............................................................................*
 *Br   37 :nHit_G4HIT_FTTL_1 : nHit_G4HIT_FTTL_1/I                            *
 *Entries :     1000 : Total  Size=       4619 bytes  File Size  =        528 *
 *Baskets :        1 : Basket Size=      32000 bytes  Compression=   7.74     *
 *............................................................................*
 *Br   38 :nHit_G4HIT_RWELL_0 : nHit_G4HIT_RWELL_0/I                          *
 *Entries :     1000 : Total  Size=       4624 bytes  File Size  =        540 *
 *Baskets :        1 : Basket Size=      32000 bytes  Compression=   7.57     *
 *............................................................................*
 *Br   39 :nHit_G4HIT_SVTX : nHit_G4HIT_SVTX/I                                *
 *Entries :     1000 : Total  Size=       4609 bytes  File Size  =        661 *
 *Baskets :        1 : Basket Size=      32000 bytes  Compression=   6.18     *
 *............................................................................*

*/
