#include "UserCode/TopFromHeavyIons/interface/anaPFvsCaloJet.h"

#include "UserCode/TopFromHeavyIons/interface/lwJet.h"

ClassImp(anaPFvsCaloJet)
   
anaPFvsCaloJet::anaPFvsCaloJet(const char *name, const char *title) 
:anaBaseTask(name,title),
  fJetsName(""),
  fJetsCont(0x0),
  fJets2Name(""),
  fJets2Cont(0x0),
  fhEventSel(),
  fhCentrality(),
  fhNPV(),
  fh2PtEtaNoMatching(),
  fh3PtEtaPhiNotMatched(),
  fh3PtEtaPhiMatched(),
  fh3PtTrueNPVDeltaPt(),
  fh3PtTrueNPVDeltaPtRel(),
  fh3PtTrueNPVScalePt(),
  fh3PtTruePtSubNPV(),
  fh3PtTrueEtaDeltaPt(),
  fh3PtTrueEtaDeltaPtRel(),
  fh3PtTrueEtaScalePt(),
  fh3PtTruePtSubEta()
{

}

//----------------------------------------------------------
void anaPFvsCaloJet::Exec(Option_t * /*option*/)
{
 
  anaBaseTask::Exec();
  if(!SelectEvent()) return;


  //printf("anaPFvsCaloJet executing\n");
  if(!fInitOutput) CreateOutputObjects();

  //Get event properties
  if(fHiEvent) {
    if(abs(fHiEvent->GetVz())>15.) return;
  }
  
  //Get PF jets
  if(!fJetsCont && !fJetsName.IsNull())
    fJetsCont = dynamic_cast<lwJetContainer*>(fEventObjects->FindObject(fJetsName.Data()));
  if(!fJetsCont) return;

   //Get calo jets
   if(!fJets2Cont && !fJets2Name.IsNull())
     fJets2Cont = dynamic_cast<lwJetContainer*>(fEventObjects->FindObject(fJets2Name.Data()));
   if(!fJets2Cont) return;

   const Int_t nJets1 = fJetsCont->GetNJets();
   const Int_t nJets2 = fJets2Cont->GetNJets();
   if(nJets1==0 || nJets2==0) {
     //Printf("nJets1: %d  nJets2: %d",nJets1,nJets2);
     return;
   }


   int npv = 1;
   if(fHiEvent) npv = fHiEvent->GetNPV();
   fhNPV->Fill(npv);

   fhEventSel->Fill(0.5);

   float weight = 1.;
   if(fHiEvent) {
     if(fHiEvent->GetWeight()>0.) weight = fHiEvent->GetWeight();
   }
   
   for(Int_t ij = 0; ij<fJetsCont->GetNJets(); ij++) {
     lwJet *jet1 = fJetsCont->GetJet(ij);
     if(!jet1) continue;

     fh2PtEtaNoMatching->Fill(jet1->Pt(),jet1->Eta(),weight);
          
     int id = jet1->GetMatchId1();
     lwJet *jet2 = fJets2Cont->GetJet(id);
     if(!jet2) {
       fh3PtEtaPhiNotMatched->Fill(jet1->Pt(),jet1->Eta(),jet1->Phi(),weight);
       continue;
     }
       
     fh3PtEtaPhiMatched->Fill(jet1->Pt(),jet1->Eta(),jet1->Phi(),weight);
     
     double dpt = jet2->Pt()-jet1->Pt();

     fh3PtTrueEtaDeltaPt->Fill(jet1->Pt(),jet1->Eta(),dpt,weight);
     if(jet1->Pt()>0.) {
       fh3PtTrueEtaDeltaPtRel->Fill(jet1->Pt(),jet1->Eta(),dpt/jet1->Pt(),weight);
       fh3PtTrueEtaScalePt->Fill(jet1->Pt(),jet1->Eta(),jet2->Pt()/jet1->Pt(),weight);
     }
     fh3PtTruePtSubEta->Fill(jet1->Pt(),jet2->Pt(),jet1->Eta(),weight);
     
     if(abs(jet1->Eta())<1.3) {
       
       fh3PtTrueNPVDeltaPt->Fill(jet1->Pt(),npv,dpt,weight);
       if(jet1->Pt()>0.) {
         fh3PtTrueNPVDeltaPtRel->Fill(jet1->Pt(),npv,dpt/jet1->Pt(),weight);
         fh3PtTrueNPVScalePt->Fill(jet1->Pt(),npv,jet2->Pt()/jet1->Pt(),weight);
       }
       fh3PtTruePtSubNPV->Fill(jet1->Pt(),jet2->Pt(),npv,weight);
     }
   }

}
//----------------------------------------------------------
void anaPFvsCaloJet::CreateOutputObjects() {

  anaBaseTask::CreateOutputObjects();

  if(!fOutput) {
    Printf("anaPFvsCaloJet: fOutput not present");
    return;
  }

  TString histTitle;
  TString histName;

  const Int_t nBinsPtDet  = 200;
  const Double_t minPtDet =  0.;
  const Double_t maxPtDet = 400.;

  const Int_t nBinsPtPart  = 200;
  const Double_t minPtPart = 0.;
  const Double_t maxPtPart = 400.;

  const Int_t nBinsDPt  = 200;
  const Double_t minDPt = -100.;
  const Double_t maxDPt = 100.;

  const Int_t nBinsDPtRel  = 600;
  const Double_t minDPtRel = -3.;
  const Double_t maxDPtRel = 3.;

  const Int_t nBinsScalePt  = 300;
  const Double_t minScalePt = 0.;
  const Double_t maxScalePt = 3.;

  const Int_t nBinsNPV = 100;
  const Double_t minNPV = 0.;
  const Double_t maxNPV = 100.;

  const Int_t nBinsEta = 100;
  const Double_t minEta = -5.;
  const Double_t maxEta = 5.;

  const Int_t nBinsPhi = 72;
  const Double_t minPhi = -TMath::Pi();
  const Double_t maxPhi = TMath::Pi();

  fhEventSel = new TH1F("fhEventSel","fhEventSel",10,0,10);
  fOutput->Add(fhEventSel);

  fhCentrality = new TH1F("fhCentrality","fhCentrality",100,0,100);
  fOutput->Add(fhCentrality);

  fhNPV = new TH1F("fhNPV","fhNPV",nBinsNPV,minNPV,maxNPV);
  fOutput->Add(fhNPV);

  histName = "fh2PtEtaNoMatching";
  histTitle = Form("%s;#it{p}_{T,PF};#eta;",histName.Data());
  fh2PtEtaNoMatching = new TH2F(histName.Data(),histTitle.Data(),nBinsPtPart,minPtPart,maxPtPart,nBinsEta,minEta,maxEta);
  fOutput->Add(fh2PtEtaNoMatching);

  histName = "fh3PtEtaPhiNotMatched";
  histTitle = Form("%s;#it{p}_{T,PF};#eta;",histName.Data());
  fh3PtEtaPhiNotMatched = new TH3F(histName.Data(),histTitle.Data(),nBinsPtPart,minPtPart,maxPtPart,nBinsEta,minEta,maxEta,nBinsPhi,minPhi,maxPhi);
  fOutput->Add(fh3PtEtaPhiNotMatched);

  histName = "fh3PtEtaPhiMatched";
  histTitle = Form("%s;#it{p}_{T,PF};#eta;",histName.Data());
  fh3PtEtaPhiMatched = new TH3F(histName.Data(),histTitle.Data(),nBinsPtPart,minPtPart,maxPtPart,nBinsEta,minEta,maxEta,nBinsPhi,minPhi,maxPhi);
  fOutput->Add(fh3PtEtaPhiMatched);
  
  histName = Form("fh3PtTrueNPVDeltaPt");
  histTitle = Form("%s;#it{p}_{T,PF};NPV;#it{p}_{T,calo}-#it{p}_{T,PF}",histName.Data());
  fh3PtTrueNPVDeltaPt = new TH3F(histName.Data(),histTitle.Data(),nBinsPtPart,minPtPart,maxPtPart,nBinsNPV,minNPV,maxNPV,nBinsDPt,minDPt,maxDPt);
  fOutput->Add(fh3PtTrueNPVDeltaPt);

  histName = Form("fh3PtTrueNPVDeltaPtRel");
  histTitle = Form("%s;#it{p}_{T,PF};NPV;(#it{p}_{T,calo}-#it{p}_{T,PF})/#it{p}_{T,PF}",histName.Data());
  fh3PtTrueNPVDeltaPtRel = new TH3F(histName.Data(),histTitle.Data(),nBinsPtPart,minPtPart,maxPtPart,nBinsNPV,minNPV,maxNPV,nBinsDPtRel,minDPtRel,maxDPtRel);
  fOutput->Add(fh3PtTrueNPVDeltaPtRel);

  histName = Form("fh3PtTrueNPVScalePt");
  histTitle = Form("%s;#it{p}_{T,PF};NPV;#it{p}_{T,calo}/#it{p}_{T,PF}",histName.Data());
  fh3PtTrueNPVScalePt = new TH3F(histName.Data(),histTitle.Data(),nBinsPtPart,minPtPart,maxPtPart,nBinsNPV,minNPV,maxNPV,nBinsScalePt,minScalePt,maxScalePt);
  fOutput->Add(fh3PtTrueNPVScalePt);

  histName = Form("fh3PtTruePtSubNPV");
  histTitle = Form("%s;#it{p}_{T,PF};#it{p}_{T,calo};NPV",histName.Data());
  fh3PtTruePtSubNPV = new TH3F(histName.Data(),histTitle.Data(),nBinsPtPart,minPtPart,maxPtPart,nBinsPtDet,minPtDet,maxPtDet,nBinsNPV,minNPV,maxNPV);
  fOutput->Add(fh3PtTruePtSubNPV);

  histName = Form("fh3PtTrueEtaDeltaPt");
  histTitle = Form("%s;#it{p}_{T,PF};Eta;#it{p}_{T,calo}-#it{p}_{T,PF}",histName.Data());
  fh3PtTrueEtaDeltaPt = new TH3F(histName.Data(),histTitle.Data(),nBinsPtPart,minPtPart,maxPtPart,nBinsEta,minEta,maxEta,nBinsDPt,minDPt,maxDPt);
  fOutput->Add(fh3PtTrueEtaDeltaPt);

  histName = Form("fh3PtTrueEtaDeltaPtRel");
  histTitle = Form("%s;#it{p}_{T,PF};Eta;(#it{p}_{T,calo}-#it{p}_{T,PF})/#it{p}_{T,PF}",histName.Data());
  fh3PtTrueEtaDeltaPtRel = new TH3F(histName.Data(),histTitle.Data(),nBinsPtPart,minPtPart,maxPtPart,nBinsEta,minEta,maxEta,nBinsDPtRel,minDPtRel,maxDPtRel);
  fOutput->Add(fh3PtTrueEtaDeltaPtRel);

  histName = Form("fh3PtTrueEtaScalePt");
  histTitle = Form("%s;#it{p}_{T,PF};Eta;#it{p}_{T,calo}/#it{p}_{T,PF}",histName.Data());
  fh3PtTrueEtaScalePt = new TH3F(histName.Data(),histTitle.Data(),nBinsPtPart,minPtPart,maxPtPart,nBinsEta,minEta,maxEta,nBinsScalePt,minScalePt,maxScalePt);
  fOutput->Add(fh3PtTrueEtaScalePt);

  histName = Form("fh3PtTruePtSubEta");
  histTitle = Form("%s;#it{p}_{T,PF};#it{p}_{T,calo};Eta",histName.Data());
  fh3PtTruePtSubEta = new TH3F(histName.Data(),histTitle.Data(),nBinsPtPart,minPtPart,maxPtPart,nBinsPtDet,minPtDet,maxPtDet,nBinsEta,minEta,maxEta);
  fOutput->Add(fh3PtTruePtSubEta);
  
}
