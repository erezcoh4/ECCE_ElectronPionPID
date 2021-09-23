// last edit Sep-23,2021

#include <stdio.h>

TString      evalpath;
TString      datapath;
TString        GitTag;
std::ofstream csvfile;

void StreamToCSVfile (std::vector<Double_t> observables );

// Oo.oOo.oOo.oOo.oOo.oOo.oOo.oOo.oOo.oOo.oOo.oOo.oOo.oOo.oOo.oOo.oOo.oOo.
void ConvertROOTdataToCSV(TString filelabel   = "hcalin",
                          TString particleGun = "singleElectron",
                          TString TChainName  = "ntp_cluster",
                          int NeventsMax      = -1,
                          int fdebug = 1){
    
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
    if (filelabel == "tracking") {
        TChainName  = "tracks";
    }
    
    TString ROOTdatapath = datapath + GitTag + "/" + particleGun + "/";
    TString  CSVdatapath = datapath + "/CSVfiles/";
    TString  csvfilename = CSVdatapath + "/" + particleGun + "_" + filelabel + "_" + TChainName + ".csv";
    // open CSV file and write header
    csvfile.open( csvfilename );
    if (filelabel == "tracking") {
        csvfile << "run,event,trackID,charge,nhits,px,py,pz,pcax,pcay,pcaz,dca2d," << std::endl;
    } else {
        csvfile << "run,event,clusterID,eta,x,y,z,e,phi,ge,gflavor," << std::endl;
    }
    
    
    // TChain
    TChain * chain;
    
    // event
    Int_t   runnumber;
    Float_t event;
    Int_t   Nevents;
    
    // for calorimeters
    Float_t clusterID, eta, x, y, z, phi, e, ge, gflavor;
    
    // for tracking
    Int_t   evt;
    Int_t   trackID, charge, nhits;
    Float_t px, py, pz, pcax, pcay, pcaz, dca2d;
    
    // run number
    char runnumberStr[7];

    // load all similar files and combine them to a large TChain
    gSystem->cd( datapath + GitTag );
    TSystemDirectory dire( particleGun, particleGun  );
    std::cout << datapath + GitTag << "/" << dire.GetName() << std::endl;
    TList * files = dire.GetListOfFiles(); //Displays a list of all files
    
    // count how many files we want to process
    Int_t Nfiles = 0, nfile = 0;
    for (auto file:*files) {
        std::string filename = (std::string)file->GetName();
        if (filename.find(filelabel) != std::string::npos) {
            Nfiles++;
        }
    }
    if (fdebug>0) { std::cout << "processing " <<  Nfiles << " " << filelabel << " files " << std::endl; }
    
    int iStart;
    if (particleGun=="singleElectron")  iStart = 43;
    else                                iStart = 39;

    for (auto file:*files) {
        std::string filename = (std::string)file->GetName();
        if (filename.find(filelabel) != std::string::npos) {
            if (fdebug>1) {std::cout << "adding " << filename << std::endl;}
            chain = new TChain(TChainName);
            chain -> Add( ROOTdatapath + filename.c_str() );
            
            // get run number from file name
            for (int i=0;i<7;i++) {runnumberStr[i] = filename[iStart+i];}
            runnumber = atoi(runnumberStr);
                        
            // number of entries
            int Nentries = chain->GetEntries();
            
            // copy TChain variables to a csv
            if (fdebug>1) {std::cout << "processing " << Nentries <<  " entries from run number " << runnumber << std::endl;}
            
            
            if (filelabel == "tracking") {
                if (fdebug>2) { std::cout << "reading tracking " << std::endl;}
                chain -> SetBranchAddress("event",      &evt        );
                // trackID,charge,nhits,px,py,pz,pcax,pcay,pcaz,dca2d
                chain -> SetBranchAddress("trackID",    &trackID    );
                chain -> SetBranchAddress("charge",     &charge     );
                chain -> SetBranchAddress("nhits",      &nhits      );
                chain -> SetBranchAddress("px",         &px         );
                chain -> SetBranchAddress("py",         &py         );
                chain -> SetBranchAddress("pz",         &pz         );
                chain -> SetBranchAddress("pcax",       &pcax       );
                chain -> SetBranchAddress("pcay",       &pcay       );
                chain -> SetBranchAddress("pcaz",       &pcaz       );
                chain -> SetBranchAddress("dca2d",      &dca2d      );
            } else {
                chain -> SetBranchAddress("event",      &event      );
                chain -> SetBranchAddress("clusterID",  &clusterID  );
                chain -> SetBranchAddress("eta",        &eta        );
                chain -> SetBranchAddress("x",          &x          );
                chain -> SetBranchAddress("y",          &y          );
                chain -> SetBranchAddress("z",          &z          );
                chain -> SetBranchAddress("phi",        &phi        );
                chain -> SetBranchAddress("e",          &e          );
                // MC info
                chain -> SetBranchAddress("ge",         &ge          );
                chain -> SetBranchAddress("gflavor",    &gflavor     );
            }
            
            
            Nevents = 0;
            for (int entry=0; entry < Nentries ; entry++){
                if (fdebug>3) { std::cout << "getting entry " << entry << std::endl;}
                chain -> GetEntry(entry);
                
                if (filelabel == "tracking") {
                    StreamToCSVfile( {(Float_t)runnumber,(Float_t)evt,(Float_t)trackID,(Float_t)charge,(Float_t)nhits,px,py,pz,pcax,pcay,pcaz,dca2d} );
                } else {
                    StreamToCSVfile( {(Float_t)runnumber,event,clusterID,eta,x,y,z,e,phi,ge,gflavor} );
                }
                if ((NeventsMax>0) && (Nevents > NeventsMax)) {
                    break;
                }
                Nevents++
            }
            if ((NeventsMax>0) && (Nevents > NeventsMax)) {
                break;
            }
            if (fdebug>3) { std::cout << "print progress: done file " << nfile << " out of " << Nfiles << " files" << std::endl;}
            // print progress
            if (Nfiles<10) {
                std::cout << "Done file " << nfile << " out of " << Nfiles << " files" << std::endl;
            }
            else if (nfile%(Nfiles/10)==0) {
                std::cout << std::setprecision(3) << 100.*nfile/Nfiles << "%" << std::endl;
            }
            nfile++;
        }
    }
    
    csvfile.close();
    
    std::cout << "Done reading " << Nevents << " events. Written variables into the CSV: " << csvfilename << std::endl;
    return;
}



// Oo.oOo.oOo.oOo.oOo.oOo.oOo.oOo.oOo.oOo.oOo.oOo.oOo.oOo.oOo.oOo.oOo.oOo.
void StreamToCSVfile ( std::vector<Double_t> observables ){
    for (auto v:observables) {
        csvfile << v << ",";
    }
    csvfile << std::endl;
}


