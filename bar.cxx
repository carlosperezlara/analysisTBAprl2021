#include "bar.h"
#include "waveform.h"

bar::bar() {
  for(int i=0; i!=2; ++i) {
    fLow[i] = 0;
    fHigh[i] = 0;
  }
}
//===========
bar::~bar() {
  for(int i=0; i!=2; ++i) {
    if(fLow[i]) {
      delete fLow[i];
      fLow[i] = 0;
    }
    if(fHigh[i]) {
      delete fHigh[i];
      fHigh[i] = 0;
    }
  }
}
//===========
void bar::process() {
  for(int iSiPM=0; iSiPM!=2; ++iSiPM) {
    // quick process of waveforms, baseline subtraction
    fLow[iSiPM]->process();
    fHigh[iSiPM]->process();
    fEnergy[iSiPM] = fLow[iSiPM]->GetAmplitude();
    fTime[iSiPM] = fHigh[iSiPM]->FixThreshold( 500 );
  }
  
}
//===========
void bar::AddLow(Int_t idx, waveform *wf) {
  if((idx<0)||(idx>=2)) return;
  if( fLow[idx] ) delete fLow[idx];
  fLow[idx] = wf;
}
//===========
void bar::AddHigh(Int_t idx, waveform *wf) {
  if((idx<0)||(idx>=2)) return;
  if( fHigh[idx] ) delete fHigh[idx];
  fHigh[idx] = wf;
}
//===========
waveform* bar::GetLow(Int_t idx) {
  if((idx<0)||(idx>=2)) return 0;
  return fLow[idx];
}
//===========
waveform* bar::GetHigh(Int_t idx) {
  if((idx<0)||(idx>=2)) return 0;
  return fHigh[idx];
}
