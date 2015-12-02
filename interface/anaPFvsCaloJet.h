// analysis to compare PF with calo jets
// Author: M. Verweij

#ifndef anaPFvsCaloJet_h
#define anaPFvsCaloJet_h

#include "TString.h"
#include "TH2F.h"
#include "TH3F.h"

#include "UserCode/TopFromHeavyIons/interface/anaBaseTask.h"
#include "UserCode/TopFromHeavyIons/interface/lwJetContainer.h"

class anaPFvsCaloJet : public anaBaseTask {
   
public:
   anaPFvsCaloJet() {;}
   anaPFvsCaloJet(const char *name, const char *title);
   virtual ~anaPFvsCaloJet() {;}
   void Exec(Option_t *option="");
   void CreateOutputObjects();

   void SetPFJetsName(TString name)   { fJetsName = name ; }
   void SetCaloJetsName(TString name) { fJets2Name = name ; } 
   
 protected:
   TString           fJetsName;       //name of jet container
   lwJetContainer   *fJetsCont;       //!jet container
   TString           fJets2Name;      //name of jet container
   lwJetContainer   *fJets2Cont;      //!jet container

   
   TH1F      *fhEventSel;
   TH1F      *fhCentrality;
   TH1F      *fhNPV;
   
   TH2F      *fh2PtEtaNoMatching;
   TH3F      *fh3PtEtaPhiNotMatched;
   TH3F      *fh3PtEtaPhiMatched;

   TH3F      *fh3PtTrueNPVDeltaPt;
   TH3F      *fh3PtTrueNPVDeltaPtRel;
   TH3F      *fh3PtTrueNPVScalePt;
   TH3F      *fh3PtTruePtSubNPV;

   TH3F      *fh3PtTrueEtaDeltaPt;
   TH3F      *fh3PtTrueEtaDeltaPtRel;
   TH3F      *fh3PtTrueEtaScalePt;
   TH3F      *fh3PtTruePtSubEta;
   
   ClassDef(anaPFvsCaloJet,2)
};
#endif
