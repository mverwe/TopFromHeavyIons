#include "UserCode/TopFromHeavyIons/interface/anaPFCandidates.h"

#include "UserCode/TopFromHeavyIons/interface/pfParticle.h"
#include "UserCode/TopFromHeavyIons/interface/lwJetContainer.h"

#include "TLorentzVector.h"

#include "TClass.h"

ClassImp(anaPFCandidates)
   
anaPFCandidates::anaPFCandidates(const char *name, const char *title) 
:anaBaseTask(name,title),
  fEvtName(""),
  fHiEvent(),
  fParticlesName(""),
  fParticles(0x0),
  fJetsName(""),
  fJetsCont(0x0),
  fMinJetPt(80.),
  fMaxJetPt(100.),
  fh1HiHF(0x0),
  fh1Cent(0x0),
  fh2CentPt(0),
  fh2CentPtInJet(0),
  fh2HiHFPt(0),
  fh2HiHFPtInJet(0),
  fh2CentPtJet(0),
  fh2HiHFPtJet(0),
  fh3CentPtEta(0),
  fh3CentPtEtaInJet(0),
  fh3HiHFPtEta(0),
  fh3HiHFPtEtaInJet(0),
  fh3CentPtEtaJet(0),
  fh3HiHFPtEtaJet(0)
{

  fh2CentPt          = new TH2F*[10];//10 particle types
  fh2HiHFPt          = new TH2F*[10];//10 particle types
  fh2CentPtInJet     = new TH2F*[10];//10 particle types
  fh2HiHFPtInJet     = new TH2F*[10];//10 particle types

  fh3CentPtEta          = new TH3F*[10];//10 particle types
  fh3HiHFPtEta          = new TH3F*[10];//10 particle types
  fh3CentPtEtaInJet     = new TH3F*[10];//10 particle types
  fh3HiHFPtEtaInJet     = new TH3F*[10];//10 particle types
  
  for(Int_t i = 0; i<10; ++i) {
    fh2CentPt[i] = 0;
    fh2HiHFPt[i] = 0;
    fh2CentPtInJet[i] = 0;
    fh2HiHFPtInJet[i] = 0;

    fh3CentPtEta[i] = 0;
    fh3HiHFPtEta[i] = 0;
    fh3CentPtEtaInJet[i] = 0;
    fh3HiHFPtEtaInJet[i] = 0;
  }
}

//----------------------------------------------------------
void anaPFCandidates::Exec(Option_t * /*option*/)
{
   //printf("anaPFCandidates executing\n");
   if(!fInitOutput) CreateOutputObjects();

   //Get event properties
   if(!fHiEvent && !fEvtName.IsNull())
     fHiEvent = dynamic_cast<hiEventContainer*>(fEventObjects->FindObject(fEvtName.Data()));
   if(!fHiEvent) {
     Printf("%s: WARNING: Couldn't locate %s branch",GetName(),fEvtName.Data());
     return;
   }
   //Get particles from which MET will be calculated
   if(!fParticles && !fParticlesName.IsNull()) {
     fParticles = dynamic_cast<TClonesArray*>(fEventObjects->FindObject(fParticlesName.Data()));
   }
   
   if(!fParticles) {
     Printf("%s: WARNING: Couldn't locate %s branch",GetName(),fParticlesName.Data());
     return;
   }

   //Get jet container
   if(!fJetsCont && !fJetsName.IsNull())
     fJetsCont = dynamic_cast<lwJetContainer*>(fEventObjects->FindObject(fJetsName.Data()));

   fh1HiHF->Fill(fHiEvent->GetHiHF());
   fh1Cent->Fill(fHiEvent->GetCentrality());
   
   for (int i = 0; i < fParticles->GetEntriesFast(); i++) {
     particleBase *p = static_cast<particleBase*>(fParticles->At(i));
     if(!p) {
       Printf("%s ERROR: couldn't get particle",GetName());
       continue;
     }
     if(p->Pt()<1e-3) continue;

     Int_t id = p->GetId();
     if(id>-1 && id<10) {
       fh3CentPtEta[id]->Fill(fHiEvent->GetCentrality(),p->Pt(),p->Eta());
       fh3HiHFPtEta[id]->Fill(fHiEvent->GetHiHF(),p->Pt(),p->Eta());
     }
     if(abs(p->Eta())<1.3) {
       if(id>-1 && id<10) {
         fh2CentPt[id]->Fill(fHiEvent->GetCentrality(),p->Pt());
         fh2HiHFPt[id]->Fill(fHiEvent->GetHiHF(),p->Pt());
       }
     }
   }

   // Printf("do jets");
   if(fJetsCont) {
     //Printf("fJetsCont exists");
     Double_t r = fJetsCont->GetJetRadius();
     // Printf("R: %f",r);
     for(Int_t ij = 0; ij<fJetsCont->GetNJets(); ij++) {
       lwJet *jet = fJetsCont->GetJet(ij);
       if(!jet) continue;

       if(jet->Pt() > fMinJetPt && jet->Pt() < fMaxJetPt) {

         if(abs(jet->Eta())<1.3) {
           fh2CentPtJet->Fill(fHiEvent->GetCentrality(),jet->Pt());
           fh2HiHFPtJet->Fill(fHiEvent->GetHiHF(),jet->Pt());
         }
         fh3CentPtEtaJet->Fill(fHiEvent->GetCentrality(),jet->Pt(),jet->Eta());
         fh3HiHFPtEtaJet->Fill(fHiEvent->GetHiHF(),jet->Pt(),jet->Eta());

         // Double_t ptById[10] = {0.};
         for (int i = 0; i < fParticles->GetEntriesFast(); i++) {
           //pfParticle *pf = static_cast<pfParticle*>(fParticles->At(i));
           particleBase *pb = static_cast<particleBase*>(fParticles->At(i));
           if(!pb) continue;
           if(pb->Pt()<1e-3) continue;
           Double_t dr = jet->DeltaR(pb);
           if(dr>r) continue; //only accept particles in cone
           
           //Printf("found particle close to jet. dr = %f",dr);
           Int_t id = pb->GetId();
           if(id>-1 && id<10) {
             if(abs(jet->Eta())<1.3) {
               fh2CentPtInJet[id]->Fill(fHiEvent->GetCentrality(),pb->Pt());
               fh2HiHFPtInJet[id]->Fill(fHiEvent->GetHiHF(),pb->Pt());
             }
             fh3CentPtEtaInJet[id]->Fill(fHiEvent->GetCentrality(),pb->Pt(),jet->Eta());
             fh3HiHFPtEtaInJet[id]->Fill(fHiEvent->GetHiHF(),pb->Pt(),jet->Eta()); 
           }
         }
         /*
         for(Int_t t = 0; t<10; ++t) {
           fh3HiHFPtFracPF[t]->Fill(fHiEvent->GetHiHF(),jet->Pt(),ptById[t]/jet->Pt());
           fh3HiHFPtFracPF[t]->Fill(fHiEvent->GetCentrality(),jet->Pt(),ptById[t]/jet->Pt());
         }
         */
       }
     }
   }
}

//----------------------------------------------------------
void anaPFCandidates::CreateOutputObjects() {

  anaBaseTask::CreateOutputObjects();

  if(!fOutput) {
    Printf("anaPFCandidates: fOutput not present");
    return;
  }

  TString histName = "";
  TString histTitle = "";

  histName = "fh1HiHF";
  histTitle = TString::Format("%s;E_{HF};",histName.Data());
  fh1HiHF = new TH1F(histName.Data(),histTitle.Data(),500,0,10000);
  fOutput->Add(fh1HiHF);

  histName = "fh1Cent";
  histTitle = TString::Format("%s;E_{HF};",histName.Data());
  fh1Cent = new TH1F(histName.Data(),histTitle.Data(),100,0,100);
  fOutput->Add(fh1Cent);
  
  histName = "fh2CentPtJet";
  histTitle = TString::Format("%s;centrality;p_{T,jet}",histName.Data());
  fh2CentPtJet = new TH2F(histName.Data(),histTitle.Data(),100,0.,100.,300.,0.,300.);
  fOutput->Add(fh2CentPtJet);

  histName = "fh2HiHFPtJet";
  histTitle = TString::Format("%s;E_{HF};p_{T,jet}",histName.Data());
  fh2HiHFPtJet = new TH2F(histName.Data(),histTitle.Data(),500,0.,10000.,300.,0.,300.);
  fOutput->Add(fh2HiHFPtJet);

  histName = "fh3CentPtEtaJet";
  histTitle = TString::Format("%s;centrality;p_{T,jet};#eta",histName.Data());
  fh3CentPtEtaJet = new TH3F(histName.Data(),histTitle.Data(),100,0.,100.,300.,0.,300.,100,-5.,5.);
  fOutput->Add(fh3CentPtEtaJet);

  histName = "fh3HiHFPtEtaJet";
  histTitle = TString::Format("%s;E_{HF};p_{T,jet};#eta",histName.Data());
  fh3HiHFPtEtaJet = new TH3F(histName.Data(),histTitle.Data(),500,0.,10000.,300.,0.,300.,100,-5.,5.);
  fOutput->Add(fh3HiHFPtEtaJet);
  
  for (Int_t i = 0; i < 10; i++) {
    histName = TString::Format("fh2CentPt_%d",i);
    histTitle = TString::Format("%s;centrality; #it{p}_{T}",histName.Data());
    fh2CentPt[i] = new TH2F(histName.Data(),histTitle.Data(),100,0,100,300,0.,300);
    fOutput->Add(fh2CentPt[i]);

    histName = TString::Format("fh2HiHFPt_%d",i);
    histTitle = TString::Format("%s;E_{HF}; #it{p}_{T}",histName.Data());
    fh2HiHFPt[i] = new TH2F(histName.Data(),histTitle.Data(),500,0,10000,300,0.,300);
    fOutput->Add(fh2HiHFPt[i]);

    histName = TString::Format("fh2CentPtInJet_%d",i);
    histTitle = TString::Format("%s;centrality; #it{p}_{T}",histName.Data());
    fh2CentPtInJet[i] = new TH2F(histName.Data(),histTitle.Data(),100,0,100,300,0.,300);
    fOutput->Add(fh2CentPtInJet[i]);

    histName = TString::Format("fh2HiHFPtInJet_%d",i);
    histTitle = TString::Format("%s;E_{HF}; #it{p}_{T}",histName.Data());
    fh2HiHFPtInJet[i] = new TH2F(histName.Data(),histTitle.Data(),500,0,10000,300,0.,300);
    fOutput->Add(fh2HiHFPtInJet[i]);


    //
    histName = TString::Format("fh3CentPtEta_%d",i);
    histTitle = TString::Format("%s;centrality; #it{p}_{T}; #eta",histName.Data());
    fh3CentPtEta[i] = new TH3F(histName.Data(),histTitle.Data(),100,0,100,300,0.,300,100,-5.,5.);
    fOutput->Add(fh3CentPtEta[i]);

    histName = TString::Format("fh3HiHFPtEta_%d",i);
    histTitle = TString::Format("%s;E_{HF}; #it{p}_{T}; #eta",histName.Data());
    fh3HiHFPtEta[i] = new TH3F(histName.Data(),histTitle.Data(),500,0,10000,300,0.,300,100,-5.,5.);
    fOutput->Add(fh3HiHFPtEta[i]);

    histName = TString::Format("fh3CentPtEtaInJet_%d",i);
    histTitle = TString::Format("%s;centrality; #it{p}_{T}; #eta_{jet}",histName.Data());
    fh3CentPtEtaInJet[i] = new TH3F(histName.Data(),histTitle.Data(),100,0,100,300,0.,300,100,-5.,5.);
    fOutput->Add(fh3CentPtEtaInJet[i]);

    histName = TString::Format("fh3HiHFPtEtaInJet_%d",i);
    histTitle = TString::Format("%s;E_{HF}; #it{p}_{T}; #eta_{jet}",histName.Data());
    fh3HiHFPtEtaInJet[i] = new TH3F(histName.Data(),histTitle.Data(),500,0,10000,300,0.,300,100,-5.,5.);
    fOutput->Add(fh3HiHFPtEtaInJet[i]);

    /*
    histName = TString::Format("fh3CentJPtFracPF_%d",i);
    histTitle = TString::Format("%s;centrality; #it{p}_{T,jet}; frac",histName.Data());
    fh3CentJPtFracPF[i] = new TH3F(histName.Data(),histTitle.Data(),100,0,100,300,0.,300,100,0.,1.);
    fOutput->Add(fh3CentJPtFracPF[i]);

    histName = TString::Format("fh3HiHFJPtFracPF_%d",i);
    histTitle = TString::Format("%s;E_{HF}; #it{p}_{T,jet}; frac",histName.Data());
    fh3HiHFJPtFracPF[i] = new TH3F(histName.Data(),histTitle.Data(),500,0,5000,300,0.,300,100,0.,1.);
    fOutput->Add(fh3HiHFJPtFracPF[i]);
    */
  }
  
}
