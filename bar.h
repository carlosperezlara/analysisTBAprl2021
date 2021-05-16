#ifndef BAR_H
#define BAR_H

#include "waveform.h"

class bar {
 public:
  bar();
  ~bar();
  waveform* GetLow(Int_t);
  waveform* GetHigh(Int_t);
  void AddLow( Int_t idx, waveform *wf );
  void AddHigh( Int_t idx, waveform *wf );
  void process();
  
 private:
  waveform *fLow[2];
  waveform *fHigh[2];
  Double_t fEnergy[2];
  Double_t fTime[2];

  TH2D *fHist_Walk[2];
  TH1D *fHist_Energy[2];
  TH1D *fHist_Time[2];
  
};

#endif
