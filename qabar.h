#ifndef QABAR_H
#define QABAR_H

#include "TString.h"
#include "waveform.h"
#include "bar.h"
class TH1D;
class TH2D;
class TProfile;

class qabar {
 public:
  qabar(TString name);
  ~qabar();
  void fill(bar *mybar);
  void saveas(TString file);
  void makesummary(TString file);
  
 private:
  TH1D *base_mean[4];
  TH1D *base_rms[4];
  TH2D *wave_all[4];
  TH1D *amp_dist[4];
  TH2D *min_spot[4];
  TH2D *thr_spot[4];
  TProfile *shape_class[3][4];
  Double_t xclass[4][4];

};

#endif
