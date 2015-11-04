#include "UserCode/TopFromHeavyIons/interface/hiEventProducer.h"
#include "UserCode/TopFromHeavyIons/interface/lwMuonProducer.h"
#include "UserCode/TopFromHeavyIons/interface/pfParticleProducer.h"
#include "UserCode/TopFromHeavyIons/interface/trackProducer.h"
#include "UserCode/TopFromHeavyIons/interface/LWJetProducer.h"
#include "UserCode/TopFromHeavyIons/interface/lwJetContainer.h"
#include "UserCode/TopFromHeavyIons/interface/lwJetFromForestProducer.h"
#include "UserCode/TopFromHeavyIons/interface/anaBaseTask.h"
#include "UserCode/TopFromHeavyIons/interface/anaJetMatching.h"
#include "UserCode/TopFromHeavyIons/interface/anaJetQA.h"
#include "UserCode/TopFromHeavyIons/interface/anaRhoProducer.h"
#include "UserCode/TopFromHeavyIons/interface/anaMET.h"
#include "UserCode/TopFromHeavyIons/interface/anaMuonIsolation.h"
#include "UserCode/TopFromHeavyIons/interface/anaMuonMatcher.h"
#include "UserCode/TopFromHeavyIons/interface/anaPFCandidates.h"
#include "UserCode/TopFromHeavyIons/interface/anaPuppiProducer.h"
#include "UserCode/TopFromHeavyIons/interface/anaPuppiParticles.h"
#include "UserCode/TopFromHeavyIons/interface/anaZToMuMu.h"

#include <TList.h>
#include <TChain.h>
#include <TFile.h>

//#include "CollectFiles.C"

#include <iostream>

using namespace std;

Bool_t doTracks        = kTRUE;

void analyzePFCandidates(std::vector<std::string> urls, const char *outname = "eventObjects.root", Long64_t nentries = 20, Int_t firstF = -1, Int_t lastF = -1) {

  // std::vector<std::string> urls = CollectFiles(list);

  // Printf("anaFile: %d",anaFile);
  
  std::cout << "nfiles: " << urls.size() << std::endl;
  for (auto i = urls.begin(); i != urls.end(); ++i)
    std::cout << *i << std::endl;

  size_t firstFile = 0;
  size_t lastFile = urls.size();

  if(firstF>-1) {
    firstFile = (size_t)firstF;
    lastFile = (size_t)lastF;
  }
  std::cout << "firstFile: " << firstFile << "  lastFile: " << lastFile << std::endl;
  
  //add files to chain
  TChain *chain = NULL;
  chain = new TChain("hiEvtAnalyzer/HiTree");
  for(size_t i=firstFile; i<lastFile; i++) chain->Add(urls[i].c_str());
  Printf("hiTree done");
  
  TChain *pfTree = new TChain("pfcandAnalyzer/pfTree");
  for(size_t i=firstFile; i<lastFile; i++) pfTree->Add(urls[i].c_str());
  chain->AddFriend(pfTree);
  Printf("pfTree done");
  
  // TChain *muTree = new TChain("hltMuTree/HLTMuTree");
  // for(size_t i=firstFile; i<lastFile; i++) muTree->Add(urls[i].c_str());
  // chain->AddFriend(muTree);
  // Printf("muTree done");

  TChain *jetTree = new TChain("akPu4CaloJetAnalyzer/t");
  for(size_t i=firstFile; i<lastFile; i++) jetTree->Add(urls[i].c_str());
  chain->AddFriend(jetTree);
  Printf("jetTree done");

  if(doTracks) {
    TChain *trackTree = new TChain("anaTrack/trackTree");
    for(size_t i=firstFile; i<lastFile; i++) trackTree->Add(urls[i].c_str());
    chain->AddFriend(trackTree);
    Printf("trackTree done");
  }
  // TChain *genTree = new TChain("HiGenParticleAna/hi");
  // for(size_t i=firstFile; i<lastFile; i++) genTree->Add(urls[i].c_str());
  // chain->AddFriend(genTree);
  // Printf("genTree done");
  
  TList *fEventObjects = new TList();


  //---------------------------------------------------------------
  // producers
  //
  hiEventProducer *p_evt = new hiEventProducer("hiEvtProd");
  p_evt->SetInput(chain);
  p_evt->SetHIEventContName("hiEventContainer");
  p_evt->SetEventObjects(fEventObjects);

  pfParticleProducer *p_pf = new pfParticleProducer("pfPartProd");
  p_pf->SetInput(chain);
  p_pf->SetpfParticlesName("pfParticles");
  p_pf->SetEventObjects(fEventObjects);

  lwJetFromForestProducer *p_PUJet = new lwJetFromForestProducer("lwJetForestProd");
  p_PUJet->SetInput(chain);
  p_PUJet->SetJetContName("aktPu4Calo");
  p_PUJet->SetGenJetContName("akt4Gen");
  p_PUJet->SetEventObjects(fEventObjects);
  p_PUJet->SetRadius(0.4);

  trackProducer *p_trk = new trackProducer("trackProd");
  if(doTracks) {
    p_trk->SetInput(chain);
    p_trk->SetTracksName("tracks");
    p_trk->SetEventObjects(fEventObjects);
  }
  
  //---------------------------------------------------------------
  //analysis modules
  //

  //handler to which all modules will be added
  anaBaseTask *handler = new anaBaseTask("handler","handler");

  anaPFCandidates *anaPFCaloJet = new anaPFCandidates("pfCandWithCaloJets","pfCandWithCaloJets");
  anaPFCaloJet->ConnectEventObject(fEventObjects);
  anaPFCaloJet->SetHiEvtName("hiEventContainer");
  anaPFCaloJet->SetParticlesName("pfParticles");
  anaPFCaloJet->SetJetsName("aktPu4Calo");
  handler->Add(anaPFCaloJet);

  anaPFCandidates *anaPF = new anaPFCandidates("pfCand","pfCand");
  anaPF->ConnectEventObject(fEventObjects);
  anaPF->SetHiEvtName("hiEventContainer");
  anaPF->SetParticlesName("pfParticles");
  anaPF->SetJetsName("akt4Gen");
  handler->Add(anaPF);

  anaPFCandidates *anaTrkCaloJet = new anaPFCandidates("tracksWithCaloJets","tracksWithCaloJets");
  anaTrkCaloJet->ConnectEventObject(fEventObjects);
  anaTrkCaloJet->SetHiEvtName("hiEventContainer");
  anaTrkCaloJet->SetParticlesName("tracks");
  anaTrkCaloJet->SetJetsName("aktPu4Calo");
  handler->Add(anaTrkCaloJet);

  anaPFCandidates *anaTrk = new anaPFCandidates("tracksWithGenJet","tracksWithGenJet");
  anaTrk->ConnectEventObject(fEventObjects);
  anaTrk->SetHiEvtName("hiEventContainer");
  anaTrk->SetParticlesName("tracks");
  anaTrk->SetJetsName("akt4Gen");
  handler->Add(anaTrk);
  
  //---------------------------------------------------------------
  //Event loop
  //---------------------------------------------------------------	  
  Long64_t entries_tot =  chain->GetEntriesFast(); //93064
  if(nentries<0) nentries = chain->GetEntries();
  // Long64_t nentries = 20;//chain->GetEntriesFast();
  Printf("nentries: %lld  tot: %lld",nentries,entries_tot);
  for (Long64_t jentry=0; jentry<nentries;jentry++) {

    //Run producers
    // Printf("produce hiEvent");
    p_evt->Run(jentry);   //hi event properties
    //Printf("produce pf particles");
    p_pf->Run(jentry);    //pf particles
    if(doTracks) p_trk->Run(jentry);    //tracks
    p_PUJet->Run(jentry); //jets
    
    //Execute all analysis tasks
    handler->ExecuteTask();
  }
    
  fEventObjects->Print();

  TFile *out = new TFile(outname,"RECREATE");
  TList *tasks = handler->GetListOfTasks();
  TIter next(tasks);
  anaBaseTask *obj;
  while ((obj = dynamic_cast<anaBaseTask*>(next()) ))
    if(obj->GetOutput()) obj->GetOutput()->Write(obj->GetName(),TObject::kSingleKey);
  
  out->Write();
  out->Close();
  
}
