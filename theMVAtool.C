#define theMVAtool_cxx

#include <iostream>

#include <boost/algorithm/string/replace.hpp>
#include <boost/numeric/conversion/cast.hpp>

#include <TCut.h>

#include <TMVA/TMVAGui.h>
#include <TMVA/Factory.h>
#include <TMVA/Tools.h>

#include "theMVAtool.h"


theMVAtool::theMVAtool(const bool doCtrlReg)
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
        theVarMap[varList[i]] = i;
    }
}


theMVAtool::theMVAtool(const vector<TString>& thevarlist,
        const vector<TString>& thesamplelist,
        const vector<TString>& thesystlist,
        const vector<TString>& thereglist)
    : varList{thevarlist}
    , samplelist{thesamplelist}
    , systlist{thesystlist}
    , regList{thereglist}
{}


theMVAtool::~theMVAtool()
{}


void theMVAtool::doTraining(const TString& channel, const TString& inDir,
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


void theMVAtool::doReading(const float bdtcut, const TString& channel, 
        const TString& inDir, const TString& outDir, const bool usePseudoData)
{
    // This loads the library
    TMVA::Tools::Instance();

    //create the BDT reader
    reader = new TMVA::Reader{"!Color:!Silent"};
    vector<float> treevars(varList.size(), 0);

    for (size_t i{0}; i < varList.size(); i++)
    {
        reader->AddVariable(varList.at(i).Data(),  &(treevars.at(i)));
    }

    reader->BookMVA("BDT", ("weights/BDT_trainning_" + channel + "_tzq_BDT.weights.xml"));

    for (const auto& sample: samplelist)
    {
        //cout << sample << endl;
        TFile* const inFile{new TFile{(inDir + "histofile_" + sample + ".root"), "read"}};
        TFile* const outFile{new TFile{(outDir + "output_" + channel + "_" + sample+".root"), "recreate"}};
        theOutputFileMap[sample] = outFile;

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


void theMVAtool::loopInSample(TFile* const input, const TString& sample,
        vector<float>& treevars, const float bdtcut, const TString& channel)
{
    input->cd();
    TTree* const theTree{dynamic_cast<TTree*>(input->Get("Ttree_"+sample))};

    for (size_t ivar{0}; ivar < varList.size(); ivar++)
    {
        theTree->SetBranchAddress(varList[ivar].Data(), &(treevars[ivar]));
    }

    float theweight{0};
    int theChannel{-1};
    float mtwValue{-1};
    int theSelChannel{-1};

    theTree->SetBranchAddress("EvtWeight", &theweight);
    theTree->SetBranchAddress("Channel", &theChannel);
    theTree->SetBranchAddress("mTW", &mtwValue);

    if (channel == "mumu")
    {
        theSelChannel = 0;
    }
    else if (channel == "ee" )
    {
        theSelChannel = 1;
    }

    if (theTree == nullptr)
    {
        cout << "no TTree found with name " << "Ttree_" + sample << endl;
    }

    for (long long int i{0}; i < theTree->GetEntries(); i++)
    {
        constexpr double sf_local{1};
        theTree->GetEntry(i);

        if (channel != "all" && theChannel != theSelChannel)
        {
            continue;
        }

        if (isnan(theweight))
        {
            cout << "NaN weight encountered!" << " : " << i << endl;
        }
        theweight *= sf_local;

        const double mvaValue = reader->EvaluateMVA("BDT");
        if (mvaValue > bdtcut)
        {
            continue;
        }

        fillHisto(sample, treevars, mvaValue, mtwValue, theweight);
    }
}


void theMVAtool::createHisto(const TString& sample, const TString& channel)
{
    vector<TH1F*> histovect;
    for (size_t j{0}; j < varList.size(); j++)
    {
        int nbins=1;
        double xmin = -1000;
        double xmax = 1000;

        if      (varList[j]=="mTW"            ){nbins = 10; xmin =  0;   xmax = 200;}
        else if (varList[j]=="wQuark1Pt"      ){nbins = 20; xmin =  0;   xmax = 500;}
        else if (varList[j]=="wQuark1Eta"     ){nbins = 20; xmin = -5.; xmax = 5.;}
        else if (varList[j]=="wQuark1Phi"     ){nbins = 20; xmin = -3.2;   xmax = 3.2;}
        else if (varList[j]=="wQuark2Pt"      ){nbins = 20; xmin =  0;   xmax = 500;}
        else if (varList[j]=="wQuark2Eta"     ){nbins = 20; xmin = -5.; xmax = 5.;}
        else if (varList[j]=="wQuark2Phi"     ){nbins = 20; xmin = -3.2;   xmax = 3.2;}
        else if (varList[j]=="wPairMass"      ){nbins = 10; xmin =  0;   xmax = 200;}
        else if (varList[j]=="wPairPt"        ){nbins = 20; xmin =  0;   xmax = 500;}
        else if (varList[j]=="wPairEta"       ){nbins = 20; xmin = -5.;   xmax = 5.;}
        else if (varList[j]=="wPairPhi"       ){nbins = 20; xmin = -3.2;   xmax = 3.2;}
        else if (varList[j]=="met"            ){nbins = 10; xmin =  0;   xmax = 200;}
        else if (varList[j]=="nJets"          ){nbins =  8; xmin =  0.5; xmax = 8.5;}
        else if (varList[j]=="leadJetPt"      ){nbins = 20; xmin =  0;   xmax = 500;}
        else if (varList[j]=="leadJetPhi"     ){nbins = 20; xmin = -3.2; xmax = 3.2;}
        else if (varList[j]=="leadJetEta"     ){nbins = 10; xmin = -2.5; xmax = 2.5;}
        else if (varList[j]=="leadJetbTag"    ){nbins = 10; xmin =  0;   xmax = 1;}
        else if (varList[j]=="secJetPt"       ){nbins = 15; xmin =  0;   xmax = 300;}
        else if (varList[j]=="secJetPhi"      ){nbins = 20; xmin = -3.2;   xmax = 3.2;}
        else if (varList[j]=="secJetEta"      ){nbins = 20; xmin = -5.;   xmax = 5.;}
        else if (varList[j]=="secJetbTag"     ){nbins = 10; xmin =  0;   xmax = 1;}
        else if (varList[j]=="thirdJetPt"     ){nbins = 15; xmin =  0;   xmax = 300;}
        else if (varList[j]=="thirdJetPhi"    ){nbins = 20; xmin = -3.2;   xmax = 3.2;}
        else if (varList[j]=="thirdJetEta"    ){nbins = 20; xmin = -5.;   xmax = 5.;}
        else if (varList[j]=="thirdJetbTag"   ){nbins = 10; xmin =  0;   xmax = 1;}
        else if (varList[j]=="fourthJetPt"    ){nbins = 15; xmin =  0;   xmax = 300;}
        else if (varList[j]=="fourthJetPhi"   ){nbins = 20; xmin = -3.2;   xmax = 3.2;}
        else if (varList[j]=="fourthJetEta"   ){nbins = 20; xmin = -5.;   xmax = 5.;}
        else if (varList[j]=="fourthJetbTag"  ){nbins = 10; xmin =  0;   xmax = 1;}
        else if (varList[j]=="nBjets"         ){nbins =  4; xmin = -0.5;xmax = 3.5;}
        else if (varList[j]=="bTagDisc"       ){nbins = 10; xmin =  0.6;   xmax = 1;}
        else if (varList[j]=="lep1Pt"         ){nbins = 20; xmin =  0;   xmax = 500;}
        else if (varList[j]=="lep1Eta"        ){nbins = 20; xmin = -5.; xmax = 5.;}
        else if (varList[j]=="lep1Phi"        ){nbins = 20; xmin = -3.2;   xmax = 3.2;}
        else if (varList[j]=="lep1RelIso"     ){nbins = 50; xmin =  0.;   xmax = 0.2;}
        else if (varList[j]=="lep1D0"         ){nbins = 10; xmin =  0.;   xmax = 0.1;}
        else if (varList[j]=="lep2Pt"         ){nbins = 20; xmin =  0;   xmax = 500;}
        else if (varList[j]=="lep2Eta"        ){nbins = 20; xmin = -5.; xmax = 5.;}
        else if (varList[j]=="lep2Phi"        ){nbins = 20; xmin = -3.2;   xmax = 3.2;}
        else if (varList[j]=="lep2RelIso"     ){nbins = 50; xmin =  0.;   xmax = 0.2;}
        else if (varList[j]=="lep2D0"         ){nbins = 10; xmin =  0.;   xmax = 0.1;}
        else if (varList[j]=="lepMass"        ){nbins = 20; xmin =  0;   xmax = 500;}
        else if (varList[j]=="lepPt"          ){nbins = 20; xmin =  0;   xmax = 500;}
        else if (varList[j]=="lepEta"         ){nbins = 20; xmin = -5.; xmax = 5.;}
        else if (varList[j]=="lepPhi"         ){nbins = 20; xmin = -3.2;   xmax = 3.2;}
        else if (varList[j]=="zMass"          ){nbins = 20; xmin =  0;   xmax = 500;}
        else if (varList[j]=="zPt"            ){nbins = 20; xmin =  0;   xmax = 500;}
        else if (varList[j]=="zEta"           ){nbins = 10; xmin = -2.5;xmax = 2.5;}
        else if (varList[j]=="zPhi"           ){nbins = 20; xmin = -3.2;xmax = 3.2;}
        else if (varList[j]=="topMass"        ){nbins = 20; xmin =  0;   xmax = 500;}
        else if (varList[j]=="topPt"          ){nbins = 20; xmin =  0;   xmax = 500;}
        else if (varList[j]=="topEta"         ){nbins = 10; xmin = -2.5;xmax = 2.5;}
        else if (varList[j]=="topPhi"         ){nbins = 20; xmin = -3.2;xmax = 3.2;}
        else if (varList[j]=="jjdelR"         ){nbins = 20; xmin =  0; xmax = 6;}
        else if (varList[j]=="jjdelPhi"       ){nbins = 10; xmin =  0; xmax = 3.2;}
        else if (varList[j]=="wwdelR"         ){nbins = 20; xmin =  0; xmax = 6;}
        else if (varList[j]=="wwdelPhi"       ){nbins = 10; xmin =  0; xmax = 3.2;}
        else if (varList[j]=="zLepdelR"       ){nbins = 15; xmin =  0; xmax = 6;}
        else if (varList[j]=="zLepdelPhi"     ){nbins = 20; xmin = -4; xmax = 4;}
        else if (varList[j]=="zl1Quark1DelR"  ){nbins = 15; xmin =  0; xmax = 6;}
        else if (varList[j]=="zl1Quark1DelPhi"){nbins = 20; xmin = -4; xmax = 4;}
        else if (varList[j]=="zl1Quark2DelR"  ){nbins = 15; xmin =  0; xmax = 6;}
        else if (varList[j]=="zl1Quark2DelPhi"){nbins = 20; xmin = -4; xmax = 4;}
        else if (varList[j]=="zl2Quark1DelR"  ){nbins = 15; xmin =  0; xmax = 6;}
        else if (varList[j]=="zl2Quark1DelPhi"){nbins = 20; xmin = -4; xmax = 4;}
        else if (varList[j]=="zl2Quark2DelR"  ){nbins = 15; xmin =  0; xmax = 6;}
        else if (varList[j]=="zl2Quark2DelPhi"){nbins = 20; xmin = -4; xmax = 4;}
        else if (varList[j]=="zlb1DelR"       ){nbins = 15; xmin =  0; xmax = 6;}
        else if (varList[j]=="zlb1DelPhi"     ){nbins = 20; xmin = -4; xmax = 4;}
        else if (varList[j]=="zlb2DelR"       ){nbins = 15; xmin =  0; xmax = 6;}
        else if (varList[j]=="zlb2DelPhi"     ){nbins = 20; xmin = -4; xmax = 4;}
        else if (varList[j]=="lepHt"          ){nbins = 15; xmin =  0; xmax = 4;}
        else if (varList[j]=="wQuarkHt"       ){nbins = 15; xmin =  0; xmax = 150;}
        else if (varList[j]=="totPt"          ){nbins = 10; xmin =  0;   xmax = 250;}
        else if (varList[j]=="totEta"         ){nbins = 10; xmin = -2.5;xmax = 2.5;}
        else if (varList[j]=="totPtVec"       ){nbins = 20; xmin =  0; xmax = 200;}
        else if (varList[j]=="totVecM"        ){nbins = 20; xmin =  0; xmax = 300;}
        else if (varList[j]=="totPt2Jet"      ){nbins = 20; xmin =  0;   xmax = 500;}
        else if (varList[j]=="wzdelR"         ){nbins = 20; xmin =  0; xmax = 7;}
        else if (varList[j]=="wzdelPhi"       ){nbins = 20; xmin =  0; xmax = 3.2;}
        else if (varList[j]=="zQuark1DelR"    ){nbins = 15; xmin =  0; xmax = 6;}
        else if (varList[j]=="zQuark1DelPhi"  ){nbins = 20; xmin = -4; xmax = 4;}
        else if (varList[j]=="zQuark2DelR"    ){nbins = 15; xmin =  0; xmax = 6;}
        else if (varList[j]=="zQuark2DelPhi"  ){nbins = 20; xmin = -4; xmax = 4;}
        else if (varList[j]=="zTopDelR"       ){nbins = 15; xmin =  0; xmax = 6;}
        else if (varList[j]=="zTopDelPhi"     ){nbins = 20; xmin = -4; xmax = 4;}
        else if (varList[j]=="zl1TopDelR"     ){nbins = 15; xmin =  0; xmax = 6;}
        else if (varList[j]=="zl1TopDelPhi"   ){nbins = 20; xmin = -4; xmax = 4;}
        else if (varList[j]=="zl2TopDelR"     ){nbins = 15; xmin =  0; xmax = 6;}
        else if (varList[j]=="zl2TopDelPhi"   ){nbins = 20; xmin = -4; xmax = 4;}
        else if (varList[j]=="zjminR"         ){nbins = 15; xmin =  0; xmax = 3.5;}
        else if (varList[j]=="zjminPhi"       ){nbins = 15; xmin =  0; xmax = 3.2;}
        else if (varList[j]=="totHt"          ){nbins = 20; xmin =150; xmax = 1500;}
        else if (varList[j]=="jetHt"          ){nbins = 20; xmin =  0; xmax = 500;}
        else if (varList[j]=="totHtOverPt"    ){nbins = 20; xmin =  0; xmax = 4;}
        else if (varList[j]=="wTopDelR"       ){nbins = 20; xmin =  0; xmax = 7;}
        else if (varList[j]=="wTopDelPhi"     ){nbins = 20; xmin =  0; xmax = 3.2;}
        else if (varList[j]=="w1TopDelR"      ){nbins = 20; xmin =  0; xmax = 7;}
        else if (varList[j]=="w1TopDelPhi"    ){nbins = 20; xmin =  0; xmax = 3.2;}
        else if (varList[j]=="w2TopDelR"      ){nbins = 20; xmin =  0; xmax = 7;}
        else if (varList[j]=="w2TopDelPhi"    ){nbins = 20; xmin =  0; xmax = 3.2;}
        else {cout << "warning : no TH1F definition for variable " << varList[j] << endl;}

        TH1F* const histo{new TH1F{(varList[j]+"_"+channel+"__"+sample).Data(),
            (varList[j]+"_"+channel+"__"+sample).Data(), nbins, xmin, xmax}};
        histo->Sumw2();
        histovect.emplace_back(histo);
    }

    TH1F* const histomTW{new TH1F{("mTW_"+channel+"__"+sample).Data(),
        ("mTW_"+channel+"__"+sample).Data(), 15,0.,200.}};
    histomTW->Sumw2();
    histovect.emplace_back(histomTW);

    TH1F* const histo{new TH1F{("MVA_"+channel+"__"+sample).Data(),
        ("MVA_"+channel+"__"+sample).Data(),  20, -1., 1.}};
    histo->Sumw2();
    histovect.emplace_back(histo);

    theHistoMap[sample] = histovect;
}


void theMVAtool::fillHisto(const TString& sample, vector<float>& theVar,
        const double mva, const double mtw, const double weight)
{
   vector<TH1F*> histovect{theHistoMap[sample]};

   for (size_t i{0}; i<  varList.size(); i++)
   {
       histovect[i]->Fill(theVar[i], weight);
   }

   histovect[varList.size()]->Fill(mtw,weight);
   histovect.back()->Fill(mva, weight);
}


void theMVAtool::scaleHisto(const TString& sample, const double thescale)
{
  vector<TH1F*> histovect{theHistoMap[sample]};
  for (size_t i{0}; i < varList.size() + 1; i++)
  {
      histovect[i]->Scale(thescale);
  }
}


void theMVAtool::writeHisto(const TString& sample, const TString& syst,
        const TString& reg)
{
    if (theOutputFileMap[sample] == nullptr)
    {
        cout << "no output file for sample " << sample << endl;
    }

    theOutputFileMap[sample]->cd();
    vector<TH1F*> histovect{theHistoMap[reg+sample+syst]};

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


void theMVAtool::makePseudoDataMVA(const TString& inDir, const TString& channel,
        const bool useData) const
{
    TRandom3 therand{0}; //Randomization

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
            (therand.Poisson(bin_content))}; //cout<<"new content = "<<new_bin_content<<endl;
        h_sum->SetBinContent(i + 1, new_bin_content);
    }

    file.cd();
    h_sum->Write("MVA_" + channel + "__DATA", TObject::kOverwrite);

    file.Close();

    cout << "--- Done with generation of pseudo-data" << endl;
}


void theMVAtool::makePseudoDataVars(const TString& inDir, const TString& channel,
        const bool useData) const
{
    TRandom3 therand{0}; //Randomization

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
                (therand.Poisson(bin_content))}; //cout<<"new content = "<<new_bin_content<<endl;
            h_sum->SetBinContent(i + 1, new_bin_content);
        }

        file.cd();
        h_sum->Write(var+"_"+channel+"__DATA", TObject::kOverwrite);
    }

    file.Close();

    cout << "--- Done with generation of pseudo-data" << endl;
}
