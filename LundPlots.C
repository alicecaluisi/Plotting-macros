#include <TFile.h>
#include <TH1F.h>
#include <TH2D.h>
#include <TH3F.h>
#include <TCanvas.h>
#include <TStyle.h>
#include <TPaveStats.h>

void ScaleHistogram(TH2D* histogram, double N_jets) {
    int nBinsX = histogram->GetNbinsX();
    int nBinsY = histogram->GetNbinsY();
    for (int i = 1; i <= nBinsX; ++i) {
        for (int j = 1; j <= nBinsY; ++j) {
            double binContent = histogram->GetBinContent(i, j);
            double binError = histogram->GetBinError(i, j); // Original error on bin content
            double xBinWidth = histogram->GetXaxis()->GetBinWidth(i);
            double yBinWidth = histogram->GetYaxis()->GetBinWidth(j);
            double binArea = xBinWidth * yBinWidth;
            if (binArea > 0 && N_jets > 0) {
                double a = 1 / (binArea * N_jets);
                double normalizedBinContent = binContent * a; // Applying the function f = A * a
                double normalizedBinError = fabs(a) * binError; // Applying the error propagation formula
                
                histogram->SetBinContent(i, j, normalizedBinContent);
                histogram->SetBinError(i, j, normalizedBinError); // Setting the new error
            }
        }
    }
}


TH1D* ScaleYProjection(TH2D* histogram, double N_jets, double xMin, double xMax) {
    int binMinX = histogram->GetXaxis()->FindBin(xMin);
    int binMaxX = histogram->GetXaxis()->FindBin(xMax);
    histogram->GetXaxis()->SetRange(binMinX, binMaxX);
    // First, make the projection over Y
    TH1D* yProjection = histogram->ProjectionY("", binMinX, binMaxX);
    // Then, calculate:
    // 1. the total deltaX by summing the widths of bins in the specified ln(R/Delta) range
    double deltaX = 0.0;
    for (int i = binMinX; i <= binMaxX; ++i) {
        deltaX += histogram->GetXaxis()->GetBinWidth(i);
    }
    for (int j = 1; j <= yProjection->GetNbinsX(); ++j) {
        double sum = 0.0;
        double errorSumSquared = 0.0;
        // 2. the sum of the contents in the 2D histogram within the specified ln(R/Delta) range for each ln(kT) bin
        for (int i = binMinX; i <= binMaxX; ++i) {
            sum += histogram->GetBinContent(i, j);
            double binError = histogram->GetBinError(i, j);
            errorSumSquared += binError * binError;
        }
        // 3. the widths of bins in ln(kT)
        double yBinWidth = yProjection->GetBinWidth(j);
        // 4. the normalization factor
        double normalizedContent = sum / ( N_jets * deltaX * yBinWidth);
        double normalizedError = sqrt(errorSumSquared) / (N_jets * deltaX * yBinWidth);

        yProjection->SetBinContent(j, normalizedContent);
        yProjection->SetBinError(j, normalizedError);
    }
    return yProjection;
}

TH1D* ScaleXProjection(TH2D* histogram, double N_jets, double yMin, double yMax) {
    int binMinY = histogram->GetYaxis()->FindBin(yMin);
    int binMaxY = histogram->GetYaxis()->FindBin(yMax);
    histogram->GetYaxis()->SetRange(binMinY, binMaxY);  

    TH1D* xProjection = histogram->ProjectionX("", binMinY, binMaxY); 

    double deltaY = 0.0;
    for (int i = binMinY; i <= binMaxY; ++i) {
        deltaY += histogram->GetYaxis()->GetBinWidth(i);
    }

    for (int i = 1; i <= xProjection->GetNbinsX(); ++i) {
        double sum = 0.0;
        double errorSumSquared = 0.0;
        for (int j = binMinY; j <= binMaxY; ++j) {
            sum += histogram->GetBinContent(i, j);
            double binError = histogram->GetBinError(i, j);
            errorSumSquared += binError * binError;
        }
        double xBinWidth = xProjection->GetBinWidth(i);

        double normalizedContent = sum / (N_jets * deltaY * xBinWidth);
        double normalizedError = sqrt(errorSumSquared) / (N_jets * deltaY * xBinWidth);

        xProjection->SetBinContent(i, normalizedContent);
        xProjection->SetBinError(i, normalizedError);
    }
    return xProjection;
}


void PlotProjection(TH1D* proj, const char* title, const char* xTitle, const char* yTitle, const char* fileName, double xMin, double xMax, Color_t lineColor = kAzure+1, Color_t markerColor = kAzure+1) {
    TCanvas* canvas = new TCanvas("canvas", title, 4000, 2700);
    canvas->SetLeftMargin(0.15);
    proj->GetXaxis()->SetRangeUser(xMin, xMax);
    proj->SetTitle(title);
    proj->GetXaxis()->SetTitle(xTitle);
    proj->GetYaxis()->SetTitle(yTitle);
    proj->SetLineColor(lineColor);
    proj->SetLineWidth(9);
    proj->SetMarkerStyle(20);
    proj->SetMarkerSize(5);
    proj->SetMarkerColor(markerColor);
    proj->Draw("E");
    gPad->Update();
    TPaveStats* st_proj = (TPaveStats*)proj->FindObject("stats");
    st_proj->SetX1NDC(0.73);
    st_proj->SetX2NDC(0.90);
    st_proj->SetY1NDC(0.73);
    st_proj->SetY2NDC(0.90);
    canvas->SaveAs(fileName);
    delete canvas;
}

void PlotLundPlane(TH2D* histo, const char* title, const char* xTitle, const char* yTitle, const char* zTitle, const char* fileName, double xMin, double xMax, double yMin, double yMax, double ptMin, double ptMax) {
    TCanvas* canvas = new TCanvas(Form("canvas_pt_%g_%g", ptMin, ptMax), Form("%s for %g < p_{T} < %g", title, ptMin, ptMax), 4000, 2700);
    canvas->SetLogz(0);
    canvas->SetRightMargin(0.20);
    gStyle->SetPalette(kBird);
    histo->SetTitle(Form("%s for %g < p_{T} < %g; %s; %s", title, ptMin, ptMax, xTitle, yTitle));
    histo->GetXaxis()->SetTitle(xTitle);
    histo->GetYaxis()->SetTitle(yTitle);
    histo->GetZaxis()->SetTitle(Form("%s", zTitle));
    histo->GetZaxis()->SetTitleOffset(1.7);
    histo->GetXaxis()->SetRangeUser(xMin, xMax);
    histo->GetYaxis()->SetRangeUser(yMin, yMax);
    histo->Draw("COLZ");
    gPad->Update();
    TPaveStats* st_histo = (TPaveStats*)histo->FindObject("stats");
    st_histo->SetX1NDC(0.63);
    st_histo->SetX2NDC(0.80);
    st_histo->SetY1NDC(0.73);
    st_histo->SetY2NDC(0.90);
    canvas->SaveAs(Form("%s_%g_%g.png", fileName, ptMin, ptMax));
    delete canvas;
}

void LundPlots() {
    gSystem->mkdir("kt_LundPlots", kTRUE);
    gSystem->mkdir("z_LundPlots", kTRUE);
    gSystem->mkdir("kt_LundPlots/yProjections", kTRUE);
    gSystem->mkdir("kt_LundPlots/xProjections", kTRUE);
    gSystem->mkdir("z_LundPlots/yProjections", kTRUE);
    gSystem->mkdir("z_LundPlots/xProjections", kTRUE);
    gStyle->SetOptStat();
    TFile *AResult = new TFile("/dcache/alice/acaluisi/Lund/LHC22_pass4_lowIR/full/AnalysisResults.root");
    TH1F *jet_pT = (TH1F*)AResult->Get("jet-finder-charged-qa/h_jet_pt");
    TH3F *PrimaryLundPlane_kT = (TH3F*)AResult->Get("jet-lund-reclustering/PrimaryLundPlane_kT");
    TH3F *PrimaryLundPlane_z = (TH3F*)AResult->Get("jet-lund-reclustering/PrimaryLundPlane_z");

    double N_jets = jet_pT->GetEntries();

    float ptBins[][2] = {{0, 20}, {20, 40}, {40, 60}, {60, 80}, {80, 100}, {100, 200}};
    int nBins = sizeof(ptBins) / sizeof(ptBins[0]);
    double N_jets_per_bin[nBins];
    for (int i = 0; i < nBins; ++i) {
        float ptMin = ptBins[i][0];
        float ptMax = ptBins[i][1];
        int binMin = jet_pT->GetXaxis()->FindBin(ptMin);
        int binMax = jet_pT->GetXaxis()->FindBin(ptMax);
        N_jets_per_bin[i] = jet_pT->Integral(binMin, binMax);
    }

/* ------------------------------- Primary Lund plane in kT ------------------------------- */

// Primary Lund plane in kT overall
    TH2D *PrimaryLundPlane_kT_2D = static_cast<TH2D*>(PrimaryLundPlane_kT->Project3D("yx"));
    TH2D *PrimaryLundPlane_kT_2D_Clone1 = (TH2D*)PrimaryLundPlane_kT_2D->Clone("PrimaryLundPlane_kT_2D_Clone1");
    TH2D *PrimaryLundPlane_kT_2D_Clone2 = (TH2D*)PrimaryLundPlane_kT_2D->Clone("PrimaryLundPlane_kT_2D_Clone2");
    TH2D *PrimaryLundPlane_kT_2D_Clone3 = (TH2D*)PrimaryLundPlane_kT_2D->Clone("PrimaryLundPlane_kT_2D_Clone3");
    TH2D *PrimaryLundPlane_kT_2D_Clone4 = (TH2D*)PrimaryLundPlane_kT_2D->Clone("PrimaryLundPlane_kT_2D_Clone4");

    ScaleHistogram(PrimaryLundPlane_kT_2D, N_jets);
    PlotLundPlane(PrimaryLundPlane_kT_2D, 
                "2D Primary Lund Plane in kT", 
                "ln(R/#Delta)", 
                "ln(k_{t}/GeV)", 
                "1/N_{jets} #frac{d^{2}n_{emissions}}{dln(k_{t}/GeV) dln(R/#Delta)}", 
                "kt_LundPlots/PrimaryLundPlane_kT_overall", 
                0, 4.5, 
                -2, 3.5, 
                0, 200);

    // Y projections    
    TH1D* projY_1_kT = ScaleYProjection(PrimaryLundPlane_kT_2D_Clone1, N_jets, 0, 1); // 0 < ln(R/#Delta) < 1
    PlotProjection(projY_1_kT, 
                "Primary Lund Plane in kT projected over ln(k_{t}/GeV) with 0 < ln(R/#Delta) < 1", 
                "ln(k_{t}/GeV)", 
                "1/N_{jets} #frac{d^{2}n_{emissions}}{dln(k_{t}/GeV) dln(R/#Delta)}", 
                "kt_LundPlots/yProjections/projY_1_kT.png", 
                -2, 3.5);  
    TH1D* projY_2_kT = ScaleYProjection(PrimaryLundPlane_kT_2D_Clone2, N_jets, 1, 4.5); // 1 < ln(R/#Delta) < 4.5
    PlotProjection(projY_2_kT, 
                "Primary Lund Plane in kT projected over ln(k_{t}/GeV) with 1 < ln(R/#Delta) < 4.5", 
                "ln(k_{t}/GeV)", 
                "1/N_{jets} #frac{d^{2}n_{emissions}}{dln(k_{t}/GeV) dln(R/#Delta)}", 
                "kt_LundPlots/yProjections/projY_2_kT.png", 
                -2, 3.5);  
    // X projections
    TH1D* projX_1_kT = ScaleXProjection(PrimaryLundPlane_kT_2D_Clone3, N_jets, -2, 0); // -2 < ln(k_{t}/GeV) < 0
    PlotProjection(projX_1_kT, 
                "Primary Lund Plane in kT projected over ln(R/#Delta) with -2 < ln(k_{t}/GeV) < 0", 
                "ln(R/#Delta)", 
                "1/N_{jets} #frac{d^{2}n_{emissions}}{dln(k_{t}/GeV) dln(R/#Delta)}", 
                "kt_LundPlots/xProjections/projX_1_kT.png", 
                0, 4.5,
                kPink+5, kPink+5);  
    TH1D* projX_2_kT = ScaleXProjection(PrimaryLundPlane_kT_2D_Clone4, N_jets, 0, 3.5); // 0 < ln(k_{t}/GeV) < 3.5
    PlotProjection(projX_2_kT, 
                "Primary Lund Plane in kT projected over ln(R/#Delta) with 0 < ln(k_{t}/GeV) < 3.5", 
                "ln(R/#Delta)", 
                "1/N_{jets} #frac{d^{2}n_{emissions}}{dln(k_{t}/GeV) dln(R/#Delta)}", 
                "kt_LundPlots/xProjections/projX_2_kT.png", 
                0, 4.5,
                kPink+5, kPink+5);  
    
// Primary Lund Plane in kT in different pT bins
    for (int i = 0; i < nBins; ++i) {
        float ptMin = ptBins[i][0];
        float ptMax = ptBins[i][1];
        PrimaryLundPlane_kT->GetZaxis()->SetRangeUser(ptMin, ptMax);
        TH2D* PrimaryLundPlane_kT_2D = static_cast<TH2D*>(PrimaryLundPlane_kT->Project3D(Form("yx_ptbin_kT_%d", i)));
        TH2D* PrimaryLundPlane_kT_2D_Clone1 = (TH2D*)PrimaryLundPlane_kT_2D->Clone(Form("PrimaryLundPlane_kT_2D_Clone1_pt_%g_%g", ptMin, ptMax));
        TH2D* PrimaryLundPlane_kT_2D_Clone2 = (TH2D*)PrimaryLundPlane_kT_2D->Clone(Form("PrimaryLundPlane_kT_2D_Clone2_pt_%g_%g", ptMin, ptMax));
        TH2D* PrimaryLundPlane_kT_2D_Clone3 = (TH2D*)PrimaryLundPlane_kT_2D->Clone(Form("PrimaryLundPlane_kT_2D_Clone3_pt_%g_%g", ptMin, ptMax));
        TH2D* PrimaryLundPlane_kT_2D_Clone4 = (TH2D*)PrimaryLundPlane_kT_2D->Clone(Form("PrimaryLundPlane_kT_2D_Clone4_pt_%g_%g", ptMin, ptMax));

        ScaleHistogram(PrimaryLundPlane_kT_2D, N_jets_per_bin[i]);
        const char* fileNameBase = "kt_LundPlots/PrimaryLundPlane_kT";
        PlotLundPlane(PrimaryLundPlane_kT_2D, 
                    "2D Primary Lund Plane in kT", "ln(R/#Delta)", 
                    "ln(k_{t}/GeV)", 
                    "1/N_{jets} #frac{d^{2}n_{emissions}}{dln(k_{t}/GeV) dln(R/#Delta)}", 
                    fileNameBase, 
                    0, 4.5, 
                    -2, 3.5, 
                    ptMin, ptMax);

        // Y projections
        TH1D* projY_1_kT = ScaleYProjection(PrimaryLundPlane_kT_2D_Clone1, N_jets_per_bin[i], 0, 1); // 0 < ln(R/#Delta) < 1
        PlotProjection(projY_1_kT, 
                    Form("Primary Lund Plane in kT projected over ln(k_{t}/GeV) with 0 < ln(R/#Delta) < 1 for %g < p_{T} < %g", ptMin, ptMax), 
                    "ln(k_{t}/GeV)", 
                    "1/N_{jets} #frac{d^{2}n_{emissions}}{dln(k_{t}/GeV) dln(R/#Delta)}", 
                    Form("kt_LundPlots/yProjections/projY_1_kT_%g_%g.png", ptMin, ptMax), 
                    -1, 3.5);
        TH1D* projY_2_kT = ScaleYProjection(PrimaryLundPlane_kT_2D_Clone2, N_jets_per_bin[i], 1, 4.5); // 1 < ln(R/#Delta) < 4.5
        PlotProjection(projY_2_kT, 
                    Form("Primary Lund Plane in kT projected over ln(k_{t}/GeV) with 1 < ln(R/#Delta) < 4.5 for %g < p_{T} < %g", ptMin, ptMax), 
                    "ln(k_{t}/GeV)", 
                    "1/N_{jets} #frac{d^{2}n_{emissions}}{dln(k_{t}/GeV) dln(R/#Delta)}", 
                    Form("kt_LundPlots/yProjections/projY_2_kT_%g_%g.png", ptMin, ptMax), 
                    -1, 3.5);
        // X projections
        TH1D* projX_1_kT = ScaleXProjection(PrimaryLundPlane_kT_2D_Clone3, N_jets_per_bin[i], -2, 0); // -2 < ln(k_{t}/GeV) < 0
        PlotProjection(projX_1_kT, 
                    Form("Primary Lund Plane in kT projected over ln(R/#Delta) with -2 < ln(k_{t}/GeV) < 0 for %g < p_{T} < %g", ptMin, ptMax), 
                    "ln(R/#Delta)", 
                    "1/N_{jets} #frac{d^{2}n_{emissions}}{dln(k_{t}/GeV) dln(R/#Delta)}", 
                    Form("kt_LundPlots/xProjections/projX_1_kT_%g_%g.png", ptMin, ptMax), 
                    0, 4.5,
                    kPink+5, kPink+5);
        TH1D* projX_2_kT = ScaleXProjection(PrimaryLundPlane_kT_2D_Clone4, N_jets_per_bin[i], 0, 3.5); // 0 < ln(k_{t}/GeV) < 3.5
        PlotProjection(projX_2_kT, 
                    Form("Primary Lund Plane in kT projected over ln(R/#Delta) with 0 < ln(k_{t}/GeV) < 3.5 for %g < p_{T} < %g", ptMin, ptMax), 
                    "ln(R/#Delta)", 
                    "1/N_{jets} #frac{d^{2}n_{emissions}}{dln(k_{t}/GeV) dln(R/#Delta)}", 
                    Form("kt_LundPlots/xProjections/projX_2_kT_%g_%g.png", ptMin, ptMax), 
                    0, 4.5,
                    kPink+5, kPink+5);
    
    }
            
/* ------------------------------- Primary Lund plane in z ------------------------------- */

// Primary Lund Plane in z overall

    TH2D *PrimaryLundPlane_z_2D = static_cast<TH2D*>(PrimaryLundPlane_z->Project3D("yx"));
    TH2D *PrimaryLundPlane_z_2D_Clone1 = (TH2D*)PrimaryLundPlane_z_2D->Clone("PrimaryLundPlane_z_2D_Clone1");
    TH2D *PrimaryLundPlane_z_2D_Clone2 = (TH2D*)PrimaryLundPlane_z_2D->Clone("PrimaryLundPlane_z_2D_Clone2");
    TH2D *PrimaryLundPlane_z_2D_Clone3 = (TH2D*)PrimaryLundPlane_z_2D->Clone("PrimaryLundPlane_z_2D_Clone3");
    TH2D *PrimaryLundPlane_z_2D_Clone4 = (TH2D*)PrimaryLundPlane_z_2D->Clone("PrimaryLundPlane_z_2D_Clone4");

    ScaleHistogram(PrimaryLundPlane_z_2D, N_jets);
    PlotLundPlane(PrimaryLundPlane_z_2D, 
                "2D Primary Lund Plane in z", 
                "ln(R/#Delta)", 
                "ln(1/z)", 
                "1/N_{jets} #frac{d^{2}n_{emissions}}{dln(1/z) dln(R/#Delta)}", 
                "z_LundPlots/PrimaryLundPlane_z_overall", 
                0, 4.5, 
                0.6, 6, 
                0, 200);

    // Y projections
    TH1D* projY_1_z = ScaleYProjection(PrimaryLundPlane_z_2D_Clone1, N_jets, 0, 1); // 0 < ln(R/#Delta) < 1
    PlotProjection(projY_1_z, 
                "Primary Lund Plane in z projected over ln(1/z) with 0 < ln(R/#Delta) < 1", 
                "ln(1/z)", 
                "1/N_{jets} #frac{d^{2}n_{emissions}}{dln(1/z) dln(R/#Delta)}", 
                "z_LundPlots/yProjections/projY_1_z.png", 
                0.6, 6,
                kViolet+6, kViolet+6);  
    TH1D* projY_2_z = ScaleYProjection(PrimaryLundPlane_z_2D_Clone2, N_jets, 1, 4.5); // 1 < ln(R/#Delta) < 4.5
    PlotProjection(projY_2_z, 
                "Primary Lund Plane in z projected over ln(1/z) with 1 < ln(R/#Delta) < 4.5", 
                "ln(1/z)", 
                "1/N_{jets} #frac{d^{2}n_{emissions}}{dln(1/z) dln(R/#Delta)}", 
                "z_LundPlots/yProjections/projY_2_z.png", 
                0.6, 6,
                kViolet+6, kViolet+6);  
    // X projections
    TH1D* projX_1_z = ScaleXProjection(PrimaryLundPlane_z_2D_Clone3, N_jets, 0.6, 2); // 0.6 < ln(1/z) < 2
    PlotProjection(projX_1_z, 
                "Primary Lund Plane in z projected over ln(R/#Delta) with 0.6 < ln(1/z) < 2", 
                "ln(R/#Delta)", 
                "1/N_{jets} #frac{d^{2}n_{emissions}}{dln(1/z) dln(R/#Delta)}", 
                "z_LundPlots/xProjections/projX_1_z.png", 
                0, 4.5,
                kTeal-6, kTeal-6);  
    TH1D* projX_2_z = ScaleXProjection(PrimaryLundPlane_z_2D_Clone4, N_jets, 2, 6); // 2 < ln(1/z) < 6
    PlotProjection(projX_1_z, 
                "Primary Lund Plane in z projected over ln(R/#Delta) with 2 < ln(1/z) < 6", 
                "ln(R/#Delta)", 
                "1/N_{jets} #frac{d^{2}n_{emissions}}{dln(1/z) dln(R/#Delta)}", 
                "z_LundPlots/xProjections/projX_2_z.png", 
                0, 4.5,
                kTeal-6, kTeal-6);  

// Primary Lund Plane in z in different pT bins
    for (int i = 0; i < nBins; ++i) {
        float ptMin = ptBins[i][0];
        float ptMax = ptBins[i][1];
        PrimaryLundPlane_z->GetZaxis()->SetRangeUser(ptMin, ptMax);
        TH2D* PrimaryLundPlane_z_2D = static_cast<TH2D*>(PrimaryLundPlane_z->Project3D(Form("yx_ptbin_z_%d", i)));
        TH2D* PrimaryLundPlane_z_2D_Clone1 = (TH2D*)PrimaryLundPlane_z_2D->Clone(Form("PrimaryLundPlane_z_2D_Clone1_pt_%g_%g", ptMin, ptMax));
        TH2D* PrimaryLundPlane_z_2D_Clone2 = (TH2D*)PrimaryLundPlane_z_2D->Clone(Form("PrimaryLundPlane_z_2D_Clone2_pt_%g_%g", ptMin, ptMax));
        TH2D* PrimaryLundPlane_z_2D_Clone3 = (TH2D*)PrimaryLundPlane_z_2D->Clone(Form("PrimaryLundPlane_z_2D_Clone3_pt_%g_%g", ptMin, ptMax));
        TH2D* PrimaryLundPlane_z_2D_Clone4 = (TH2D*)PrimaryLundPlane_z_2D->Clone(Form("PrimaryLundPlane_z_2D_Clone4_pt_%g_%g", ptMin, ptMax));

        ScaleHistogram(PrimaryLundPlane_z_2D, N_jets_per_bin[i]);
        const char* fileNameBase = "z_LundPlots/PrimaryLundPlane_z";
        PlotLundPlane(PrimaryLundPlane_z_2D, 
                    "2D Primary Lund Plane in z", "ln(R/#Delta)", 
                    "ln(1/z)", 
                    "1/N_{jets} #frac{d^{2}n_{emissions}}{dln(1/z) dln(R/#Delta)}", 
                    fileNameBase, 
                    0, 4.5, 
                    0.6, 6, 
                    ptMin, ptMax);

        // Y projections
        TH1D* projY_1_z = ScaleYProjection(PrimaryLundPlane_z_2D_Clone1, N_jets_per_bin[i], 0, 1); // 0 < ln(R/#Delta) < 1
        PlotProjection(projY_1_z, 
                    Form("Primary Lund Plane in z projected over ln(1/z) with 0 < ln(R/#Delta) < 1 for %g < p_{T} < %g", ptMin, ptMax), 
                    "ln(1/z)", 
                    "1/N_{jets} #frac{d^{2}n_{emissions}}{dln(1/z) dln(R/#Delta)}", 
                    Form("z_LundPlots/yProjections/projY_1_z_%g_%g.png", ptMin, ptMax), 
                    0.6, 6,
                    kViolet+6, kViolet+6);  
        TH1D* projY_2_z = ScaleYProjection(PrimaryLundPlane_z_2D_Clone2, N_jets_per_bin[i], 1, 4.5); // 1 < ln(R/#Delta) < 4.5
        PlotProjection(projY_2_z, 
                    Form("Primary Lund Plane in z projected over ln(1/z) with 1 < ln(R/#Delta) < 4.5 for %g < p_{T} < %g", ptMin, ptMax), 
                    "ln(1/z)", 
                    "1/N_{jets} #frac{d^{2}n_{emissions}}{dln(1/z) dln(R/#Delta)}", 
                    Form("z_LundPlots/yProjections/projY_2_z_%g_%g.png", ptMin, ptMax), 
                    0.6, 6,
                    kViolet+6, kViolet+6);  
        // X projections
        TH1D* projX_1_z = ScaleXProjection(PrimaryLundPlane_z_2D_Clone3, N_jets_per_bin[i], 0.6, 2); // 0.6 < ln(1/z) < 2
        PlotProjection(projX_1_z, 
                    Form("Primary Lund Plane in z projected over ln(R/#Delta) with 0.6 < ln(1/z) < 2 for %g < p_{T} < %g", ptMin, ptMax), 
                    "ln(R/#Delta)", 
                    "1/N_{jets} #frac{d^{2}n_{emissions}}{dln(1/z) dln(R/#Delta)}", 
                    Form("z_LundPlots/xProjections/projX_1_z_%g_%g.png", ptMin, ptMax), 
                    0, 4.5,
                    kTeal-6, kTeal-6);  
        TH1D* projX_2_z = ScaleXProjection(PrimaryLundPlane_z_2D_Clone4, N_jets_per_bin[i], 2, 6); // 2 < ln(1/z) < 6
        PlotProjection(projX_2_z, 
                    Form("Primary Lund Plane in z projected over ln(R/#Delta) with 2 < ln(1/z) < 6 for %g < p_{T} < %g", ptMin, ptMax), 
                    "ln(R/#Delta)", 
                    "1/N_{jets} #frac{d^{2}n_{emissions}}{dln(1/z) dln(R/#Delta)}", 
                    Form("z_LundPlots/xProjections/projX_2_z_%g_%g.png", ptMin, ptMax), 
                    0, 4.5,
                    kTeal-6, kTeal-6);  
    }

    // jet pT
    TCanvas *p = new TCanvas("p", "jet_pT", 4000, 2700);
    p->SetLogy();
    jet_pT->SetLineColor(kBlue-3);
    jet_pT->SetLineWidth(3);
    jet_pT->SetMarkerStyle(20);
    jet_pT->SetMarkerSize(2);
    jet_pT->SetMarkerColor(kBlue-3);
    jet_pT->Draw("E");
    gPad->Update();
    TPaveStats* st_pt = (TPaveStats*)jet_pT->FindObject("stats");
    st_pt->SetX1NDC(0.68);
    st_pt->SetX2NDC(0.85);
    st_pt->SetY1NDC(0.73);
    st_pt->SetY2NDC(0.90);
    p->SaveAs("jet_pT.png");
    delete p;

}