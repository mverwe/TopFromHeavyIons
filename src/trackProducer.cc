//
// producer of reconstructed tracks
//

#include "UserCode/TopFromHeavyIons/interface/trackProducer.h"
#include "UserCode/TopFromHeavyIons/interface/particleBase.h"

ClassImp(trackProducer)

//__________________________________________________________
trackProducer::trackProducer() :
inputBase("trackProducer"),
  fTracksName("tracks"),
  fTracks(0x0),
  fForestTracks(),
  fSelectHighPurity(kTRUE)
{
  //default constructor
}

//__________________________________________________________
trackProducer::trackProducer(const char *name) :
  inputBase(name),
  fTracksName("tracks"),
  fTracks(0x0),
  fForestTracks(),
  fSelectHighPurity(kTRUE)
{
  //standard constructor
}

//__________________________________________________________
Bool_t trackProducer::Init() {

  if(!inputBase::Init()) return kFALSE;
  
  if(fInputMode==hiForest) {
    if(fChain->GetBranch("N"))
      fChain->SetBranchAddress("N", &fForestTracks.N, &fForestTracks.b_N);
    if(fChain->GetBranch("nVtx"))
      fChain->SetBranchAddress("nVtx", &fForestTracks.nVtx, &fForestTracks.b_nVtx);
    if(fChain->GetBranch("nTrk"))
      fChain->SetBranchAddress("nTrk", &fForestTracks.nTrk, &fForestTracks.b_nTrk);
   fChain->SetBranchAddress("maxPtVtx", &fForestTracks.maxPtVtx, &fForestTracks.b_maxPtVtx);
   fChain->SetBranchAddress("maxMultVtx", &fForestTracks.maxMultVtx, &fForestTracks.b_maxMultVtx);
   fChain->SetBranchAddress("nTrkVtx", fForestTracks.nTrkVtx, &fForestTracks.b_nTrkVtx);
   fChain->SetBranchAddress("normChi2Vtx", fForestTracks.normChi2Vtx, &fForestTracks.b_normChi2Vtx);
   fChain->SetBranchAddress("sumPtVtx", fForestTracks.sumPtVtx, &fForestTracks.b_sumPtVtx);
   fChain->SetBranchAddress("xVtx", fForestTracks.xVtx, &fForestTracks.b_xVtx);
   fChain->SetBranchAddress("yVtx", fForestTracks.yVtx, &fForestTracks.b_yVtx);
   fChain->SetBranchAddress("zVtx", fForestTracks.zVtx, &fForestTracks.b_zVtx);
   fChain->SetBranchAddress("xVtxErr", fForestTracks.xVtxErr, &fForestTracks.b_xVtxErr);
   fChain->SetBranchAddress("yVtxErr", fForestTracks.yVtxErr, &fForestTracks.b_yVtxErr);
   fChain->SetBranchAddress("zVtxErr", fForestTracks.zVtxErr, &fForestTracks.b_zVtxErr);
   fChain->SetBranchAddress("vtxDist2D", fForestTracks.vtxDist2D, &fForestTracks.b_vtxDist2D);
   fChain->SetBranchAddress("vtxDist2DErr", fForestTracks.vtxDist2DErr, &fForestTracks.b_vtxDist2DErr);
   fChain->SetBranchAddress("vtxDist2DSig", fForestTracks.vtxDist2DSig, &fForestTracks.b_vtxDist2DSig);
   fChain->SetBranchAddress("vtxDist3DErr", fForestTracks.vtxDist3DErr, &fForestTracks.b_vtxDist3DErr);
   fChain->SetBranchAddress("vtxDist3DSig", fForestTracks.vtxDist3DSig, &fForestTracks.b_vtxDist3DSig);
   fChain->SetBranchAddress("nVtxSim", &fForestTracks.nVtxSim, &fForestTracks.b_nVtxSim);
   fChain->SetBranchAddress("xVtxSim", fForestTracks.xVtxSim, &fForestTracks.b_xVtxSim);
   fChain->SetBranchAddress("yVtxSim", fForestTracks.yVtxSim, &fForestTracks.b_yVtxSim);
   fChain->SetBranchAddress("zVtxSim", fForestTracks.zVtxSim, &fForestTracks.b_zVtxSim);
   fChain->SetBranchAddress("trkPt", fForestTracks.trkPt, &fForestTracks.b_trkPt);
   fChain->SetBranchAddress("trkPtError", fForestTracks.trkPtError, &fForestTracks.b_trkPtError);
   fChain->SetBranchAddress("trkNHit", fForestTracks.trkNHit, &fForestTracks.b_trkNHit);
   fChain->SetBranchAddress("trkNlayer", fForestTracks.trkNlayer, &fForestTracks.b_trkNlayer);
   fChain->SetBranchAddress("trkEta", fForestTracks.trkEta, &fForestTracks.b_trkEta);
   fChain->SetBranchAddress("trkPhi", fForestTracks.trkPhi, &fForestTracks.b_trkPhi);
   fChain->SetBranchAddress("trkCharge", fForestTracks.trkCharge, &fForestTracks.b_trkCharge);
   fChain->SetBranchAddress("trkNVtx", fForestTracks.trkNVtx, &fForestTracks.b_trkNVtx);
   fChain->SetBranchAddress("nTrkTimesnVtx", &fForestTracks.nTrkTimesnVtx, &fForestTracks.b_nTrkTimesnVtx);
   fChain->SetBranchAddress("trkAssocVtx", fForestTracks.trkAssocVtx, &fForestTracks.b_trkAssocVtx);
   fChain->SetBranchAddress("highPurity", fForestTracks.highPurity, &fForestTracks.b_highPurity);
   //fChain->SetBranchAddress("highPuritySetWithPV", fForestTracks.highPuritySetWithPV, &fForestTracks.b_highPuritySetWithPV);
   fChain->SetBranchAddress("trkChi2", fForestTracks.trkChi2, &fForestTracks.b_trkChi2);
   fChain->SetBranchAddress("trkNdof", fForestTracks.trkNdof, &fForestTracks.b_trkNdof);
   fChain->SetBranchAddress("trkDxy1", fForestTracks.trkDxy1, &fForestTracks.b_trkDxy1);
   fChain->SetBranchAddress("trkDxyError1", fForestTracks.trkDxyError1, &fForestTracks.b_trkDxyError1);
   fChain->SetBranchAddress("trkDz1", fForestTracks.trkDz1, &fForestTracks.b_trkDz1);
   fChain->SetBranchAddress("trkDzError1", fForestTracks.trkDzError1, &fForestTracks.b_trkDzError1);
   fChain->SetBranchAddress("trkDzError2", fForestTracks.trkDzError2, &fForestTracks.b_trkDzError2);
   fChain->SetBranchAddress("trkDxy2", fForestTracks.trkDxy2, &fForestTracks.b_trkDxy2);
   fChain->SetBranchAddress("trkDz2", fForestTracks.trkDz2, &fForestTracks.b_trkDz2);
   fChain->SetBranchAddress("trkDxyError2", fForestTracks.trkDxyError2, &fForestTracks.b_trkDxyError2);
   fChain->SetBranchAddress("trkFake", fForestTracks.trkFake, &fForestTracks.b_trkFake);
   fChain->SetBranchAddress("trkAlgo", fForestTracks.trkAlgo, &fForestTracks.b_trkAlgo);
   fChain->SetBranchAddress("pfType", fForestTracks.pfType, &fForestTracks.b_pfType);
   fChain->SetBranchAddress("pfCandPt", fForestTracks.pfCandPt, &fForestTracks.b_pfCandPt);
   fChain->SetBranchAddress("pfEcal", fForestTracks.pfEcal, &fForestTracks.b_pfEcal);
   fChain->SetBranchAddress("pfHcal", fForestTracks.pfHcal, &fForestTracks.b_pfHcal);
   /* //MVnote: does not exist in modern forest
   fChain->SetBranchAddress("pfSumEcal", fForestTracks.pfSumEcal, &fForestTracks.b_pfSumEcal);
   fChain->SetBranchAddress("pfSumHcal", fForestTracks.pfSumHcal, &fForestTracks.b_pfSumHcal);
   fChain->SetBranchAddress("trkStatus", fForestTracks.trkStatus, &fForestTracks.b_trkStatus);
   fChain->SetBranchAddress("trkPId", fForestTracks.trkPId, &fForestTracks.b_trkPId);
   fChain->SetBranchAddress("trkMPId", fForestTracks.trkMPId, &fForestTracks.b_trkMPId);
   fChain->SetBranchAddress("trkGMPId", fForestTracks.trkGMPId, &fForestTracks.b_trkGMPId);
   fChain->SetBranchAddress("nParticle", &fForestTracks.nParticle, &fForestTracks.b_nParticle);
   fChain->SetBranchAddress("pStatus", fForestTracks.pStatus, &fForestTracks.b_pStatus);
   fChain->SetBranchAddress("pPId", fForestTracks.pPId, &fForestTracks.b_pPId);
   fChain->SetBranchAddress("pEta", fForestTracks.pEta, &fForestTracks.b_pEta);
   fChain->SetBranchAddress("pPhi", fForestTracks.pPhi, &fForestTracks.b_pPhi);
   fChain->SetBranchAddress("pPt", fForestTracks.pPt, &fForestTracks.b_pPt);
   fChain->SetBranchAddress("pAcc", fForestTracks.pAcc, &fForestTracks.b_pAcc);
   fChain->SetBranchAddress("pAccPair", fForestTracks.pAccPair, &fForestTracks.b_pAccPair);
   fChain->SetBranchAddress("pNRec", fForestTracks.pNRec, &fForestTracks.b_pNRec);
   fChain->SetBranchAddress("pNHit", fForestTracks.pNHit, &fForestTracks.b_pNHit);
   fChain->SetBranchAddress("mtrkPt", fForestTracks.mtrkPt, &fForestTracks.b_mtrkPt);
   fChain->SetBranchAddress("mtrkPtError", fForestTracks.mtrkPtError, &fForestTracks.b_mtrkPtError);
   fChain->SetBranchAddress("mtrkNHit", fForestTracks.mtrkNHit, &fForestTracks.b_mtrkNHit);
   fChain->SetBranchAddress("mtrkNlayer", fForestTracks.mtrkNlayer, &fForestTracks.b_mtrkNlayer);
   fChain->SetBranchAddress("mtrkNlayer3D", fForestTracks.mtrkNlayer3D, &fForestTracks.b_mtrkNlayer3D);
   fChain->SetBranchAddress("mtrkQual", fForestTracks.mtrkQual, &fForestTracks.b_mtrkQual);
   fChain->SetBranchAddress("mtrkChi2", fForestTracks.mtrkChi2, &fForestTracks.b_mtrkChi2);
   fChain->SetBranchAddress("mtrkNdof", fForestTracks.mtrkNdof, &fForestTracks.b_mtrkNdof);
   fChain->SetBranchAddress("mtrkDz1", fForestTracks.mtrkDz1, &fForestTracks.b_mtrkDz1);
   fChain->SetBranchAddress("mtrkDzError1", fForestTracks.mtrkDzError1, &fForestTracks.b_mtrkDzError1);
   fChain->SetBranchAddress("mtrkDxy1", fForestTracks.mtrkDxy1, &fForestTracks.b_mtrkDxy1);
   fChain->SetBranchAddress("mtrkDxyError1", fForestTracks.mtrkDxyError1, &fForestTracks.b_mtrkDxyError1);
   fChain->SetBranchAddress("mtrkDz2", fForestTracks.mtrkDz2, &fForestTracks.b_mtrkDz2);
   fChain->SetBranchAddress("mtrkDzError2", fForestTracks.mtrkDzError2, &fForestTracks.b_mtrkDzError2);
   fChain->SetBranchAddress("mtrkDxy2", fForestTracks.mtrkDxy2, &fForestTracks.b_mtrkDxy2);
   fChain->SetBranchAddress("mtrkDxyError2", fForestTracks.mtrkDxyError2, &fForestTracks.b_mtrkDxyError2);
   fChain->SetBranchAddress("mtrkAlgo", fForestTracks.mtrkAlgo, &fForestTracks.b_mtrkAlgo);
   fChain->SetBranchAddress("mtrkPfType", fForestTracks.mtrkPfType, &fForestTracks.b_mtrkPfType);
   fChain->SetBranchAddress("mtrkPfCandPt", fForestTracks.mtrkPfCandPt, &fForestTracks.b_mtrkPfCandPt);
   fChain->SetBranchAddress("mtrkPfSumEcal", fForestTracks.mtrkPfSumEcal, &fForestTracks.b_mtrkPfSumEcal);
   fChain->SetBranchAddress("mtrkPfSumHcal", fForestTracks.mtrkPfSumHcal, &fForestTracks.b_mtrkPfSumHcal);
   */
   fInit = kTRUE;
  }
  return kTRUE;
}

//__________________________________________________________
Bool_t trackProducer::InitEventObjects() {
  //Create event objects
  if(!fEventObjects) {
    Printf("%s: fEventObjects does not exist. Cannot store output",GetName());
    return kFALSE;
  } else {
    if(!fEventObjects->FindObject(fTracksName)) {
      fTracks = new TClonesArray("particleBase");
      fTracks->SetName(fTracksName);
      fEventObjects->Add(fTracks);
    }
  }
  return kTRUE;
}

//__________________________________________________________
Bool_t trackProducer::Run(Long64_t entry) {

  //run analysis
  Long64_t centry = LoadTree(entry);
  if(centry<0) return kFALSE;

  if(!InitEventObjects()) return kFALSE;
 
  //clear array
  fTracks->Delete();
  
  //put particles of this event in array
  Int_t trkCount = 0;
  for(Int_t i = 0; i<fForestTracks.nTrk; i++) {
    if(fSelectHighPurity && !fForestTracks.highPurity[i]) continue;
    Double_t mass = 0.13957;
    Int_t charge = fForestTracks.trkCharge[i];

    particleBase *pPart = new ((*fTracks)[trkCount])
      particleBase(fForestTracks.trkPt[i],
                   fForestTracks.trkEta[i],
                   fForestTracks.trkPhi[i],
                   mass,
                   1);
    pPart->SetCharge(charge);
    ++trkCount;
  }
  fTracks->Sort();

  return kTRUE;
}

//__________________________________________________________
Long64_t trackProducer::LoadTree(Long64_t entry) {

  //overloaded LoadTree function 
  if(!fChain) {
    Printf("fChain doesn't exist");
    return -5;
  }
  
  if (fChain->GetTreeNumber() != fCurrent) {
    fCurrent = fChain->GetTreeNumber();
    Init();
    //  Printf("%lld fCurrent: %d",entry,fCurrent);
  }

  // fChain->SetMakeClass(1);
 
  Long64_t centry = fChain->LoadTree(entry);
  if (centry < 0) {
    Printf("trackProducer: centry smaller than 0");
    return centry;  
  } 

  fChain->GetEntry(entry);
  
  return centry;
}
