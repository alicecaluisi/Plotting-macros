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

void QAplots_pT() {  
    TFile* file = new TFile("/dcache/alice/acaluisi/JetQA/LHC22_pass4_highIR_sampling/full/AnalysisResults.root");
    THnSparseD* collisionVtxZSparse_GT = dynamic_cast<THnSparseD*>(file->Get("track-jet-qa_id9648/EventProp/collisionVtxZ")); // GlobalTracks
    THnSparseD* hPtSparse_GT = dynamic_cast<THnSparseD*>(file->Get("track-jet-qa_id9648/Kine/pt"));
    THnSparseD* collisionVtxZSparse_tight = dynamic_cast<THnSparseD*>(file->Get("track-jet-qa_id9647/EventProp/collisionVtxZ")); // Tight cuts
    THnSparseD* hPtSparse_tight = dynamic_cast<THnSparseD*>(file->Get("track-jet-qa_id9647/Kine/pt"));
    THnSparseD* collisionVtxZSparse_loose = dynamic_cast<THnSparseD*>(file->Get("track-jet-qa_id9646/EventProp/collisionVtxZ")); // Loose
    THnSparseD* hPtSparse_loose = dynamic_cast<THnSparseD*>(file->Get("track-jet-qa_id9646/Kine/pt"));

    TH1D* collisionVtxZ_GT = dynamic_cast<TH1D*>(collisionVtxZSparse_GT->Projection(0));
    double numberOfEvents_GT = collisionVtxZ_GT->GetEntries();
    TH1D* collisionVtxZ_tight = dynamic_cast<TH1D*>(collisionVtxZSparse_tight->Projection(0));
    double numberOfEvents_tight = collisionVtxZ_tight->GetEntries();
    TH1D* collisionVtxZ_loose = dynamic_cast<TH1D*>(collisionVtxZSparse_loose->Projection(0));
    double numberOfEvents_loose = collisionVtxZ_loose->GetEntries();

    TH1D* pt_GT = Project1D(hPtSparse_GT, 0, "E");
    TH1D* pt_tight = Project1D(hPtSparse_tight, 0, "E");
    TH1D* pt_loose = Project1D(hPtSparse_loose, 0, "E");
    pt_GT->SetStats(kFALSE);
    pt_tight->SetStats(kFALSE); 
    pt_loose->SetStats(kFALSE);
    pt_GT->Scale(1.0 / numberOfEvents_GT);
    pt_tight->Scale(1.0 / numberOfEvents_tight);
    pt_loose->Scale(1.0 / numberOfEvents_loose);
    pt_GT->SetTitle(" ; p_{T} (GeV/c); Number of entries / Number of events");
    pt_tight->SetTitle(" ; p_{T} (GeV/c); Number of entries / Number of events");
    pt_loose->SetTitle(" ; p_{T} (GeV/c); Number of entries / Number of events");
    TCanvas* canvas = new TCanvas("canvas", "pT comparison and ratio", 3000, 1200);
    canvas->SetTopMargin(0.20);
    canvas->Divide(2, 1);
    canvas->cd(1);
    gPad->SetLogy();
    pt_GT->SetLineColor(kTeal-5);
    pt_GT->SetLineWidth(6);
    pt_GT->SetMarkerStyle(20);
    pt_GT->SetMarkerSize(2);
    pt_GT->SetMarkerColor(kTeal-5);
    pt_tight->SetLineColor(kViolet+2);
    pt_tight->SetLineWidth(6);
    pt_tight->SetMarkerStyle(20);
    pt_tight->SetMarkerSize(2);
    pt_tight->SetMarkerColor(kViolet+2);
    pt_loose->SetLineColor(kPink+7);
    pt_loose->SetLineWidth(6);
    pt_loose->SetMarkerStyle(20);
    pt_loose->SetMarkerSize(2);
    pt_loose->SetMarkerColor(kPink+7);
    pt_GT->Draw("HIST");
    pt_tight->Draw("HIST SAME");
    pt_loose->Draw("HIST SAME");
    gPad->Update();
    /*
    TLatex latex1;
    latex1.SetTextSize(0.03);
    latex1.DrawLatexNDC(0.1, 0.93, "p_{T} spectra for GlobalTracks, tight and loose cuts");
    TPaveStats* st_pt = (TPaveStats*)pt_GT->FindObject("stats");
    if (st_pt) {
        st_pt->SetX1NDC(0.73);
        st_pt->SetX2NDC(0.90);
        st_pt->SetY1NDC(0.90);
        st_pt->SetY2NDC(0.99);
    }
    */
    auto legend = new TLegend(0.73, 0.73, 0.9, 0.9);
    legend->SetFillStyle(0);
    legend->AddEntry(pt_tight, "Tight cuts p_{T}", "lep");
    legend->AddEntry(pt_GT, "GlobalTracks p_{T}", "lep");
    legend->AddEntry(pt_loose, "Loose cuts p_{T}", "lep");
    legend->Draw();
    canvas->cd(2);
    TH1D* pt_GT_reb = (TH1D*)pt_GT->Clone("pt_GT_reb");
    TH1D* pt_tight_reb = (TH1D*)pt_tight->Clone("pt_tight_reb");
    TH1D* pt_loose_reb = (TH1D*)pt_loose->Clone("pt_loose_reb");
    pt_GT_reb->Rebin(2);
    pt_tight_reb->Rebin(2);
    pt_loose_reb->Rebin(2);

    // Creating the ratio histograms
    TH1D* ptRatio_GT_Tight = (TH1D*)pt_tight_reb->Clone("ptRatio_GT_Tight");
    TH1D* ptRatio_GT_Loose = (TH1D*)pt_loose_reb->Clone("ptRatio_GT_Loose");
    ptRatio_GT_Tight->Divide(pt_GT_reb);
    ptRatio_GT_Loose->Divide(pt_GT_reb);

    // Setting titles and labels for the first ratio histogram
    ptRatio_GT_Tight->SetTitle(" ; p_{T} (GeV/c); Ratio");
    ptRatio_GT_Tight->GetYaxis()->SetTitle("Tight (Loose) cuts p_{T} / GlobalTracks p_{T}");
    ptRatio_GT_Tight->SetMinimum(0.7); // Set the minimum value of y-axis to 0.7
    ptRatio_GT_Tight->SetMaximum(1.4); // Set the maximum value of y-axis to 1.4
    ptRatio_GT_Loose->SetMinimum(0.7); // Ensure consistency across both histograms
    ptRatio_GT_Loose->SetMaximum(1.4);

    // Drawing the first ratio histogram
    ptRatio_GT_Tight->SetLineColor(kRed-7);
    ptRatio_GT_Tight->SetLineWidth(3);
    ptRatio_GT_Tight->SetMarkerStyle(20);
    ptRatio_GT_Tight->SetMarkerSize(2);
    ptRatio_GT_Tight->SetMarkerColor(kRed-7);
    ptRatio_GT_Tight->Draw("E");  // Use "E" option for error bars

    // Drawing the second ratio histogram on the same pad
    ptRatio_GT_Loose->SetLineColor(kBlue-7);
    ptRatio_GT_Loose->SetLineWidth(3);
    ptRatio_GT_Loose->SetMarkerStyle(21);  // Slightly different marker style for distinction
    ptRatio_GT_Loose->SetMarkerSize(2);
    ptRatio_GT_Loose->SetMarkerColor(kBlue-7);
    ptRatio_GT_Loose->Draw("E SAME");  // Use "SAME" to overlay on the existing histogram

    // Draw a horizontal dashed line at y=1 for reference
    double xMin = ptRatio_GT_Tight->GetXaxis()->GetXmin();
    double xMax = ptRatio_GT_Tight->GetXaxis()->GetXmax();
    TLine* line = new TLine(xMin, 1, xMax, 1);
    line->SetLineStyle(7); // Dashed line
    line->Draw();

    // Updating the pad
    gPad->Update();
    /*
    TLatex latex2;
    latex2.SetTextSize(0.03);
    latex2.DrawLatexNDC(0.1, 0.93, "Tight (Loose) cuts p_{T} / GlobalTracks p_{T}");    
    TPaveStats* st_p = (TPaveStats*)ptRatio_GT_Tight->FindObject("stats");
    if (st_p) {
        st_p->SetX1NDC(0.73);
        st_p->SetX2NDC(0.90);
        st_p->SetY1NDC(0.90);
        st_p->SetY2NDC(0.99);
    }
    */
    auto legend2 = new TLegend(0.10, 0.72, 0.40, 0.90);
    legend2->SetFillStyle(0);
    legend2->AddEntry(ptRatio_GT_Tight, "Tight cuts p_{T} / GlobalTracks p_{T}", "lep");
    legend2->AddEntry(ptRatio_GT_Loose, "Loose cuts p_{T} / GlobalTracks p_{T}", "lep");
    legend2->Draw();
    canvas->SaveAs("pt_comparison_cuts.png");
    delete canvas;
    file->Close();
    delete file;

}