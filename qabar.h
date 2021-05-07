#ifndef QABAR_H
#define QABAR_H

#include "waveform.h"
#include "bar.h"
class TH1D;
class TH2D;
class TProfile;

class qabar {
 public:
  qabar();
  ~qabar();
  void fill(bar *mybar);
  void saveas(TString file);
  
 private:
  TH1D *base_mean[4];
  TH1D *base_rms[4];
  TH2D *min_spot[4];
  TH2D *thr_spot[4];
  TProfile *shape_class[3][4];
  Double_t xclass[4][4];

};

#endif
