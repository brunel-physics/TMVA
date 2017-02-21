#ifndef MVATool_h
#define MVATool_h

#include <TFile.h>
#include <TFile.h>
#include <TH1F.h>
#include <TString.h>
#include <TString.h>
#include <TTree.h>

#include <map>
#include <vector>

#include <TMVA/Reader.h>

class MVATool final
{
    public:
        explicit MVATool(const bool doCtrlReg = false);
        MVATool(const vector<TString>& varList_,
                const vector<TString>& dataList_,
                const vector<TString>& signalList_,
                const vector<TString>& backgroundList_,
                const vector<TString>& systList_,
                const vector<TString>& regList_);
        ~MVATool();

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

        // deal with histograms
        void createHisto(const TString& sample, const TString& channel);
        void fillHisto(const TString& sample, vector<float>& vars,
                const double mva, const double mtw, const double weight);
        void writeHisto(const TString& sample, const TString& syst,
                const TString& reg);
        void scaleHisto(const TString& sample, const double sf);

        // generate pseudodata
        void makePseudoDataMVA(const TString& inDir, const TString& channel,
                const bool useData) const;
        void makePseudoDataVars(const TString& inDir, const TString& channel,
                const bool useData) const;

        const vector<TString> varList;
        const vector<TString> dataList;
        const vector<TString> signalList;
        const vector<TString> backgroundList;
        const vector<TString> systlist;
        const vector<TString> regList;

        map<TString, vector<TH1F*>> histMap;
        map<TString, int> varMap;
        map<TString, TFile*> outFileMap;

        TMVA::Reader reader; 
};

#endif
