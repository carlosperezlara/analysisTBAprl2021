#include <iostream>
#include "TFile.h"
#include "TTree.h"
#include "TStyle.h"

#include "TGraph.h"
#include "TCanvas.h"

#include "waveform.h"
#include "bar.h"
#include "qabar.h"

void qa_bar(bar *prototype) {
  waveform *lowA = prototype->GetLow(0);
  lowA->process();
  std::cout << lowA->GetAmplitude() << " " << lowA->GetTime() << std::endl;
}

//void loadlogbook() {
//  ifstream fin;
//  
//}

//===========================

int main(int nvar, char** carg) {
  //////////////////////////////////////
  if(nvar<2) return 1;
  TString crun = carg[1];
  std::cout << crun.Data() << std::endl;
  int run = crun.Atoi();//44734;
  //////////////////////////////////////
  TString inputfile = Form("/Volumes/uva/analysis/rootfiles/Run_%d.root",run);
  std::cout << "Readding from " << inputfile.Data() << std::endl;
  
  struct pulse_t {
    Int_t event;
    Short_t tc[2];
    Float_t channel[18][1024];
    Float_t times[2][1024];
    Float_t xIntercept;
    Float_t yIntercept;
    Float_t xSlope;
    Float_t ySlope;
    Float_t chi2;
    Int_t   ntracks;
  } pulse;
  

  gStyle->SetOptStat(0);
  qabar *mybarqa[4];
  for(int i=0; i!=4; ++i)
    mybarqa[i] = new qabar();
  
  TFile *file = new TFile( inputfile.Data() );
  TTree *tree = (TTree*) file->Get("pulse");
  tree->SetBranchAddress("channel",&pulse.channel);
  tree->SetBranchAddress("times",&pulse.times);
  tree->SetBranchAddress("xIntercept",&pulse.xIntercept);
  tree->SetBranchAddress("yIntercept",&pulse.yIntercept);
  tree->SetBranchAddress("xSlope",&pulse.xSlope);
  tree->SetBranchAddress("ySlope",&pulse.ySlope);
  tree->SetBranchAddress("chi2",&pulse.chi2);
  //tree->Print();

  Long_t nevts = tree->GetEntries();
  //nevts = 49;

  waveform *DRSchan[16];
  bar *prototype[4];
  for(int i=0; i!=4; ++i) prototype[i] = new bar();
  //TCanvas *main = new TCanvas();
  //main->Divide(4,4);
  for(int i=0; i!=nevts; ++i) {
    tree->GetEntry(i);
    if((i%500)==0)
      std::cout << "Events processed: " << i << std::endl;

    for(int ich=0; ich!=8; ++ich) {
      DRSchan[ich]   = new waveform( pulse.times[0], pulse.channel[ich]   );
      DRSchan[ich+8] = new waveform( pulse.times[1], pulse.channel[ich+9] );
    }
    for(int ibar=0; ibar!=4; ++ibar) {
      waveform *lowA  = DRSchan[8+ibar*2];
      waveform *lowB  = DRSchan[9+ibar*2];
      waveform *highA = DRSchan[0+ibar*2];
      waveform *highB = DRSchan[1+ibar*2];
      prototype[ibar]->AddHigh( 0, highA );
      prototype[ibar]->AddHigh( 1, highB );
      prototype[ibar]->AddLow(  0, lowA  );
      prototype[ibar]->AddLow(  1, lowB  );
      lowA->process();
      lowB->process();
      highA->process();
      highB->process();
      /*
      main->cd(1+ibar*4); lowA->GetGraph()->Draw("A*");
      main->cd(2+ibar*4); lowB->GetGraph()->Draw("A*");
      main->cd(3+ibar*4); highA->GetGraph()->Draw("A*");
      main->cd(4+ibar*4); highB->GetGraph()->Draw("A*");
      */
      mybarqa[ibar]->fill( prototype[ibar] );
    }
    //if(i<50) main->SaveAs("check.pdf[","pdf");

  }
  //main->SaveAs("check.pdf]","pdf");
  for(int i=0; i!=4; ++i) {
    mybarqa[i]->saveas( Form("pdf/QA_Run%d_Bar%d",run,i) );
    mybarqa[i]->makesummary( Form("log/QA_Run%d_Bar%d",run,i) );
  }
  return 0;
}
