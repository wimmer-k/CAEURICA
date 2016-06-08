{
  TFile *f[5];
  TTree *tr[5];
  TH1F *h[5];
  for(int i=0;i<5;i++){
    //f[i]  = new TFile(Form("../../data1/sync_test_eurica/run2_sorted_m%d.root",i+1));
    f[i]  = new TFile(Form("../run0_sorted_m%d.root",i+1));
    tr[i] = (TTree*)f[i]->Get("tr");
    tr[i]->Draw("ts:Entry$>>hp(1000,0,1e6,1000,0,1e11)");
    h[i] = (TH1F*)gROOT->FindObject("hp");
    if(h[i]){
      h[i]->SetName(Form("h%d",i));
      h[i]->SetMarkerColor(1+i);
    }
  }
  TCanvas *c = new TCanvas();
  c->Divide(3,2);
  c->cd(6);
  if(h[0])
  h[0]->Draw();
  for(int i=0;i<5;i++){
    if(h[i]){
      c->cd(6);
      h[i]->Draw("same");
      c->cd(1+i);
      h[i]->Draw("colz");
    } 
  }

}
/*
{
  TFile *f = new TFile("test.root");
  TTree *tr = (TTree*)f->Get("tr");
  TH1F *h[5];
  for(int i=0;i<5;i++){
    tr->Draw("ts:Entry$>>hp(1000,0,12e6,1000,0,7e10)",Form("board==%d",i));
    h[i] = (TH1F*)gROOT->FindObject("hp");
    h[i]->SetName(Form("h%d",i));
    h[i]->SetMarkerColor(1+i);
  }
  TCanvas *c = new TCanvas();
  c->Divide(3,2);
  c->cd(6);
  h[0]->Draw();
  for(int i=0;i<5;i++){
    c->cd(6);
    h[i]->Draw("same");
    c->cd(1+i);
    h[i]->Draw();
 } 

}
*/
