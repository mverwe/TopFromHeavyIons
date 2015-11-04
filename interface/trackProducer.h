#ifndef trackProducer_h
#define trackProducer_h

//
// tracks from hiForest producer
//

#include <TNamed.h>
#include <TChain.h>
#include <TString.h>
#include <TClonesArray.h>

#include "UserCode/TopFromHeavyIons/interface/inputBase.h"
#include <UserCode/TopFromHeavyIons/interface/ForestTracks.h>

class trackProducer : public inputBase {
 public:
  trackProducer();
  trackProducer(const char *name);
  virtual ~trackProducer() {;}

  Bool_t   Init();
  Long64_t LoadTree(Long64_t entry);
  Bool_t   InitEventObjects();
  Bool_t   Run(Long64_t entry);
  
  void     SetTracksName(TString n) { fTracksName = n; }
  void     SetHighPurity(Bool_t b)  { fSelectHighPurity = b; }

  const char* GetTracksName() const { return fTracksName.Data() ; }
  
 protected:
  TString                      fTracksName;       // name of track array
  TClonesArray                *fTracks;           //!track array
  ForestTracks                 fForestTracks;     //!tracks in tree
  Bool_t                       fSelectHighPurity; //select high purity tracks
  
 private:
  trackProducer(const trackProducer& obj); // copy constructor
  trackProducer& operator=(const trackProducer& other); // assignment
  
  ClassDef(trackProducer,1)
};
#endif
