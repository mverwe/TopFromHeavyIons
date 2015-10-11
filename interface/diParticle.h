#ifndef diParticle_h
#define diParticle_h

//
// particle reconstructed from two decay products
//

#include "TObjArray.h"

#include "UserCode/TopFromHeavyIons/interface/particleBase.h"

class diParticle : public particleBase {
 public:
  diParticle();
  diParticle(Double_t pt, Double_t eta, Double_t phi, Double_t m, Int_t id = -1);
  virtual ~diParticle() {;}
  diParticle(const diParticle& obj); // copy constructor
  diParticle& operator=(const diParticle& other); // assignment

  void AddParticle(particleBase *p);
  
 protected:
  TObjArray            fDecayParticles;   // pointer to decay particles

 private:
  ClassDef(diParticle,1)
};
#endif
