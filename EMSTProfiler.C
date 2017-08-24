#include "TFile.h"
#include "TH1.h"
#include "TLegend.h"
#include <fstream>
#include <string>
#include <cstdio>
#include <cstdlib>
#include <vector>
#include "TGraph.h"
#include <cmath>

class EMSTProfiler {
  public:
    EMSTProfiler() { }
    void load(const char* inFile, int verbose = 0);
    void plotGraph();
    void saveGraph();
    void writeConstants();
  private:
    TGraph* amplgraph_;
    const char* getJobName(const char* inFile, int verbose = 0);
    int getCharIndex(const char* Charray, char Char, int stopNumber = 1, int limit = 1000);
};

//const double en_per_MIP[34]={2.608, 5.596, 8.403, 10.760, 14.347, 14.301, 15.295, 15.027, 15.989, 15.208, 16.253, 15.534, 16.347, 15.766, 16.810, 15.583, 16.629, 15.700, 16.487, 15.895, 16.890, 15.906, 24.398, 29.422, 31.120, 29.727, 30.717, 29.192, 31.088, 30.133, 31.441, 28.979, 30.771, 29.042};
const double en_per_MIP[34]={1.0,1.0,1.0,1.0,1.0,1.0,1.0,1.0,1.0,1.0,1.0,1.0,1.0,1.0,1.0,1.0,1.0,1.0,1.0,1.0,1.0,1.0,1.0,1.0,1.0,1.0,1.0,1.0,1.0,1.0,1.0,1.0,1.0,1.0};

void EMSTProfiler::load(const char* inFile, int verbose = 0) {
  TFile fin(inFile);

  if (!fin.IsOpen()) return;
  char pdfFileName[1024];
  sprintf(pdfFileName,"%s.pdf",getJobName(inFile, verbose));
// plot ProfileX
  TH2* hist2D=(TH2*)(fin.Get("analyzer/LayerEnergy2D"));
  const int nbins=34;
  double t[34],terr[34],ampl[34],amplerr[34];

  TProfile* profile = hist2D->ProfileX(getJobName(inFile, verbose),1,3500);
  double totalT=0;
  for (int i = 1; i <= profile->GetNbinsX(); i++) {
    totalT=totalT+en_per_MIP[i-1];
    t[i-1]=totalT;
    terr[i-1]=0;
    ampl[i-1]=profile->GetBinContent(i)/en_per_MIP[i-1];
    amplerr[i-1]=std::sqrt(ampl[i-1]); // FIXME, eventually

//    amplerr[i-1]/=sqrt(profile->GetBinEntries(i)/100000.0);
  }
//  amplgraph_=new TGraphErrors(nbins,t,ampl,terr,amplerr);
  TCanvas* c = new TCanvas("ca","ca",1500,1000);
  profile->SetTitle("");
  profile->GetXaxis()->SetTitle("ECAL Layer Number");
  profile->GetYaxis()->SetTitle("Average Energy Deposited In Each Layer [MeV]");
  profile->Draw();
//  amplpraph_->SetTitle("");
//  amplgraph_->GetXaxis()->SetTitle("ECAL Layer Number");
//  amplgraph_->GetYaxis()->SetTitle("Average Energy Deposited In Each Layer [MeV]");
  c->Print(pdfFileName);
}

void EMSTProfiler::plotGraph() {
  amplgraph_->Draw();
}

void EMSTProfiler::writeConstants() {
  printf("  const double values[34]={");
  for (int i=0; i<34; i++) 
    printf("%.3f, ",amplgraph_->GetY()[i]);
  printf("};\n");
}

const char* EMSTProfiler::getJobName(const char* inFile,  int verbose = 0) {
  if (verbose == 1) {cout << "getJobName: getting job name..." << endl;}
  int dot_index = getCharIndex(inFile,'.');
  if (verbose == 1) {cout << "getJobName: got dot_index: " << dot_index << endl;}
  int slash_index = getCharIndex(inFile,'/',0,dot_index);
  if (verbose == 1) {cout << "getJobName: got slash_index: " << slash_index << endl;}
  int array_size = dot_index - slash_index;
  char jobname[array_size];
  for (int i = 0; i < array_size-1; i++) {
    if (verbose == 1) {cout << "getJobName: got inFile char: " << inFile[slash_index+i+1] << endl;}
    jobname[i] = inFile[slash_index+i+1];
    if (verbose == 1) {cout << "getJobName: save jobname char: " << jobname[i] << endl;}
  }
  jobname[array_size-1] = '\0';
  if (verbose == 1) {cout << "getJobName: jobname written: " << jobname << endl;}
  static char* jobpointer = jobname;
  return jobpointer;
}

int EMSTProfiler::getCharIndex(const char* Charray, char Char, int stopNumber = 1, int limit = 1000) {
  vector<int> indices;
  for (int i = 0; i < limit; i++) {
    if (Charray[i] == Char) {
      indices.push_back(i);
      if (indices.size() == stopNumber) {break;}
    }
  }
  if (indices.size() == 0) {
    cout << "getCharIndex: " << Char << " not found!" << endl;
    cout << "getCharIndex: limit = " << limit << endl;
    return 0;
  }
  else if (abs(stopNumber) > indices.size()) {
    cout << "getCharIndex: Found " << indices.size() << " \'" << Char << "\', cannot take number " << stopNumber << endl;
    return 0;
  }
  else if (stopNumber == 0) {
    return indices.back();
  }
  else if (stopNumber < 0) {
    return indices[indices.size()+stopNumber];
  }
  else if (stopNumber > 0) {
    return indices[stopNumber-1];
  }
  else {
    return 0;
  }
}
