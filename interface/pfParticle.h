#ifndef pfParticle_h
#define pfParticle_h

//
// light-weight particle flow candidate
//

#include "UserCode/TopFromHeavyIons/interface/particleBase.h"

class pfParticle : public particleBase {
 public:
  pfParticle();
  pfParticle(Double_t pt, Double_t eta, Double_t phi, Double_t m, Int_t id = -1, Int_t c = 0);
  virtual ~pfParticle() {;}
  pfParticle(const pfParticle& obj); // copy constructor
  pfParticle& operator=(const pfParticle& other); // assignment
  
  
  void           SetPuppiId(Int_t id)         { fPupId = id;    }
  void           SetPuppiAlpha(Double_t a)    { fPupAlpha = a;  }
  void           SetPuppiMetric2(Double_t a)  { fPupMetric2 = a;}
  void           SetPuppiWeight(Double_t w)   { fPupWeight = w; }
  void           SetPuppiWeight2(Double_t w)  { fPupWeight2= w; }
  void           SetPuppiWeight3(Double_t w)  { fPupWeight3= w; }
  void           SetPtVS(Double_t pt)         { fPtVS  = pt;    }
  
  Int_t          GetPuppiId() const      { return fPupId; }
  Double_t       GetPuppiAlpha() const   { return fPupAlpha; }
  Double_t       GetPuppiMetric2() const { return fPupMetric2;}
  Double_t       GetPuppiWeight() const  { return fPupWeight; }
  Double_t       GetPuppiWeight2() const { return fPupWeight2;}
  Double_t       GetPuppiWeight3() const { return fPupWeight3;}
  Double_t       PtVS()       const      { return fPtVS;  } 

 protected:
  Int_t          fPupId;       //puppi id
  Double_t       fPupAlpha;    //puppi alpha
  Double_t       fPupMetric2;  //puppi alpha
  Double_t       fPupWeight;   //puppi weight
  Double_t       fPupWeight2;  //puppi weight2
  Double_t       fPupWeight3;  //puppi weight3
  Double_t       fPtVS;        //subtracted pt from HF-Voronoi algo
  
  ClassDef(pfParticle,2)
};
#endif
