#include "UserCode/TopFromHeavyIons/interface/anaPuppiParticles.h"

#include "UserCode/TopFromHeavyIons/interface/pfParticle.h"
#include "UserCode/TopFromHeavyIons/interface/lwJetContainer.h"

#include "TLorentzVector.h"

#include "TClass.h"

ClassImp(anaPuppiParticles)
   
anaPuppiParticles::anaPuppiParticles(const char *name, const char *title) 
:anaBaseTask(name,title),
  fParticlesName(""),
  fParticles(0x0),
  fJetsName(""),
  fJetsCont(0x0),
  fMinPt(0.),
  fh2AlphaCentAll(),
  fh2AlphaCentUE(),
  fh2AlphaCentJet(),
  fh2Metric2CentAll(),
  fh2Metric2CentUE(),
  fh2Metric2CentJet(),
  fh2WeightCentAll(),
  fh2WeightCentUE(),
  fh2WeightCentJet(),
  fh2Weight2CentAll(),
  fh2Weight2CentUE(),
  fh2Weight2CentJet(),
  fh2Weight3CentAll(),
  fh2Weight3CentUE(),
  fh2Weight3CentJet(),
  fh2Weight4CentAll(),
  fh2Weight4CentUE(),
  fh2Weight4CentJet(),
  fh3AlphaMetric2CentAll(),
  fh3AlphaMetric2CentUE(),
  fh3AlphaMetric2CentJet()
{

}

//----------------------------------------------------------
void anaPuppiParticles::Exec(Option_t * /*option*/)
{
   //printf("anaPuppiParticles executing\n");
   if(!fInitOutput) CreateOutputObjects();

   //Get objects from event

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
   if(!fJetsCont || !(fJetsCont->GetJets())) return;
   
   Double_t cent = fHiEvent->GetCentrality();

   //Find signal jets
   Int_t nSignalJets = 0;
   // Int_t sigDetIds[999];
   Double_t sigDetPhi[999];
   Double_t sigDetEta[999];
   for(Int_t ij = 0; ij<fJetsCont->GetNJets(); ij++) {
     lwJet *jet = fJetsCont->GetJet(ij);
     if(jet->Pt()>20.) {
       // sigDetIds[nSignalJets] = ij;
       sigDetPhi[nSignalJets] = jet->Phi();
       sigDetEta[nSignalJets] = jet->Eta();
       nSignalJets++;
     }
   }
   
   for (int i = 0; i < fParticles->GetEntriesFast(); i++) {
     pfParticle *p = static_cast<pfParticle*>(fParticles->At(i));
     if(!p) {
       Printf("%s ERROR: couldn't get particle",GetName());
       continue;
     }

     //only QA particle at midrapidity for now. TODO
     if(abs(p->Eta())>2.1) continue;

     //check distance to closest signal jet
     Double_t drMin = 999.;
     Bool_t isInUE = kTRUE;
     for(Int_t is = 0; is<nSignalJets; is++) {
       Double_t dPhi = p->Phi() - sigDetPhi[is];
       Double_t dEta = p->Eta() - sigDetEta[is];
       dPhi = TVector2::Phi_mpi_pi(dPhi);
       Double_t dr2tmp = dPhi * dPhi + dEta * dEta;
       Double_t drtmp = 0.;
       if(dr2tmp>0.) drtmp = TMath::Sqrt(dr2tmp);
       if(drtmp<drMin) {
         drMin = drtmp;
       }
       if(drtmp<0.8) isInUE = kFALSE;
     }//signal jets


     //store puppi metrics and weights for different regions
     fh2AlphaCentAll->Fill(cent,p->GetPuppiAlpha());
     fh2Metric2CentAll->Fill(cent,p->GetPuppiMetric2());

     if((p->Pt()*p->GetPuppiWeight()) > fMinPt) fh2WeightCentAll->Fill(cent,p->GetPuppiWeight());
     fh2Weight2CentAll->Fill(cent,p->GetPuppiWeight2());
     fh2Weight3CentAll->Fill(cent,p->GetPuppiWeight3());
     fh2Weight4CentAll->Fill(cent,p->GetPuppiWeight3()*p->GetPuppiWeight());
     fh3AlphaMetric2CentAll->Fill(cent,p->GetPuppiAlpha(),p->GetPuppiMetric2());
     if(drMin<0.4) { //jetty region
       fh2AlphaCentJet->Fill(cent,p->GetPuppiAlpha());
       fh2Metric2CentJet->Fill(cent,p->GetPuppiMetric2());
       if((p->Pt()*p->GetPuppiWeight()) > fMinPt) fh2WeightCentJet->Fill(cent,p->GetPuppiWeight());
       fh2Weight2CentJet->Fill(cent,p->GetPuppiWeight2());
       fh2Weight3CentJet->Fill(cent,p->GetPuppiWeight3());
       fh2Weight4CentJet->Fill(cent,p->GetPuppiWeight3()*p->GetPuppiWeight());
       fh3AlphaMetric2CentJet->Fill(cent,p->GetPuppiAlpha(),p->GetPuppiMetric2());
     }

     //UE region
     if(isInUE) {
       fh2AlphaCentUE->Fill(cent,p->GetPuppiAlpha());
       fh2Metric2CentUE->Fill(cent,p->GetPuppiMetric2());
       if((p->Pt()*p->GetPuppiWeight()) > fMinPt) fh2WeightCentUE->Fill(cent,p->GetPuppiWeight());
       fh2Weight2CentUE->Fill(cent,p->GetPuppiWeight2());
       fh2Weight3CentUE->Fill(cent,p->GetPuppiWeight3());
       fh2Weight4CentUE->Fill(cent,p->GetPuppiWeight3()*p->GetPuppiWeight());
       fh3AlphaMetric2CentUE->Fill(cent,p->GetPuppiAlpha(),p->GetPuppiMetric2());
     }
   }
}

//----------------------------------------------------------
void anaPuppiParticles::CreateOutputObjects() {

  anaBaseTask::CreateOutputObjects();

  if(!fOutput) {
    Printf("anaPuppiParticles: fOutput not present");
    return;
  }

  fh2AlphaCentAll = new TH2F("fh2AlphaCentAll","fh2AlphaCentAll;centrality;#alpha",100,0,100,40,0,20.);
  fOutput->Add(fh2AlphaCentAll);
  
  fh2AlphaCentUE = new TH2F("fh2AlphaCentUE","fh2AlphaCentUE;centrality;#alpha",100,0,100,40,0,20.);
  fOutput->Add(fh2AlphaCentUE);
  
  fh2AlphaCentJet = new TH2F("fh2AlphaCentJet","fh2AlphaCentJet;centrality;#alpha",100,0,100,40,0,20.);
  fOutput->Add(fh2AlphaCentJet);

  fh2Metric2CentAll = new TH2F("fh2Metric2CentAll","fh2Metric2CentAll;centrality;metric2",100,0,100,200,0.,200.);
  fOutput->Add(fh2Metric2CentAll);
  
  fh2Metric2CentUE = new TH2F("fh2Metric2CentUE","fh2Metric2CentUE;centrality;metric2",100,0,100,200,0.,200.);
  fOutput->Add(fh2Metric2CentUE);
  
  fh2Metric2CentJet = new TH2F("fh2Metric2CentJet","fh2Metric2CentJet;centrality;metric2",100,0,100,100,0.,200.);
  fOutput->Add(fh2Metric2CentJet);

  fh2WeightCentAll = new TH2F("fh2WeightCentAll","fh2WeightCentAll;centrality;weight",100,0,100,102,0.,1.02);
  fOutput->Add(fh2WeightCentAll);
  
  fh2WeightCentUE = new TH2F("fh2WeightCentUE","fh2WeightCentUE;centrality;weight",100,0,100,102,0.,1.02);
  fOutput->Add(fh2WeightCentUE);
  
  fh2WeightCentJet = new TH2F("fh2WeightCentJet","fh2WeightCentJet;centrality;weight",100,0,100,102,0.,1.02);
  fOutput->Add(fh2WeightCentJet);

  fh2Weight2CentAll = new TH2F("fh2Weight2CentAll","fh2Weight2CentAll;centrality;weight",100,0,100,102,0.,1.02);
  fOutput->Add(fh2Weight2CentAll);
  
  fh2Weight2CentUE = new TH2F("fh2Weight2CentUE","fh2Weight2CentUE;centrality;weight",100,0,100,102,0.,1.02);
  fOutput->Add(fh2Weight2CentUE);
  
  fh2Weight2CentJet = new TH2F("fh2Weight2CentJet","fh2Weight2CentJet;centrality;weight",100,0,100,102,0.,1.02);
  fOutput->Add(fh2Weight2CentJet);

  fh2Weight3CentAll = new TH2F("fh2Weight3CentAll","fh2Weight3CentAll;centrality;weight",100,0,100,102,0.,1.02);
  fOutput->Add(fh2Weight3CentAll);
  
  fh2Weight3CentUE = new TH2F("fh2Weight3CentUE","fh2Weight3CentUE;centrality;weight",100,0,100,102,0.,1.02);
  fOutput->Add(fh2Weight3CentUE);
  
  fh2Weight3CentJet = new TH2F("fh2Weight3CentJet","fh2Weight3CentJet;centrality;weight",100,0,100,102,0.,1.02);
  fOutput->Add(fh2Weight3CentJet);

  fh2Weight4CentAll = new TH2F("fh2Weight4CentAll","fh2Weight4CentAll;centrality;weight",100,0,100,102,0.,1.02);
  fOutput->Add(fh2Weight4CentAll);
  
  fh2Weight4CentUE = new TH2F("fh2Weight4CentUE","fh2Weight4CentUE;centrality;weight",100,0,100,102,0.,1.02);
  fOutput->Add(fh2Weight4CentUE);
  
  fh2Weight4CentJet = new TH2F("fh2Weight4CentJet","fh2Weight4CentJet;centrality;weight",100,0,100,102,0.,1.02);
  fOutput->Add(fh2Weight4CentJet);

  fh3AlphaMetric2CentAll = new TH3F("fh3AlphaMetric2CentAll","fh3AlphaMetric2CentAll;centrality;#alpha;metric2",100,0,100,40,0,20,200,0.,200.);
  fOutput->Add(fh3AlphaMetric2CentAll);
  
  fh3AlphaMetric2CentUE = new TH3F("fh3AlphaMetric2CentUE","fh3AlphaMetric2CentUE;centrality;#alpha;metric2",100,0,100,40,0,20,200,0.,200.);
  fOutput->Add(fh3AlphaMetric2CentUE);
  
  fh3AlphaMetric2CentJet = new TH3F("fh3AlphaMetric2CentJet","fh3AlphaMetric2CentJet;centrality;#alpha;metric2",100,0,100,40,0,20,100,0.,200.);
  fOutput->Add(fh3AlphaMetric2CentJet);
  
}
