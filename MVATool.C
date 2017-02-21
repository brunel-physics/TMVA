#define MVATool_cxx

#include <iostream>
#include <unordered_map>

#include <boost/algorithm/string/replace.hpp>
#include <boost/numeric/conversion/cast.hpp>

#include <TCut.h>

#include <TMVA/TMVAGui.h>
#include <TMVA/DataLoader.h>
#include <TMVA/Factory.h>
#include <TMVA/Tools.h>

#include "MVATool.h"


MVATool::MVATool(const bool doCtrlReg)
    : varList{
        "bTagDisc",
        // "fourthJetEta",
        // "fourthJetPhi",
        // "fourthJetPt",
        // "fourthJetbTag",
        // "jetHt",
        // "jjdelPhi",
        "jjdelR",
        // "leadJetEta",
        // "leadJetPhi",
        // "leadJetPt",
        // "leadJetbTag",
        // "lep1D0",
        "lep1Eta",
        // "lep1Phi",
        // "lep1Pt",
        // "lep1RelIso",
        // "lep2D0",
        // "lep2Eta",
        // "lep2Phi",
        // "lep2Pt",
        // "lep2RelIso",
        // "lepEta",
        // "lepHt",
        "lepMass",
        // "lepPhi",
        // "lepPt",
        // "mTW",
        "met",
        // "nBjets",
        // "nJets",
        // "secJetEta",
        // "secJetPhi",
        // "secJetPt",
        // "secJetbTag",
        "thirdJetEta",
        // "thirdJetPhi",
        // "thirdJetPt",
        // "thirdJetbTag",
        // "topEta",
        "topMass",
        // "topPhi",
        // "topPt",
        // "totEta",
        // "totHt",
        // "totHtOverPt",
        // "totPt",
        // "totPt2Jet",
        // "totPtVec",
        "totVecM",
        // "w1TopDelPhi",
        // "w1TopDelR",
        "w2TopDelPhi",
        // "w2TopDelR",
        // "wPairEta",
        "wPairMass",
        // "wPairPhi",
        // "wPairPt",
        // "wQuark1Eta",
        // "wQuark1Phi",
        // "wQuark1Pt",
        // "wQuark2Eta",
        // "wQuark2Phi",
        // "wQuark2Pt",
        // "wQuarkHt",
        // "wTopDelPhi",
        // "wTopDelR",
        // "wwdelPhi",
        // "wwdelR",
        // "wzdelPhi",
        // "wzdelR",
        // "zEta",
        "zLepdelPhi",
        "zLepdelR",
        // "zMass",
        // "zPhi",
        // "zPt",
        // "zQuark1DelPhi",
        // "zQuark1DelR",
        // "zQuark2DelPhi",
        // "zQuark2DelR",
        // "zTopDelPhi",
        // "zTopDelR",
        // "zjminPhi",
        // "zjminR",
        // "zl1Quark1DelPhi",
        // "zl1Quark1DelR",
        "zl1Quark2DelPhi",
        // "zl1Quark2DelR",
        // "zl1TopDelPhi",
        // "zl1TopDelR",
        // "zl2Quark1DelPhi",
        // "zl2Quark1DelR",
        // "zl2Quark2DelPhi",
        // "zl2Quark2DelR",
        "zl2TopDelPhi",
        // "zl2TopDelR",
        "zlb1DelPhi",
        "zlb1DelR",
        // "zlb2DelPhi",
        "zlb2DelR",
        ////  "Channel",
    }
    , dataList{
        "DataMu",
        "DataEG"
    }
    , signalList{
        "tZq"
    }
    , backgroundList{
        "DYToLL_M10To50",
        "DYToLL_M50",
        // "THQ", // Currently broken
        "TT",
        "TTW",
        "TTZ",
        "TbartChan",
        "TbartW",
        "TsChan",
        "TtChan",
        "TtW",
        "WW",
        "WZ",
        "Wjets",
        "ZZ"
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
        const vector<TString>& dataList_,
        const vector<TString>& signalList_,
        const vector<TString>& backgroundList_,
        const vector<TString>& systList_,
        const vector<TString>& regList_)
    : varList{varList_}
    , dataList{dataList_}
    , signalList{signalList_}
    , backgroundList{backgroundList_}
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
    TMVA::DataLoader loader{"loader"};

    vector <TFile*> signalFiles;
    vector <TFile*> backgroundFiles;
    const TString treePost{regList.size() < 2 ? ""
                                    : sigMode ? "sig_"
                                    : "ctrl_"};

    for (const auto& signal: signalList)  // add signal trees
    {
        signalFiles.emplace_back(TFile::Open(inDir + "/histofile_" + signal
                    + ".root"));
        TTree* signalTree{dynamic_cast<TTree*>
            (signalFiles.back()->Get("Ttree_" + treePost + signal))};
        if (signalTree->GetEntries() != 0)
        {
            loader.AddSignalTree(signalTree);
        }
        else
        {
            cout << "                         : Don't add Tree " << signal
                << " as it's empty" << endl;
        }
    }
    for (const auto& background: backgroundList)  // add background trees
    {
        backgroundFiles.emplace_back(TFile::Open(inDir + "/histofile_" + background
                    + ".root"));
        TTree* const backgroundTree{dynamic_cast<TTree*>
            (backgroundFiles.back()->Get("Ttree_" + treePost + background))};
        if (backgroundTree->GetEntries() != 0)
        {
            loader.AddBackgroundTree(backgroundTree);
        }
        else
        {
            cout << "                         : Don't add Tree " << background
                << " as it's empty" << endl;
        }
    }
    for (const auto& var: varList)  // add variables
    {
        loader.AddVariable(var, 'F');
    }
    loader.SetSignalWeightExpression    ("abs(EvtWeight)");
    loader.SetBackgroundWeightExpression("abs(EvtWeight)");

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

    loader.PrepareTrainingAndTestTree(mycuts, mycutb,
            "nTrain_Signal=0:nTrain_Background=0:SplitMode=Random:NormMode=NumEvents:!V");

    factory.BookMethod(&loader, TMVA::Types::kBDT, "BDT",
            "!H:!V:NTrees=100:nEventsMin=100:MaxDepth=5:BoostType=AdaBoost:SeparationType=GiniIndex:nCuts=20:PruneMethod=NoPruning");

    // Train MVAs using the set of training events
    factory.TrainAllMethods();

    // Evaluate all MVAs using the set of test events
    factory.TestAllMethods();

    // Evaluate and compare performance of all configured MVAs
    factory.EvaluateAllMethods();

    // Save the output
    outputFile->Close();

    for (const auto& list: {signalFiles, backgroundFiles})  // close input files
    {
        for (const auto& file: list)
        {
            file->Close();
        }
    }

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

    reader->BookMVA("BDT", ("loader/weights/BDT_trainning_" + channel + "_tzq_BDT.weights.xml"));

    for (const auto& sampleList: {dataList, signalList, backgroundList})
    {
        for (const auto& sample: sampleList)
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

    vector<TString> sampleList;
    if (useData)
    {
        sampleList = dataList;
    }
    else
    {
        sampleList = signalList;
        sampleList.insert(sampleList.end(), backgroundList.begin(),
                backgroundList.end());
    }

    for (const auto& sample: sampleList)
    {
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

    vector<TString> sampleList;
    if (useData)
    {
        sampleList = dataList;
    }
    else
    {
        sampleList = signalList;
        sampleList.insert(sampleList.end(), backgroundList.begin(),
                backgroundList.end());
    }

    for (const auto& var: varList)
    {
        cout << " --- " << var << endl;

        h_sum = nullptr;

        for (const auto& sample: sampleList)
        {
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
