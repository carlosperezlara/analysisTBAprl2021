#include <iostream>
#include <fstream>

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

qabar::qabar(TString name) {
  int color[3] = {kBlue-3,kGreen-3,kRed-3};
  for(int i=0; i!=4; ++i) {
    double minPed = 280;
    double maxPed = 360;
    double maxRMS = 20;
    if(i>=2) { //high gain
      minPed = 160; maxPed = 600; maxRMS = 200;
    }
    base_mean[i] = new TH1D( Form("base_mean_%d_%s",i,name.Data()),Form("base_mean_%d;mV",i),   100,minPed,maxPed);
    base_rms[i] =  new TH1D( Form("base_rms_%d_%s",i,name.Data()), Form("base_rms_%d;mV",i),    100,0,maxRMS);
    wave_all[i] =  new TH2D( Form("wave_all_%d_%s",i,name.Data()), Form("wave_all_%d;mV",i),    100,0,200, 100,-1050,+1050);
    min_spot[i] =  new TH2D( Form("min_spot_%d_%s",i,name.Data()), Form("min_spot_%d;bin;mV",i),100,0,1024,100,0,1000);
    thr_spot[i] =  new TH2D( Form("thr_spot_%d_%s",i,name.Data()), Form("thr_spot_%d;ns;mV",i), 100,0,200, 100,0,1000);
    amp_dist[i] =  new TH1D( Form("amp_dist_%d_%s",i,name.Data()), Form("amp_dist_%d;mV",i),    100,0,1000);
    for(int cl=0; cl!=3; ++cl) {
      shape_class[cl][i] = new TProfile( Form("shape_class%d_%d_%s",cl,i,name.Data()), Form("shape_class%d_%d;ns;mV",cl,i),
					 2048,-120,+120 );;
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
  for(int iSiPM=0; iSiPM!=2; ++iSiPM) {
    waveform *channelL = mybar->GetLow(iSiPM);
    waveform *channelH = mybar->GetHigh(iSiPM);
    // [low0 low1] high0 high1
    base_mean[iSiPM]->Fill( channelL->GetBaseMean() );
    base_rms[iSiPM]->Fill( channelL->GetBaseRMS() );
    min_spot[iSiPM]->Fill( channelL->GetAmplitudeBin(), -channelL->GetAmplitude() );
    thr_spot[iSiPM]->Fill( channelL->GetTime(), -channelL->GetAmplitude() );
    amp_dist[iSiPM]->Fill( -channelL->GetAmplitude() );
    // low0 low1 [high0 high1]
    base_mean[iSiPM+2]->Fill( channelH->GetBaseMean() );
    base_rms[iSiPM+2]->Fill( channelH->GetBaseRMS() );
    min_spot[iSiPM+2]->Fill( channelH->GetAmplitudeBin(), -channelH->GetAmplitude() );
    thr_spot[iSiPM+2]->Fill( channelH->GetTime(), -channelH->GetAmplitude() );
    amp_dist[iSiPM+2]->Fill( -channelH->GetAmplitude() );
    // class is based on low gain amplitude
    Double_t amplitudeRaw = -channelL->GetAmplitude();
    int nclass=-1;
    if( (amplitudeRaw>xclass[0][iSiPM]) && (amplitudeRaw<xclass[1][iSiPM]) ) nclass = 0;
    if( (amplitudeRaw>xclass[1][iSiPM]) && (amplitudeRaw<xclass[2][iSiPM]) ) nclass = 1;
    if( (amplitudeRaw>xclass[2][iSiPM]) && (amplitudeRaw<xclass[3][iSiPM]) ) nclass = 2;
    TGraph *grL = channelL->GetGraph();
    TGraph *grH = channelH->GetGraph();
    for(int isa=0;isa!=1024;++isa) {
      Double_t xxL = grL->GetPointX(isa);
      Double_t yyL = grL->GetPointY(isa);
      Double_t xxH = grH->GetPointX(isa);
      Double_t yyH = grH->GetPointY(isa);
      wave_all[iSiPM]->Fill( xxL, yyL ); // LOWs
      wave_all[iSiPM+2]->Fill( xxH, yyH ); // HIGHs
      if(nclass>-1) {
	double align = channelL->GetTime(); // alignment wrt raw lowa gain T30 (CHANGE ME LATER)
        shape_class[nclass][iSiPM]->Fill(   xxL - align, yyL );  // LOWs
        shape_class[nclass][iSiPM+2]->Fill( xxH - align, yyH ); // HIGHs
      }
    }
    delete grL;
    delete grH;
  }
}
//===============
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
    TF1 *fit = new TF1( Form("fit%d",i), "gaus" );
    //TF1 *fit = new TF1( Form("fit%d",i), "landau" );
    base_rms[i]->Fit( fit );
    base_rms[i]->DrawCopy();
  }
  main->SaveAs( Form("%s.pdf[",file.Data()), "PDF" );
  for(int i=0; i!=4; ++i) {
    main->cd(1+i);
    amp_dist[i]->DrawCopy();
  }

  main->SaveAs( Form("%s.pdf[",file.Data()), "PDF" );
  for(int i=0; i!=4; ++i) {
    main->cd(1+i)->SetLogz(1);
    wave_all[i]->DrawCopy("colz");
  }

  main->SaveAs( Form("%s.pdf[",file.Data()), "PDF" );
  for(int i=0; i!=4; ++i) {
    main->cd(1+i)->SetLogz(1);
    min_spot[i]->DrawCopy("colz");
  }
  main->SaveAs( Form("%s.pdf[",file.Data()), "PDF" );
  for(int i=0; i!=4; ++i) {
    main->cd(1+i)->SetLogz(1);
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
      shape_class[icl][i]->Scale(-1/shape_class[icl][i]->GetMinimum());
    }
  }
  for(int i=0; i!=4; ++i) {
    main->cd(1+i);
    shape_class[2][i]->DrawCopy();
    shape_class[1][i]->DrawCopy("same");
    shape_class[0][i]->DrawCopy("same");
  }
  main->SaveAs( Form("%s.root",file.Data()), "ROOT" );
  main->SaveAs( Form("%s.pdf[",file.Data()), "PDF" );
  main->SaveAs( Form("%s.pdf]",file.Data()), "PDF" ); 
}

void qabar::makesummary(TString file){
  std::ofstream fout( Form("%s.txt",file.Data()) );
  for(int i=0; i!=4; ++i) {
    TF1 *fit = (TF1*) (base_rms[i]->GetListOfFunctions())->At(0);
    fout << fit->GetParameter(1) << " " << std::endl;
  }
  fout.close();
}
