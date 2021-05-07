#include "waveform.h"
#include "bar.h"
#include "qabar.h"
#include "TH1D.h"
#include "TH2D.h"
#include "TProfile.h"
#include "TGraph.h"
#include "TCanvas.h"
#include "TLegend.h"
#include "TString.h"

qabar::qabar() {
  int color[3] = {kBlue-3,kGreen-3,kRed-3};
  for(int i=0; i!=4; ++i) {
    base_mean[i] = new TH1D( Form("base_mean_%d",i),Form("base_mean_%d;mV",i),   100,280,360);
    base_rms[i] =  new TH1D( Form("base_rms_%d",i), Form("base_rms_%d;mV",i),    100,0,3);
    min_spot[i] =  new TH2D( Form("min_spot_%d",i), Form("min_spot_%d;bin;mV",i),100,0,1024,100,0,1000);
    thr_spot[i] =  new TH2D( Form("thr_spot_%d",i), Form("thr_spot_%d;ns;mV",i), 100,0,200,100,0,1000);
    for(int cl=0; cl!=3; ++cl) {
      shape_class[cl][i] = new TProfile( Form("shape_class%d_%d",cl,i), Form("shape_class%d_%d;ns;mV",cl,i), 2048,-120,+120 );;
      shape_class[cl][i]->SetLineColor( color[cl] );
    }
  }
  Double_t classes[4];
  classes[0] = 100.0;
  classes[1] = 200.0;
  classes[2] = 300.0;
  classes[3] = 400.0;
  for(int i=0; i!=4; ++i)
    for(int j=0; j!=4; ++j) {
      xclass[j][i] = classes[j];
    }
}
qabar::~qabar() {
  //need to clean pointers
}
void qabar::fill(bar *mybar) {
  for(int i=0; i!=2; ++i) {
    waveform *channelL = mybar->GetLow(i);
    waveform *channelH = mybar->GetHigh(i);
    base_mean[i]->Fill( channelL->GetBaseMean() );
    base_rms[i]->Fill( channelL->GetBaseRMS() );
    min_spot[i]->Fill( channelL->GetAmplitudeBin(), -channelL->GetAmplitude() );
    thr_spot[i]->Fill( channelL->GetTime(), -channelL->GetAmplitude() );
    base_mean[i+2]->Fill( channelH->GetBaseMean() );
    base_rms[i+2]->Fill( channelH->GetBaseRMS() );
    min_spot[i+2]->Fill( channelH->GetAmplitudeBin(), -channelH->GetAmplitude() );
    thr_spot[i+2]->Fill( channelH->GetTime(), -channelH->GetAmplitude() );

    Double_t amplitudeRaw = -channelL->GetAmplitude();
    int nclass=-1;
    if( (amplitudeRaw>xclass[0][i]) && (amplitudeRaw<xclass[1][i]) ) nclass = 0;
    if( (amplitudeRaw>xclass[1][i]) && (amplitudeRaw<xclass[2][i]) ) nclass = 1;
    if( (amplitudeRaw>xclass[2][i]) && (amplitudeRaw<xclass[3][i]) ) nclass = 2;
    if(nclass>-1) {
      TGraph *gr = channelL->GetGraph();
      for(int isa=0;isa!=1024;++isa) {
        Double_t xxx = gr->GetPointX(isa) - channelL->GetTime();
        Double_t yyy = gr->GetPointY(isa);
        shape_class[nclass][i]->Fill( xxx, yyy );
      }
      delete gr;
      gr = channelH->GetGraph();
      for(int isa=0;isa!=1024;++isa) {
        Double_t xxx = gr->GetPointX(isa) - channelL->GetTime();
        Double_t yyy = gr->GetPointY(isa);
        shape_class[nclass][i+2]->Fill( xxx, yyy );
      }
      delete gr;
      
    }
  }

}
void qabar::saveas(TString file){
  TCanvas *main = new TCanvas();
  main->Divide(2,2);
  main->SaveAs( Form("%s.pdf[",file.Data()), "PDF" );
  for(int i=0; i!=4; ++i) {
    main->cd(1+i);
    base_mean[i]->DrawCopy();
  }
  main->SaveAs( Form("%s.pdf[",file.Data()), "PDF" );
  for(int i=0; i!=4; ++i) {
    main->cd(1+i);
    base_rms[i]->DrawCopy();
  }
  main->SaveAs( Form("%s.pdf[",file.Data()), "PDF" );
  for(int i=0; i!=4; ++i) {
    main->cd(1+i);
    min_spot[i]->DrawCopy("colz");
  }
  main->SaveAs( Form("%s.pdf[",file.Data()), "PDF" );
  for(int i=0; i!=4; ++i) {
    main->cd(1+i);
    thr_spot[i]->DrawCopy("colz");
  }
  main->SaveAs( Form("%s.pdf[",file.Data()), "PDF" );

  TLegend *legA = new TLegend(0.1,0.2,0.4,0.5);
  legA->AddEntry(shape_class[0][0],Form("[%.0f - %0.f]",xclass[0][0],xclass[1][0]));
  legA->AddEntry(shape_class[1][0],Form("[%.0f - %0.f]",xclass[1][0],xclass[2][0]));
  legA->AddEntry(shape_class[2][0],Form("[%.0f - %0.f]",xclass[2][0],xclass[3][0]));
  TLegend *legB = new TLegend(0.1,0.2,0.4,0.5);
  legB->AddEntry(shape_class[0][0],Form("[%.0f - %0.f]",xclass[0][1],xclass[1][1]));
  legB->AddEntry(shape_class[1][0],Form("[%.0f - %0.f]",xclass[1][1],xclass[2][1]));
  legB->AddEntry(shape_class[2][0],Form("[%.0f - %0.f]",xclass[2][1],xclass[3][1]));
  for(int i=0; i!=4; ++i) {
    main->cd(1+i);
    shape_class[2][i]->DrawCopy();
    shape_class[1][i]->DrawCopy("same");
    shape_class[0][i]->DrawCopy("same");
  }
  main->SaveAs( Form("%s.pdf[",file.Data()), "PDF" );
  for(int i=0; i!=4; ++i) {
    for(int icl=0; icl!=3; ++icl) {
      shape_class[icl][i]->Scale(1/shape_class[icl][i]->GetMinimum());
    }
  }
  for(int i=0; i!=4; ++i) {
    main->cd(1+i);
    shape_class[2][i]->DrawCopy();
    shape_class[1][i]->DrawCopy("same");
    shape_class[0][i]->DrawCopy("same");
  }
  main->SaveAs( Form("%s.pdf[",file.Data()), "PDF" );
  main->SaveAs( Form("%s.pdf]",file.Data()), "PDF" ); 
}