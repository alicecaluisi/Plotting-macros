#include <TCanvas.h>
#include <TImage.h>
#include <TPad.h>
#include <TSystem.h>

void CompareImages(const char* imgPath1, const char* imgPath2, const char* imgPath3, const char* outFileName) {
    TCanvas* canvas = new TCanvas("canvas", "Image Comparison", 3300, 1000);
    canvas->Divide(3, 1); 
    const char* imgPaths[] = {imgPath1, imgPath2, imgPath3};
    const char* titles[] = {"Tight cuts", "GlobalTracks selection", "Loose cuts"};
    Color_t colors[] = {kViolet+2, kTeal-5, kPink+7};
    for (int i = 0; i < 3; ++i) {
        canvas->cd(i + 1);
        /*
        TPad* titlePad = new TPad("title", "title", 0, 0.9, 1, 1);
        titlePad->Draw();
        titlePad->cd();
        titlePad->SetBottomMargin(0.5); 
        TLatex* latex = new TLatex(0.5, 0.5, titles[i]); 
        latex->SetTextAlign(22); 
        latex->SetTextSize(0.4); 
        latex->SetTextColor(colors[i]);
        latex->Draw();
        */
        TPad* imagePad = new TPad("image", "image", 0, 0, 1, 0.9);
        imagePad->Draw();
        imagePad->cd();
        if (!gSystem->AccessPathName(imgPaths[i])) {
            TImage* img = TImage::Open(imgPaths[i]);
            if (img) {
                img->Draw("X");
            } else {
                printf("Failed to open image %s\n", imgPaths[i]);
            }
        } else {
            printf("Image not found: %s\n", imgPaths[i]);
        }
    }
    canvas->SaveAs(outFileName);
    delete canvas;
}


void CompareProj(const char* imgPath1, const char* imgPath2, const char* imgPath3, const char* imgPath4, const char* outFileName) {
    TCanvas* canvas = new TCanvas("canvas", "Image Comparison", 4000, 4000);
    canvas->Divide(2, 2);
    const char* imgPaths[] = {imgPath1, imgPath2, imgPath3, imgPath4};
    for (int i = 0; i < 4; ++i) {
        canvas->cd(i + 1);
        TPad* titlePad = new TPad("title", "title", 0, 0.9, 1, 1);
        titlePad->Draw();
        titlePad->cd();
        canvas->cd(i + 1);
        TPad* imagePad = new TPad("image", "image", 0, 0, 1, 1);
        imagePad->Draw();
        imagePad->cd();
        imagePad->SetTopMargin(0); 
        if (!gSystem->AccessPathName(imgPaths[i])) { 
            TImage* img = TImage::Open(imgPaths[i]);
            if (img) {
                img->Draw("X");
            } else {
                printf("Failed to open image %s\n", imgPaths[i]);
            }
        } else {
            printf("Image not found: %s\n", imgPaths[i]);
        }
    }
    canvas->SaveAs(outFileName);
    delete canvas;
}

void CompareEtaPhi(const char* imgPath1, const char* imgPath2, const char* imgPath3, const char* outFileName) {
    TCanvas* canvas = new TCanvas("canvas", "Image Comparison", 3300, 1000);
    canvas->Divide(3, 1);
    const char* imgPaths[] = {imgPath1, imgPath2, imgPath3};
    for (int i = 0; i < 3; ++i) {
        canvas->cd(i + 1);
        TPad* titlePad = new TPad("title", "title", 0, 0.9, 1, 1);
        titlePad->Draw();
        titlePad->cd();
        titlePad->SetBottomMargin(0.5); 
        TLatex* latex = new TLatex(0.5, 0.5, "GlobalTracks selection"); 
        latex->SetTextAlign(22); 
        latex->SetTextSize(0.4); 
        latex->SetTextColor(kTeal-5);
        canvas->cd(i + 1);
        TPad* imagePad = new TPad("image", "image", 0, 0, 1, 0.9);
        imagePad->Draw();
        imagePad->cd();
        imagePad->SetTopMargin(0); 
        if (!gSystem->AccessPathName(imgPaths[i])) { 
            TImage* img = TImage::Open(imgPaths[i]);
            if (img) {
                img->Draw("X");
            } else {
                printf("Failed to open image %s\n", imgPaths[i]);
            }
        } else {
            printf("Image not found: %s\n", imgPaths[i]);
        }
    }
    canvas->SaveAs(outFileName);
    delete canvas;
}

void CompareProj3(const char* imgPath1, const char* imgPath2, const char* imgPath3, const char* outFileName) {
    TCanvas* canvas = new TCanvas("canvas", "Image Comparison", 3300, 1000);
    canvas->Divide(3, 1);
    const char* imgPaths[] = {imgPath1, imgPath2, imgPath3};
    for (int i = 0; i < 3; ++i) {
        canvas->cd(i + 1);
        TPad* titlePad = new TPad("title", "title", 0, 0.9, 1, 1);
        titlePad->Draw();
        titlePad->cd();
        canvas->cd(i + 1);
        TPad* imagePad = new TPad("image", "image", 0, 0, 1, 0.9);
        imagePad->Draw();
        imagePad->cd();
        imagePad->SetTopMargin(0); 
        if (!gSystem->AccessPathName(imgPaths[i])) { 
            TImage* img = TImage::Open(imgPaths[i]);
            if (img) {
                img->Draw("X");
            } else {
                printf("Failed to open image %s\n", imgPaths[i]);
            }
        } else {
            printf("Image not found: %s\n", imgPaths[i]);
        }
    }
    canvas->SaveAs(outFileName);
    delete canvas;
}

void plotting() {
    gSystem->mkdir("EventProp", kTRUE); 
    gSystem->mkdir("Kine", kTRUE); 
    gSystem->mkdir("TrackPar", kTRUE); 
    gSystem->mkdir("ITS", kTRUE); 
    gSystem->mkdir("TPC", kTRUE); 

    // TPC

    CompareImages(
        "/data/alice/acaluisi/alice/workdir/TrackJetQA/LHC22_pass4_highIR_sampling/full/tight/TPC/tpcNClsFindable.png",
        "/data/alice/acaluisi/alice/workdir/TrackJetQA/LHC22_pass4_highIR_sampling/full/GlobalTracks/TPC/tpcNClsFindable.png",
        "/data/alice/acaluisi/alice/workdir/TrackJetQA/LHC22_pass4_highIR_sampling/full/loose/TPC/tpcNClsFindable.png",
        "TPC/tpcNClsFindable.png" 
    );
    CompareProj(
        "/data/alice/acaluisi/alice/workdir/TrackJetQA/LHC22_pass4_highIR_sampling/full/Comparisons/tpcNClsFindable_0_1.png",
        "/data/alice/acaluisi/alice/workdir/TrackJetQA/LHC22_pass4_highIR_sampling/full/Comparisons/tpcNClsFindable_3_5.png", 
        "/data/alice/acaluisi/alice/workdir/TrackJetQA/LHC22_pass4_highIR_sampling/full/Comparisons/tpcNClsFindable_50_100.png",
        "/data/alice/acaluisi/alice/workdir/TrackJetQA/LHC22_pass4_highIR_sampling/full/Comparisons/tpcNClsFindable_100_200.png",       
        "TPC/tpcNClsFindable_proj.png" 
    );

    CompareImages(
        "/data/alice/acaluisi/alice/workdir/TrackJetQA/LHC22_pass4_highIR_sampling/full/tight/TPC/tpcNClsFound.png",
        "/data/alice/acaluisi/alice/workdir/TrackJetQA/LHC22_pass4_highIR_sampling/full/GlobalTracks/TPC/tpcNClsFound.png",
        "/data/alice/acaluisi/alice/workdir/TrackJetQA/LHC22_pass4_highIR_sampling/full/loose/TPC/tpcNClsFound.png",
        "TPC/tpcNClsFound.png" 
    );
    CompareProj(
        "/data/alice/acaluisi/alice/workdir/TrackJetQA/LHC22_pass4_highIR_sampling/full/Comparisons/tpcNClsFound_0_1.png",
        "/data/alice/acaluisi/alice/workdir/TrackJetQA/LHC22_pass4_highIR_sampling/full/Comparisons/tpcNClsFound_3_5.png", 
        "/data/alice/acaluisi/alice/workdir/TrackJetQA/LHC22_pass4_highIR_sampling/full/Comparisons/tpcNClsFound_50_100.png",
        "/data/alice/acaluisi/alice/workdir/TrackJetQA/LHC22_pass4_highIR_sampling/full/Comparisons/tpcNClsFound_100_200.png",       
        "TPC/tpcNClsFound_proj.png" 
    );

    CompareImages(        
        "/data/alice/acaluisi/alice/workdir/TrackJetQA/LHC22_pass4_highIR_sampling/full/tight/TPC/tpcNClsCrossedRows.png",
        "/data/alice/acaluisi/alice/workdir/TrackJetQA/LHC22_pass4_highIR_sampling/full/GlobalTracks/TPC/tpcNClsCrossedRows.png",
        "/data/alice/acaluisi/alice/workdir/TrackJetQA/LHC22_pass4_highIR_sampling/full/loose/TPC/tpcNClsCrossedRows.png",
        "TPC/tpcNClsCrossedRows.png" 
    );
    CompareProj(
        "/data/alice/acaluisi/alice/workdir/TrackJetQA/LHC22_pass4_highIR_sampling/full/Comparisons/tpcNClsCrossedRows_0_1.png",
        "/data/alice/acaluisi/alice/workdir/TrackJetQA/LHC22_pass4_highIR_sampling/full/Comparisons/tpcNClsCrossedRows_3_5.png", 
        "/data/alice/acaluisi/alice/workdir/TrackJetQA/LHC22_pass4_highIR_sampling/full/Comparisons/tpcNClsCrossedRows_50_100.png",
        "/data/alice/acaluisi/alice/workdir/TrackJetQA/LHC22_pass4_highIR_sampling/full/Comparisons/tpcNClsCrossedRows_100_200.png",       
        "TPC/tpcNClsCrossedRows_proj.png" 
    );

    CompareImages(        
        "/data/alice/acaluisi/alice/workdir/TrackJetQA/LHC22_pass4_highIR_sampling/full/tight/TPC/tpcChi2NCl.png",
        "/data/alice/acaluisi/alice/workdir/TrackJetQA/LHC22_pass4_highIR_sampling/full/GlobalTracks/TPC/tpcChi2NCl.png",
        "/data/alice/acaluisi/alice/workdir/TrackJetQA/LHC22_pass4_highIR_sampling/full/loose/TPC/tpcChi2NCl.png",
        "TPC/tpcChi2NCl.png" 
    );
    CompareProj(
        "/data/alice/acaluisi/alice/workdir/TrackJetQA/LHC22_pass4_highIR_sampling/full/Comparisons/tpcChi2NCl_0_1.png",
        "/data/alice/acaluisi/alice/workdir/TrackJetQA/LHC22_pass4_highIR_sampling/full/Comparisons/tpcChi2NCl_3_5.png", 
        "/data/alice/acaluisi/alice/workdir/TrackJetQA/LHC22_pass4_highIR_sampling/full/Comparisons/tpcChi2NCl_50_100.png",
        "/data/alice/acaluisi/alice/workdir/TrackJetQA/LHC22_pass4_highIR_sampling/full/Comparisons/tpcChi2NCl_100_200.png",       
        "TPC/tpcChi2NCl_proj.png" 
    );

    CompareImages(        
        "/data/alice/acaluisi/alice/workdir/TrackJetQA/LHC22_pass4_highIR_sampling/full/tight/TPC/tpcCrossedRowsOverFindableCls.png",
        "/data/alice/acaluisi/alice/workdir/TrackJetQA/LHC22_pass4_highIR_sampling/full/GlobalTracks/TPC/tpcCrossedRowsOverFindableCls.png",
        "/data/alice/acaluisi/alice/workdir/TrackJetQA/LHC22_pass4_highIR_sampling/full/loose/TPC/tpcCrossedRowsOverFindableCls.png",
        "TPC/tpcCrossedRowsOverFindableCls.png" 
    );
    CompareProj(
        "/data/alice/acaluisi/alice/workdir/TrackJetQA/LHC22_pass4_highIR_sampling/full/Comparisons/tpcCrossedRowsOverFindableCls_0_1.png",
        "/data/alice/acaluisi/alice/workdir/TrackJetQA/LHC22_pass4_highIR_sampling/full/Comparisons/tpcCrossedRowsOverFindableCls_3_5.png", 
        "/data/alice/acaluisi/alice/workdir/TrackJetQA/LHC22_pass4_highIR_sampling/full/Comparisons/tpcCrossedRowsOverFindableCls_50_100.png",
        "/data/alice/acaluisi/alice/workdir/TrackJetQA/LHC22_pass4_highIR_sampling/full/Comparisons/tpcCrossedRowsOverFindableCls_100_200.png",       
        "TPC/tpcCrossedRowsOverFindableCls_proj.png" 
    );

    CompareImages(        
        "/data/alice/acaluisi/alice/workdir/TrackJetQA/LHC22_pass4_highIR_sampling/full/tight/TPC/tpcFractionSharedCls.png",
        "/data/alice/acaluisi/alice/workdir/TrackJetQA/LHC22_pass4_highIR_sampling/full/GlobalTracks/TPC/tpcFractionSharedCls.png",
        "/data/alice/acaluisi/alice/workdir/TrackJetQA/LHC22_pass4_highIR_sampling/full/loose/TPC/tpcFractionSharedCls.png",
        "TPC/tpcFractionSharedCls.png" 
    );
    CompareProj(
        "/data/alice/acaluisi/alice/workdir/TrackJetQA/LHC22_pass4_highIR_sampling/full/Comparisons/tpcFractionSharedCls_0_1.png",
        "/data/alice/acaluisi/alice/workdir/TrackJetQA/LHC22_pass4_highIR_sampling/full/Comparisons/tpcFractionSharedCls_3_5.png", 
        "/data/alice/acaluisi/alice/workdir/TrackJetQA/LHC22_pass4_highIR_sampling/full/Comparisons/tpcFractionSharedCls_50_100.png",
        "/data/alice/acaluisi/alice/workdir/TrackJetQA/LHC22_pass4_highIR_sampling/full/Comparisons/tpcFractionSharedCls_100_200.png",       
        "TPC/tpcFractionSharedCls_proj.png" 
    );

    CompareImages(        
        "/data/alice/acaluisi/alice/workdir/TrackJetQA/LHC22_pass4_highIR_sampling/full/tight/TPC/tpcNClsShared.png",
        "/data/alice/acaluisi/alice/workdir/TrackJetQA/LHC22_pass4_highIR_sampling/full/GlobalTracks/TPC/tpcNClsShared.png",
        "/data/alice/acaluisi/alice/workdir/TrackJetQA/LHC22_pass4_highIR_sampling/full/loose/TPC/tpcNClsShared.png",
        "TPC/tpcNClsShared.png" 
    );
    CompareProj(
        "/data/alice/acaluisi/alice/workdir/TrackJetQA/LHC22_pass4_highIR_sampling/full/Comparisons/tpcNClsShared_0_1.png",
        "/data/alice/acaluisi/alice/workdir/TrackJetQA/LHC22_pass4_highIR_sampling/full/Comparisons/tpcNClsShared_3_5.png", 
        "/data/alice/acaluisi/alice/workdir/TrackJetQA/LHC22_pass4_highIR_sampling/full/Comparisons/tpcNClsShared_50_100.png",
        "/data/alice/acaluisi/alice/workdir/TrackJetQA/LHC22_pass4_highIR_sampling/full/Comparisons/tpcNClsShared_100_200.png",       
        "TPC/tpcNClsShared_proj.png" 
    );

    // ITS

    CompareImages(        
        "/data/alice/acaluisi/alice/workdir/TrackJetQA/LHC22_pass4_highIR_sampling/full/tight/ITS/itsHits.png",
        "/data/alice/acaluisi/alice/workdir/TrackJetQA/LHC22_pass4_highIR_sampling/full/GlobalTracks/ITS/itsHits.png",
        "/data/alice/acaluisi/alice/workdir/TrackJetQA/LHC22_pass4_highIR_sampling/full/loose/ITS/itsHits.png",
        "ITS/itsHits.png" 
    );
    CompareProj(
        "/data/alice/acaluisi/alice/workdir/TrackJetQA/LHC22_pass4_highIR_sampling/full/Comparisons/itsHits_0_1.png",
        "/data/alice/acaluisi/alice/workdir/TrackJetQA/LHC22_pass4_highIR_sampling/full/Comparisons/itsHits_3_5.png", 
        "/data/alice/acaluisi/alice/workdir/TrackJetQA/LHC22_pass4_highIR_sampling/full/Comparisons/itsHits_50_100.png",
        "/data/alice/acaluisi/alice/workdir/TrackJetQA/LHC22_pass4_highIR_sampling/full/Comparisons/itsHits_100_200.png",       
        "ITS/itsHits_proj.png" 
    );

    CompareImages(        
        "/data/alice/acaluisi/alice/workdir/TrackJetQA/LHC22_pass4_highIR_sampling/full/tight/ITS/itsNCls.png",
        "/data/alice/acaluisi/alice/workdir/TrackJetQA/LHC22_pass4_highIR_sampling/full/GlobalTracks/ITS/itsNCls.png",
        "/data/alice/acaluisi/alice/workdir/TrackJetQA/LHC22_pass4_highIR_sampling/full/loose/ITS/itsNCls.png",
        "ITS/itsNCls.png" 
    );
    CompareProj(
        "/data/alice/acaluisi/alice/workdir/TrackJetQA/LHC22_pass4_highIR_sampling/full/Comparisons/itsNCls_0_1.png",
        "/data/alice/acaluisi/alice/workdir/TrackJetQA/LHC22_pass4_highIR_sampling/full/Comparisons/itsNCls_3_5.png", 
        "/data/alice/acaluisi/alice/workdir/TrackJetQA/LHC22_pass4_highIR_sampling/full/Comparisons/itsNCls_50_100.png",
        "/data/alice/acaluisi/alice/workdir/TrackJetQA/LHC22_pass4_highIR_sampling/full/Comparisons/itsNCls_100_200.png",       
        "ITS/itsNCls_proj.png" 
    );

    CompareImages(        
        "/data/alice/acaluisi/alice/workdir/TrackJetQA/LHC22_pass4_highIR_sampling/full/tight/ITS/itsChi2NCl.png",
        "/data/alice/acaluisi/alice/workdir/TrackJetQA/LHC22_pass4_highIR_sampling/full/GlobalTracks/ITS/itsChi2NCl.png",
        "/data/alice/acaluisi/alice/workdir/TrackJetQA/LHC22_pass4_highIR_sampling/full/loose/ITS/itsChi2NCl.png",
        "ITS/itsChi2NCl.png" 
    );
    CompareProj(
        "/data/alice/acaluisi/alice/workdir/TrackJetQA/LHC22_pass4_highIR_sampling/full/Comparisons/itsChi2NCl_0_1.png",
        "/data/alice/acaluisi/alice/workdir/TrackJetQA/LHC22_pass4_highIR_sampling/full/Comparisons/itsChi2NCl_3_5.png", 
        "/data/alice/acaluisi/alice/workdir/TrackJetQA/LHC22_pass4_highIR_sampling/full/Comparisons/itsChi2NCl_50_100.png",
        "/data/alice/acaluisi/alice/workdir/TrackJetQA/LHC22_pass4_highIR_sampling/full/Comparisons/itsChi2NCl_100_200.png",       
        "ITS/itsChi2NCl_proj.png" 
    );

    // Kine

    CompareEtaPhi(
        "/data/alice/acaluisi/alice/workdir/TrackJetQA/LHC22_pass4_highIR_sampling/full/GlobalTracks/Kine/etaphi_3_5.png",
        "/data/alice/acaluisi/alice/workdir/TrackJetQA/LHC22_pass4_highIR_sampling/full/GlobalTracks/Kine/etaphi_50_100.png", 
        "/data/alice/acaluisi/alice/workdir/TrackJetQA/LHC22_pass4_highIR_sampling/full/GlobalTracks/Kine/etaphi_100_200.png",
        "Kine/etaphi.png" 
    );

    // TrackPar

    CompareImages(        
        "/data/alice/acaluisi/alice/workdir/TrackJetQA/LHC22_pass4_highIR_sampling/full/tight/TrackPar/Sigma1Pt_Layers12.png",
        "/data/alice/acaluisi/alice/workdir/TrackJetQA/LHC22_pass4_highIR_sampling/full/GlobalTracks/TrackPar/Sigma1Pt_Layers12.png",
        "/data/alice/acaluisi/alice/workdir/TrackJetQA/LHC22_pass4_highIR_sampling/full/loose/TrackPar/Sigma1Pt_Layers12.png",
        "TrackPar/Sigma1Pt_Layers12.png" 
    );
    CompareProj3(
        "/data/alice/acaluisi/alice/workdir/TrackJetQA/LHC22_pass4_highIR_sampling/full/Comparisons/Sigma1Pt_Layers12_0_10.png",
        "/data/alice/acaluisi/alice/workdir/TrackJetQA/LHC22_pass4_highIR_sampling/full/Comparisons/Sigma1Pt_Layers12_10_80.png",
        "/data/alice/acaluisi/alice/workdir/TrackJetQA/LHC22_pass4_highIR_sampling/full/Comparisons/Sigma1Pt_Layers12_80_200.png",
        "TrackPar/Sigma1Pt_Layers12_proj.png" 
    );

    CompareImages(        
        "/data/alice/acaluisi/alice/workdir/TrackJetQA/LHC22_pass4_highIR_sampling/full/tight/TrackPar/Sigma1Pt_Layers456.png",
        "/data/alice/acaluisi/alice/workdir/TrackJetQA/LHC22_pass4_highIR_sampling/full/GlobalTracks/TrackPar/Sigma1Pt_Layers456.png",
        "/data/alice/acaluisi/alice/workdir/TrackJetQA/LHC22_pass4_highIR_sampling/full/loose/TrackPar/Sigma1Pt_Layers456.png",
        "TrackPar/Sigma1Pt_Layers456.png" 
    );
    CompareProj3(
        "/data/alice/acaluisi/alice/workdir/TrackJetQA/LHC22_pass4_highIR_sampling/full/Comparisons/Sigma1Pt_Layers456_0_10.png",
        "/data/alice/acaluisi/alice/workdir/TrackJetQA/LHC22_pass4_highIR_sampling/full/Comparisons/Sigma1Pt_Layers456_10_80.png",
        "/data/alice/acaluisi/alice/workdir/TrackJetQA/LHC22_pass4_highIR_sampling/full/Comparisons/Sigma1Pt_Layers456_80_200.png",
        "TrackPar/Sigma1Pt_Layers456_proj.png" 
    );

    CompareImages(        
        "/data/alice/acaluisi/alice/workdir/TrackJetQA/LHC22_pass4_highIR_sampling/full/tight/TrackPar/Sigma1Pt_hasTRD.png",
        "/data/alice/acaluisi/alice/workdir/TrackJetQA/LHC22_pass4_highIR_sampling/full/GlobalTracks/TrackPar/Sigma1Pt_hasTRD.png",
        "/data/alice/acaluisi/alice/workdir/TrackJetQA/LHC22_pass4_highIR_sampling/full/loose/TrackPar/Sigma1Pt_hasTRD.png",
        "TrackPar/Sigma1Pt_hasTRD.png" 
    );

    CompareImages(        
        "/data/alice/acaluisi/alice/workdir/TrackJetQA/LHC22_pass4_highIR_sampling/full/tight/TrackPar/Sigma1Pt.png",
        "/data/alice/acaluisi/alice/workdir/TrackJetQA/LHC22_pass4_highIR_sampling/full/GlobalTracks/TrackPar/Sigma1Pt.png",
        "/data/alice/acaluisi/alice/workdir/TrackJetQA/LHC22_pass4_highIR_sampling/full/loose/TrackPar/Sigma1Pt.png",
        "TrackPar/Sigma1Pt.png" 
    );

    CompareImages(        
        "/data/alice/acaluisi/alice/workdir/TrackJetQA/LHC22_pass4_highIR_sampling/full/tight/TrackPar/dcaXY.png",
        "/data/alice/acaluisi/alice/workdir/TrackJetQA/LHC22_pass4_highIR_sampling/full/GlobalTracks/TrackPar/dcaXY.png",
        "/data/alice/acaluisi/alice/workdir/TrackJetQA/LHC22_pass4_highIR_sampling/full/loose/TrackPar/dcaXY.png",
        "TrackPar/dcaXY.png" 
    );

    CompareImages(        
        "/data/alice/acaluisi/alice/workdir/TrackJetQA/LHC22_pass4_highIR_sampling/full/tight/TrackPar/dcaZ.png",
        "/data/alice/acaluisi/alice/workdir/TrackJetQA/LHC22_pass4_highIR_sampling/full/GlobalTracks/TrackPar/dcaZ.png",
        "/data/alice/acaluisi/alice/workdir/TrackJetQA/LHC22_pass4_highIR_sampling/full/loose/TrackPar/dcaZ.png",
        "TrackPar/dcaZ.png" 
    );

    // EventProp 

    CompareImages(        
        "/data/alice/acaluisi/alice/workdir/TrackJetQA/LHC22_pass4_highIR_sampling/full/GlobalTracks/EventProp/collisionVtxZnoSel.png",
        "/data/alice/acaluisi/alice/workdir/TrackJetQA/LHC22_pass4_highIR_sampling/full/GlobalTracks/EventProp/collisionVtxZSel8.png",
        "/data/alice/acaluisi/alice/workdir/TrackJetQA/LHC22_pass4_highIR_sampling/full/GlobalTracks/EventProp/collisionVtxZ.png",
        "EventProp/collisionVtxZ.png" 
    );

}
