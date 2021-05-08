struct logbookentry_t {
  TString sizeDRS;
  TString dateDRS;
  TString sizePIXEL;
  Float_t Temp[8];
  Float_t Vop[8];
  Float_t Cur[8];
  Float_t Vbr[8];
  Float_t DCR[8];
} lb_entry;

struct qalogentry_t {
  Float_t RMSbase[8][2]; //eight sipms | two gains
} qalog_entry;

map<Int_t,logbookentry_t> logbook;
map<Int_t,qalogentry_t> qalog;

void loadlogbook();
void addtoqalog(int);

TGraph* makegraph(int color) {
  TGraph *gr = new TGraph();
  gr->SetMarkerColor( color );
  gr->SetLineColor( color );
  gr->SetMarkerStyle( 20 );
  return gr;
}
int plotQA(TString file="runs.txt") {
  loadlogbook();
  ifstream fin(file.Data());
  TGraph *grVol[4][8];
  TGraph *grRMS0[4][8], *grRMS1[4][8];
  int color[4] = { kRed-3, kOrange-3, kCyan-3, kBlue-3  };
  for(int isipm=0; isipm!=8; ++isipm) {
    for(int iclass=0; iclass!=4; ++iclass) {
      grVol[iclass][isipm] = makegraph( color[iclass] );
      grRMS0[iclass][isipm] = makegraph( color[iclass] );
      grRMS1[iclass][isipm] = makegraph( color[iclass] );
    }
  }
  for(std::string line; std::getline(fin,line);) {
    int run = atoi(line.c_str());
    int iclass = 0;
    if( (run>=44612)&&(run<44688) ) iclass = 1;
    if( (run>=44688)&&(run<44725) ) iclass = 2;
    if( (run>=44725) ) iclass = 3;
    for(int isipm=2; isipm!=8; ++isipm) {
      grVol[iclass][isipm]->SetPoint( grVol[iclass][isipm]->GetN(), logbook[run].Vop[isipm], logbook[run].Cur[isipm] );
      grRMS0[iclass][isipm]->SetPoint( grRMS0[iclass][isipm]->GetN(), qalog[run].RMSbase[isipm][0], logbook[run].Cur[isipm] );
      grRMS1[iclass][isipm]->SetPoint( grRMS1[iclass][isipm]->GetN(), qalog[run].RMSbase[isipm][1], logbook[run].Cur[isipm] );
    }
    //cout << logbook[run].Vop[1] << endl;
  }
  fin.close();

  TH2D *axis0 = new TH2D("axis0",";Bias Voltage  (V);Current  (mA)",100,37,43,100,0,6);  
  TH2D *axis10 = new TH2D("axis10",";RMS baseline  (mV);Current  (mA)",100,0,7,100,0,6);  
  TH2D *axis11 = new TH2D("axis11",";RMS baseline  (mV);Current  (mA)",100,0,45,100,0,6);  
  
  for(int isipm=2; isipm!=8; ++isipm) {
    new TCanvas();
    axis0->SetTitle( Form("SIPM No %d",isipm) );
    axis0->DrawCopy();
    for(int iclass=0; iclass!=4; ++iclass)
      grVol[iclass][isipm]->Draw("PSAME");

    TCanvas *main = new TCanvas();
    main->Divide(1,2);
    axis11->SetTitle( Form("SIPM No %d",isipm) );
    main->cd(1);
    axis10->DrawCopy();
    main->cd(2);
    axis11->DrawCopy();
    for(int iclass=0; iclass!=4; ++iclass) {
      main->cd(1);
      grRMS0[iclass][isipm]->Draw("PSAME");
      main->cd(2);
      grRMS1[iclass][isipm]->Draw("PSAME");
    }
  }
  
  return 0;
}

void loadlogbook() {
  ifstream fin("logbook/gruns.csv");
  int n=-2;
  for(std::string line; std::getline(fin,line);++n) {
    if(n<0) continue;
    TString rline = line;
    TObjArray *obj = rline.Tokenize(",");

    Int_t run = ((TObjString*)obj->At(0))->GetString().Atoi();
    //cout << run << " npieces: " << obj->GetEntries() << endl;
    //cout << rline.Data() << endl;
    lb_entry.sizeDRS = ((TObjString*)obj->At(1))->GetString();
    lb_entry.dateDRS =  ((TObjString*)obj->At(2))->GetString();
    lb_entry.sizePIXEL =  ((TObjString*)obj->At(3))->GetString();
    // SiPM1
    lb_entry.Temp[0] = ((TObjString*)obj->At(7))->GetString().Atof();
    lb_entry.Vop[0] =  ((TObjString*)obj->At(8))->GetString().Atof();
    // SiPM2
    lb_entry.Temp[1] = ((TObjString*)obj->At(9))->GetString().Atof();
    lb_entry.Vop[1] =  ((TObjString*)obj->At(10))->GetString().Atof();
    // SiPM3
    lb_entry.Temp[2] = ((TObjString*)obj->At(11))->GetString().Atof();
    lb_entry.Vop[2] =  ((TObjString*)obj->At(12))->GetString().Atof();
    lb_entry.Cur[2] =  ((TObjString*)obj->At(13))->GetString().Atof();
    lb_entry.Vbr[2] =  ((TObjString*)obj->At(14))->GetString().Atof();
    lb_entry.DCR[2] =  ((TObjString*)obj->At(16))->GetString().Atof();
    // SiPM4
    lb_entry.Temp[3] = ((TObjString*)obj->At(17))->GetString().Atof();
    lb_entry.Vop[3] =  ((TObjString*)obj->At(18))->GetString().Atof();
    lb_entry.Cur[3] =  ((TObjString*)obj->At(19))->GetString().Atof();
    // SiPM5
    lb_entry.Temp[4] = ((TObjString*)obj->At(20))->GetString().Atof();
    lb_entry.Vop[4] =  ((TObjString*)obj->At(21))->GetString().Atof();
    lb_entry.Cur[4] =  ((TObjString*)obj->At(22))->GetString().Atof();
    lb_entry.Vbr[4] =  ((TObjString*)obj->At(23))->GetString().Atof();
    lb_entry.DCR[4] =  ((TObjString*)obj->At(25))->GetString().Atof();
    // SiPM6
    lb_entry.Temp[5] = ((TObjString*)obj->At(26))->GetString().Atof();
    lb_entry.Vop[5] =  ((TObjString*)obj->At(27))->GetString().Atof();
    lb_entry.Cur[5] =  ((TObjString*)obj->At(28))->GetString().Atof();
    // SiPM7
    lb_entry.Temp[6] = ((TObjString*)obj->At(29))->GetString().Atof();
    lb_entry.Vop[6] =  ((TObjString*)obj->At(30))->GetString().Atof();
    lb_entry.Cur[6] =  ((TObjString*)obj->At(31))->GetString().Atof();
    // SiPM8
    lb_entry.Temp[7] = ((TObjString*)obj->At(32))->GetString().Atof();
    lb_entry.Vop[7] =  ((TObjString*)obj->At(33))->GetString().Atof();
    lb_entry.Cur[7] =  ((TObjString*)obj->At(34))->GetString().Atof();
    logbook[run] = lb_entry;

    // attempt to read qalog
    addtoqalog(run);
  }
  fin.close();
  cout << "number of good runs: " << n << endl;
}

void addtoqalog(int run) {
  for(int ibar=0; ibar!=4; ++ibar) {
    ifstream finL( Form("log/QA_Run%d_Bar%d.txt",run,ibar) );
    int isipm0 = ibar*2;
    int isipm1 = isipm0 + 1;
    finL >> qalog_entry.RMSbase[isipm0][0];
    finL >> qalog_entry.RMSbase[isipm1][0];
    finL >> qalog_entry.RMSbase[isipm0][1];
    finL >> qalog_entry.RMSbase[isipm1][1];

    finL.close();
  }
  qalog[run] = qalog_entry;
}
