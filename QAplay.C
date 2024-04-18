#include <vector>
#include <utility>
#include <iostream>
#include <TFile.h>
#include <TH1D.h>
#include <TH2D.h>
#include <TString.h>
#include <TCanvas.h>
#include <TLegend.h>
#include <TSystem.h>
#include <TPaveStats.h>

std::map<std::string, std::tuple<std::string, std::string, std::string>> histogramMetadata;

void InitializeHistogramMetadata() {
    histogramMetadata["tpcNClsFindable"] = std::make_tuple("Number of findable TPC clusters", "Number of findable TPC clusters", "Number of entries / Number of events");
    histogramMetadata["tpcNClsFound"] = std::make_tuple("Number of found TPC clusters", "Number of findable TPC clusters", "Number of entries / Number of events");
    histogramMetadata["tpcNClsShared"] = std::make_tuple("Number of shared TPC clusters", "Number of shared TPC clusters", "Number of entries / Number of events");
    histogramMetadata["tpcNClsCrossedRows"] = std::make_tuple("Number of crossed TPC rows", "Number of crossed TPC rows", "Number of entries / Number of events");
    histogramMetadata["tpcFractionSharedCls"] = std::make_tuple("Fraction of shared TPC clusters", "Fraction of shared TPC clusters", "Number of entries / Number of events");
    histogramMetadata["tpcCrossedRowsOverFindableCls"] = std::make_tuple("Crossed TPC rows over findable clusters", "#frac{crossed TPC rows}{findable clusters}", "Number of entries / Number of events");
    histogramMetadata["tpcChi2NCl"] = std::make_tuple("#chi^{2} per TPC clusters", "#chi^{2} per TPC clusters", "Number of entries / Number of events");

    histogramMetadata["itsHits"] = std::make_tuple("ITS hitmap", "ITS layers", "Number of entries / Number of events");
    histogramMetadata["itsNCls"] = std::make_tuple("ITS clusters", "Number of ITS clusters", "Number of entries / Number of events");
    histogramMetadata["itsChi2NCl"] = std::make_tuple("#chi^{2} per ITS clusters", "#frac{#chi^{2}}{ITS clusters}", "Number of entries / Number of events");
    
    histogramMetadata["Sigma1Pt_Layers12"] = std::make_tuple("Uncertainty over #it{p}_{T} with inner ITS layers active", "p_{T}#sigma(1/p_{T})", "Number of entries / Number of events");
    histogramMetadata["Sigma1Pt_Layers456"] = std::make_tuple("Uncertainty over #it{p}_{T} with outer ITS layers active", "p_{T}#sigma(1/p_{T})", "Number of entries / Number of events");
}

TH1D* Project1D(THnSparse* h, int axis, Option_t* option = "") {
    if (!h) return nullptr;
    for (int i = 0; i < h->GetNdimensions(); ++i) {
        if (i != axis) {
            h->GetAxis(i)->SetRange(1, -1);
        }
    }
    TH1D* h1 = dynamic_cast<TH1D*>(h->Projection(axis, option));
    if (h1) {
        h1->SetName(TString::Format("%s_Proj1D_%d", h->GetName(), axis));
    }
    for (int i = 0; i < h->GetNdimensions(); ++i) {
        h->GetAxis(i)->SetRange(1, -1);
    }
    return h1;
}

TH2D* Project2D(THnSparseD* h, int axis1, int axis2, double ptMin, double ptMax, Option_t* option = "") {
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

void SaveProjection_sigma1pT_loose(TFile* file, const char* histPath, const char* baseTitle, const char* xTitle, const char* yTitle, const char* baseFileName, int axis1, int axis2) {
    THnSparseD* histSparse = dynamic_cast<THnSparseD*>(file->Get(histPath));
    THnSparseD* hPtSparse = dynamic_cast<THnSparseD*>(file->Get("track-jet-qa_id9646/Kine/pt")); 
    TH1D* pt = Project1D(hPtSparse, 0, "E");
    std::vector<std::pair<double, double>> ptBins = {{0, 10}, {10, 80}, {80, 200}};
    std::vector<double> N_entries_per_bin(ptBins.size());
    TFile* outFile = new TFile("Comparisons/projections_sigma1pT_loose.root", "UPDATE");
    for (size_t i = 0; i < ptBins.size(); ++i) {
        double ptMin = ptBins[i].first;
        double ptMax = ptBins[i].second;
        int binMin = pt->GetXaxis()->FindBin(ptMin);
        int binMax = pt->GetXaxis()->FindBin(ptMax) - 1; 
        N_entries_per_bin[i] = pt->Integral(binMin, binMax);
        TH2D* hist2D = Project2D(histSparse, axis1, axis2, ptMin, ptMax, "E");
        if (!hist2D) {
            std::cerr << "Projection failed for " << histPath << " in pT range [" << ptMin << ", " << ptMax << "]." << std::endl;
            continue;
        }
        if (N_entries_per_bin[i] != 0) { 
            hist2D->Scale(1.0 / N_entries_per_bin[i]);
        }
        TH1D* histY = hist2D->ProjectionY(TString::Format("%s_py_%g_%g", hist2D->GetName(), ptMin, ptMax), 1, -1, "e");
        outFile->cd();
        histY->Write();
    }
    outFile->Close();
    delete outFile;
}

void SaveProjection_sigma1pT_tight(TFile* file, const char* histPath, const char* baseTitle, const char* xTitle, const char* yTitle, const char* baseFileName, int axis1, int axis2) {
    THnSparseD* histSparse = dynamic_cast<THnSparseD*>(file->Get(histPath));
    THnSparseD* hPtSparse = dynamic_cast<THnSparseD*>(file->Get("track-jet-qa_id9647/Kine/pt")); 
    TH1D* pt = Project1D(hPtSparse, 0, "E");
    std::vector<std::pair<double, double>> ptBins = {{0, 10}, {10, 80}, {80, 200}};
    std::vector<double> N_entries_per_bin(ptBins.size());
    TFile* outFile = new TFile("Comparisons/projections_sigma1pT_tight.root", "UPDATE");
    for (size_t i = 0; i < ptBins.size(); ++i) {
        double ptMin = ptBins[i].first;
        double ptMax = ptBins[i].second;
        int binMin = pt->GetXaxis()->FindBin(ptMin);
        int binMax = pt->GetXaxis()->FindBin(ptMax) - 1; 
        N_entries_per_bin[i] = pt->Integral(binMin, binMax);
        TH2D* hist2D = Project2D(histSparse, axis1, axis2, ptMin, ptMax, "E");
        if (!hist2D) {
            std::cerr << "Projection failed for " << histPath << " in pT range [" << ptMin << ", " << ptMax << "]." << std::endl;
            continue;
        }
        if (N_entries_per_bin[i] != 0) { 
            hist2D->Scale(1.0 / N_entries_per_bin[i]);
        }
        TH1D* histY = hist2D->ProjectionY(TString::Format("%s_py_%g_%g", hist2D->GetName(), ptMin, ptMax), 1, -1, "e");
        outFile->cd();
        histY->Write();
    }
    outFile->Close();
    delete outFile;
}

void SaveProjection_sigma1pT_GlobalTracks(TFile* file, const char* histPath, const char* baseTitle, const char* xTitle, const char* yTitle, const char* baseFileName, int axis1, int axis2) {
    THnSparseD* histSparse = dynamic_cast<THnSparseD*>(file->Get(histPath));
    THnSparseD* hPtSparse = dynamic_cast<THnSparseD*>(file->Get("track-jet-qa_id9648/Kine/pt")); 
    TH1D* pt = Project1D(hPtSparse, 0, "E");
    std::vector<std::pair<double, double>> ptBins = {{0, 10}, {10, 80}, {80, 200}};
    std::vector<double> N_entries_per_bin(ptBins.size());
    TFile* outFile = new TFile("Comparisons/projections_sigma1pT_GlobalTracks.root", "UPDATE");
    for (size_t i = 0; i < ptBins.size(); ++i) {
        double ptMin = ptBins[i].first;
        double ptMax = ptBins[i].second;
        int binMin = pt->GetXaxis()->FindBin(ptMin);
        int binMax = pt->GetXaxis()->FindBin(ptMax) - 1; 
        N_entries_per_bin[i] = pt->Integral(binMin, binMax);
        TH2D* hist2D = Project2D(histSparse, axis1, axis2, ptMin, ptMax, "E");
        if (!hist2D) {
            std::cerr << "Projection failed for " << histPath << " in pT range [" << ptMin << ", " << ptMax << "]." << std::endl;
            continue;
        }
        if (N_entries_per_bin[i] != 0) { 
            hist2D->Scale(1.0 / N_entries_per_bin[i]);
        }
        TH1D* histY = hist2D->ProjectionY(TString::Format("%s_py_%g_%g", hist2D->GetName(), ptMin, ptMax), 1, -1, "e");
        outFile->cd();
        histY->Write();
    }
    outFile->Close();
    delete outFile;
}

void SaveProjection_loose(TFile* file, const char* histPath, const char* baseTitle, const char* xTitle, const char* yTitle, const char* baseFileName, int axis1, int axis2) {
    THnSparseD* histSparse = dynamic_cast<THnSparseD*>(file->Get(histPath));
    THnSparseD* hPtSparse = dynamic_cast<THnSparseD*>(file->Get("track-jet-qa_id9646/Kine/pt")); 
    TH1D* pt = Project1D(hPtSparse, 0, "E");
    THnSparseD* collisionVtxZSparse_loose = dynamic_cast<THnSparseD*>(file->Get("track-jet-qa_id9646/EventProp/collisionVtxZ"));
    if (!collisionVtxZSparse_loose) {
    std::cerr << "collisionVtxZ THnSparseD histogram not found." << std::endl;
    file->Close();
    delete file;
    return;
    }
    TH1D* collisionVtxZ_loose = dynamic_cast<TH1D*>(collisionVtxZSparse_loose->Projection(0));
    if (!collisionVtxZ_loose) {
        std::cerr << "Projection of collisionVtxZ failed." << std::endl;
        file->Close();
        delete file;
        return;
    }
    double numberOfEvents_loose = collisionVtxZ_loose->GetEntries(); 
    std::vector<std::pair<double, double>> ptBins = {{0, 1}, {1, 3}, {3, 5}, {5, 10}, {10, 50}, {50, 100}, {100, 200}};
    std::vector<double> N_entries_per_bin(ptBins.size());
    TFile* outFile = new TFile("Comparisons/projections_loose.root", "UPDATE");
    for (size_t i = 0; i < ptBins.size(); ++i) {
        double ptMin = ptBins[i].first;
        double ptMax = ptBins[i].second;
        int binMin = pt->GetXaxis()->FindBin(ptMin);
        int binMax = pt->GetXaxis()->FindBin(ptMax) - 1; 
        N_entries_per_bin[i] = pt->Integral(binMin, binMax);
        TH2D* hist2D = Project2D(histSparse, axis1, axis2, ptMin, ptMax, "E");
        if (!hist2D) {
            std::cerr << "Projection failed for " << histPath << " in pT range [" << ptMin << ", " << ptMax << "]." << std::endl;
            continue;
        }
        /*
        if (N_entries_per_bin[i] != 0) { 
            hist2D->Scale(1.0 / N_entries_per_bin[i]);
        }
        */
        hist2D->Scale(1.0 / numberOfEvents_loose);
        TH1D* histY = hist2D->ProjectionY(TString::Format("%s_py_%g_%g", hist2D->GetName(), ptMin, ptMax), 1, -1, "e");
        outFile->cd();
        histY->Write();
    }
    outFile->Close();
    delete outFile;
}

void SaveProjection_tight(TFile* file, const char* histPath, const char* baseTitle, const char* xTitle, const char* yTitle, const char* baseFileName, int axis1, int axis2) {
    THnSparseD* histSparse = dynamic_cast<THnSparseD*>(file->Get(histPath));
    THnSparseD* hPtSparse = dynamic_cast<THnSparseD*>(file->Get("track-jet-qa_id9647/Kine/pt"));
    TH1D* pt = Project1D(hPtSparse, 0, "E");
    THnSparseD* collisionVtxZSparse_tight = dynamic_cast<THnSparseD*>(file->Get("track-jet-qa_id9647/EventProp/collisionVtxZ"));
    if (!collisionVtxZSparse_tight) {
    std::cerr << "collisionVtxZ THnSparseD histogram not found." << std::endl;
    file->Close();
    delete file;
    return;
    }
    TH1D* collisionVtxZ_tight = dynamic_cast<TH1D*>(collisionVtxZSparse_tight->Projection(0));
    if (!collisionVtxZ_tight) {
        std::cerr << "Projection of collisionVtxZ failed." << std::endl;
        file->Close();
        delete file;
        return;
    }
    double numberOfEvents = collisionVtxZ_tight->GetEntries(); 
    std::vector<std::pair<double, double>> ptBins = {{0, 1}, {1, 3}, {3, 5}, {5, 10}, {10, 50}, {50, 100}, {100, 200}};
    std::vector<double> N_entries_per_bin(ptBins.size());
    TFile* outFile = new TFile("Comparisons/projections_tight.root", "UPDATE");
    for (size_t i = 0; i < ptBins.size(); ++i) {
        double ptMin = ptBins[i].first;
        double ptMax = ptBins[i].second;
        int binMin = pt->GetXaxis()->FindBin(ptMin);
        int binMax = pt->GetXaxis()->FindBin(ptMax) - 1; 
        N_entries_per_bin[i] = pt->Integral(binMin, binMax);
        TH2D* hist2D = Project2D(histSparse, axis1, axis2, ptMin, ptMax, "E");
        if (!hist2D) {
            std::cerr << "Projection failed for " << histPath << " in pT range [" << ptMin << ", " << ptMax << "]." << std::endl;
            continue;
        }
        /*
        if (N_entries_per_bin[i] != 0) { 
            hist2D->Scale(1.0 / N_entries_per_bin[i]);
        }
        */
        hist2D->Scale(1.0 / numberOfEvents);
        TH1D* histY = hist2D->ProjectionY(TString::Format("%s_py_%g_%g", hist2D->GetName(), ptMin, ptMax), 1, -1, "e");
        outFile->cd();
        histY->Write();
    }
    outFile->Close();
    delete outFile;
}

void SaveProjection_GlobalTracks(TFile* file, const char* histPath, const char* baseTitle, const char* xTitle, const char* yTitle, const char* baseFileName, int axis1, int axis2) {
    THnSparseD* histSparse = dynamic_cast<THnSparseD*>(file->Get(histPath));
    THnSparseD* hPtSparse = dynamic_cast<THnSparseD*>(file->Get("track-jet-qa_id9648/Kine/pt")); 
    TH1D* pt = Project1D(hPtSparse, 0, "E");
    THnSparseD* collisionVtxZSparse_gb = dynamic_cast<THnSparseD*>(file->Get("track-jet-qa_id9648/EventProp/collisionVtxZ"));
    if (!collisionVtxZSparse_gb) {
    std::cerr << "collisionVtxZ THnSparseD histogram not found." << std::endl;
    file->Close();
    delete file;
    return;
    }
    TH1D* collisionVtxZ_gb = dynamic_cast<TH1D*>(collisionVtxZSparse_gb->Projection(0));
    if (!collisionVtxZ_gb) {
        std::cerr << "Projection of collisionVtxZ failed." << std::endl;
        file->Close();
        delete file;
        return;
    }
    double numberOfEvents = collisionVtxZ_gb->GetEntries(); 
    std::vector<std::pair<double, double>> ptBins = {{0, 1}, {1, 3}, {3, 5}, {5, 10}, {10, 50}, {50, 100}, {100, 200}};
    std::vector<double> N_entries_per_bin(ptBins.size());
    TFile* outFile = new TFile("Comparisons/projections_GlobalTracks.root", "UPDATE");
    for (size_t i = 0; i < ptBins.size(); ++i) {
        double ptMin = ptBins[i].first;
        double ptMax = ptBins[i].second;
        int binMin = pt->GetXaxis()->FindBin(ptMin);
        int binMax = pt->GetXaxis()->FindBin(ptMax) - 1; 
        N_entries_per_bin[i] = pt->Integral(binMin, binMax);
        TH2D* hist2D = Project2D(histSparse, axis1, axis2, ptMin, ptMax, "E");
        if (!hist2D) {
            std::cerr << "Projection failed for " << histPath << " in pT range [" << ptMin << ", " << ptMax << "]." << std::endl;
            continue;
        }
        hist2D->Scale(1.0 / numberOfEvents);
        /*
        if (N_entries_per_bin[i] != 0) { 
            hist2D->Scale(1.0 / N_entries_per_bin[i]);
        }
        */
        TH1D* histY = hist2D->ProjectionY(TString::Format("%s_py_%g_%g", hist2D->GetName(), ptMin, ptMax), 1, -1, "e");
        outFile->cd();
        histY->Write();
    }
    outFile->Close();
    delete outFile;
}

void PlotProjectionsTogether(TFile* file_loose, TFile* file_tight, TFile* file_GlobalTracks, const char* baseHistName, int axis1, int axis2, double ptMin, double ptMax, const char* saveFileName, bool useLogY = false) {
    TString histName = TString::Format("%s_Proj2D_%d_%d_py_%g_%g", baseHistName, axis1, axis2, ptMin, ptMax);
    TH1D* h_loose = dynamic_cast<TH1D*>(file_loose->Get(histName));
    TH1D* h_tight = dynamic_cast<TH1D*>(file_tight->Get(histName));
    TH1D* h_GlobalTracks = dynamic_cast<TH1D*>(file_GlobalTracks->Get(histName));
    if (!h_loose || !h_tight || !h_GlobalTracks) {
        std::cerr << "Error retrieving histograms for pT range [" << ptMin << ", " << ptMax << "] with name " << histName << std::endl;
        return;
    }
    /*
    TString canvasTitle;
    auto metadataIt = histogramMetadata.find(baseHistName);
    if (metadataIt != histogramMetadata.end()) {
        auto [plotTitle, xAxisTitle, yAxisTitle] = metadataIt->second;
        canvasTitle = TString::Format("%s for %g < p_{T} < %g GeV/c; %s; %s", plotTitle.c_str(), ptMin, ptMax, xAxisTitle.c_str(), yAxisTitle.c_str());
        h_loose->SetTitle(canvasTitle);
    } else {
        std::cerr << "Metadata not found for histogram base name: " << baseHistName << std::endl;
        canvasTitle = "Projection Comparison";
    }
    */
    h_loose->SetTitle("");
    h_tight->SetTitle("");
    h_GlobalTracks->SetTitle("");
    auto metadataIt = histogramMetadata.find(baseHistName);
    if (metadataIt != histogramMetadata.end()) {
        auto [plotTitle, xAxisTitle, yAxisTitle] = metadataIt->second;
        h_loose->SetXTitle(xAxisTitle.c_str());
        h_loose->SetYTitle(yAxisTitle.c_str());
        h_tight->SetXTitle(xAxisTitle.c_str());
        h_tight->SetYTitle(yAxisTitle.c_str());
        h_GlobalTracks->SetXTitle(xAxisTitle.c_str());
        h_GlobalTracks->SetYTitle(yAxisTitle.c_str());
    } else {
        std::cerr << "Metadata not found for histogram base name: " << baseHistName << std::endl;
    }
    h_loose->SetLineColor(kPink+7);
    h_loose->SetLineWidth(20);
    h_loose->SetMarkerColor(kPink+7);
    h_loose->SetMarkerStyle(20);
    h_loose->SetMarkerSize(6);

    h_tight->SetLineColor(kViolet+2);
    h_tight->SetLineWidth(20);
    h_tight->SetMarkerColor(kViolet+2);
    h_tight->SetMarkerStyle(21);
    h_tight->SetMarkerSize(6);

    h_GlobalTracks->SetLineColor(kTeal-5);
    h_GlobalTracks->SetLineWidth(20);
    h_GlobalTracks->SetMarkerColor(kTeal-5);
    h_GlobalTracks->SetMarkerStyle(22);
    h_GlobalTracks->SetMarkerSize(6);

    TCanvas* canvas = new TCanvas("canvas", "Projection Comparison", 4000, 4000);
    //canvas->SetTopMargin(0.22);
    canvas->SetLeftMargin(0.20);
    if (useLogY) {
        canvas->SetLogy();
    }
    h_loose->SetStats(kFALSE);
    h_tight->SetStats(kFALSE);
    h_GlobalTracks->SetStats(kFALSE);
    h_loose->Draw("E");
    h_tight->Draw("ESAME");
    h_GlobalTracks->Draw("ESAME");
    gPad->Update();
    /*
    TPaveStats* st_loose = (TPaveStats*)h_loose->FindObject("stats");
    if (st_loose) {  
        st_loose->SetX1NDC(0.71); 
        st_loose->SetX2NDC(0.90); 
        st_loose->SetY1NDC(0.78); 
        st_loose->SetY2NDC(0.93);
    }
    TPaveStats* st_tight = (TPaveStats*)h_tight->FindObject("stats");
    if (st_tight) {  
        st_tight->SetX1NDC(0.71); 
        st_tight->SetX2NDC(0.90); 
        st_tight->SetY1NDC(0.78); 
        st_tight->SetY2NDC(0.93);
    }
    TPaveStats* st_GlobalTracks = (TPaveStats*)h_GlobalTracks->FindObject("stats");
    if (st_GlobalTracks) {  
        st_GlobalTracks->SetX1NDC(0.71); 
        st_GlobalTracks->SetX2NDC(0.90); 
        st_GlobalTracks->SetY1NDC(0.78); 
        st_GlobalTracks->SetY2NDC(0.93);
    }
    */
    TLegend* legend = new TLegend(0.25, 0.80, 0.55, 0.90);
    legend->AddEntry(h_tight, "Tight cuts", "lep");    
    legend->AddEntry(h_GlobalTracks, "GlobalTracks selection", "lep");
    legend->AddEntry(h_loose, "Loose cuts", "lep");
    legend->Draw();
    canvas->SaveAs(saveFileName);
    delete canvas;
}

void PlotAllProjections() {
    TFile* file_loose = TFile::Open("Comparisons/projections_loose.root");
    TFile* file_tight = TFile::Open("Comparisons/projections_tight.root");
    TFile* file_GlobalTracks = TFile::Open("Comparisons/projections_GlobalTracks.root");
    std::vector<std::pair<double, double>> ptBins = {{0, 1}, {1, 3}, {3, 5}, {5, 10}, {10, 50}, {50, 100}, {100, 200}};
    for (const auto& bin : ptBins) {
        double ptMin = bin.first;
        double ptMax = bin.second;
        TString tpcNClsFindable = TString::Format("Comparisons/tpcNClsFindable_%g_%g.png", ptMin, ptMax);
        PlotProjectionsTogether(file_loose, file_tight, file_GlobalTracks, "tpcNClsFindable", 2, 0, ptMin, ptMax, tpcNClsFindable, false);
        TString tpcNClsFound = TString::Format("Comparisons/tpcNClsFound_%g_%g.png", ptMin, ptMax);
        PlotProjectionsTogether(file_loose, file_tight, file_GlobalTracks, "tpcNClsFound", 2, 0, ptMin, ptMax, tpcNClsFound, false);
        TString tpcNClsShared = TString::Format("Comparisons/tpcNClsShared_%g_%g.png", ptMin, ptMax);
        PlotProjectionsTogether(file_loose, file_tight, file_GlobalTracks, "tpcNClsShared", 2, 0, ptMin, ptMax, tpcNClsShared, true);
        TString tpcNClsCrossedRows = TString::Format("Comparisons/tpcNClsCrossedRows_%g_%g.png", ptMin, ptMax);
        PlotProjectionsTogether(file_loose, file_tight, file_GlobalTracks, "tpcNClsCrossedRows", 2, 0, ptMin, ptMax, tpcNClsCrossedRows, true);
        TString tpcFractionSharedCls = TString::Format("Comparisons/tpcFractionSharedCls_%g_%g.png", ptMin, ptMax);
        PlotProjectionsTogether(file_loose, file_tight, file_GlobalTracks, "tpcFractionSharedCls", 2, 0, ptMin, ptMax, tpcFractionSharedCls, true);
        TString tpcCrossedRowsOverFindableCls = TString::Format("Comparisons/tpcCrossedRowsOverFindableCls_%g_%g.png", ptMin, ptMax);
        PlotProjectionsTogether(file_loose, file_tight, file_GlobalTracks, "tpcCrossedRowsOverFindableCls", 2, 0, ptMin, ptMax, tpcCrossedRowsOverFindableCls, true);
        TString tpcChi2NCl = TString::Format("Comparisons/tpcChi2NCl_%g_%g.png", ptMin, ptMax);
        PlotProjectionsTogether(file_loose, file_tight, file_GlobalTracks, "tpcChi2NCl", 2, 0, ptMin, ptMax, tpcChi2NCl, true);
        TString itsHits = TString::Format("Comparisons/itsHits_%g_%g.png", ptMin, ptMax);
        PlotProjectionsTogether(file_loose, file_tight, file_GlobalTracks, "itsHits", 2, 0, ptMin, ptMax, itsHits, false);
        TString itsNCls = TString::Format("Comparisons/itsNCls_%g_%g.png", ptMin, ptMax);
        PlotProjectionsTogether(file_loose, file_tight, file_GlobalTracks, "itsNCls", 2, 0, ptMin, ptMax, itsNCls, false);
        TString itsChi2NCl = TString::Format("Comparisons/itsChi2NCl_%g_%g.png", ptMin, ptMax);
        PlotProjectionsTogether(file_loose, file_tight, file_GlobalTracks, "itsChi2NCl", 2, 0, ptMin, ptMax, itsChi2NCl, true);
    }
    file_loose->Close();
    file_tight->Close();
    file_GlobalTracks->Close();
    delete file_loose;
    delete file_tight;
    delete file_GlobalTracks;
}

void PlotAllProjections_sigma1pT() {
    TFile* file_loose = TFile::Open("Comparisons/projections_sigma1pT_loose.root");
    TFile* file_tight = TFile::Open("Comparisons/projections_sigma1pT_tight.root");
    TFile* file_GlobalTracks = TFile::Open("Comparisons/projections_sigma1pT_GlobalTracks.root");
    std::vector<std::pair<double, double>> ptBins = {{0, 10}, {10, 80}, {80, 200}};
    for (const auto& bin : ptBins) {
        double ptMin = bin.first;
        double ptMax = bin.second;
        TString Sigma1Pt_Layers12 = TString::Format("Comparisons/Sigma1Pt_Layers12_%g_%g.png", ptMin, ptMax);
        PlotProjectionsTogether(file_loose, file_tight, file_GlobalTracks, "Sigma1Pt_Layers12", 1, 0, ptMin, ptMax, Sigma1Pt_Layers12, true);
        TString Sigma1Pt_Layers456 = TString::Format("Comparisons/Sigma1Pt_Layers456_%g_%g.png", ptMin, ptMax);
        PlotProjectionsTogether(file_loose, file_tight, file_GlobalTracks, "Sigma1Pt_Layers456", 1, 0, ptMin, ptMax, Sigma1Pt_Layers456, true);
    }
    file_loose->Close();
    file_tight->Close();
    file_GlobalTracks->Close();
    delete file_loose;
    delete file_tight;
    delete file_GlobalTracks;
}

void QAplay() {  
    InitializeHistogramMetadata();     
    gSystem->mkdir("Comparisons", kTRUE); 
    TFile* file = new TFile("/dcache/alice/acaluisi/JetQA/LHC22_pass4_highIR_sampling/full/AnalysisResults.root"); 
    if (!file || file->IsZombie()) {
        std::cerr << "Error opening file." << std::endl;
        return;
    }
    SaveProjection_loose(file, "track-jet-qa_id9646/TPC/tpcNClsFindable", "Number of findable TPC clusters", "p_{T} (GeV/c)", "Number of findable TPC clusters", "Comparisons/tpcNClsFindable_loose.root", 2, 0);
    SaveProjection_tight(file, "track-jet-qa_id9647/TPC/tpcNClsFindable", "Number of findable TPC clusters", "p_{T} (GeV/c)", "Number of findable TPC clusters", "Comparisons/tpcNClsFindable_tight.root", 2, 0);
    SaveProjection_GlobalTracks(file, "track-jet-qa_id9648/TPC/tpcNClsFindable", "Number of findable TPC clusters", "p_{T} (GeV/c)", "Number of findable TPC clusters", "Comparisons/tpcNClsFindable_GlobalTracks.root", 2, 0);
    
    SaveProjection_loose(file, "track-jet-qa_id9646/TPC/tpcNClsFound", "Number of found TPC clusters", "p_{T} (GeV/c)", "Number of found TPC clusters", "Comparisons/tpcNClsFound_loose.root", 2, 0);
    SaveProjection_tight(file, "track-jet-qa_id9647/TPC/tpcNClsFound", "Number of found TPC clusters", "p_{T} (GeV/c)", "Number of found TPC clusters", "Comparisons/tpcNClsFound_tight.root", 2, 0);
    SaveProjection_GlobalTracks(file, "track-jet-qa_id9648/TPC/tpcNClsFound", "Number of found TPC clusters", "p_{T} (GeV/c)", "Number of found TPC clusters", "Comparisons/tpcNClsFound_GlobalTracks.root", 2, 0);
    
    SaveProjection_loose(file, "track-jet-qa_id9646/TPC/tpcNClsShared", "Number of shared TPC clusters", "p_{T} (GeV/c)", "Number of shared TPC clusters", "Comparisons/tpcNClsShared_loose.root", 2, 0);
    SaveProjection_tight(file, "track-jet-qa_id9647/TPC/tpcNClsShared", "Number of shared TPC clusters", "p_{T} (GeV/c)", "Number of shared TPC clusters", "Comparisons/tpcNClsShared_tight.root", 2, 0);
    SaveProjection_GlobalTracks(file, "track-jet-qa_id9648/TPC/tpcNClsShared", "Number of shared TPC clusters", "p_{T} (GeV/c)", "Number of shared TPC clusters", "Comparisons/tpcNClsShared_GlobalTracks.root", 2, 0);
    
    SaveProjection_loose(file, "track-jet-qa_id9646/TPC/tpcFractionSharedCls", "Fraction of shared TPC clusters", "p_{T} (GeV/c)", "Fraction of shared TPC clusters", "Comparisons/tpcFractionSharedCls_loose.root", 2, 0);
    SaveProjection_tight(file, "track-jet-qa_id9647/TPC/tpcFractionSharedCls", "Fraction of shared TPC clusters", "p_{T} (GeV/c)", "Fraction of shared TPC clusters", "Comparisons/tpcFractionSharedCls_tight.root", 2, 0);
    SaveProjection_GlobalTracks(file, "track-jet-qa_id9648/TPC/tpcFractionSharedCls", "Fraction of shared TPC clusters", "p_{T} (GeV/c)", "Fraction of shared TPC clusters", "Comparisons/tpcFractionSharedCls_GlobalTracks.root", 2, 0);
  
    SaveProjection_loose(file, "track-jet-qa_id9646/TPC/tpcCrossedRowsOverFindableCls", "Number of crossed TPC rows over findable TPC clusters", "p_{T} (GeV/c)", "Number of crossed TPC rows over findable TPC clusters", "Comparisons/tpcCrossedRowsOverFindableCls_loose.root", 2, 0);
    SaveProjection_tight(file, "track-jet-qa_id9647/TPC/tpcCrossedRowsOverFindableCls", "Number of crossed TPC rows over findable TPC clusters", "p_{T} (GeV/c)", "Number of crossed TPC rows over findable TPC clusters", "Comparisons/tpcCrossedRowsOverFindableCls_tight.root", 2, 0);
    SaveProjection_GlobalTracks(file, "track-jet-qa_id9648/TPC/tpcCrossedRowsOverFindableCls", "Number of crossed TPC rows over findable TPC clusters", "p_{T} (GeV/c)", "Number of crossed TPC rows over findable TPC clusters", "Comparisons/tpcCrossedRowsOverFindableCls_GlobalTracks.root", 2, 0);
    
    SaveProjection_loose(file, "track-jet-qa_id9646/TPC/tpcNClsCrossedRows", "Number of crossed TPC rows", "p_{T} (GeV/c)", "Number of crossed TPC rows", "Comparisons/tpcNClsCrossedRows_loose.root", 2, 0);
    SaveProjection_tight(file, "track-jet-qa_id9647/TPC/tpcNClsCrossedRows", "Number of crossed TPC rows", "p_{T} (GeV/c)", "Number of crossed TPC rows", "Comparisons/tpcNClsCrossedRows_tight.root", 2, 0);
    SaveProjection_GlobalTracks(file, "track-jet-qa_id9648/TPC/tpcNClsCrossedRows", "Number of crossed TPC rows", "p_{T} (GeV/c)", "Number of crossed TPC rows", "Comparisons/tpcNClsCrossedRows_GlobalTracks.root", 2, 0);
    
    SaveProjection_loose(file, "track-jet-qa_id9646/TPC/tpcChi2NCl", "#chi^{2} per TPC clusters", "p_{T} (GeV/c)", "#frac{#chi^{2}}{TPC clusters}", "Comparisons/tpcChi2NCl_loose.root", 2, 0);
    SaveProjection_tight(file, "track-jet-qa_id9647/TPC/tpcChi2NCl", "#chi^{2} per TPC clusters", "p_{T} (GeV/c)", "#frac{#chi^{2}}{TPC clusters}", "Comparisons/tpcChi2NCl_tight.root", 2, 0);
    SaveProjection_GlobalTracks(file, "track-jet-qa_id9648/TPC/tpcChi2NCl", "#chi^{2} per TPC clusters", "p_{T} (GeV/c)", "#frac{#chi^{2}}{TPC clusters}", "Comparisons/tpcChi2NCl_GlobalTracks.root", 2, 0);
    
    
    SaveProjection_loose(file, "track-jet-qa_id9646/ITS/itsHits", "ITS hitmap", "p_{T} (GeV/c)", "ITS layers", "Comparisons/itsHits_loose.root", 2, 0);
    SaveProjection_tight(file, "track-jet-qa_id9647/ITS/itsHits", "ITS hitmap", "p_{T} (GeV/c)", "ITS layers", "Comparisons/itsHits_tight.root", 2, 0);
    SaveProjection_GlobalTracks(file, "track-jet-qa_id9648/ITS/itsHits", "ITS hitmap", "p_{T} (GeV/c)", "ITS layers", "Comparisons/itsHits_GlobalTracks.root", 2, 0);
    
    SaveProjection_loose(file, "track-jet-qa_id9646/ITS/itsNCls", "ITS clusters", "ITS clusters", "Number of entries", "Comparisons/itsNCls_loose.root", 2, 0);
    SaveProjection_tight(file, "track-jet-qa_id9647/ITS/itsNCls", "ITS clusters", "ITS clusters", "Number of entries", "Comparisons/itsNCls_tight.root", 2, 0);
    SaveProjection_GlobalTracks(file, "track-jet-qa_id9648/ITS/itsNCls", "ITS clusters", "ITS clusters", "Number of entries", "Comparisons/itsNCls_GlobalTracks.root", 2, 0);
    
    SaveProjection_loose(file, "track-jet-qa_id9646/ITS/itsChi2NCl", "#chi^{2} per ITS clusters", "p_{T} (GeV/c)", "#frac{#chi^{2}}{ITS clusters}", "Comparisons/itsChi2NCl_loose.root", 2, 0);
    SaveProjection_tight(file, "track-jet-qa_id9647/ITS/itsChi2NCl", "#chi^{2} per ITS clusters", "p_{T} (GeV/c)", "#frac{#chi^{2}}{ITS clusters}", "Comparisons/itsChi2NCl_tight.root", 2, 0);
    SaveProjection_GlobalTracks(file, "track-jet-qa_id9648/ITS/itsChi2NCl", "#chi^{2} per ITS clusters", "p_{T} (GeV/c)", "#frac{#chi^{2}}{ITS clusters}", "Comparisons/itsChi2NCl_GlobalTracks.root", 2, 0);
    
    SaveProjection_sigma1pT_loose(file, "track-jet-qa_id9646/TrackPar/Sigma1Pt_Layers12", "Uncertainty over p_{T} with inner ITS layers active", "p_{T} (GeV/c)", "p_{T}#sigma(1/p_{T})", "Comparisons/Sigma1Pt_Layers12_loose.root", 1, 0);
    SaveProjection_sigma1pT_tight(file, "track-jet-qa_id9647/TrackPar/Sigma1Pt_Layers12", "Uncertainty over p_{T} with inner ITS layers active", "p_{T} (GeV/c)", "p_{T}#sigma(1/p_{T})", "Comparisons/Sigma1Pt_Layers12_tight.root", 1, 0);
    SaveProjection_sigma1pT_GlobalTracks(file, "track-jet-qa_id9648/TrackPar/Sigma1Pt_Layers12", "Uncertainty over p_{T} with inner ITS layers active", "p_{T} (GeV/c)", "p_{T}#sigma(1/p_{T})", "Comparisons/Sigma1Pt_Layers12_GlobalTracks.root", 1, 0);
    
    SaveProjection_sigma1pT_loose(file, "track-jet-qa_id9646/TrackPar/Sigma1Pt_Layers456", "Uncertainty over p_{T} with outer ITS layers active", "p_{T} (GeV/c)", "p_{T}#sigma(1/p_{T})", "Comparisons/Sigma1Pt_Layers456_loose.root", 1, 0);
    SaveProjection_sigma1pT_tight(file, "track-jet-qa_id9647/TrackPar/Sigma1Pt_Layers456", "Uncertainty over p_{T} with outer ITS layers active", "p_{T} (GeV/c)", "p_{T}#sigma(1/p_{T})", "Comparisons/Sigma1Pt_Layers456_tight.root", 1, 0);
    SaveProjection_sigma1pT_GlobalTracks(file, "track-jet-qa_id9648/TrackPar/Sigma1Pt_Layers456", "Uncertainty over p_{T} with outer ITS layers active", "p_{T} (GeV/c)", "p_{T}#sigma(1/p_{T})", "Comparisons/Sigma1Pt_Layers456_GlobalTracks.root", 1, 0);
 
    file->Close();
    delete file;

    PlotAllProjections();
    PlotAllProjections_sigma1pT();
}