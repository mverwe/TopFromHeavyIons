#ifndef anaPFCandidates_h
#define anaPFCandidates_h

#include "TString.h"
#include "TH2F.h"
#include "TH3F.h"
#include "TClonesArray.h"

#include "UserCode/TopFromHeavyIons/interface/hiEventContainer.h"
#include "UserCode/TopFromHeavyIons/interface/lwJetContainer.h"

#include "UserCode/TopFromHeavyIons/interface/anaBaseTask.h"

//
// analyze particle flow candidates
//

class anaPFCandidates : public anaBaseTask {
   
public:
  anaPFCandidates() {;}
  anaPFCandidates(const char *name, const char *title);
  virtual ~anaPFCandidates() {;}
  void Exec(Option_t *option="");
  void CreateOutputObjects();
  
  void SetHiEvtName(TString name)     { fEvtName  = name ; }
  void SetParticlesName(TString name) { fParticlesName = name ; }
  void SetJetsName(TString name)      { fJetsName = name ; }
  void SetJetPtRange(Double_t min, Double_t max) {fMinJetPt = min; fMaxJetPt = max; }
  
 protected:
  TString           fEvtName;              //name of hi event container
  hiEventContainer *fHiEvent;              //!event container
  TString           fParticlesName;        //name of particles
  TClonesArray     *fParticles;            //!pf particle array
  TString           fJetsName;             //name of jets
  lwJetContainer   *fJetsCont;             //!jet container
  Double_t          fMinJetPt;             //minimum jet pt
  Double_t          fMaxJetPt;             //maximum jet pt

  TH1F             *fh1HiHF;               //!HFE distribution
  TH1F             *fh1Cent;               //!centrality distribution
  
  TH2F            **fh2CentPt;             //!pt vs centrality full event
  TH2F            **fh2CentPtInJet;        //!pt vs centrality in jet
  TH2F            **fh2HiHFPt;             //!pt vs HF energy full event
  TH2F            **fh2HiHFPtInJet;        //!pt vs HF energy in jet
  TH2F             *fh2CentPtJet;          //!centrality vs pt jet
  TH2F             *fh2HiHFPtJet;          //!HF energy vs pt jet

  //add eta axis
  TH3F            **fh3CentPtEta;             //!pt vs eta vs centrality full event
  TH3F            **fh3CentPtEtaInJet;        //!pt vs eta vs centrality in jet
  TH3F            **fh3HiHFPtEta;             //!pt vs eta vs HF energy full event
  TH3F            **fh3HiHFPtEtaInJet;        //!pt vs eta vs HF energy in jet
  TH3F             *fh3CentPtEtaJet;          //!centrality vs pt vs eta jet
  TH3F             *fh3HiHFPtEtaJet;          //!HF energy vs pt vs eta jet
  
  ClassDef(anaPFCandidates,2)
};
#endif
