#include "UserCode/TopFromHeavyIons/interface/anaMET.h"

#include "UserCode/TopFromHeavyIons/interface/particleBase.h"
#include "UserCode/TopFromHeavyIons/interface/pfParticle.h"
#include "UserCode/TopFromHeavyIons/interface/lwJetContainer.h"

#include "TLorentzVector.h"

#include "TClass.h"

ClassImp(anaMET)
   
anaMET::anaMET(const char *name, const char *title) 
:anaBaseTask(name,title),
  fEvtName(""),
  fHiEvent(),
  fParticlesName(""),
  fParticles(0x0),
  fMetType(kGen),
  fMinPt(0.),
  fh2MetCent(),
  fh2SumEtCent(),
  fh3PtEtaPhi()
{

  for(Int_t j = 0; j<10; ++j)
    fh2MetCentPtMin[j] = 0;
  
}

//----------------------------------------------------------
void anaMET::Exec(Option_t * /*option*/)
{
   //printf("anaMET executing\n");
   if(!fInitOutput) CreateOutputObjects();

   //Get event properties
   if(!fHiEvent && !fEvtName.IsNull())
     fHiEvent = dynamic_cast<hiEventContainer*>(fEventObjects->FindObject(fEvtName.Data()));
   if(!fHiEvent) return;

   //Get particles from which MET will be calculated
   if(!fParticles && !fParticlesName.IsNull()) {
     //  fEventObjects->Print();
     fParticles = dynamic_cast<TClonesArray*>(fEventObjects->FindObject(fParticlesName.Data()));
     //inheritance check gives crash. Don't know why. Skipping for now. Dangerous
     //     if(fParticles)
     //  if(!CheckInheritance(fParticles,"particleBase")) return;
     if(!fParticles) {
       //check if in jet branch
       lwJetContainer *jetsCont = dynamic_cast<lwJetContainer*>(fEventObjects->FindObject(fParticlesName.Data()));
       if(jetsCont) fParticles = jetsCont->GetJets();
     }
   }
   
   if(!fParticles) {
     Printf("%s: WARNING: Couldn't locate %s branch",GetName(),fParticlesName.Data());
     return;
   }
   
   Double_t cent = fHiEvent->GetCentrality();
   
   TLorentzVector p4(0.,0.,0.,0.);
   Double_t sumEt = 0.;

   const Int_t nptmin = 10;
   Double_t ptarr[nptmin] {0.,1.,2.,3.,5.,10.,15.,20.,30.,40.};
   TLorentzVector r4[nptmin];
   for(Int_t j = 0; j<nptmin; ++j)
     r4[j].SetPtEtaPhiM(0.,0.,0.,0.);
   
   for (int i = 0; i < fParticles->GetEntriesFast(); i++) {
     particleBase *p = static_cast<particleBase*>(fParticles->At(i));
     if(!p) {
       Printf("%s ERROR: couldn't get particle",GetName());
       continue;
     }
     TLorentzVector l;
     if(fMetType==kGen || fMetType==kPFRaw) {
       if(p->Pt() < fMinPt)
         continue;
       l = p->GetLorentzVector();
     }
     else if(fMetType==kVS) {
       pfParticle *pf = dynamic_cast<pfParticle*>(p);
       if(!pf) {
         Printf("%s ERROR: couldn't cast particle to pfParticle",GetName());
         return;
       }
       l.SetPtEtaPhiM(pf->PtVS(),pf->Eta(),pf->Phi(),pf->M());
     }
     else if(fMetType==kPuppi) {
       pfParticle *pf = dynamic_cast<pfParticle*>(p);
       if(!pf) {
         Printf("%s ERROR: couldn't cast particle to pfParticle",GetName());
         return;
       }
       l = pf->GetPuppiWeight()*p->GetLorentzVector();
     }
     
     if(l.Pt() < fMinPt) continue;
     fh3PtEtaPhi->Fill(l.Pt(),l.Eta(),l.Phi());
     p4+=l;
     sumEt+=l.Et();

     for(Int_t j = 0; j<nptmin; ++j) {
       if(l.Pt()>ptarr[j])
         r4[j]+=l;
     }
     
   }//particle loop

   TLorentzVector met = -p4;
   fh2MetCent->Fill(cent,met.Pt());
   fh2SumEtCent->Fill(cent,sumEt);

   Int_t nhists = TMath::Min(nptmin,10);
   for(Int_t j = 0; j<nhists; ++j) {
     TLorentzVector met2 = -r4[j];
     fh2MetCentPtMin[j]->Fill(cent,met2.Pt());
   }
}

//----------------------------------------------------------
void anaMET::CreateOutputObjects() {

  anaBaseTask::CreateOutputObjects();

  if(!fOutput) {
    Printf("anaMET: fOutput not present");
    return;
  }

  fh2MetCent = new TH2F("fh2MetCent","fh2MetCent;centrality;MET",100,0,100,500,0,1000.);
  fOutput->Add(fh2MetCent);

  fh2SumEtCent = new TH2F("fh2SumEtCent","fh2SumEtCent;centrality;sumET",100,0,100,500,0,10000.);
  fOutput->Add(fh2SumEtCent);

  fh3PtEtaPhi = new TH3F("fh3PtEtaPhi","fh3PtEtaPhi;pt;eta;phi",500,0,500,100,-5,5,72,-TMath::Pi(),TMath::Pi());
  fOutput->Add(fh3PtEtaPhi);

  for(Int_t j = 0; j<10; ++j) {
    fh2MetCentPtMin[j] = new TH2F(Form("fh2MetCentPtMin%d",j),"fh2MetCent;centrality;MET",100,0,100,500,0,1000.);
    fOutput->Add(fh2MetCentPtMin[j]);
  }
    
  
}
