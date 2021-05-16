#ifndef WAVEFORM_H
#define WAVEFORM_H

#include "TGraph.h"
#include "TSpline.h"
#include "TF1.h"

class waveform {
 public:
  waveform();
  ~waveform();
  waveform(TGraph *);
  waveform(Float_t x[1024], Float_t y[1024]);
  void process();
  TGraph* GetGraph();
  void loadTemplate(TString csvfile);
  void fitEnergy(Double_t baseline, Double_t t30, Double_t amplitude);
  Double_t GetAmplitude() {return fAmp;}
  Int_t GetAmplitudeBin() {return fAmpBin;}
  Double_t GetTime() {return fTime;}
  Double_t GetBaseMean() {return fBaseMean;}
  Double_t GetBaseRMS() {return fBaseRMS;}
  Double_t FixThreshold(Double_t thr_in_mV);
  
 private:
  void statX(Double_t &mean, Double_t &rms,Int_t ini, Int_t fin);
  void offsetY(Double_t y);
  void minimumY(Double_t &mean, Int_t &bin,Int_t ini, Int_t fin);
  Double_t timingCTD(Double_t thr, Int_t ini, Int_t fin );
  
  Float_t fX[1024];
  Float_t fY[1024];
  Double_t fBaseMean; // baseline mean
  Double_t fBaseRMS;  // baseline rms
  Double_t fAmp;      // raw amplitude
  Int_t    fAmpBin;   // raw tbin for amplitude
  Double_t fTime;     // raw time 30% from CFD

  TSpline3 *fPulseShape;
  TF1 *fFit;
};

#endif
