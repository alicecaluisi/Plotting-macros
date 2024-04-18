#include <TFile.h>
#include <TH1.h>
#include <TH2.h>
#include <THnSparse.h>
#include <TAxis.h>
#include <TString.h>
#include <TCanvas.h>
#include <TStyle.h>
#include <TPaveStats.h>
#include <vector>
#include <string>
#include <tuple>

TH1D* Project1D(THnSparse* h, int axis, Option_t* option = "") {
    if (!h) return nullptr;
    // Set range to -1 for all axes except the one we are projecting
    for (int i = 0; i < h->GetNdimensions(); ++i) {
        if (i != axis) {
            h->GetAxis(i)->SetRange(1, -1);
        }
    }
    TH1D* h1 = dynamic_cast<TH1D*>(h->Projection(axis, option));
    if (h1) {
        h1->SetName(TString::Format("%s_Proj1D_%d", h->GetName(), axis));
    }
    // Reset range for all axes
    for (int i = 0; i < h->GetNdimensions(); ++i) {
        h->GetAxis(i)->SetRange(1, -1);
    }
    return h1;
}

TH2D* Project2D(THnSparse* h, int axis1, int axis2, Option_t* option = "") {
    if (!h) return nullptr;
    for (int i = 0; i < h->GetNdimensions(); ++i) {
        if (i != axis1 && i != axis2) {
            h->GetAxis(i)->SetRange(1, -1);
        }
    }
    TH2D* h2 = dynamic_cast<TH2D*>(h->Projection(axis1, axis2, option));
    if (h2) {
        h2->SetName(TString::Format("%s_Proj2D_%d_%d", h->GetName(), axis2, axis1));
    }
    for (int i = 0; i < h->GetNdimensions(); ++i) {
        h->GetAxis(i)->SetRange(1, -1);
    }
    return h2;
}

void Plot1FHistogram(TH1F* histo, const char* title, const char* xTitle, const char* fileName) {
    if (!histo) return; 
    TCanvas* canvas = new TCanvas("canvas", title, 4000, 3100);
    //canvas->SetLogy();
    histo->SetLineColor(kAzure-2);
    histo->SetLineWidth(6);
    histo->SetMarkerStyle(20);
    histo->SetMarkerSize(3);
    histo->SetMarkerColor(kAzure-2);
    histo->Draw();
    canvas->SetTopMargin(0.20);
    histo->SetTitle(title);
    histo->GetXaxis()->SetTitle(xTitle);
    gPad->Update();
    TPaveStats* st_histo = (TPaveStats*)histo->FindObject("stats");
    if (st_histo) {  
        st_histo->SetX1NDC(0.73); 
        st_histo->SetX2NDC(0.90); 
        st_histo->SetY1NDC(0.80); 
        st_histo->SetY2NDC(0.95);
    }
    canvas->SaveAs(fileName);
    delete canvas;
}

void Plot1DHistogram(TH1D* histo, const char* title, const char* xTitle, const char* fileName) {
    if (!histo) return; 
    TCanvas* canvas = new TCanvas("canvas", title, 4000, 3100);
    //canvas->SetLogy();
    histo->SetLineColor(kCyan-2);
    histo->SetLineWidth(4);
    histo->SetMarkerStyle(20);
    histo->SetMarkerSize(3);
    histo->SetMarkerColor(kCyan-2);
    histo->Draw();
    canvas->SetTopMargin(0.20);
    histo->SetTitle(title);
    histo->GetXaxis()->SetTitle(xTitle);
    gPad->Update();
    TPaveStats* st_histo = (TPaveStats*)histo->FindObject("stats");
    if (st_histo) {  
        st_histo->SetX1NDC(0.73); 
        st_histo->SetX2NDC(0.90); 
        st_histo->SetY1NDC(0.80); 
        st_histo->SetY2NDC(0.95);
    }
    canvas->SaveAs(fileName);
    delete canvas;
}

void Plot2DHistogram(TH2D* histo, const char* title, const char* xTitle, const char* yTitle, const char* fileName, double numberOfEvents) {
    if (!histo || numberOfEvents <= 0) return;
    double currentMin = histo->GetMinimum(0); 
    double currentMax = histo->GetMaximum();
    histo->Scale(1.0 / numberOfEvents);
    double newMin = currentMin / numberOfEvents;
    double newMax = currentMax / numberOfEvents;
    TCanvas* canvas = new TCanvas("canvas", title, 4000, 3100);
    canvas->SetLogz();
    gStyle->SetPalette(kBird);
    canvas->SetTopMargin(0.20);
    canvas->SetRightMargin(0.12);
    histo->SetTitle(title);
    histo->GetXaxis()->SetTitle(xTitle);
    histo->GetYaxis()->SetTitle(yTitle);
    histo->SetMinimum(newMin);
    histo->SetMaximum(newMax);
    histo->Draw("COLZ");
    gPad->Update();
    TPaveStats* st_histo = (TPaveStats*)histo->FindObject("stats");
    if (st_histo) {
        st_histo->SetX1NDC(0.75);
        st_histo->SetX2NDC(0.93);
        st_histo->SetY1NDC(0.80);
        st_histo->SetY2NDC(0.95);
    }
    canvas->SaveAs(fileName);
    delete canvas;
}

TH2D* Project2DRange(THnSparseD* h, int axis1, int axis2, double ptMin, double ptMax, Option_t* option = "") {
    if (!h) return nullptr;
    int ptAxis = 0;
    int binMin = h->GetAxis(ptAxis)->FindBin(ptMin);
    int binMax = h->GetAxis(ptAxis)->FindBin(ptMax) - 1; 
    h->GetAxis(ptAxis)->SetRange(binMin, binMax);
    TH2D* h2 = dynamic_cast<TH2D*>(h->Projection(axis1, axis2, option));
    if (h2) {
        h2->SetName(TString::Format("%s_Proj2D_%d_%d", h->GetName(), axis1, axis2));
    }
    h->GetAxis(ptAxis)->SetRange();
    return h2;
}


void PlotPtRangeHistograms(TFile* file, const char* histPath, const char* baseTitle, const char* xTitle, const char* yTitle, const char* baseFileName, int axis1, int axis2) {
    THnSparseD* histSparse = dynamic_cast<THnSparseD*>(file->Get(histPath));
    if (!histSparse) {
        std::cerr << "Histogram " << histPath << " not found." << std::endl;
        return;
    }
    //THnSparseD* hPtSparse = dynamic_cast<THnSparseD*>(file->Get("track-jet-qa_id9646/Kine/pt")); // <------------- loose cuts
    //THnSparseD* hPtSparse = dynamic_cast<THnSparseD*>(file->Get("track-jet-qa_id9647/Kine/pt")); // <------------- tigth cuts
    THnSparseD* hPtSparse = dynamic_cast<THnSparseD*>(file->Get("track-jet-qa_id9648/Kine/pt")); // <------------- GlobalTracks
    TH1D* pt = Project1D(hPtSparse, 0, "E");
    std::vector<std::pair<double, double>> ptBins = {{0, 1}, {1, 3}, {3, 5}, {5, 10}, {10, 50}, {50, 100}, {100, 200}};
    for (const auto& bin : ptBins) {
        double ptMin = bin.first;
        double ptMax = bin.second;
        int binMin = pt->FindBin(ptMin);
        int binMax = pt->FindBin(ptMax) - 1;
        double ptRangeEntries = pt->Integral(binMin, binMax);
        TH2D* hist2D = Project2DRange(histSparse, axis1, axis2, ptMin, ptMax, "E"); 
        if (!hist2D) {
            std::cerr << "Projection failed for " << histPath << " in pT range [" << ptMin << ", " << ptMax << "]." << std::endl;
            continue;
        }
        TString title = TString::Format("%s for %.0f < p_{T} < %.0f GeV/c", baseTitle, ptMin, ptMax);
        TString fileName = TString::Format("%s_%.0f_%.0f.png", baseFileName, ptMin, ptMax);
        Plot2DHistogram(hist2D, title, xTitle, yTitle, fileName, ptRangeEntries);
    }
    delete pt;
}



void QAplots() {       
    gSystem->mkdir("GlobalTracks", kTRUE); 
    gSystem->mkdir("loose", kTRUE); 
    gSystem->mkdir("tight", kTRUE); 
    gSystem->mkdir("GlobalTracks/Kine", kTRUE); 
    gSystem->mkdir("GlobalTracks/EventProp", kTRUE); 
    gSystem->mkdir("GlobalTracks/TrackPar", kTRUE); 
    gSystem->mkdir("GlobalTracks/ITS", kTRUE); 
    gSystem->mkdir("GlobalTracks/TPC", kTRUE); 
    gSystem->mkdir("loose/Kine", kTRUE); 
    gSystem->mkdir("loose/EventProp", kTRUE); 
    gSystem->mkdir("loose/TrackPar", kTRUE); 
    gSystem->mkdir("loose/ITS", kTRUE); 
    gSystem->mkdir("loose/TPC", kTRUE); 
    gSystem->mkdir("tight/Kine", kTRUE); 
    gSystem->mkdir("tight/EventProp", kTRUE); 
    gSystem->mkdir("tight/TrackPar", kTRUE); 
    gSystem->mkdir("tight/ITS", kTRUE); 
    gSystem->mkdir("tight/TPC", kTRUE); 
    TFile* file = new TFile("/dcache/alice/acaluisi/JetQA/LHC22_pass4_highIR_sampling/full/AnalysisResults.root"); 
    if (!file || file->IsZombie()) {
        std::cerr << "Error opening file." << std::endl;
        return;
    }
    
    //THnSparseD* collisionVtxZSparse = dynamic_cast<THnSparseD*>(file->Get("track-jet-qa_id9646/EventProp/collisionVtxZ")); // <------------- loose cuts
    //THnSparseD* collisionVtxZSparse = dynamic_cast<THnSparseD*>(file->Get("track-jet-qa_id9647/EventProp/collisionVtxZ")); // <------------- tight cuts
    THnSparseD* collisionVtxZSparse = dynamic_cast<THnSparseD*>(file->Get("track-jet-qa_id9648/EventProp/collisionVtxZ")); // <------------- Global Tracks

    if (!collisionVtxZSparse) {
        std::cerr << "collisionVtxZ THnSparseD histogram not found." << std::endl;
        file->Close();
        delete file;
        return;
    }
    TH1D* collisionVtxZ = dynamic_cast<TH1D*>(collisionVtxZSparse->Projection(0));
    if (!collisionVtxZ) {
        std::cerr << "Projection of collisionVtxZ failed." << std::endl;
        file->Close();
        delete file;
        return;
    }
    double numberOfEvents = collisionVtxZ->GetEntries();
    
    auto processHistogram = [&](const char* path, const char* title, const char* xTitle, const char* yTitle, const char* fileName, int axis = -1, bool project2D = false, int axis2 = -1) {
    if (axis == -1) { // Check if we're not dealing with a THnSparse histogram
        TH1F* hist1F = dynamic_cast<TH1F*>(file->Get(path));
        if (hist1F) { 
            Plot1FHistogram(hist1F, title, xTitle, fileName);
            return;
        } else {
            std::cerr << "Histogram " << path << " not found or not a TH1F." << std::endl;
            return;
        }
    }
    THnSparseD* histSparse = dynamic_cast<THnSparseD*>(file->Get(path));
    if (!histSparse) {
        std::cerr << "Histogram " << path << " not found." << std::endl;
        return;
    }

    if (project2D) {
        TH2D* hist2D = Project2D(histSparse, axis, axis2, "E");
        if (!hist2D) {
            std::cerr << "Projection failed for " << path << "." << std::endl;
            return;
        }
        Plot2DHistogram(hist2D, title, xTitle, yTitle, fileName, numberOfEvents);
    } else {
        TH1D* hist1D = Project1D(histSparse, axis, "E");
        if (!hist1D) {
            std::cerr << "Projection failed for " << path << "." << std::endl;
            return;
        }
        Plot1DHistogram(hist1D, title, xTitle, fileName); 
        }
    };
    

    // ---------------------------- id9648 = GlobalTracks ---------------------------- 
    // ---- Event property histograms ---- 
    processHistogram("track-jet-qa_id9648/EventProp/collisionVtxZ", "Collsion Vertex Z position", "Vtx_{z} [cm]", "", "GlobalTracks/EventProp/collisionVtxZ.png", 0);
    processHistogram("track-jet-qa_id9648/EventProp/collisionVtxZnoSel", "Collsion Vertex Z position without event selection", "Vtx_{z} [cm]", "", "GlobalTracks/EventProp/collisionVtxZnoSel.png", 0);
    processHistogram("track-jet-qa_id9648/EventProp/collisionVtxZSel8", "Collsion Vertex Z position with event selection", "Vtx_{z} [cm]", "", "GlobalTracks/EventProp/collisionVtxZSel8.png", 0);
    processHistogram("track-jet-qa_id9648/EventProp/rejectedCollId", "CollisionId of collisions that did not pass the event selection", "collisionId", "", "GlobalTracks/EventProp/rejectedCollId.png");
    // ---- Kinetic histograms ---- 
    processHistogram("track-jet-qa_id9648/Kine/EtaPhiPt", "Correlation of #eta and #it{p}_{T}", "p_{T} (GeV/c)", "#eta", "GlobalTracks/Kine/eta.png", 2, true, 0);
    processHistogram("track-jet-qa_id9648/Kine/EtaPhiPt", "Correlation of #phi and #it{p}_{T}", "p_{T} (GeV/c)", "#phi [rad]", "GlobalTracks/Kine/phi.png", 3, true, 0);
    processHistogram("track-jet-qa_id9648/Kine/EtaPhiPt", "Correlation of #eta and #phi", "#eta", "#phi [rad]", "GlobalTracks/Kine/etaphi.png", 3, true, 2);
    PlotPtRangeHistograms(file, "track-jet-qa_id9648/Kine/EtaPhiPt", "Correlation of #eta and #phi", "#eta", "#phi [rad]", "GlobalTracks/Kine/etaphi", 3, 2);
    THnSparseD* hPtSparse = dynamic_cast<THnSparseD*>(file->Get("track-jet-qa_id9648/Kine/pt")); 
    THnSparseD* hPt_TRDSparse = dynamic_cast<THnSparseD*>(file->Get("track-jet-qa_id9648/Kine/pt_TRD"));
    if (!hPtSparse || !hPt_TRDSparse) {
        std::cerr << "Sparse histograms could not be found." << std::endl;
        file->Close();
        delete file;
        return;
    }
    TH1D* pt = Project1D(hPtSparse, 0, "E");
    TH1D* pt_TRD = Project1D(hPt_TRDSparse, 0, "E");
    if (!pt || !pt_TRD) {
        std::cerr << "Projection failed." << std::endl;
        file->Close();
        delete file;
        return;
    }
    pt->Scale(1.0 / numberOfEvents);
    pt_TRD->Scale(1.0 / numberOfEvents);
    pt->SetTitle(" ; p_{T} (GeV/c); Number of entries / Number of events");
    pt_TRD->SetTitle(" ; p_{T} (GeV/c); Number of entries / Number of events");
    TCanvas* pVSp = new TCanvas("pVSp", "pt_comparison", 3000, 1200);
    pVSp->SetTopMargin(0.20);
    pVSp->Divide(2,1);
    pVSp->cd(1);
    gPad->SetLogy();
    pt->SetLineColor(kGreen-2);
    pt->SetLineWidth(6);
    pt->SetMarkerStyle(20);
    pt->SetMarkerSize(2);
    pt->SetMarkerColor(kGreen-2);
    pt_TRD->SetLineColor(kSpring+10);
    pt_TRD->SetLineWidth(6);
    pt_TRD->SetMarkerStyle(20);
    pt_TRD->SetMarkerSize(2);
    pt_TRD->SetMarkerColor(kSpring+10);
    pt->Draw("HIST");
    pt_TRD->Draw("HIST SAME");
    gPad->Update();
    TLatex latex1;
    latex1.SetTextSize(0.03);
    latex1.DrawLatexNDC(0.1, 0.93, "p_{T} spectra overall and with TRD match");
    TPaveStats* st_pt = (TPaveStats*)pt->FindObject("stats");
    if (st_pt) {
        st_pt->SetX1NDC(0.73);
        st_pt->SetX2NDC(0.90);
        st_pt->SetY1NDC(0.90);
        st_pt->SetY2NDC(0.99);
    }
    auto legend = new TLegend(0.60, 0.73, 0.9, 0.9);
    legend->SetFillStyle(0);
    legend->AddEntry(pt, "overall p_{T}", "lep");
    legend->AddEntry(pt_TRD, "p_{T} if track has a TRD match", "lep");
    legend->Draw();
    pVSp->cd(2);
    TH1D* pt_reb = (TH1D*)pt->Clone("pt_reb");
    TH1D* pt_TRD_reb = (TH1D*)pt_TRD->Clone("pt_TRD_reb");
    pt_reb->Rebin(5);
    pt_TRD_reb->Rebin(5);
    TH1D* pt_ratio = (TH1D*)pt_TRD_reb->Clone("pt_ratio");
    pt_ratio->Divide(pt_reb);
    pt_ratio->SetLineColor(kRed-7);
    pt_ratio->SetLineWidth(3);
    pt_ratio->SetMarkerStyle(20);
    pt_ratio->SetMarkerColor(kRed-7);
    pt_ratio->GetYaxis()->SetTitle("pt_TRD / pt");
    pt_ratio->Draw("E");
    gPad->Update();
    TLatex latex2;
    latex2.SetTextSize(0.03);
    latex2.DrawLatexNDC(0.1, 0.93, "Ratio between p_{T} with TRD match and p_{T} overall");
    TPaveStats* st_pt_ratio = (TPaveStats*)pt_ratio->FindObject("stats");
    if (st_pt_ratio) {
        st_pt_ratio->SetX1NDC(0.73);
        st_pt_ratio->SetX2NDC(0.90);
        st_pt_ratio->SetY1NDC(0.90);
        st_pt_ratio->SetY2NDC(0.99);
    }
    pVSp->SaveAs("GlobalTracks/Kine/pt_comparison.png"); 
    delete pVSp; 
    // ---- Track Parameter histograms ---- 
    processHistogram("track-jet-qa_id9648/TrackPar/xyz", "Track #it{x} position at dca in local coordinate system", "p_{T} (GeV/c)", "#it{x} [cm]", "GlobalTracks/TrackPar/x.png", 2, true, 0);
    processHistogram("track-jet-qa_id9648/TrackPar/xyz", "Track #it{y} position at dca in local coordinate system", "p_{T} (GeV/c)", "#it{y} [cm]", "GlobalTracks/TrackPar/y.png", 3, true, 0);
    processHistogram("track-jet-qa_id9648/TrackPar/xyz", "Track #it{z} position at dca in local coordinate system", "p_{T} (GeV/c)", "#it{z} [cm]", "GlobalTracks/TrackPar/z.png", 4, true, 0);
    processHistogram("track-jet-qa_id9648/TrackPar/alpha", "Rotation angle of local wrt global coordinate system", "p_{T} (GeV/c)", "#alpha [rad]", "GlobalTracks/TrackPar/alpha.png", 2, true, 0);
    processHistogram("track-jet-qa_id9648/TrackPar/signed1Pt", "Track signed 1/#it{p}_{T}", "p_{T} (GeV/c)", "q/p_{T}", "GlobalTracks/TrackPar/signed1Pt.png", 2, true, 0);
    processHistogram("track-jet-qa_id9648/TrackPar/snp", "Sinus of track momentum azimuthal angle", "p_{T} (GeV/c)", "snp", "GlobalTracks/TrackPar/snp.png", 2, true, 0);
    processHistogram("track-jet-qa_id9648/TrackPar/tgl", "Tangent of the track momentum dip angle", "p_{T} (GeV/c)", "tgl", "GlobalTracks/TrackPar/tgl.png", 2, true, 0);
    processHistogram("track-jet-qa_id9648/TrackPar/dcaXY", "Distance of closest approach in #it{xy} plane", "p_{T} (GeV/c)", "dcaXY [cm]", "GlobalTracks/TrackPar/dcaXY.png", 2, true, 0);
    processHistogram("track-jet-qa_id9648/TrackPar/dcaZ", "Distance of closest approach in #it{z} plane", "p_{T} (GeV/c)", "dcaZ [cm]", "GlobalTracks/TrackPar/dcaZ.png", 2, true, 0);
    processHistogram("track-jet-qa_id9648/TrackPar/length", "Track length", "p_{T} (GeV/c)", "length [cm]", "GlobalTracks/TrackPar/length.png", 2, true, 0);
    processHistogram("track-jet-qa_id9648/TrackPar/Sigma1Pt", "Uncertainty over #it{p}_{T}", "p_{T} (GeV/c)", "p_{T}#sigma(1/p_{T})", "GlobalTracks/TrackPar/Sigma1Pt.png", 1, true, 0);
    processHistogram("track-jet-qa_id9648/TrackPar/Sigma1Pt_hasTRD", "Uncertainty over #it{p}_{T} for tracks with TRD", "p_{T} (GeV/c)", "p_{T}#sigma(1/p_{T})", "GlobalTracks/TrackPar/Sigma1Pt_hasTRD.png", 1, true, 0);
    processHistogram("track-jet-qa_id9648/TrackPar/Sigma1Pt_hasNoTRD", "Uncertainty over #it{p}_{T} for tracks without TRD", "p_{T} (GeV/c)", "p_{T}#sigma(1/p_{T})", "GlobalTracks/TrackPar/Sigma1Pt_hasNoTRD.png", 1, true, 0);
    processHistogram("track-jet-qa_id9648/TrackPar/Sigma1Pt_Layer1", "Uncertainty over #it{p}_{T} with only 1st ITS layer active", "p_{T} (GeV/c)", "p_{T}#sigma(1/p_{T})", "GlobalTracks/TrackPar/Sigma1Pt_Layer1.png", 1, true, 0);
    processHistogram("track-jet-qa_id9648/TrackPar/Sigma1Pt_Layer2", "Uncertainty over #it{p}_{T} with only 2nd ITS layer active", "p_{T} (GeV/c)", "p_{T}#sigma(1/p_{T})", "GlobalTracks/TrackPar/Sigma1Pt_Layer2.png", 1, true, 0);
    processHistogram("track-jet-qa_id9648/TrackPar/Sigma1Pt_Layers12", "Uncertainty over #it{p}_{T} with only 1st and 2nd ITS layers active", "p_{T} (GeV/c)", "p_{T}#sigma(1/p_{T})", "GlobalTracks/TrackPar/Sigma1Pt_Layers12.png", 1, true, 0);
    processHistogram("track-jet-qa_id9648/TrackPar/Sigma1Pt_Layer4", "Uncertainty over #it{p}_{T} with only 4th ITS layer active", "p_{T} (GeV/c)", "p_{T}#sigma(1/p_{T})", "GlobalTracks/TrackPar/Sigma1Pt_Layer4.png", 1, true, 0);
    processHistogram("track-jet-qa_id9648/TrackPar/Sigma1Pt_Layer5", "Uncertainty over #it{p}_{T} with only 5th ITS layer active", "p_{T} (GeV/c)", "p_{T}#sigma(1/p_{T})", "GlobalTracks/TrackPar/Sigma1Pt_Layer5.png", 1, true, 0);
    processHistogram("track-jet-qa_id9648/TrackPar/Sigma1Pt_Layer6", "Uncertainty over #it{p}_{T} with only 6th ITS layer active", "p_{T} (GeV/c)", "p_{T}#sigma(1/p_{T})", "GlobalTracks/TrackPar/Sigma1Pt_Layer6.png", 1, true, 0);
    processHistogram("track-jet-qa_id9648/TrackPar/Sigma1Pt_Layers45", "Uncertainty over #it{p}_{T} with only 4th and 5th ITS layers active", "p_{T} (GeV/c)", "p_{T}#sigma(1/p_{T})", "GlobalTracks/TrackPar/Sigma1Pt_Layers45.png", 1, true, 0);
    processHistogram("track-jet-qa_id9648/TrackPar/Sigma1Pt_Layers46", "Uncertainty over #it{p}_{T} with only 4th and 6th ITS layers active", "p_{T} (GeV/c)", "p_{T}#sigma(1/p_{T})", "GlobalTracks/TrackPar/Sigma1Pt_Layers46.png", 1, true, 0);
    processHistogram("track-jet-qa_id9648/TrackPar/Sigma1Pt_Layers56", "Uncertainty over #it{p}_{T} with only 5th and 6th ITS layers active", "p_{T} (GeV/c)", "p_{T}#sigma(1/p_{T})", "GlobalTracks/TrackPar/Sigma1Pt_Layers56.png", 1, true, 0);
    processHistogram("track-jet-qa_id9648/TrackPar/Sigma1Pt_Layers456", "Uncertainty over #it{p}_{T} with only 4th, 5th and 6th ITS layers active", "p_{T} (GeV/c)", "p_{T}#sigma(1/p_{T})", "GlobalTracks/TrackPar/Sigma1Pt_Layers456.png", 1, true, 0);
    // ---- ITS histograms ---- 
    processHistogram("track-jet-qa_id9648/ITS/itsNCls", "ITS clusters", "p_{T} (GeV/c)", "Number of ITS clusters", "GlobalTracks/ITS/itsNCls.png", 2, true, 0);
    processHistogram("track-jet-qa_id9648/ITS/itsChi2NCl", "#chi^{2} per ITS clusters", "p_{T} (GeV/c)", "#frac{#chi^{2}}{ITS clusters}", "GlobalTracks/ITS/itsChi2NCl.png", 2, true, 0);
    processHistogram("track-jet-qa_id9648/ITS/itsHits", "ITS hitmap", "p_{T} (GeV/c)", "ITS layers", "GlobalTracks/ITS/itsHits.png", 2, true, 0);
    // ---- TPC histograms ----
    processHistogram("track-jet-qa_id9648/TPC/tpcNClsFindable", "Number of findable TPC clusters", "p_{T} (GeV/c)", "Number of findable TPC clusters", "GlobalTracks/TPC/tpcNClsFindable.png", 2, true, 0);
    processHistogram("track-jet-qa_id9648/TPC/tpcNClsFound", "Number of found TPC clusters", "p_{T} (GeV/c)", "Number of found TPC clusters", "GlobalTracks/TPC/tpcNClsFound.png", 2, true, 0);
    processHistogram("track-jet-qa_id9648/TPC/tpcNClsShared", "Number of shared TPC clusters", "p_{T} (GeV/c)", "Number of shared TPC clusters", "GlobalTracks/TPC/tpcNClsShared.png", 2, true, 0);
    processHistogram("track-jet-qa_id9648/TPC/tpcNClsCrossedRows", "Number of crossed TPC rows", "p_{T} (GeV/c)", "Number of crossed TPC rows", "GlobalTracks/TPC/tpcNClsCrossedRows.png", 2, true, 0);
    processHistogram("track-jet-qa_id9648/TPC/tpcFractionSharedCls", "Fraction of shared TPC clusters", "p_{T} (GeV/c)", "Fraction of shared TPC clusters", "GlobalTracks/TPC/tpcFractionSharedCls.png", 2, true, 0);
    processHistogram("track-jet-qa_id9648/TPC/tpcCrossedRowsOverFindableCls", "Crossed TPC rows over findable clusters", "p_{T} (GeV/c)", "#frac{crossed TPC rows}{findable clusters}", "GlobalTracks/TPC/tpcCrossedRowsOverFindableCls.png", 2, true, 0);
    processHistogram("track-jet-qa_id9648/TPC/tpcChi2NCl", "#chi^{2} per TPC clusters", "p_{T} (GeV/c)", "#frac{#chi^{2}}{TPC clusters}", "GlobalTracks/TPC/tpcChi2NCl.png", 2, true, 0);


/*
    // ---------------------------- id9646 = loose cuts ---------------------------- 
    // ---- Event property histograms ---- 
    processHistogram("track-jet-qa_id9646/EventProp/collisionVtxZ", "Collsion Vertex Z position", "Vtx_{z} [cm]", "", "loose/EventProp/collisionVtxZ.png", 0);
    processHistogram("track-jet-qa_id9646/EventProp/collisionVtxZnoSel", "Collsion Vertex Z position without event selection", "Vtx_{z} [cm]", "", "loose/EventProp/collisionVtxZnoSel.png", 0);
    processHistogram("track-jet-qa_id9646/EventProp/collisionVtxZSel8", "Collsion Vertex Z position with event selection", "Vtx_{z} [cm]", "", "loose/EventProp/collisionVtxZSel8.png", 0);
    processHistogram("track-jet-qa_id9646/EventProp/rejectedCollId", "CollisionId of collisions that did not pass the event selection", "collisionId", "", "loose/EventProp/rejectedCollId.png");
    // ---- Kinetic histograms ---- 
    processHistogram("track-jet-qa_id9646/Kine/EtaPhiPt", "Correlation of #eta and #it{p}_{T}", "p_{T} (GeV/c)", "#eta", "loose/Kine/eta.png", 2, true, 0);
    processHistogram("track-jet-qa_id9646/Kine/EtaPhiPt", "Correlation of #phi and #it{p}_{T}", "p_{T} (GeV/c)", "#phi [rad]", "loose/Kine/phi.png", 3, true, 0);
    processHistogram("track-jet-qa_id964/Kine/EtaPhiPt", "Correlation of #eta and #phi", "#eta", "#phi [rad]", "loose/Kine/etaphi.png", 3, true, 2);
    PlotPtRangeHistograms(file, "track-jet-qa_id9646/Kine/EtaPhiPt", "Correlation of #eta and #phi", "#eta", "#phi [rad]", "loose/Kine/etaphi", 3, 2);
    THnSparseD* hPtSparse = dynamic_cast<THnSparseD*>(file->Get("track-jet-qa_id9646/Kine/pt")); 
    THnSparseD* hPt_TRDSparse = dynamic_cast<THnSparseD*>(file->Get("track-jet-qa_id9646/Kine/pt_TRD"));
    if (!hPtSparse || !hPt_TRDSparse) {
        std::cerr << "Sparse histograms could not be found." << std::endl;
        file->Close();
        delete file;
        return;
    }
    TH1D* pt = Project1D(hPtSparse, 0, "E");
    TH1D* pt_TRD = Project1D(hPt_TRDSparse, 0, "E");
    if (!pt || !pt_TRD) {
        std::cerr << "Projection failed." << std::endl;
        file->Close();
        delete file;
        return;
    }
    pt->Scale(1.0 / numberOfEvents);
    pt_TRD->Scale(1.0 / numberOfEvents);
    pt->SetTitle(" ; p_{T} (GeV/c); Number of entries / Number of events");
    pt_TRD->SetTitle(" ; p_{T} (GeV/c); Number of entries / Number of events");
    TCanvas* pVSp = new TCanvas("pVSp", "pt_comparison", 3000, 1200);
    pVSp->SetTopMargin(0.20);
    pVSp->Divide(2,1);
    pVSp->cd(1);
    gPad->SetLogy();
    pt->SetLineColor(kGreen-2);
    pt->SetLineWidth(6);
    pt->SetMarkerStyle(20);
    pt->SetMarkerSize(2);
    pt->SetMarkerColor(kGreen-2);
    pt_TRD->SetLineColor(kSpring+10);
    pt_TRD->SetLineWidth(6);
    pt_TRD->SetMarkerStyle(20);
    pt_TRD->SetMarkerSize(2);
    pt_TRD->SetMarkerColor(kSpring+10);
    pt->Draw("HIST");
    pt_TRD->Draw("HIST SAME");
    gPad->Update();
    TLatex latex1;
    latex1.SetTextSize(0.03);
    latex1.DrawLatexNDC(0.1, 0.93, "p_{T} spectra overall and with TRD match");
    TPaveStats* st_pt = (TPaveStats*)pt->FindObject("stats");
    if (st_pt) {
        st_pt->SetX1NDC(0.73);
        st_pt->SetX2NDC(0.90);
        st_pt->SetY1NDC(0.90);
        st_pt->SetY2NDC(0.99);
    }
    auto legend = new TLegend(0.60, 0.73, 0.9, 0.9);
    legend->SetFillStyle(0);
    legend->AddEntry(pt, "overall p_{T}", "lep");
    legend->AddEntry(pt_TRD, "p_{T} if track has a TRD match", "lep");
    legend->Draw();
    pVSp->cd(2);
    TH1D* pt_reb = (TH1D*)pt->Clone("pt_reb");
    TH1D* pt_TRD_reb = (TH1D*)pt_TRD->Clone("pt_TRD_reb");
    pt_reb->Rebin(5);
    pt_TRD_reb->Rebin(5);
    TH1D* pt_ratio = (TH1D*)pt_TRD_reb->Clone("pt_ratio");
    pt_ratio->Divide(pt_reb);
    pt_ratio->SetLineColor(kRed-7);
    pt_ratio->SetLineWidth(3);
    pt_ratio->SetMarkerStyle(20);
    pt_ratio->SetMarkerColor(kRed-7);
    pt_ratio->GetYaxis()->SetTitle("pt_TRD / pt");
    pt_ratio->Draw("E");
    gPad->Update();
    TLatex latex2;
    latex2.SetTextSize(0.03);
    latex2.DrawLatexNDC(0.1, 0.93, "Ratio between p_{T} with TRD match and p_{T} overall");
    TPaveStats* st_pt_ratio = (TPaveStats*)pt_ratio->FindObject("stats");
    if (st_pt_ratio) {
        st_pt_ratio->SetX1NDC(0.73);
        st_pt_ratio->SetX2NDC(0.90);
        st_pt_ratio->SetY1NDC(0.90);
        st_pt_ratio->SetY2NDC(0.99);
    }
    pVSp->SaveAs("GlobalTracks/Kine/pt_comparison.png"); 
    delete pVSp; 
    // ---- Track Parameter histograms ---- 
    processHistogram("track-jet-qa_id9646/TrackPar/xyz", "Track #it{x} position at dca in local coordinate system", "p_{T} (GeV/c)", "#it{x} [cm]", "loose/TrackPar/x.png", 2, true, 0);
    processHistogram("track-jet-qa_id9646/TrackPar/xyz", "Track #it{y} position at dca in local coordinate system", "p_{T} (GeV/c)", "#it{y} [cm]", "loose/TrackPar/y.png", 3, true, 0);
    processHistogram("track-jet-qa_id9646/TrackPar/xyz", "Track #it{z} position at dca in local coordinate system", "p_{T} (GeV/c)", "#it{z} [cm]", "loose/TrackPar/z.png", 4, true, 0);
    processHistogram("track-jet-qa_id9646/TrackPar/alpha", "Rotation angle of local wrt global coordinate system", "p_{T} (GeV/c)", "#alpha [rad]", "loose/TrackPar/alpha.png", 2, true, 0);
    processHistogram("track-jet-qa_id9646/TrackPar/signed1Pt", "Track signed 1/#it{p}_{T}", "p_{T} (GeV/c)", "q/p_{T}", "loose/TrackPar/signed1Pt.png", 2, true, 0);
    processHistogram("track-jet-qa_id9646/TrackPar/snp", "Sinus of track momentum azimuthal angle", "p_{T} (GeV/c)", "snp", "loose/TrackPar/snp.png", 2, true, 0);
    processHistogram("track-jet-qa_id9646/TrackPar/tgl", "Tangent of the track momentum dip angle", "p_{T} (GeV/c)", "tgl", "loose/TrackPar/tgl.png", 2, true, 0);
    processHistogram("track-jet-qa_id9646/TrackPar/dcaXY", "Distance of closest approach in #it{xy} plane", "p_{T} (GeV/c)", "dcaXY [cm]", "loose/TrackPar/dcaXY.png", 2, true, 0);
    processHistogram("track-jet-qa_id9646/TrackPar/dcaZ", "Distance of closest approach in #it{z} plane", "p_{T} (GeV/c)", "dcaZ [cm]", "loose/TrackPar/dcaZ.png", 2, true, 0);
    processHistogram("track-jet-qa_id9646/TrackPar/length", "Track length", "p_{T} (GeV/c)", "length [cm]", "loose/TrackPar/length.png", 2, true, 0);
    processHistogram("track-jet-qa_id9646/TrackPar/Sigma1Pt", "Uncertainty over #it{p}_{T}", "p_{T} (GeV/c)", "p_{T}#sigma(1/p_{T})", "loose/TrackPar/Sigma1Pt.png", 1, true, 0);
    processHistogram("track-jet-qa_id9646/TrackPar/Sigma1Pt_hasTRD", "Uncertainty over #it{p}_{T} for tracks with TRD", "p_{T} (GeV/c)", "p_{T}#sigma(1/p_{T})", "loose/TrackPar/Sigma1Pt_hasTRD.png", 1, true, 0);
    processHistogram("track-jet-qa_id9646/TrackPar/Sigma1Pt_hasNoTRD", "Uncertainty over #it{p}_{T} for tracks without TRD", "p_{T} (GeV/c)", "p_{T}#sigma(1/p_{T})", "loose/TrackPar/Sigma1Pt_hasNoTRD.png", 1, true, 0);
    processHistogram("track-jet-qa_id9646/TrackPar/Sigma1Pt_Layer1", "Uncertainty over #it{p}_{T} with only 1st ITS layer active", "p_{T} (GeV/c)", "p_{T}#sigma(1/p_{T})", "loose/TrackPar/Sigma1Pt_Layer1.png", 1, true, 0);
    processHistogram("track-jet-qa_id9646/TrackPar/Sigma1Pt_Layer2", "Uncertainty over #it{p}_{T} with only 2nd ITS layer active", "p_{T} (GeV/c)", "p_{T}#sigma(1/p_{T})", "loose/TrackPar/Sigma1Pt_Layer2.png", 1, true, 0);
    processHistogram("track-jet-qa_id9646/TrackPar/Sigma1Pt_Layers12", "Uncertainty over #it{p}_{T} with only 1st and 2nd ITS layers active", "p_{T} (GeV/c)", "p_{T}#sigma(1/p_{T})", "loose/TrackPar/Sigma1Pt_Layer12.png", 1, true, 0);
    processHistogram("track-jet-qa_id9646/TrackPar/Sigma1Pt_Layer4", "Uncertainty over #it{p}_{T} with only 4th ITS layer active", "p_{T} (GeV/c)", "p_{T}#sigma(1/p_{T})", "loose/TrackPar/Sigma1Pt_Layer4.png", 1, true, 0);
    processHistogram("track-jet-qa_id9646/TrackPar/Sigma1Pt_Layer5", "Uncertainty over #it{p}_{T} with only 5th ITS layer active", "p_{T} (GeV/c)", "p_{T}#sigma(1/p_{T})", "loose/TrackPar/Sigma1Pt_Layer5.png", 1, true, 0);
    processHistogram("track-jet-qa_id9646/TrackPar/Sigma1Pt_Layer6", "Uncertainty over #it{p}_{T} with only 6th ITS layer active", "p_{T} (GeV/c)", "p_{T}#sigma(1/p_{T})", "loose/TrackPar/Sigma1Pt_Layer6.png", 1, true, 0);
    processHistogram("track-jet-qa_id9646/TrackPar/Sigma1Pt_Layers45", "Uncertainty over #it{p}_{T} with only 4th and 5th ITS layers active", "p_{T} (GeV/c)", "p_{T}#sigma(1/p_{T})", "loose/TrackPar/Sigma1Pt_Layer45.png", 1, true, 0);
    processHistogram("track-jet-qa_id9646/TrackPar/Sigma1Pt_Layers46", "Uncertainty over #it{p}_{T} with only 4th and 6th ITS layers active", "p_{T} (GeV/c)", "p_{T}#sigma(1/p_{T})", "loose/TrackPar/Sigma1Pt_Layer46.png", 1, true, 0);
    processHistogram("track-jet-qa_id9646/TrackPar/Sigma1Pt_Layers56", "Uncertainty over #it{p}_{T} with only 5th and 6th ITS layers active", "p_{T} (GeV/c)", "p_{T}#sigma(1/p_{T})", "loose/TrackPar/Sigma1Pt_Layer56.png", 1, true, 0);
    processHistogram("track-jet-qa_id9646/TrackPar/Sigma1Pt_Layers456", "Uncertainty over #it{p}_{T} with only 4th, 5th and 6th ITS layers active", "p_{T} (GeV/c)", "p_{T}#sigma(1/p_{T})", "loose/TrackPar/Sigma1Pt_Layer456.png", 1, true, 0);
    // ---- ITS histograms -----
    processHistogram("track-jet-qa_id9646/ITS/itsNCls", "ITS clusters", "p_{T} (GeV/c)", "Number of ITS clusters", "loose/ITS/itsNCls.png", 2, true, 0);
    processHistogram("track-jet-qa_id9646/ITS/itsChi2NCl", "#chi^{2} per ITS clusters", "p_{T} (GeV/c)", "#frac{#chi^{2}}{ITS clusters}", "loose/ITS/itsChi2NCl.png", 2, true, 0);
    processHistogram("track-jet-qa_id9646/ITS/itsHits", "ITS hitmap", "p_{T} (GeV/c)", "ITS layers", "loose/ITS/itsHits.png", 2, true, 0);
    // ---- TPC histograms -----
    processHistogram("track-jet-qa_id9646/TPC/tpcNClsFindable", "Number of findable TPC clusters", "p_{T} (GeV/c)", "Number of findable TPC clusters", "loose/TPC/tpcNClsFindable.png", 2, true, 0);
    processHistogram("track-jet-qa_id9646/TPC/tpcNClsFound", "Number of found TPC clusters", "p_{T} (GeV/c)", "Number of found TPC clusters", "loose/TPC/tpcNClsFound.png", 2, true, 0);
    processHistogram("track-jet-qa_id9646/TPC/tpcNClsShared", "Number of shared TPC clusters", "p_{T} (GeV/c)", "Number of shared TPC clusters", "loose/TPC/tpcNClsShared.png", 2, true, 0);
    processHistogram("track-jet-qa_id9646/TPC/tpcNClsCrossedRows", "Number of crossed TPC rows", "p_{T} (GeV/c)", "Number of crossed TPC rows", "loose/TPC/tpcNClsCrossedRows.png", 2, true, 0);
    processHistogram("track-jet-qa_id9646/TPC/tpcFractionSharedCls", "Fraction of shared TPC clusters", "p_{T} (GeV/c)", "Fraction of shared TPC clusters", "loose/TPC/tpcFractionSharedCls.png", 2, true, 0);
    processHistogram("track-jet-qa_id9646/TPC/tpcCrossedRowsOverFindableCls", "Crossed TPC rows over findable clusters", "p_{T} (GeV/c)", "#frac{crossed TPC rows}{findable clusters}", "loose/TPC/tpcCrossedRowsOverFindableCls.png", 2, true, 0);
    processHistogram("track-jet-qa_id9646/TPC/tpcChi2NCl", "#chi^{2} per TPC clusters", "p_{T} (GeV/c)", "#frac{#chi^{2}}{TPC clusters}", "loose/TPC/tpcChi2NCl.png", 2, true, 0);
*/

/*
    // ---------------------------- id9647 = tight cuts ---------------------------- 
    // ---- Event property histograms ---- 
    processHistogram("track-jet-qa_id9647/EventProp/collisionVtxZ", "Collsion Vertex Z position", "Vtx_{z} [cm]", "", "tight/EventProp/collisionVtxZ.png", 0);
    processHistogram("track-jet-qa_id9647/EventProp/collisionVtxZnoSel", "Collsion Vertex Z position without event selection", "Vtx_{z} [cm]", "", "tight/EventProp/collisionVtxZnoSel.png", 0);
    processHistogram("track-jet-qa_id9647/EventProp/collisionVtxZSel8", "Collsion Vertex Z position with event selection", "Vtx_{z} [cm]", "", "tight/EventProp/collisionVtxZSel8.png", 0);
    processHistogram("track-jet-qa_id9647/EventProp/rejectedCollId", "CollisionId of collisions that did not pass the event selection", "collisionId", "", "tight/EventProp/rejectedCollId.png");
    // ---- Kinetic histograms -----
    processHistogram("track-jet-qa_id9647/Kine/EtaPhiPt", "Correlation of #eta and #it{p}_{T}", "p_{T} (GeV/c)", "#eta", "tight/Kine/eta.png", 2, true, 0);
    processHistogram("track-jet-qa_id9647/Kine/EtaPhiPt", "Correlation of #phi and #it{p}_{T}", "p_{T} (GeV/c)", "#phi [rad]", "tight/Kine/phi.png", 3, true, 0);
    processHistogram("track-jet-qa_id9647/Kine/EtaPhiPt", "Correlation of #eta and #phi", "#eta", "#phi [rad]", "tight/Kine/etaphi.png", 3, true, 2);
    PlotPtRangeHistograms(file, "track-jet-qa_id9647/Kine/EtaPhiPt", "Correlation of #eta and #phi", "#eta", "#phi [rad]", "tight/Kine/etaphi", 3, 2);
    THnSparseD* hPtSparse = dynamic_cast<THnSparseD*>(file->Get("track-jet-qa_id9647/Kine/pt")); 
    THnSparseD* hPt_TRDSparse = dynamic_cast<THnSparseD*>(file->Get("track-jet-qa_id9647/Kine/pt_TRD"));
    if (!hPtSparse || !hPt_TRDSparse) {
        std::cerr << "Sparse histograms could not be found." << std::endl;
        file->Close();
        delete file;
        return;
    }
    TH1D* pt = Project1D(hPtSparse, 0, "E");
    TH1D* pt_TRD = Project1D(hPt_TRDSparse, 0, "E");
    if (!pt || !pt_TRD) {
        std::cerr << "Projection failed." << std::endl;
        file->Close();
        delete file;
        return;
    }
    pt->Scale(1.0 / numberOfEvents);
    pt_TRD->Scale(1.0 / numberOfEvents);
    pt->SetTitle(" ; p_{T} (GeV/c); Number of entries / Number of events");
    pt_TRD->SetTitle(" ; p_{T} (GeV/c); Number of entries / Number of events");
    TCanvas* pVSp = new TCanvas("pVSp", "pt_comparison", 3000, 1200);
    pVSp->SetTopMargin(0.20);
    pVSp->Divide(2,1);
    pVSp->cd(1);
    gPad->SetLogy();
    pt->SetLineColor(kGreen-2);
    pt->SetLineWidth(6);
    pt->SetMarkerStyle(20);
    pt->SetMarkerSize(2);
    pt->SetMarkerColor(kGreen-2);
    pt_TRD->SetLineColor(kSpring+10);
    pt_TRD->SetLineWidth(6);
    pt_TRD->SetMarkerStyle(20);
    pt_TRD->SetMarkerSize(2);
    pt_TRD->SetMarkerColor(kSpring+10);
    pt->Draw("HIST");
    pt_TRD->Draw("HIST SAME");
    gPad->Update();
    TLatex latex1;
    latex1.SetTextSize(0.03);
    latex1.DrawLatexNDC(0.1, 0.93, "p_{T} spectra overall and with TRD match");
    TPaveStats* st_pt = (TPaveStats*)pt->FindObject("stats");
    if (st_pt) {
        st_pt->SetX1NDC(0.73);
        st_pt->SetX2NDC(0.90);
        st_pt->SetY1NDC(0.90);
        st_pt->SetY2NDC(0.99);
    }
    auto legend = new TLegend(0.60, 0.73, 0.9, 0.9);
    legend->SetFillStyle(0);
    legend->AddEntry(pt, "overall p_{T}", "lep");
    legend->AddEntry(pt_TRD, "p_{T} if track has a TRD match", "lep");
    legend->Draw();
    pVSp->cd(2);
    TH1D* pt_reb = (TH1D*)pt->Clone("pt_reb");
    TH1D* pt_TRD_reb = (TH1D*)pt_TRD->Clone("pt_TRD_reb");
    pt_reb->Rebin(5);
    pt_TRD_reb->Rebin(5);
    TH1D* pt_ratio = (TH1D*)pt_TRD_reb->Clone("pt_ratio");
    pt_ratio->Divide(pt_reb);
    pt_ratio->SetLineColor(kRed-7);
    pt_ratio->SetLineWidth(3);
    pt_ratio->SetMarkerStyle(20);
    pt_ratio->SetMarkerColor(kRed-7);
    pt_ratio->GetYaxis()->SetTitle("pt_TRD / pt");
    pt_ratio->Draw("E");
    gPad->Update();
    TLatex latex2;
    latex2.SetTextSize(0.03);
    latex2.DrawLatexNDC(0.1, 0.93, "Ratio between p_{T} with TRD match and p_{T} overall");
    TPaveStats* st_pt_ratio = (TPaveStats*)pt_ratio->FindObject("stats");
    if (st_pt_ratio) {
        st_pt_ratio->SetX1NDC(0.73);
        st_pt_ratio->SetX2NDC(0.90);
        st_pt_ratio->SetY1NDC(0.90);
        st_pt_ratio->SetY2NDC(0.99);
    }
    pVSp->SaveAs("GlobalTracks/Kine/pt_comparison.png"); 
    delete pVSp; 
    // ---- Track Parameter histograms ---- 
    processHistogram("track-jet-qa_id9647/TrackPar/xyz", "Track #it{x} position at dca in local coordinate system", "p_{T} (GeV/c)", "#it{x} [cm]", "tight/TrackPar/x.png", 2, true, 0);
    processHistogram("track-jet-qa_id9647/TrackPar/xyz", "Track #it{y} position at dca in local coordinate system", "p_{T} (GeV/c)", "#it{y} [cm]", "tight/TrackPar/y.png", 3, true, 0);
    processHistogram("track-jet-qa_id9647/TrackPar/xyz", "Track #it{z} position at dca in local coordinate system", "p_{T} (GeV/c)", "#it{z} [cm]", "tight/TrackPar/z.png", 4, true, 0);
    processHistogram("track-jet-qa_id9647/TrackPar/alpha", "Rotation angle of local wrt global coordinate system", "p_{T} (GeV/c)", "#alpha [rad]", "tight/TrackPar/alpha.png", 2, true, 0);
    processHistogram("track-jet-qa_id9647/TrackPar/signed1Pt", "Track signed 1/#it{p}_{T}", "p_{T} (GeV/c)", "q/p_{T}", "tight/TrackPar/signed1Pt.png", 2, true, 0);
    processHistogram("track-jet-qa_id9647/TrackPar/snp", "Sinus of track momentum azimuthal angle", "p_{T} (GeV/c)", "snp", "tight/TrackPar/snp.png", 2, true, 0);
    processHistogram("track-jet-qa_id9647/TrackPar/tgl", "Tangent of the track momentum dip angle", "p_{T} (GeV/c)", "tgl", "tight/TrackPar/tgl.png", 2, true, 0);
    processHistogram("track-jet-qa_id9647/TrackPar/dcaXY", "Distance of closest approach in #it{xy} plane", "p_{T} (GeV/c)", "dcaXY [cm]", "tight/TrackPar/dcaXY.png", 2, true, 0);
    processHistogram("track-jet-qa_id9647/TrackPar/dcaZ", "Distance of closest approach in #it{z} plane", "p_{T} (GeV/c)", "dcaZ [cm]", "tight/TrackPar/dcaZ.png", 2, true, 0);
    processHistogram("track-jet-qa_id9647/TrackPar/length", "Track length", "p_{T} (GeV/c)", "length [cm]", "tight/TrackPar/length.png", 2, true, 0);
    processHistogram("track-jet-qa_id9647/TrackPar/Sigma1Pt", "Uncertainty over #it{p}_{T}", "p_{T} (GeV/c)", "p_{T}#sigma(1/p_{T})", "tight/TrackPar/Sigma1Pt.png", 1, true, 0);
    processHistogram("track-jet-qa_id9647/TrackPar/Sigma1Pt_hasTRD", "Uncertainty over #it{p}_{T} for tracks with TRD", "p_{T} (GeV/c)", "p_{T}#sigma(1/p_{T})", "tight/TrackPar/Sigma1Pt_hasTRD.png", 1, true, 0);
    processHistogram("track-jet-qa_id9647/TrackPar/Sigma1Pt_hasNoTRD", "Uncertainty over #it{p}_{T} for tracks without TRD", "p_{T} (GeV/c)", "p_{T}#sigma(1/p_{T})", "tight/TrackPar/Sigma1Pt_hasNoTRD.png", 1, true, 0);
    processHistogram("track-jet-qa_id9647/TrackPar/Sigma1Pt_Layer1", "Uncertainty over #it{p}_{T} with only 1st ITS layer active", "p_{T} (GeV/c)", "p_{T}#sigma(1/p_{T})", "tight/TrackPar/Sigma1Pt_Layer1.png", 1, true, 0);
    processHistogram("track-jet-qa_id9647/TrackPar/Sigma1Pt_Layer2", "Uncertainty over #it{p}_{T} with only 2nd ITS layer active", "p_{T} (GeV/c)", "p_{T}#sigma(1/p_{T})", "tight/TrackPar/Sigma1Pt_Layer2.png", 1, true, 0);
    processHistogram("track-jet-qa_id9647/TrackPar/Sigma1Pt_Layers12", "Uncertainty over #it{p}_{T} with only 1st and 2nd ITS layers active", "p_{T} (GeV/c)", "p_{T}#sigma(1/p_{T})", "tight/TrackPar/Sigma1Pt_Layer12.png", 1, true, 0);
    processHistogram("track-jet-qa_id9647/TrackPar/Sigma1Pt_Layer4", "Uncertainty over #it{p}_{T} with only 4th ITS layer active", "p_{T} (GeV/c)", "p_{T}#sigma(1/p_{T})", "tight/TrackPar/Sigma1Pt_Layer4.png", 1, true, 0);
    processHistogram("track-jet-qa_id9647/TrackPar/Sigma1Pt_Layer5", "Uncertainty over #it{p}_{T} with only 5th ITS layer active", "p_{T} (GeV/c)", "p_{T}#sigma(1/p_{T})", "tight/TrackPar/Sigma1Pt_Layer5.png", 1, true, 0);
    processHistogram("track-jet-qa_id9647/TrackPar/Sigma1Pt_Layer6", "Uncertainty over #it{p}_{T} with only 6th ITS layer active", "p_{T} (GeV/c)", "p_{T}#sigma(1/p_{T})", "tight/TrackPar/Sigma1Pt_Layer6.png", 1, true, 0);
    processHistogram("track-jet-qa_id9647/TrackPar/Sigma1Pt_Layers45", "Uncertainty over #it{p}_{T} with only 4th and 5th ITS layers active", "p_{T} (GeV/c)", "p_{T}#sigma(1/p_{T})", "tight/TrackPar/Sigma1Pt_Layer45.png", 1, true, 0);
    processHistogram("track-jet-qa_id9647/TrackPar/Sigma1Pt_Layers46", "Uncertainty over #it{p}_{T} with only 4th and 6th ITS layers active", "p_{T} (GeV/c)", "p_{T}#sigma(1/p_{T})", "tight/TrackPar/Sigma1Pt_Layer46.png", 1, true, 0);
    processHistogram("track-jet-qa_id9647/TrackPar/Sigma1Pt_Layers56", "Uncertainty over #it{p}_{T} with only 5th and 6th ITS layers active", "p_{T} (GeV/c)", "p_{T}#sigma(1/p_{T})", "tight/TrackPar/Sigma1Pt_Layer56.png", 1, true, 0);
    processHistogram("track-jet-qa_id9647/TrackPar/Sigma1Pt_Layers456", "Uncertainty over #it{p}_{T} with only 4th, 5th and 6th ITS layers active", "p_{T} (GeV/c)", "p_{T}#sigma(1/p_{T})", "tight/TrackPar/Sigma1Pt_Layer456.png", 1, true, 0);
    // ---- ITS histograms -----
    processHistogram("track-jet-qa_id9647/ITS/itsNCls", "ITS clusters", "p_{T} (GeV/c)", "Number of ITS clusters", "tight/ITS/itsNCls.png", 2, true, 0);
    processHistogram("track-jet-qa_id9647/ITS/itsChi2NCl", "#chi^{2} per ITS clusters", "p_{T} (GeV/c)", "#frac{#chi^{2}}{ITS clusters}", "tight/ITS/itsChi2NCl.png", 2, true, 0);
    processHistogram("track-jet-qa_id9647/ITS/itsHits", "ITS hitmap", "p_{T} (GeV/c)", "ITS layers", "tight/ITS/itsHits.png", 2, true, 0);
    // ---- TPC histograms ----
    processHistogram("track-jet-qa_id9647/TPC/tpcNClsFindable", "Number of findable TPC clusters", "p_{T} (GeV/c)", "Number of findable TPC clusters", "tight/TPC/tpcNClsFindable.png", 2, true, 0);
    processHistogram("track-jet-qa_id9647/TPC/tpcNClsFound", "Number of found TPC clusters", "p_{T} (GeV/c)", "Number of found TPC clusters", "tight/TPC/tpcNClsFound.png", 2, true, 0);
    processHistogram("track-jet-qa_id9647/TPC/tpcNClsShared", "Number of shared TPC clusters", "p_{T} (GeV/c)", "Number of shared TPC clusters", "tight/TPC/tpcNClsShared.png", 2, true, 0);
    processHistogram("track-jet-qa_id9647/TPC/tpcNClsCrossedRows", "Number of crossed TPC rows", "p_{T} (GeV/c)", "Number of crossed TPC rows", "tight/TPC/tpcNClsCrossedRows.png", 2, true, 0);
    processHistogram("track-jet-qa_id9647/TPC/tpcFractionSharedCls", "Fraction of shared TPC clusters", "p_{T} (GeV/c)", "Fraction of shared TPC clusters", "tight/TPC/tpcFractionSharedCls.png", 2, true, 0);
    processHistogram("track-jet-qa_id9647/TPC/tpcCrossedRowsOverFindableCls", "Crossed TPC rows over findable clusters", "p_{T} (GeV/c)", "#frac{crossed TPC rows}{findable clusters}", "tight/TPC/tpcCrossedRowsOverFindableCls.png", 2, true, 0);
    processHistogram("track-jet-qa_id9647/TPC/tpcChi2NCl", "#chi^{2} per TPC clusters", "p_{T} (GeV/c)", "#frac{#chi^{2}}{TPC clusters}", "tight/TPC/tpcChi2NCl.png", 2, true, 0);
*/

    file->Close();
    delete file;
}
