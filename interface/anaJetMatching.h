#ifndef anaJetMatching_h
#define anaJetMatching_h

#include "TString.h"
#include "TH2F.h"

#include "UserCode/TopFromHeavyIons/interface/anaBaseTask.h"
#include "UserCode/TopFromHeavyIons/interface/lwJetContainer.h"
#include "UserCode/TopFromHeavyIons/interface/hiEventContainer.h"

class anaJetMatching : public anaBaseTask {
   
public:
   anaJetMatching() {;}
   anaJetMatching(const char *name, const char *title);
   virtual ~anaJetMatching() {;}
   void Exec(Option_t *option="");
   void CreateOutputObjects();

   void SetHiEvtName(TString name)    { fEvtName  = name ; }
   void SetJetsNameBase(TString name) { fJetsNameBase = name ; }
   void SetJetsNameTag(TString name)  { fJetsNameTag  = name ; }
   void SetNCentBins(Int_t n)         { fNcentBins = n ; }

   void MatchJetsGeo();
   
 protected:
   TString           fEvtName;          //name of hi event container
   hiEventContainer *fHiEvent;          //!event container
   TString          fJetsNameBase;    //name of jet container
   lwJetContainer  *fJetsContBase;    //!jet container
   TString          fJetsNameTag;     //name of jet container
   lwJetContainer  *fJetsContTag;     //!jet container
   Int_t            fNcentBins;

   TH2F            **fh2PtJet1VsPtJet2;            //!pT of base jet vs tagged jet
   TH2F            **fh2PtJet2VsRelPt;             //!pT of tagged jet vs pt base jet / pt tagged jet
   TH2F            **fh2PtJet1VsDeltaR;            //!pt jet 1 vs dR
   
   ClassDef(anaJetMatching,1)
};
#endif
