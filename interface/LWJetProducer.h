
#ifndef LWJetProducer_h
#define LWJetProducer_h

class FJWrapper;

#include <memory>
#include <vector>
#include <boost/shared_ptr.hpp>

#include <TNamed.h>
#include <TClonesArray.h>

#include "UserCode/TopFromHeavyIons/interface/inputBase.h"
#include <UserCode/TopFromHeavyIons/interface/FJWrapper.h>
#include <UserCode/TopFromHeavyIons/interface/lwJetContainer.h>
#include <UserCode/TopFromHeavyIons/interface/rhoMap.h>

namespace fastjet {
  class PseudoJet;
}

//
// Light-weight jet producer
//

class LWJetProducer : public inputBase {
 public:

  enum JetType {
    kNone=0,
    kKT=1<<0,
    kAKT=1<<1
  };
  
  LWJetProducer();
  LWJetProducer(const char *name);
  virtual ~LWJetProducer() {;}

  void                   SetParticlesName(TString n) { fConstName     = n ; }
  void                   SetJetContName(TString n)   { flwJetContName = n ; }
  void                   SetCSJetContName(TString n) { flwCSJetContName = n ; }
  
  void                   SetJetType(UInt_t t)        { fJetType       = t ; }
  void                   SetRadius(Double_t r)       { fRadius        = r ; }
  void                   SetRecombScheme(Int_t s)    { fRecombScheme  = s ; }
  void                   SetGhostArea(Double_t gh)   { fGhostArea     = gh; }
  void                   SetPtMinConst(Double_t pt)  { fPtMinConst    = pt; } 

  void                   SetDoConstituentSubtraction(Bool_t b) {fDoConstSubtraction = b;}
  void                   SetRhoMapName(TString n)    { fRhoMapName    = n ; }
  void                   SetRhoMMapName(TString n)   { fRhoMMapName   = n ; }
  
  UInt_t                 GetJetType()          const { return fJetType         ; }
  Double_t               GetRadius()           const { return fRadius          ; }
  Int_t                  GetRecombScheme()     const { return fRecombScheme    ; }
  lwJetContainer        *GetJetContainer()     const { return flwJetContainer  ; }
    
  Bool_t Init();
  Int_t  FindJets();
  
 protected:
  Bool_t           GetSortedArray(Int_t indexes[], std::vector<fastjet::PseudoJet> array) const;
  
  UInt_t           fJetType;                // jet type (algorithm)
  Double_t         fRadius;                 // the R parameter to use
  Int_t            fRecombScheme;           // recombination scheme used by fastjet
  Double_t         fGhostArea;              // ghost area
  Double_t         fPtMinConst;             // minimum pT of input constituents

  std::vector<fastjet::PseudoJet> fjInputs; // fastjet inputs
  std::vector<fastjet::PseudoJet> fjJets;   // fastjet reconstructed jets

  Bool_t           fIsInit;                 //!=true if already initialized
  TClonesArray    *fConst;                  //! container with jet constituents
  TString          fConstName;              // name of constituents
  lwJetContainer  *flwJetContainer;         //!lwJetContainer
  TString          flwJetContName;          // name of jet container
  FJWrapper        fFastJetWrapper;         //!fastjet wrapper

  Bool_t           fDoConstSubtraction;     // do constituent subtraction
  TString          fRhoMapName;             //rho map name
  rhoMap          *fRhoMap;                 //!rho map
  TString          fRhoMMapName;            //rhom map name
  rhoMap          *fRhoMMap;                //!rhom map
  lwJetContainer  *flwCSJetContainer;       //!lwJetContainer for constituent subtracted jets
  TString          flwCSJetContName;        // name of constituent subtracted jet container

  
 private:
  LWJetProducer(const LWJetProducer& obj); // copy constructor
  LWJetProducer& operator=(const LWJetProducer& other); // assignment
  
  ClassDef(LWJetProducer,2)

};
#endif
