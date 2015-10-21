#include "UserCode/TopFromHeavyIons/interface/anaPuppiProducer.h"

#include "UserCode/TopFromHeavyIons/interface/lwJet.h"
#include "UserCode/TopFromHeavyIons/interface/pfParticle.h"

#include "Math/QuantFuncMathCore.h"
#include "Math/SpecFuncMathCore.h"
#include "Math/ProbFunc.h"
#include "TClass.h"
#include "TMath.h"

ClassImp(anaPuppiProducer)
   
anaPuppiProducer::anaPuppiProducer(const char *name, const char *title) 
:anaBaseTask(name,title),
  fConeRadius(0.3),
  fCentBin(-1),
  fNExLJ(2),
  fMinPtExLJ(20.),
  fdRMaxJet(0.4),
  fAddMetricType(kSumPt),
  fEvtName(""),
  fHiEvent(),
  fPFParticlesName(""),
  fPFParticles(0x0),
  fJetsName(""),
  fJetsCont(0x0),
  fMapEtaRanges(),
  fh2CentMedianAlpha(),
  fh2CentRMSAlpha(),
  fh2CentMedianMetric2(),
  fh2CentRMSMetric2()
{

  //Set default eta ranges
  //[-5,-3,-2.1,2.1,3,5] -> 5 intervals
  fMapEtaRanges[1] = -5.;
  fMapEtaRanges[2] = -3.;
  fMapEtaRanges[3] = -2.1;
  fMapEtaRanges[4] =  2.1;
  fMapEtaRanges[5] =  3.;
  fMapEtaRanges[6] =  5.;
  
}

//________________________________________________________________________
anaPuppiProducer::~anaPuppiProducer() {
  // Destructor
 
}

//----------------------------------------------------------
void anaPuppiProducer::Exec(Option_t * /*option*/)
{
  // printf("anaPuppiProducer executing\n");
   if(!fInitOutput) CreateOutputObjects();

   if(!fEventObjects) {
     Printf("%s: fEventObjects does not exist. Cannot store output",GetName());
     return;
   }
   
   //Get event properties
   if(!fHiEvent && !fEvtName.IsNull())
     fHiEvent = dynamic_cast<hiEventContainer*>(fEventObjects->FindObject(fEvtName.Data()));
   if(!fHiEvent) return;
   
   //Get jet container
   if(!fJetsCont && !fJetsName.IsNull())
     fJetsCont = dynamic_cast<lwJetContainer*>(fEventObjects->FindObject(fJetsName.Data()));
   if(!fJetsCont) return;
   TClonesArray *jets = fJetsCont->GetJets();
   if(!jets) return;
      
   //Get pf particles
   if(!fPFParticles && !fPFParticlesName.IsNull()) {
     fPFParticles = dynamic_cast<TClonesArray*>(fEventObjects->FindObject(fPFParticlesName.Data()));
   }
   if(!fPFParticles) return;

   //Determine centrality bin
   Double_t cent = fHiEvent->GetCentrality();
   if(cent>=0. && cent<10.)       fCentBin = 0;
   else if(cent>=10. && cent<30.) fCentBin = 1;
   else if(cent>=30. && cent<50.) fCentBin = 2;
   else if(cent>=50. && cent<80.) fCentBin = 3;
   else fCentBin = -1;

   //Find signal jets at detector level
   Int_t nSignalJetsDet = 0;
   // Int_t sigDetIds[999];
   Double_t sigDetPhi[999];
   Double_t sigDetEta[999];
   Int_t nj = TMath::Min(fNExLJ,fJetsCont->GetNJets());
   for(Int_t ij = 0; ij<nj; ij++) {
     lwJet *jet = static_cast<lwJet*>(jets->At(ij));
     if(jet->Pt()>fMinPtExLJ) {
       // sigDetIds[nSignalJetsDet] = ij;
       sigDetPhi[nSignalJetsDet] = jet->Phi();
       sigDetEta[nSignalJetsDet] = jet->Eta();
       nSignalJetsDet++;
     }
   }
   
   //pf candidate loop to calculate alpha for each particle
   for (int i = 0; i < fPFParticles->GetEntriesFast(); i++) {
     pfParticle *p1 = static_cast<pfParticle*>(fPFParticles->At(i));
     Double_t var = 0.;
     Double_t var2 = 0.;
     TLorentzVector lsum(0.,0.,0.,0.);
     for (int j = 0; j < fPFParticles->GetEntriesFast(); j++) {
       if(i==j) continue;
       pfParticle *p2 = static_cast<pfParticle*>(fPFParticles->At(j));
       Double_t dr = p1->DeltaR(p2);
       if(dr>fConeRadius) continue;
       var += p2->Pt() /dr/dr;
       if(fAddMetricType==kSumPt)
         var2 += p2->Pt();
       else if(fAddMetricType==kMass)
         lsum+=p2->GetLorentzVector();
     }
     if(var!=0.) var = log(var);
     p1->SetPuppiAlpha(var);
     if(fAddMetricType==kMass)
       var2+=lsum.M();
     p1->SetPuppiMetric2(var2);
   }//particles loop

   //calculation of median and RMS alpha in eta ranges
   std::map<int,double> fMapMedianAlpha;   //median alpha in eta regions
   std::map<int,double> fMapRmsAlpha;      //rms alpha in eta regions
   std::map<int,double> fMapMedianMetric2; //median metric2 in eta regions
   std::map<int,double> fMapRmsMetric2;    //rms metric2 in eta regions
   Int_t neta = (Int_t)fMapEtaRanges.size();
   for(Int_t ieta = 1; ieta<neta; ieta++) {
     static Double_t alphaArrExLJ[9999] = {0.};
     static Double_t metric2ArrExLJ[9999] = {0.};
     Int_t count = 0;

     Double_t etaMin = fMapEtaRanges.at(ieta)+fConeRadius;
     Double_t etaMax = fMapEtaRanges.at(ieta+1)-fConeRadius;
     
     for (int i = 0; i < fPFParticles->GetEntriesFast(); i++) {
       pfParticle *p1 = static_cast<pfParticle*>(fPFParticles->At(i));
       if(!p1) continue;
       
       if(p1->Eta()>=etaMin && p1->Eta()<etaMax) {       
         //check distance to closest signal jet
         Double_t drDet = 999.;
         for(Int_t is = 0; is<nSignalJetsDet; is++) {
           Double_t dPhi = p1->Phi() - sigDetPhi[is];
           Double_t dEta = p1->Eta() - sigDetEta[is];
           dPhi = TVector2::Phi_mpi_pi(dPhi);
           Double_t dr2tmp = dPhi * dPhi + dEta * dEta;
           Double_t drtmp = 0.;
           if(dr2tmp>0.) drtmp = TMath::Sqrt(dr2tmp);
           if(drtmp<drDet) {
             drDet = drtmp;
           }
         }//signal jets

         //Excluding regions close to leading detector-level jet
         if(drDet>fdRMaxJet) {
           alphaArrExLJ[count] = p1->GetPuppiAlpha();
           metric2ArrExLJ[count] = p1->GetPuppiMetric2();
           count++;
         }
       }//eta selection
     }//particles loop
     
     static Int_t indexes[9999] = {-1};//indexes for sorting
     TMath::Sort(count,alphaArrExLJ,indexes);
     Double_t medAlpha = TMath::Median(count,alphaArrExLJ);
     static Int_t indexes2[9999] = {-1};//indexes for sorting
     TMath::Sort(count,metric2ArrExLJ,indexes2);
     Double_t medMetric2 = TMath::Median(count,metric2ArrExLJ);

     //Calculate LHS RMS. LHS defined as all entries up to median
     Int_t nias = TMath::FloorNint((Double_t)count/2.);
     Double_t rmsAlpha   = 0.;
     Double_t rmsMetric2 = 0.;
     for(Int_t ia = 0; ia<count; ia++) { //taking entries starting from nias since sorted from high to low
       Double_t alph = alphaArrExLJ[ia];//indexes[ia]];
       if(alph<medAlpha)
         rmsAlpha += (alph - medAlpha)*(alph - medAlpha);
       
       //      if(alph>medAlpha) Printf("WARNING: alph (%f) larger than medAlpha (%f)",alph,medAlpha);
       
       //metric2
       Double_t metr2 = metric2ArrExLJ[ia];//indexes[ia]];
       if(metr2<medMetric2) 
         rmsMetric2 += (metr2 - medMetric2)*(metr2 - medMetric2);

       //       if(metr2>medMetric2) Printf("WARNING: metr2 (%f) larger than medMetric2 (%f)",metr2,medMetric2);

     }
     if(rmsAlpha>0.) rmsAlpha = TMath::Sqrt(rmsAlpha/((double)nias));
     if(rmsMetric2>0.) rmsMetric2 = TMath::Sqrt(rmsMetric2/((double)nias));

     //Fill histograms, only for mid rapidity
     if(ieta==3) {
       fh2CentMedianAlpha->Fill(cent,medAlpha);
       fh2CentRMSAlpha->Fill(cent,rmsAlpha);
       fh2CentMedianMetric2->Fill(cent,medMetric2);
       fh2CentRMSMetric2->Fill(cent,rmsMetric2);
     }
     fMapMedianAlpha[ieta] = medAlpha;
     fMapRmsAlpha[ieta] = rmsAlpha;
     fMapMedianMetric2[ieta] = medMetric2;
     fMapRmsMetric2[ieta] = rmsMetric2;
     
   }//eta bins
     
   //Set puppi weight for each particle
   for (int i = 0; i < fPFParticles->GetEntriesFast(); i++) {
     pfParticle *p1 = static_cast<pfParticle*>(fPFParticles->At(i));
     Double_t prob = 1.;
     Int_t etaBin = -1;
     for(Int_t ieta = 1; ieta<neta; ++ieta) {
       Double_t etaMin = fMapEtaRanges.at(ieta);
       Double_t etaMax = fMapEtaRanges.at(ieta+1);
       if(p1->Eta()>=etaMin && p1->Eta()<etaMax)
         etaBin = ieta;
     }
     Double_t medAlpha = fMapMedianAlpha[etaBin];
     Double_t rmsAlpha = fMapRmsAlpha[etaBin];
     Double_t chiAlpha = 1.;
     if(rmsAlpha>0.) {
       chiAlpha = (p1->GetPuppiAlpha() - medAlpha) * fabs(p1->GetPuppiAlpha() - medAlpha) / rmsAlpha / rmsAlpha;
       prob = ROOT::Math::chisquared_cdf(chiAlpha,1.);
     }
     p1->SetPuppiWeight(prob);

     //weight metric2
     Double_t medMetric2 = fMapMedianMetric2[etaBin];
     Double_t rmsMetric2 = fMapRmsMetric2[etaBin];
     Double_t prob2 = 1.;
     if(rmsMetric2>0.) {
       Double_t chiMetric2 = (p1->GetPuppiMetric2() - medMetric2) * fabs(p1->GetPuppiMetric2() - medMetric2) / rmsMetric2 / rmsMetric2;
       Double_t chii = chiAlpha + chiMetric2;
       prob = ROOT::Math::chisquared_cdf(chii,2.);
       prob2= ROOT::Math::chisquared_cdf(chiMetric2,1.);
     }
     p1->SetPuppiWeight2(prob);
     p1->SetPuppiWeight3(prob2);
   }
}

//----------------------------------------------------------
void anaPuppiProducer::CreateOutputObjects() {

  anaBaseTask::CreateOutputObjects();

  if(!fOutput) {
    Printf("anaPuppiProducer: fOutput not present");
    return;
  }

  fh2CentMedianAlpha = new TH2F("fh2CentMedianAlpha","fh2CentMedianAlpha;centrality (%);med{#alpha}",10,0,100,40,0,20);
  fOutput->Add(fh2CentMedianAlpha);

  fh2CentRMSAlpha = new TH2F("fh2CentRMSAlpha","fh2CentRMSAlpha;centrality (%);RMS{#alpha}",10,0,100,40,0,4);
  fOutput->Add(fh2CentRMSAlpha);

  fh2CentMedianMetric2 = new TH2F("fh2CentMedianMetric2","fh2CentMedianMetric2;centrality (%);med{metric2}",10,0,100,100,0,100);
  fOutput->Add(fh2CentMedianMetric2);

  fh2CentRMSMetric2 = new TH2F("fh2CentRMSMetric2","fh2CentRMSMetric2;centrality (%);RMS{metric2}",10,0,100,30,0,30);
  fOutput->Add(fh2CentRMSMetric2);
}
