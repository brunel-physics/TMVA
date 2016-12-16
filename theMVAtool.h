#ifndef theMVAtool_h
#define theMVAtool_h

#include <TFile.h>
#include <TFile.h>
#include <TH1F.h>
#include <TString.h>
#include <TString.h>
#include <TTree.h>

#include <map>
#include <vector>

#include <TMVA/Reader.h>

class theMVAtool
{
    public:
        explicit theMVAtool(const bool doCtrlReg = false);
        theMVAtool(const vector<TString>& thevarlist,
                const vector<TString>& thesamplelist,
                const vector<TString>& thesystlist,
                const vector<TString>& thereglist);
        ~theMVAtool();

        void doTraining(const TString& channel = "all",
                const TString& inDir = "inputroot/met0mtw0/",
                const bool sigMode = false) const;
        void doReading(const float bdtcut = 100,
                const TString& channel= "all", 
                const TString& inDir = "inputroot/met0mtw0",
                const TString& outDir = "outputroot/",
                const bool usePseudoData = true);

        void loopInSample(TFile* const input, const TString& sample,
                vector<float>& treevars, const float bdtcut,
                const TString& channel);

        //deal with histograms
        void createHisto(const TString& sample, const TString& channel);
        void fillHisto(const TString& sample, vector<float>& theVar,
                const double mva, const double mtw, const double weight);
        void writeHisto(const TString& sample, const TString& syst,
                const TString& reg);
        void scaleHisto(const TString& sample, const double thescale);

        const vector<TString> varList;
        const vector<TString> samplelist;
        const vector<TString> systlist;
        const vector<TString> regList;

        void makePseudoDataMVA(const TString& inDir, const TString& channel,
                const bool useData) const;
        void makePseudoDataVars(const TString& inDir, const TString& channel,
                const bool useData) const;

        map<TString, vector<TH1F*>> theHistoMap;
        map<TString, int> theVarMap;

        map<TString, TFile*> theOutputFileMap;

        TMVA::Reader *reader; 
};

#endif
