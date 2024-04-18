# Welcome to my plotting macro repository ✨
Here you can find all the plotting macros I wrote and used for obtaining the results presented in my Experimental Physics Master Thesis (link [here](https://studenttheses.uu.nl/handle/20.500.12932/46251)):
- **LundPlots.C**: given the results obtained after having run the [jetLundDeclustering.cxx task](https://github.com/AliceO2Group/O2Physics/blob/master/PWGJE/Tasks/jetLundReclustering.cxx), it plots the Primary Lund plane in kT and z and its projections over the X and Y axes
- **QAplots.C**: given the results obtained after having run the [trackJetqa.cxx task](https://github.com/alicecaluisi/O2Physics/blob/master/PWGJE/Tasks/trackJetqa.cxx), it converts the quality assurance histograms from THnSparse to TH1F, TH1D and TH2D histograms and plots their projections for GlobalTracks, loose and tight cuts
- **QAplots_pT.C**: given the results obtained after having run the [trackJetqa.cxx task](https://github.com/alicecaluisi/O2Physics/blob/master/PWGJE/Tasks/trackJetqa.cxx), it plots the comparison of the pT histograms for GlobalTracks, loose and tight cuts
- **QAplay.C**: given the results obtained by QAplots.C, it plots the comparison of the projections of the quality assurance histograms for GlobalTracks, loose and tight cuts
- **QA_plot_comparisons.C**: given the results obtained by QAplots.C, it plots the comparison of the quality assurance histograms for GlobalTracks, loose and tight cuts

These tasks can be run inside the O2Physics environment by running: root *macro_name*
