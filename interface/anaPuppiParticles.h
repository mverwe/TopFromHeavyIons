#ifndef anaPuppiParticles_h
#define anaPuppiParticles_h

#include "TString.h"
#include "TH2F.h"
#include "TH3F.h"
#include "TClonesArray.h"

#include "UserCode/TopFromHeavyIons/interface/hiEventContainer.h"
#include "UserCode/TopFromHeavyIons/interface/lwJetContainer.h"

#include "UserCode/TopFromHeavyIons/interface/anaBaseTask.h"

//
// geometrical matching of muons to other pfParticleBase object in fMatch
// match info is stored in muons (fMuons)
//

class anaPuppiParticles : public anaBaseTask {
   
public:
  anaPuppiParticles() {;}
  anaPuppiParticles(const char *name, const char *title);
  virtual ~anaPuppiParticles() {;}
  void Exec(Option_t *option="");
  void CreateOutputObjects();
  
  void SetHiEvtName(TString name)     { fEvtName  = name ; }
  void SetParticlesName(TString name) { fParticlesName = name ; }
  void SetJetsName(TString name)      { fJetsName = name ; }
  
  void SetMinPt(Float_t m)            { fMinPt = m; }
   
 protected:
  TString           fEvtName;              //name of hi event container
  hiEventContainer *fHiEvent;              //!event container
  TString           fParticlesName;        //name of particles
  TClonesArray     *fParticles;            //!pf particle array
  TString           fJetsName;             //name of jet array
  lwJetContainer   *fJetsCont;             //!jet container
  Float_t           fMinPt;                //minimum pT of particles

  TH2F             *fh2AlphaCentAll;       //!alpha vs centrality
  TH2F             *fh2AlphaCentUE;        //!alpha vs centrality
  TH2F             *fh2AlphaCentJet;       //!alpha vs centrality
  TH2F             *fh2Metric2CentAll;     //!metric2 vs centrality
  TH2F             *fh2Metric2CentUE;      //!metric2 vs centrality
  TH2F             *fh2Metric2CentJet;     //!metric2 vs centrality
  TH2F             *fh2WeightCentAll;      //!weight vs centrality
  TH2F             *fh2WeightCentUE;       //!weight vs centrality
  TH2F             *fh2WeightCentJet;      //!weight vs centrality
  TH2F             *fh2Weight2CentAll;     //!weight2 vs centrality
  TH2F             *fh2Weight2CentUE;      //!weight2 vs centrality
  TH2F             *fh2Weight2CentJet;     //!weight2 vs centrality
  TH2F             *fh2Weight3CentAll;     //!weight3 vs centrality
  TH2F             *fh2Weight3CentUE;      //!weight3 vs centrality
  TH2F             *fh2Weight3CentJet;     //!weight3 vs centrality
  TH2F             *fh2Weight4CentAll;     //!weight4 vs centrality
  TH2F             *fh2Weight4CentUE;      //!weight4 vs centrality
  TH2F             *fh2Weight4CentJet;     //!weight4 vs centrality

  TH3F             *fh3AlphaMetric2CentAll;     //!alpha vs metric2 vs centrality
  TH3F             *fh3AlphaMetric2CentUE;      //!alpha vs metric2 vs centrality
  TH3F             *fh3AlphaMetric2CentJet;     //!alpha vs metric2 vs centrality
  
  ClassDef(anaPuppiParticles,1)
};
#endif
