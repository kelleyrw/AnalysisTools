{
    // load the AnalysisTools for interactive use 
    const TString cmssw_base = gSystem->Getenv("CMSSW_BASE");
    const bool cmssw = cmssw_base.Length() > 0;
    if (cmssw)
    {
        const TString scram_arch = gSystem->Getenv("SCRAM_ARCH");
        const TString cmssw_lib  = Form("%s/lib/%s", cmssw_base.Data(), scram_arch.Data());
        const TString dy_suffix  = (scram_arch.Contains("osx") ? "dylib" : "so");

        gSystem->AddIncludePath(Form("-I%s/AnalysisTools/RootTools/interface", cmssw_base.Data()));
        gSystem->Load(Form("%s/libAnalysisToolsRootTools.%s", cmssw_lib.Data(), dy_suffix.Data()));

        gSystem->AddIncludePath(Form("-I%s/AnalysisTools/LanguageTools/interface", cmssw_base.Data()));
        gSystem->Load(Form("%s/libAnalysisToolsLanguageTools.%s", cmssw_lib.Data(), dy_suffix.Data()));
    }
}
