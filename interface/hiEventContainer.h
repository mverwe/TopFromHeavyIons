#ifndef hiEventContainer_h
#define hiEventContainer_h

//
// heavy-ion event container: hi event properties
//

#include <TNamed.h>
#include <TChain.h>

class hiEventContainer : public TNamed {
 public:
  hiEventContainer();
  hiEventContainer(const char *name);
  virtual ~hiEventContainer() {;}

  void     SetRun(Int_t r)                     { fRun   = r  ; }
  void     SetEvent(Int_t e)                   { fEvt   = e  ; }
  void     SetLumi(Int_t l)                    { fLumi  = l  ; }
  void     SetWeight(Float_t w)                { fWeight= w  ; }
  void     SetVx(Float_t vx)                   { fVx    = vx ; }
  void     SetVy(Float_t vy)                   { fVy    = vy ; }
  void     SetVz(Float_t vz)                   { fVz    = vz ; }
  void     SetHiBin(Int_t b)                   { fHiBin = b  ; fCent = (Double_t)fHiBin/2.;}
  void     SetHiHF(Int_t b)                    { fHiHF = b   ; }
  void     SetNPV(Int_t n)                     { fNPV = n    ; }
  void     SetPFJet80(Int_t n)                 { fPFJet80 = n; }
  void     SetPhoton30(Int_t n)                { fPhoton30= n; }
  void     SetTrk24(Int_t n)                   { fTrk24 = n  ; }
  void     SetTrk45(Int_t n)                   { fTrk45 = n  ; }

  void     SetColl(Int_t c)                    { fColl = c     ; }
  void     SetHBHENoise(Int_t n)               { fHBHENoise = n; }
  
  Int_t    GetRun()        const               { return fRun;  }
  Int_t    GetEvent()      const               { return fEvt;  }
  Int_t    GetLumi()       const               { return fLumi; }
  Float_t  GetWeight()     const               { return fWeight;}
  Float_t  GetVx()         const               { return fVx;   }
  Float_t  GetVy()         const               { return fVy;   }
  Float_t  GetVz()         const               { return fVz;   }
  Int_t    GetHiBin()      const               { return fHiBin;}
  Float_t  GetHiHF()       const               { return fHiHF; }
  Double_t GetCentrality() const               { return fCent; }
  Int_t    GetNPV()        const               { return fNPV;  }
  Int_t    GetPFJet80()    const               { return fPFJet80;  }
  Int_t    GetPhoton30()   const               { return fPhoton30;  }
  Int_t    GetTrk24()      const               { return fTrk24;  }
  Int_t    GetTrk45()      const               { return fTrk45;  }

  Int_t    GetColl()       const               { return fColl; }
  Int_t    GetHBHENoise()  const               { return fHBHENoise; }
  
 protected:
  Int_t                        fRun;     //
  Int_t                        fEvt;     //
  Int_t                        fLumi;    //
  Float_t                      fWeight;  //
  Float_t                      fVx;      //
  Float_t                      fVy;      //
  Float_t                      fVz;      //
  Int_t                        fHiBin;   //
  Float_t                      fHiHF;    //
  Double_t                     fCent;    //centrality
  Int_t                        fNPV;     //number of primary vertices
  Int_t                        fPFJet80; //
  Int_t                        fPhoton30;//
  Int_t                        fTrk24;   //
  Int_t                        fTrk45;   //
  Int_t                        fColl;    //
  Int_t                        fHBHENoise;//
  
 private:
  hiEventContainer(const hiEventContainer& obj); // copy constructor
  hiEventContainer& operator=(const hiEventContainer& other); // assignment
  
  ClassDef(hiEventContainer,3)
};
#endif
