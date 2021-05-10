#include "waveform.h"
#include "TSpline.h"
#include "TF1.h"

//===========
waveform::waveform() {
  for(int i=0; i!=1024; ++i) {
    fX[i] = fY[i] = 0;
  }
  fBaseMean = 0;
  fBaseRMS = 0;
  fAmp = 0;
  fAmpBin = 0;
  fTime = 0;
  fPulseShape = NULL;
  fFit = NULL;
}
//===========
waveform::~waveform() {
  if(fPulseShape) {
    delete fPulseShape;
    fPulseShape = 0;
  }
  if(fFit) {
    delete fFit;
    fFit = 0;
  }
}
//===========
waveform::waveform(Float_t xxx[1024], Float_t yyy[1024]) {
  for(int i=0; i!=1024; ++i) {
    fX[i] = xxx[i];
    fY[i] = yyy[i];
  }
  fBaseMean = 0;
  fBaseRMS = 0;
  fAmp = 0;
  fAmpBin = 0;
  fTime = 0;
  fPulseShape = NULL;
  fFit = NULL;
}
//===========
waveform::waveform(TGraph *gr) {
  int max = gr->GetN();
  if(max>1024) max=1024;
  for(int i=0; i!=max; ++i) {
    fX[i] = gr->GetPointX(i);
    fY[i] = gr->GetPointY(i);
  }
  for(int i=max; i<1024; ++i) {
    fX[i] = 0;
    fY[i] = 0;
  }
  fBaseMean = 0;
  fBaseRMS = 0;
  fAmp = 0;
  fAmpBin = 0;
  fTime = 0;
  fPulseShape = NULL;
  fFit = NULL;
}
//===========
void waveform::process() {
  statX( fBaseMean, fBaseRMS, 5, 205 ); // is [5,204] a good range?
  //
  offsetY( -fBaseMean );
  //
  minimumY( fAmp, fAmpBin, 205, 1021 ); // is [205,1020] a good range?
  //
  fTime = timingCTD( 0.3*fAmp, 5, fAmpBin );
  //
}
//===========
TGraph* waveform::GetGraph() {
  TGraph *gr = new TGraph(1024,fX,fY);
  return gr; // pass ownership
}
//===========
void waveform::loadTemplate(TString csvfile) {
  TGraph *gr = new TGraph( csvfile.Data(), "%lg, %lg" );
  fPulseShape = new TSpline3("pulse",gr);
  fFit = new TF1("fit", [&](double*x, double *p){ return p[0] - p[1] * fPulseShape->Eval(x[0] + p[2]); },
		 50, 150, 3 );
}
//===========
void waveform::fitEnergy(Double_t baseline, Double_t t30, Double_t amplitude) {
  fFit->SetParameter( 0, baseline); fFit->SetParLimits( 0, -5+baseline, +5+baseline);
}
//===========
void waveform::statX(Double_t &mean, Double_t &rms, Int_t ini, Int_t fin ) {
  if(ini<0) ini=0;
  if(fin>1024) fin=1024;
  Double_t sumx = 0;
  Double_t sumxx = 0;
  int nnn = 0;
  for(int i=ini; i!=fin; ++i) {
    sumx += fY[i];
    sumxx += fY[i]*fY[i];
    nnn++;
  }
  mean = sumx/nnn;
  rms = TMath::Sqrt( sumxx/nnn - mean*mean );
}
//===========
void waveform::offsetY(Double_t y) {
  for(int i=0; i!=1024; ++i) {
    fY[i] += y;
  }
}
//===========
void waveform::minimumY(Double_t &minV, Int_t &minA, Int_t ini, Int_t fin) {
  if(ini<0) ini=0;
  if(fin>1024) fin=1024;
  minV = 99999;
  minA = -1;
  for(int i=ini; i!=fin; ++i) {
    if(minV>fY[i]) {
      minV = fY[i];
      minA = i;
    }
  }
}
//===========
Double_t waveform::timingCTD(Double_t thr, Int_t ini, Int_t fin ) {
  if(ini<0) ini=0;
  if(fin>1024) fin=1024;
  int right = ini;
  for(int i=ini; i!=fin; ++i ) {
    if(fY[i]<thr) {
      right = i;
      break;
    }
  }
  double y0 = fY[right-1];
  double y1 = fY[right];
  double bw = fX[right] - fX[right-1];
  double dx = bw*(thr-y0)/(y1-y0);
  Double_t ret = fX[right-1]+dx;
  return ret;
}
