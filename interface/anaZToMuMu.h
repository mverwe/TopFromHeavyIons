#ifndef anaZToMuMu_h
#define anaZToMuMu_h

#include "TString.h"
#include "TH2F.h"
#include "TH3F.h"
#include "TClonesArray.h"

#include "UserCode/TopFromHeavyIons/interface/anaBaseTask.h"
#include "UserCode/TopFromHeavyIons/interface/particleBase.h"

//
// Z->mu+mu- analysis
//

class anaZToMuMu : public anaBaseTask {
   
public:
   anaZToMuMu() {;}
   anaZToMuMu(const char *name, const char *title);
   virtual ~anaZToMuMu() {;}
   void Exec(Option_t *option="");
   void CreateOutputObjects();

   void SetMuonsName(TString name)     { fMuonsName = name ; }
   void SetCheckPid(Bool_t b)          { fCheckPid = b; }
   
 protected:
   Bool_t            CheckPid(particleBase *p);
   
   TString           fMuonsName;            //name of particles
   TClonesArray     *fMuons;                //!muon array
   TString           fZsName;               // name of Z candidates
   TClonesArray     *fZs;                   //!Z candidates container
   
   Bool_t            fCheckPid;             //check if candidates are really muons (for simulation)
   TH1F             *fh1NMuons;             //!# selected muons in event
   TH3F             *fh3CentPtInvMass;      //!centrality vs Pt vs inv mass
   TH3F             *fh3CentPtInvMassSC;    //!centrality vs Pt vs inv mass for same charge
   
   ClassDef(anaZToMuMu,2)
};
#endif
