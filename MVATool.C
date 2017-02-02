#define MVATool_cxx

#include <iostream>
#include <unordered_map>

#include <boost/algorithm/string/replace.hpp>
#include <boost/numeric/conversion/cast.hpp>

#include <TCut.h>

#include <TMVA/TMVAGui.h>
#include <TMVA/Factory.h>
#include <TMVA/Tools.h>

#include "MVATool.h"


MVATool::MVATool(const bool doCtrlReg)
    : varList{
        // "mTW",
        // "wQuark1Pt",
        // "wQuark1Eta",
        // "wQuark1Phi",
        // "wQuark2Pt",
        // "wQuark2Eta",
        // "wQuark2Phi",
        "wPairMass",
        "wPairPt",
        // "wPairEta",
        // "wPairPhi",
        "met",
        // "nJets",
        "leadJetPt",
        // "leadJetPhi",
        // "leadJetEta",
        "leadJetbTag",
        "secJetPt",
        // "secJetPhi",
        // "secJetEta",
        // "secJetbTag",
        "thirdJetPt",
        // "thirdJetPhi",
        // "thirdJetEta",
        // "thirdJetbTag",
        "fourthJetPt",
        // "fourthJetPhi",
        // "fourthJetEta",
        // "fourthJetbTag",
        // "nBjets",
        // "bTagDisc",
        // "lep1Pt",
        // "lep1Eta",
        // "lep1Phi",
        // "lep1RelIso",
        // "lep1D0",
        // "lep2Pt",
        // "lep2Eta",
        // "lep2Phi",
        // "lep2RelIso",
        // "lep2D0",
        // "lepMass",
        // "lepPt",
        // "lepEta",
        // "lepPhi",
        // "zMass",
        // "zPt",
        // "zEta",
        // "zPhi",
        "topMass",
        "topPt",
        // "topEta",
        // "topPhi",
        "jjdelR",
        // "jjdelPhi",
        "wwdelR",
        // "wwdelPhi",
        // "zLepdelR",
        // "zLepdelPhi",
        // "zl1Quark1DelR",
        // "zl1Quark1DelPhi",
        // "zl1Quark2DelR",
        // "zl1Quark2DelPhi",
        // "zl2Quark1DelR",
        // "zl2Quark1DelPhi",
        // "zl2Quark2DelR",
        // "zl2Quark2DelPhi",
        // "zlb1DelR",
        // "zlb1DelPhi",
        "zlb2DelR",
        // "zlb2DelPhi",
        // "lepHt",
        "wQuarkHt",
        // "totPt",
        // "totEta",
        // "totPtVec",
        "totVecM",
        ////  "Channel",
        // "totPt2Jet",
        // "wzdelR",
        // "wzdelPhi",
        // "zQuark1DelR",
        // "zQuark1DelPhi",
        // "zQuark2DelR",
        // "zQuark2DelPhi",
        // "zTopDelR",
        // "zTopDelPhi",
        // "zl1TopDelR",
        // "zl1TopDelPhi",
        // "zl2TopDelR",
        // "zl2TopDelPhi",
        // "zjminR",
        // "zjminPhi",
        // "totHt",
        // "jetHt",
        // "totHtOverPt",
        "wTopDelR"
        // "wTopDelPhi",
        // "w1TopDelR",
        // "w1TopDelPhi",
        // "w2TopDelR",
        // "w2TopDelPhi"
    }
    , samplelist{
        "DataMu",
        "DataEG",
        "tZq",
        // "THQ", // Empty tree - background totally cut
        "TTZ",
        "TTW",
        "TT",
        // "DYToLL_M10To50", // Empty tree - background totally cut
        "DYToLL_M50",
        // "Wjets", // Empty tree - background totally cut
        // "WW", // Empty tree - background totally cut
        // "WZ", // Empty tree - background totally cut for sig/ctrl, all has 1 event
        "ZZ",
        // "TsChan", // Empty tree - background totally cut
        // "TtChan", // Empty tree - background totally cut
        "TbartChan",
        "TtW",
        "TbartW"
    }
    , systlist{
        "",
        "__trig__plus",
        "__trig__minus",
        "__jer__plus",
        "__jer__minus",
        "__jes__plus",
        "__jes__minus",
        "__pileup__plus",
        "__pileup__minus",
        "__bTag__plus",
        "__bTag__minus",
        "__met__plus",
        "__met__minus",
        "__pdf__plus",
        "__pdf__minus",
        "__ME_PS__plus",
        "__ME_PS__minus",
    }
    , regList{[=] () -> vector<TString> {if (doCtrlReg) return {"sig_", "ctrl_"};
                                         else return{""};}()}
{
    for (size_t i{0}; i < varList.size(); i++)
    {
        varMap[varList[i]] = i;
    }
}


MVATool::MVATool(const vector<TString>& varList_,
        const vector<TString>& sampleList_,
        const vector<TString>& systList_,
        const vector<TString>& regList_)
    : varList{varList_}
    , samplelist{sampleList_}
    , systlist{systList_}
    , regList{regList_}
{}


MVATool::~MVATool()
{}


void MVATool::doTraining(const TString& channel, const TString& inDir,
        const bool sigMode) const
{
    // Load the library
    TMVA::Tools::Instance();

    const TString outfileName{("trainingBDT_" + channel + "_tZq_.root")};
    TFile* const outputFile{TFile::Open(outfileName, "RECREATE")};

    TMVA::Factory factory{"BDT_trainning_" + channel + "_tzq", outputFile,
            "!V:!Silent:Color:DrawProgressBar:Transformations=I:AnalysisType=Classification"};

    TFile* const input_sig{TFile::Open(inDir + "/histofile_tZq.root")};
    TFile* const input_TTZ{TFile::Open(inDir + "/histofile_TTZ.root")};
    TFile* const input_TTW{TFile::Open(inDir + "/histofile_TTW.root")};
    // TFile* const input_THQ{TFile::Open(inDir + "/histofile_THQ.root")}; // Empty tree - background totally cut
    TFile* const input_TT{TFile::Open(inDir + "/histofile_TT.root")};
    // TFile* const input_WW{TFile::Open(inDir + "/histofile_WW.root")}; // Empty tree - background totally cut
    // TFile* const input_WZ{TFile::Open(inDir + "/histofile_WZ.root")};
    TFile* const input_ZZ{TFile::Open(inDir + "/histofile_ZZ.root")};
    // TFile* const input_TtChan{TFile::Open(inDir + "/histofile_TtChan.root")}; // Empty tree - background totally cut
    TFile* const input_TbartChan{TFile::Open(inDir + "/histofile_TbartChan.root")};
    // TFile* const input_TsChan{TFile::Open(inDir + "/histofile_TsChan.root")}; // Empty tree - background totally cut
    TFile* const input_TtW{TFile::Open(inDir + "/histofile_TtW.root")};
    TFile* const input_TbartW{TFile::Open(inDir + "/histofile_TbartW.root")};
    TFile* const input_DY50{TFile::Open(inDir + "/histofile_DYToLL_M50.root")};
    // TFile* const input_DY10To50{TFile::Open(inDir + "/histofile_DYToLL_M10To50.root")}; // Empty tree - background totally cut

    const TString treePost{regList.size() < 2 ? ""
                                    : sigMode ? "sig_"
                                    : "ctrl_"};

    TTree* const signal              {dynamic_cast<TTree*>(input_sig->Get("Ttree_" + treePost + "tZq"))};
    TTree* const background_TTZ      {dynamic_cast<TTree*>(input_TTZ->Get("Ttree_" + treePost + "TTZ"))};
    TTree* const background_TTW      {dynamic_cast<TTree*>(input_TTW->Get("Ttree_" + treePost + "TTW"))};
    // TTree* const background_THQ     {dynamic_cast<TTree*>(input_TTW->Get("Ttree_" + treePost + "THQ"))};
    TTree* const background_TT       {dynamic_cast<TTree*>(input_TT->Get("Ttree_" + treePost + "TT"))};
    // TTree* const background_WW       {dynamic_cast<TTree*>(input_WW->Get("Ttree_" + treePost + "WW"))};
    // TTree* const background_WZ       {dynamic_cast<TTree*>(input_WZ->Get("Ttree_" + treePost + "WZ"))};
    TTree* const background_ZZ       {dynamic_cast<TTree*>(input_ZZ->Get("Ttree_" + treePost + "ZZ"))};
    // TTree* const background_TtChan   {dynamic_cast<TTree*>(input_TtChan->Get("Ttree_" + treePost + "TtChan"))};
    TTree* const background_TbartChan{dynamic_cast<TTree*>(input_TbartChan->Get("Ttree_" + treePost + "TbartChan"))};
    // TTree* const background_TsChan   {dynamic_cast<TTree*>(input_TsChan->Get("Ttree_" + treePost + "TsChan"))};
    TTree* const background_TtW      {dynamic_cast<TTree*>(input_TtW->Get("Ttree_" + treePost + "TtW"))};
    TTree* const background_TbartW   {dynamic_cast<TTree*>(input_TbartW->Get("Ttree_" + treePost + "TbartW"))};
    TTree* const background_DY50     {dynamic_cast<TTree*>(input_DY50->Get("Ttree_" + treePost + "DYToLL_M50"))};
    // TTree* const background_DY10To50 {dynamic_cast<TTree*>(input_DY10To50->Get("Ttree_" + treePost + "DYToLL_M10To50"))};

    factory.AddSignalTree(signal, 1.);
    // factory.AddBackgroundTree(background_TTZ, 1.);
    // factory.AddBackgroundTree(background_TTW, 1.);
    factory.AddBackgroundTree(background_TT, 1.);
    // factory.AddBackgroundTree(background_WZ, 1.);
    factory.AddBackgroundTree(background_ZZ, 1.);
    // factory.AddBackgroundTree(background_TtChan, 1.);
    factory.AddBackgroundTree(background_TbartChan, 1.);
    // factory.AddBackgroundTree(background_TsChan, 1.);
    factory.AddBackgroundTree(background_TtW, 1.);
    // factory.AddBackgroundTree(background_TbartW, 1.);
    factory.AddBackgroundTree(background_DY50, 1.);
    // factory.AddBackgroundTree(background_DY10To50, 1. );

    for (const auto& var: varList)
    {
        factory.AddVariable(var, 'F');
    }

    factory.SetSignalWeightExpression    ("EvtWeight");
    factory.SetBackgroundWeightExpression("EvtWeight");

    // Apply additional cuts on the signal and background samples (can be different)
    TCut mycuts{""}; // for example: TCut mycuts = "abs(var1)<0.5 && abs(var2-0.5)<1";
    const TCut mycutb{""}; // for example: TCut mycutb = "abs(var1)<0.5";

    if (channel != "all")
    {
        if (channel == "mumu")
        {
            mycuts = "Channel == 0";
        }
        else if (channel == "ee")
        {
            mycuts = "Channel == 1";
        }
        else
        {
            cout << "WARNING wrong channel name while training " << endl;
        }
    }

    factory.PrepareTrainingAndTestTree(mycuts, mycutb,
            "nTrain_Signal=0:nTrain_Background=0:SplitMode=Random:NormMode=NumEvents:!V");

    factory.BookMethod(TMVA::Types::kBDT, "BDT",
            "!H:!V:NTrees=100:nEventsMin=100:MaxDepth=5:BoostType=AdaBoost:SeparationType=GiniIndex:nCuts=20:PruneMethod=NoPruning");

    // Train MVAs using the set of training events
    factory.TrainAllMethods();

    // Evaluate all MVAs using the set of test events
    factory.TestAllMethods();

    // Evaluate and compare performance of all configured MVAs
    factory.EvaluateAllMethods();

    // Save the output
    outputFile->Close();

    // Close input files
    input_sig->Close();
    input_TTZ->Close();
    input_TTW->Close();
    // input_THQ->Close();
    input_TT->Close();
    // input_WW->Close();
    // input_WZ->Close();
    input_ZZ->Close();
    // input_TtChan->Close();
    input_TbartChan->Close();
    // input_TsChan->Close();
    input_TtW->Close();
    input_TbartW->Close();
    input_DY50->Close();
    // input_DY10To50->Close();

    cout << "==> Wrote root file: " << outputFile->GetName() << endl;
    cout << "==> TMVAClassification is done!" << endl;

    // Launch the GUI for the root macros
    if (!gROOT->IsBatch())
    {
        TMVA::TMVAGui(outfileName);
    }
}


void MVATool::doReading(const float bdtcut, const TString& channel, 
        const TString& inDir, const TString& outDir, const bool usePseudoData)
{
    // This loads the library
    TMVA::Tools::Instance();

    //create the BDT reader
    reader = new TMVA::Reader{"!Color:!Silent"};
    vector<float> treevars(varList.size(), 0);

    for (size_t i{0}; i < varList.size(); i++)
    {
        reader->AddVariable(varList.at(i),  &(treevars.at(i)));
    }

    reader->BookMVA("BDT", ("weights/BDT_trainning_" + channel + "_tzq_BDT.weights.xml"));

    for (const auto& sample: samplelist)
    {
        //cout << sample << endl;
        TFile* const inFile{new TFile{(inDir + "histofile_" + sample + ".root"), "read"}};
        TFile* const outFile{new TFile{(outDir + "output_" + channel + "_" + sample+".root"), "recreate"}};
        outFileMap[sample] = outFile;

        //loop over systematics
        cout << "processing sample " << sample << endl;

        for (const auto& syst: systlist)
        {
            for (const auto& region: regList)
            {
                if ((sample == "DataMu" || sample == "DataEG") && syst != "")
                {
                    continue;
                }

                createHisto(region+sample + syst, channel);
                loopInSample(inFile, region + sample + syst, treevars, bdtcut, channel);
                writeHisto(sample, syst, region);
            }
        }

        inFile->Close();
        outFile->Close();
    }
}


void MVATool::loopInSample(TFile* const input, const TString& sample,
        vector<float>& treevars, const float bdtcut, const TString& channel)
{
    input->cd();
    TTree* const theTree{dynamic_cast<TTree*>(input->Get("Ttree_"+sample))};

    for (size_t ivar{0}; ivar < varList.size(); ivar++)
    {
        theTree->SetBranchAddress(varList[ivar], &(treevars[ivar]));
    }

    float weight{0};
    int eventChannel{-1};
    float mtwValue{-1};
    int selChannel{-1};

    theTree->SetBranchAddress("EvtWeight", &weight);
    theTree->SetBranchAddress("Channel", &eventChannel);
    theTree->SetBranchAddress("mTW", &mtwValue);

    if (channel == "mumu")
    {
        selChannel = 0;
    }
    else if (channel == "ee" )
    {
        selChannel = 1;
    }

    if (theTree == nullptr)
    {
        cout << "no TTree found with name " << "Ttree_" + sample << endl;
    }

    for (long long int i{0}; i < theTree->GetEntries(); i++)
    {
        constexpr double sf_local{1};
        theTree->GetEntry(i);

        if (channel != "all" && eventChannel != selChannel)
        {
            continue;
        }

        if (isnan(weight))
        {
            cout << "NaN weight encountered!" << " : " << i << endl;
        }
        weight *= sf_local;

        const double mvaValue = reader->EvaluateMVA("BDT");
        if (mvaValue > bdtcut)
        {
            continue;
        }

        fillHisto(sample, treevars, mvaValue, mtwValue, weight);
    }
}


void MVATool::createHisto(const TString& sample, const TString& channel)
{
    static const unordered_map<string, unordered_map<string, double>>
        histProperties
    {
        {"bTagDisc",       {{"nbins", 10}, {"xmin",  0.6}, {"xmax", 1}}},
        {"fourthJetEta",   {{"nbins", 20}, {"xmin", -5  }, {"xmax", 5.}}},
        {"fourthJetPhi",   {{"nbins", 20}, {"xmin", -3.2}, {"xmax", 3.2}}},
        {"fourthJetPt",    {{"nbins", 15}, {"xmin",  0  }, {"xmax", 300}}},
        {"fourthJetbTag",  {{"nbins", 10}, {"xmin",  0  }, {"xmax", 1}}},
        {"jetHt",          {{"nbins", 20}, {"xmin",  0  }, {"xmax", 500}}},
        {"jjdelPhi",       {{"nbins", 10}, {"xmin",  0  }, {"xmax", 3.2}}},
        {"jjdelR",         {{"nbins", 20}, {"xmin",  0  }, {"xmax", 6}}},
        {"leadJetEta",     {{"nbins", 10}, {"xmin", -2.5}, {"xmax", 2.5}}},
        {"leadJetPhi",     {{"nbins", 20}, {"xmin", -3.2}, {"xmax", 3.2}}},
        {"leadJetPt",      {{"nbins", 20}, {"xmin",  0  }, {"xmax", 500}}},
        {"leadJetbTag",    {{"nbins", 10}, {"xmin",  0  }, {"xmax", 1}}},
        {"lep1D0",         {{"nbins", 10}, {"xmin",  0  }, {"xmax", 0.1}}},
        {"lep1Eta",        {{"nbins", 20}, {"xmin", -5. }, {"xmax", 5.}}},
        {"lep1Phi",        {{"nbins", 20}, {"xmin", -3.2}, {"xmax", 3.2}}},
        {"lep1Pt",         {{"nbins", 20}, {"xmin",  0  }, {"xmax", 500}}},
        {"lep1RelIso",     {{"nbins", 50}, {"xmin",  0  }, {"xmax", 0.2}}},
        {"lep2D0",         {{"nbins", 10}, {"xmin",  0  }, {"xmax", 0.1}}},
        {"lep2Eta",        {{"nbins", 20}, {"xmin", -5  }, {"xmax", 5.}}},
        {"lep2Phi",        {{"nbins", 20}, {"xmin", -3.2}, {"xmax", 3.2}}},
        {"lep2Pt",         {{"nbins", 20}, {"xmin",  0  }, {"xmax", 500}}},
        {"lep2RelIso",     {{"nbins", 50}, {"xmin",  0  }, {"xmax", 0.2}}},
        {"lepEta",         {{"nbins", 20}, {"xmin", -5  }, {"xmax", 5.}}},
        {"lepHt",          {{"nbins", 15}, {"xmin",  0  }, {"xmax", 4}}},
        {"lepMass",        {{"nbins", 20}, {"xmin",  0  }, {"xmax", 500}}},
        {"lepPhi",         {{"nbins", 20}, {"xmin", -3.2}, {"xmax", 3.2}}},
        {"lepPt",          {{"nbins", 20}, {"xmin",  0  }, {"xmax", 500}}},
        {"mTW",            {{"nbins", 10}, {"xmin",  0  }, {"xmax", 200}}},
        {"met",            {{"nbins", 10}, {"xmin",  0  }, {"xmax", 200}}},
        {"nBjets",         {{"nbins",  4}, {"xmin", -0.5}, {"xmax", 3.5}}},
        {"nJets",          {{"nbins",  8}, {"xmin",  0.5}, {"xmax", 8.5}}},
        {"secJetEta",      {{"nbins", 20}, {"xmin", -5  }, {"xmax", 5.}}},
        {"secJetPhi",      {{"nbins", 20}, {"xmin", -3.2}, {"xmax", 3.2}}},
        {"secJetPt",       {{"nbins", 15}, {"xmin",  0  }, {"xmax", 300}}},
        {"secJetbTag",     {{"nbins", 10}, {"xmin",  0  }, {"xmax", 1}}},
        {"thirdJetEta",    {{"nbins", 20}, {"xmin", -5  }, {"xmax", 5.}}},
        {"thirdJetPhi",    {{"nbins", 20}, {"xmin", -3.2}, {"xmax", 3.2}}},
        {"thirdJetPt",     {{"nbins", 15}, {"xmin",  0  }, {"xmax", 300}}},
        {"thirdJetbTag",   {{"nbins", 10}, {"xmin",  0  }, {"xmax", 1}}},
        {"topEta",         {{"nbins", 10}, {"xmin", -2.5}, {"xmax", 2.5}}},
        {"topMass",        {{"nbins", 20}, {"xmin",  0  }, {"xmax", 500}}},
        {"topPhi",         {{"nbins", 20}, {"xmin", -3.2}, {"xmax", 3.2}}},
        {"topPt",          {{"nbins", 20}, {"xmin",  0  }, {"xmax", 500}}},
        {"totEta",         {{"nbins", 10}, {"xmin", -2.5}, {"xmax", 2.5}}},
        {"totHt",          {{"nbins", 20}, {"xmin",150  }, {"xmax", 1500}}},
        {"totHtOverPt",    {{"nbins", 20}, {"xmin",  0  }, {"xmax", 4}}},
        {"totPt",          {{"nbins", 10}, {"xmin",  0  }, {"xmax", 250}}},
        {"totPt2Jet",      {{"nbins", 20}, {"xmin",  0  }, {"xmax", 500}}},
        {"totPtVec",       {{"nbins", 20}, {"xmin",  0  }, {"xmax", 200}}},
        {"totVecM",        {{"nbins", 20}, {"xmin",  0  }, {"xmax", 300}}},
        {"w1TopDelPhi",    {{"nbins", 20}, {"xmin",  0  }, {"xmax", 3.2}}},
        {"w1TopDelR",      {{"nbins", 20}, {"xmin",  0  }, {"xmax", 7}}},
        {"w2TopDelPhi",    {{"nbins", 20}, {"xmin",  0  }, {"xmax", 3.2}}},
        {"w2TopDelR",      {{"nbins", 20}, {"xmin",  0  }, {"xmax", 7}}},
        {"wPairEta",       {{"nbins", 20}, {"xmin", -5  }, {"xmax", 5.}}},
        {"wPairMass",      {{"nbins", 10}, {"xmin",  0  }, {"xmax", 200}}},
        {"wPairPhi",       {{"nbins", 20}, {"xmin", -3.2}, {"xmax", 3.2}}},
        {"wPairPt",        {{"nbins", 20}, {"xmin",  0  }, {"xmax", 500}}},
        {"wQuark1Eta",     {{"nbins", 20}, {"xmin", -5  }, {"xmax", 5.}}},
        {"wQuark1Phi",     {{"nbins", 20}, {"xmin", -3.2}, {"xmax", 3.2}}},
        {"wQuark1Pt",      {{"nbins", 20}, {"xmin",  0  }, {"xmax", 500}}},
        {"wQuark2Eta",     {{"nbins", 20}, {"xmin", -5  }, {"xmax", 5.}}},
        {"wQuark2Phi",     {{"nbins", 20}, {"xmin", -3.2}, {"xmax", 3.2}}},
        {"wQuark2Pt",      {{"nbins", 20}, {"xmin",  0  }, {"xmax", 500}}},
        {"wQuarkHt",       {{"nbins", 15}, {"xmin",  0  }, {"xmax", 150}}},
        {"wTopDelPhi",     {{"nbins", 20}, {"xmin",  0  }, {"xmax", 3.2}}},
        {"wTopDelR",       {{"nbins", 20}, {"xmin",  0  }, {"xmax", 7}}},
        {"wwdelPhi",       {{"nbins", 10}, {"xmin",  0  }, {"xmax", 3.2}}},
        {"wwdelR",         {{"nbins", 20}, {"xmin",  0  }, {"xmax", 6}}},
        {"wzdelPhi",       {{"nbins", 20}, {"xmin",  0  }, {"xmax", 3.2}}},
        {"wzdelR",         {{"nbins", 20}, {"xmin",  0  }, {"xmax", 7}}},
        {"zEta",           {{"nbins", 10}, {"xmin", -2.5}, {"xmax", 2.5}}},
        {"zLepdelPhi",     {{"nbins", 20}, {"xmin", -4  }, {"xmax", 4}}},
        {"zLepdelR",       {{"nbins", 15}, {"xmin",  0  }, {"xmax", 6}}},
        {"zMass",          {{"nbins", 20}, {"xmin",  0  }, {"xmax", 500}}},
        {"zPhi",           {{"nbins", 20}, {"xmin", -3.2}, {"xmax", 3.2}}},
        {"zPt",            {{"nbins", 20}, {"xmin",  0  }, {"xmax", 500}}},
        {"zQuark1DelPhi",  {{"nbins", 20}, {"xmin", -4  }, {"xmax", 4}}},
        {"zQuark1DelR",    {{"nbins", 15}, {"xmin",  0  }, {"xmax", 6}}},
        {"zQuark2DelPhi",  {{"nbins", 20}, {"xmin", -4  }, {"xmax", 4}}},
        {"zQuark2DelR",    {{"nbins", 15}, {"xmin",  0  }, {"xmax", 6}}},
        {"zTopDelPhi",     {{"nbins", 20}, {"xmin", -4  }, {"xmax", 4}}},
        {"zTopDelR",       {{"nbins", 15}, {"xmin",  0  }, {"xmax", 6}}},
        {"zjminPhi",       {{"nbins", 15}, {"xmin",  0  }, {"xmax", 3.2}}},
        {"zjminR",         {{"nbins", 15}, {"xmin",  0  }, {"xmax", 3.5}}},
        {"zl1Quark1DelPhi",{{"nbins", 20}, {"xmin", -4  }, {"xmax", 4}}},
        {"zl1Quark1DelR",  {{"nbins", 15}, {"xmin",  0  }, {"xmax", 6}}},
        {"zl1Quark2DelPhi",{{"nbins", 20}, {"xmin", -4  }, {"xmax", 4}}},
        {"zl1Quark2DelR",  {{"nbins", 15}, {"xmin",  0  }, {"xmax", 6}}},
        {"zl1TopDelPhi",   {{"nbins", 20}, {"xmin", -4  }, {"xmax", 4}}},
        {"zl1TopDelR",     {{"nbins", 15}, {"xmin",  0  }, {"xmax", 6}}},
        {"zl2Quark1DelPhi",{{"nbins", 20}, {"xmin", -4  }, {"xmax", 4}}},
        {"zl2Quark1DelR",  {{"nbins", 15}, {"xmin",  0  }, {"xmax", 6}}},
        {"zl2Quark2DelPhi",{{"nbins", 20}, {"xmin", -4  }, {"xmax", 4}}},
        {"zl2Quark2DelR",  {{"nbins", 15}, {"xmin",  0  }, {"xmax", 6}}},
        {"zl2TopDelPhi",   {{"nbins", 20}, {"xmin", -4  }, {"xmax", 4}}},
        {"zl2TopDelR",     {{"nbins", 15}, {"xmin",  0  }, {"xmax", 6}}},
        {"zlb1DelPhi",     {{"nbins", 20}, {"xmin", -4  }, {"xmax", 4}}},
        {"zlb1DelR",       {{"nbins", 15}, {"xmin",  0  }, {"xmax", 6}}},
        {"zlb2DelPhi",     {{"nbins", 20}, {"xmin", -4  }, {"xmax", 4}}},
        {"zlb2DelR",       {{"nbins", 15}, {"xmin",  0  }, {"xmax", 6}}}
    };

    vector<TH1F*> histovect;
    for (size_t j{0}; j < varList.size(); j++)
    {
        int nbins;
        double xmin;
        double xmax;

        if (histProperties.find(varList[j].Data()) != histProperties.end())
        {
            nbins = histProperties.at(varList[j].Data()).at("nbins");
            xmin = histProperties.at(varList[j].Data()).at("xmin");
            xmax = histProperties.at(varList[j].Data()).at("xmax");
        }
        else
        {
            cout << "warning : no TH1F definition for variable "
                << varList[j] << endl;

            nbins = 1;
            xmin = -1000;
            xmax = 1000;
        }

        TH1F* const histo{new TH1F{(varList[j]+"_"+channel+"__"+sample),
            (varList[j]+"_"+channel+"__"+sample), nbins, xmin, xmax}};
        histo->Sumw2();
        histovect.emplace_back(histo);
    }

    TH1F* const histomTW{new TH1F{("mTW_"+channel+"__"+sample),
        ("mTW_"+channel+"__"+sample), 15,0.,200.}};
    histomTW->Sumw2();
    histovect.emplace_back(histomTW);

    TH1F* const histo{new TH1F{("MVA_"+channel+"__"+sample),
        ("MVA_"+channel+"__"+sample),  20, -1., 1.}};
    histo->Sumw2();
    histovect.emplace_back(histo);

    histMap[sample] = histovect;
}


void MVATool::fillHisto(const TString& sample, vector<float>& vars,
        const double mva, const double mtw, const double weight)
{
   vector<TH1F*> histovect{histMap[sample]};

   for (size_t i{0}; i<  varList.size(); i++)
   {
       histovect[i]->Fill(vars[i], weight);
   }

   histovect[varList.size()]->Fill(mtw,weight);
   histovect.back()->Fill(mva, weight);
}


void MVATool::scaleHisto(const TString& sample, const double sf)
{
  vector<TH1F*> histovect{histMap[sample]};
  for (size_t i{0}; i < varList.size() + 1; i++)
  {
      histovect[i]->Scale(sf);
  }
}


void MVATool::writeHisto(const TString& sample, const TString& syst,
        const TString& reg)
{
    if (outFileMap[sample] == nullptr)
    {
        cout << "no output file for sample " << sample << endl;
    }

    outFileMap[sample]->cd();
    vector<TH1F*> histovect{histMap[reg+sample+syst]};

    for (const auto& hist: histovect)
    {
        if (hist == nullptr)
        {
            cout << "no histogram existing for " << reg+sample+syst << endl;
        }
        if (reg != "")
        {
            string name = hist->GetName();
            boost::replace_last(name, reg.Data(), "");
            hist->SetName(name.c_str());
        }

        hist->Write();
    }
}


void MVATool::makePseudoDataMVA(const TString& inDir, const TString& channel,
        const bool useData) const
{
    TRandom3 rand{0}; //Randomization

    TFile file{inDir + "output_merged_" + channel + ".root", "UPDATE"};

    cout << "\n--- GENERATION OF PSEUDODATA IN " << file.GetName() << " ! ---\n" << endl;

    TH1F* h_sum{nullptr};
    TH1F* h_tmp{nullptr};

    for (const auto& sample: samplelist)
    {
        if (!useData && sample.Contains("Data")) //From MC only
        {
            continue;
        }
        if (useData && (
                    sample.Contains("DYToLL_M50")
                    || sample.Contains("TT")
                    || sample.Contains("TTW")
                    || sample.Contains("TTZ")
                    || sample.Contains("ZZ")
                    || sample.Contains("WZ")
                    || sample.Contains("WW")
                    || sample.Contains("tZq")
                    || sample.Contains("TbartChan")
                    || sample.Contains("TtW")
                    || sample.Contains("TbartW"))) //From Data only
        {
            continue;
        }

        h_tmp = nullptr;
        const TString histo_name{"MVA_" + channel + "__" + sample};
        if (!file.GetListOfKeys()->Contains(histo_name))
        {
            cout << histo_name << " : problem" << endl;
            continue;
        }

        h_tmp = dynamic_cast<TH1F*>(file.Get(histo_name)->Clone());

        if (h_sum == nullptr)
        {
            h_sum = dynamic_cast<TH1F*>(h_tmp->Clone());
        }
        else
        {
            h_sum->Add(h_tmp);
        }
    }

    const int nofbins{h_sum->GetNbinsX()};

    for (int i{0}; i < nofbins; i++)
    {
        const int bin_content{boost::numeric_cast<int>
            (h_sum->GetBinContent(i + 1))}; //cout<<"initial content = "<<bin_content<<endl;
        const int new_bin_content{boost::numeric_cast<int>
            (rand.Poisson(bin_content))}; //cout<<"new content = "<<new_bin_content<<endl;
        h_sum->SetBinContent(i + 1, new_bin_content);
    }

    file.cd();
    h_sum->Write("MVA_" + channel + "__DATA", TObject::kOverwrite);

    file.Close();

    cout << "--- Done with generation of pseudo-data" << endl;
}


void MVATool::makePseudoDataVars(const TString& inDir, const TString& channel,
        const bool useData) const
{
    TRandom3 rand{0}; //Randomization

    TFile file{inDir+"output_merged_" + channel + ".root", "UPDATE"};

    cout << "\n--- GENERATION OF PSEUDODATA IN " << file.GetName() << " ! ---\n" << endl;

    TH1F* h_sum{nullptr};
    TH1F* h_tmp{nullptr};

    for (const auto& var: varList)
    {
        cout << " --- " << var << endl;

        h_sum = nullptr;

        for (const auto& sample: samplelist)
        {
            if (!useData && sample.Contains("Data")) //From MC only
            {
                continue;
            }
            if (useData &&
                    (sample.Contains("DYToLL_M50")
                     || sample.Contains("TT")
                     || sample.Contains("TTW")
                     || sample.Contains("TTZ")
                     || sample.Contains("ZZ")
                     || sample.Contains("WZ")
                     || sample.Contains("WW")
                     || sample.Contains("tZq")
                     || sample.Contains("TbartChan")
                     || sample.Contains("TtW")
                     || sample.Contains("TbartW"))) //From Data only
            {
                continue;
            }

            h_tmp = nullptr;
            const TString histo_name{var + "_" + channel + "__" + sample};
            if (!file.GetListOfKeys()->Contains(histo_name))
            {
                cout << histo_name << " : problem" << endl;
                continue;
            }

            h_tmp = dynamic_cast<TH1F*>(file.Get(histo_name)->Clone());

            if (h_sum == nullptr)
            {
                h_sum = dynamic_cast<TH1F*>(h_tmp->Clone());
            }
            else
            {
                h_sum->Add(h_tmp);
            }
        }

        const int nofbins{h_sum->GetNbinsX()};

        for (int i{0}; i < nofbins; i++)
        {
            const int bin_content{boost::numeric_cast<int>
                (h_sum->GetBinContent(i + 1))}; //cout<<"initial content = "<<bin_content<<endl;
            const int new_bin_content{boost::numeric_cast<int>
                (rand.Poisson(bin_content))}; //cout<<"new content = "<<new_bin_content<<endl;
            h_sum->SetBinContent(i + 1, new_bin_content);
        }

        file.cd();
        h_sum->Write(var+"_"+channel+"__DATA", TObject::kOverwrite);
    }

    file.Close();

    cout << "--- Done with generation of pseudo-data" << endl;
}
