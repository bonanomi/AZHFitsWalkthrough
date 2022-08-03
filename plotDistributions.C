void plotDistributions() {
  gStyle->SetOptStat(0000);

  // Load the output of PostFitShapesFromWorkspace
  TFile* fin = TFile::Open("shapes_SR.root");

  // Get the histograms for the total background and total signal contributions
  TH1F* TotalBkg = (TH1F*) fin->Get("SignalRegion_diMuon_postfit/TotalBkg");
  TH1F* TotalSig = (TH1F*) fin->Get("SignalRegion_diMuon_postfit/TotalSig");

  // Do not load the data, as the analysis is blinded!
  //TH1F* data_obs = (TH1F*) fin->Get("SignalRegion_postfit/data_obs");
  // Instead, use TotalProcs (i.e. S+B distribution) as dataset,
  // this corresponds to the Asimov dataset generated under the --expectSignal <X> hypothesis
  TH1F* data_obs = (TH1F*) fin->Get("SignalRegion_diMuon_postfit/TotalProcs"); 
  // Error bars on MC background can be obtained from the TotalBkg TH1F
  // Hence we clone it and we plot it with "E2" option later 
  // These uncertainties are the syst+stat uncertainties on MC
  TH1F* h_err = (TH1F*) TotalBkg->Clone("h_err");

  // Some style/drawing settings
  TotalBkg->SetFillColor(kBlue-4);
  TotalSig->SetLineColor(2);
  TotalSig->SetLineWidth(2);

  h_err->SetFillColorAlpha(1, 0.4);
  h_err->SetFillStyle(3356);
  h_err->SetMarkerSize(0);
  
  // We will draw Data (Asimov S+B in this case) as points with errorbars
  data_obs->SetMarkerStyle(20);
  data_obs->SetMarkerColor(1);

  TCanvas* c2 = new TCanvas();

  // Create ration plot between Bkg (B only) and Data (S+B)
  auto rp1 = new TRatioPlot(TotalBkg,data_obs);
  
  rp1->Draw();

  rp1->GetUpperPad()->cd();

  // Draw the shaded bands for the sys uncertainties
  h_err->Draw("E2SAME");
  // Draw the Signal (generated under --expectSignal <X> HP) unstacked
  TotalSig->Draw("HIST SAME");

  rp1->GetLowerPad()->cd();

  // Get the S/B to plot in the ratio plot
  TH1F* h_band = (TH1F*) h_err->Clone("h_band");
  h_band->Divide(h_err);

  h_band->SetFillColorAlpha(1, 0.4);
  h_band->SetFillStyle(3003);
  h_band->SetMarkerSize(0);
  
  TH1F* h_mc = (TH1F*) data_obs->Clone("h_mc");

  // Plot Data and S/B in the ratio plot
  h_mc->Divide(TotalBkg);
  h_band->Draw("E2 SAME");
  h_mc->SetMarkerStyle(20);
  h_mc->SetMarkerColor(1);
  h_mc->Draw("PE0 SAME");
  h_band->GetYaxis()->SetRangeUser(0.2, 2.);

  // Some style, legend and labels
  rp1->GetUpperRefXaxis()->SetTitle("p_{T}(Z) [GeV]");
  rp1->GetLowerRefYaxis()->SetTitle("Data/MC");
  rp1->GetLowerRefYaxis()->SetRangeUser(0.1, 2.1);
  rp1->GetUpperPad()->SetLogy();
  rp1->GetLowerRefGraph()->SetMarkerSize(0);
  rp1->GetLowerRefGraph()->SetLineWidth(0);
  rp1->GetUpperRefYaxis()->SetTitle("Events");

  rp1->GetUpperPad()->cd();
  TLegend* legend = new TLegend(0.50, 0.50, 0.90, 0.91, "", "NBNDC");
  legend->AddEntry(TotalBkg, "Background", "F");
  legend->AddEntry(TotalSig, "Signal, (mH=350 GeV, mA=900 GeV)", "L");
  legend->AddEntry(data_obs, "Asymov Data (#mu=1.0)", "PE");
  legend->AddEntry(h_err, "Prefit uncertainty", "F");
  legend->Draw();
}
