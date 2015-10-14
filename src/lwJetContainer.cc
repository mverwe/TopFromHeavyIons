//
// container with recontructed jets
//

#include "UserCode/TopFromHeavyIons/interface/lwJetContainer.h"

ClassImp(lwJetContainer)

//__________________________________________________________
lwJetContainer::lwJetContainer() :
TNamed("lwJetContainer","lwJetContainer"),
  fJets(),
  fConst(),
  fJetsName("jets"),
  fRadius()
{
  //default constructor
}

//__________________________________________________________
lwJetContainer::lwJetContainer(const char *name) :
  TNamed(name,name),
  fJets(),
  fConst(),
  fJetsName(name),//"jets"),
  fRadius()
{
  //standard constructor
}

//__________________________________________________________
Bool_t lwJetContainer::Init() {

  if(fJets) ClearVec();
  else {
    fJets = new TClonesArray("lwJet");
    fJets->SetName(fJetsName);
  }
  return kTRUE;
}

//__________________________________________________________
void lwJetContainer::AddJet(lwJet *jet,Int_t pos) {

  //add jet to branch fJets
  (*fJets)[pos] = jet;
}

//__________________________________________________________
lwJet* lwJetContainer::GetJet(Int_t i) const {

  if(i<0 || i>fJets->GetEntriesFast()) return 0;
  lwJet *jet = static_cast<lwJet*>(fJets->At(i));
  return jet;
}

