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
};

#endif
