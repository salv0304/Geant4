// save as analyzeRutherford.C
#include "TFile.h"
#include "TTree.h"
#include "TH1D.h"
#include "TGraphErrors.h"
#include "TF1.h"
#include "TMath.h"
#include "TCanvas.h"
#include <cmath>
#include <iostream>

void analyzeRutherford(const char* infile="output.root", const char* treename="Hits",
                       double N_inc = 1000,
                       double rho = 19.32,   // Gold density (g/cm3)
                       double t = 1e-4,      // Foil width in cm (1 micron = 1e-4 cm)
                       double M = 197.0)     // Au g/mol
{
  // open file
  TFile *f = TFile::Open(infile);
  if(!f || f->IsZombie()){ std::cerr<<"Cannot open "<<infile<<"\n"; return; }
  TTree *tr = (TTree*)f->Get(treename);
  if(!tr){ std::cerr<<"Tree not found\n"; return; }

  double x,y,z;
  tr->SetBranchAddress("fx",&x);
  tr->SetBranchAddress("fy",&y);
  tr->SetBranchAddress("fz",&z);

  // hist settings
  int nBins = 50;
  double thetaMin = 0.0;
  double thetaMax = TMath::Pi(); // 0..pi
  TH1D *hTheta = new TH1D("hTheta","Theta distribution;#theta [rad];Counts", nBins, thetaMin, thetaMax);

  Long64_t nEntries = tr->GetEntries();
  for(Long64_t i=0;i<nEntries;++i){
    tr->GetEntry(i);
    // Radius
    double r = std::sqrt(x*x + y*y + z*z);
    if(r<=0) continue;
    // Theta
    double theta = std::acos(z/r);
    hTheta->Fill(theta);
  }

  // areal density (atoms/cm^2)
  const double NA = 6.02214076e23;
  double n_s = rho * t * NA / M;

  // build graph of dSigma/dOmega per bin
  TGraphErrors *gDS = new TGraphErrors();
  int point = 0;
  for(int b=1;b<=nBins;++b){
    double theta1 = hTheta->GetBinLowEdge(b);
    double theta2 = hTheta->GetBinLowEdge(b) + hTheta->GetBinWidth(b);
    double Nbin = hTheta->GetBinContent(b);
    if(Nbin<=0) continue;

    double dOmega = 2.0 * TMath::Pi() * (std::cos(theta1) - std::cos(theta2));
    double thetaCenter = hTheta->GetBinCenter(b);

    double dsigm = Nbin / (N_inc * n_s * dOmega); // cm^2/sr
    double errN = std::sqrt(Nbin);
    double err_dsig = errN / (N_inc * n_s * dOmega);

    gDS->SetPoint(point, thetaCenter, dsigm);
    gDS->SetPointError(point, 0.0, err_dsig);
    point++;
  }

  // Fit to A/sin^4(theta/2)
  TF1 *fr = new TF1("fr","[0]/pow(sin(x/2.),4)", 0.05, 2.5); // avoid theta~0 singularity
  gDS->Fit(fr,"R");

  // Draw
  TCanvas *c1 = new TCanvas("c1","dSigma/dOmega",800,600);
  gDS->SetMarkerStyle(20);
  gDS->Draw("AP");
  fr->Draw("SAME");

  // Save results
  TFile *out = new TFile("analysis_out.root","RECREATE");
  hTheta->Write();
  gDS->Write("dSigmaGraph");
  fr->Write("RutherfordFit");
  out->Close();

  std::cout<<"n_s = "<<n_s<<" atoms/cm^2\n";
  std::cout<<"Fit constant A = "<<fr->GetParameter(0)<<"\n";

  f->Close();
}

