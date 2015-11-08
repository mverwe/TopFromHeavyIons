// analysis to compare PF with calo jets
// Author: M. Verweij

#ifndef anaPFvsCaloJet_h
#define anaPFvsCaloJet_h

#include "TString.h"
#include "TH3F.h"

#include "UserCode/TopFromHeavyIons/interface/anaBaseTask.h"
#include "UserCode/TopFromHeavyIons/interface/lwJetContainer.h"
#include "UserCode/TopFromHeavyIons/interface/hiEventContainer.h"

class anaPFvsCaloJet : public anaBaseTask {
   
public:
   anaPFvsCaloJet() {;}
   anaPFvsCaloJet(const char *name, const char *title);
   virtual ~anaPFvsCaloJet() {;}
   void Exec(Option_t *option="");
   void CreateOutputObjects();

   void SetHiEvtName(TString name)    { fEvtName  = name ; }
   void SetPFJetsName(TString name)   { fJetsName = name ; }
   void SetCaloJetsName(TString name) { fJets2Name = name ; } 
   
 protected:
   TString           fEvtName;        //name of hi event container
   hiEventContainer *fHiEvent;        //!event container
   TString           fJetsName;       //name of jet container
   lwJetContainer   *fJetsCont;       //!jet container
   TString           fJets2Name;      //name of jet container
   lwJetContainer   *fJets2Cont;      //!jet container

   
   TH1F      *fhEventSel;
   TH1F      *fhCentrality;
   TH1F      *fhNPV;
   
   TH3F      *fh3PtTrueNPVDeltaPt;
   TH3F      *fh3PtTrueNPVDeltaPtRel;
   TH3F      *fh3PtTrueNPVScalePt;
   TH3F      *fh3PtTruePtSubNPV;
   
   ClassDef(anaPFvsCaloJet,1)
};
#endif
