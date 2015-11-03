#include "UserCode/TopFromHeavyIons/interface/genParticleProducer.h"
#include "UserCode/TopFromHeavyIons/interface/hiEventProducer.h"
#include "UserCode/TopFromHeavyIons/interface/lwMuonProducer.h"
#include "UserCode/TopFromHeavyIons/interface/pfParticleProducer.h"
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
#include "UserCode/TopFromHeavyIons/interface/anaPuppiProducer.h"
#include "UserCode/TopFromHeavyIons/interface/anaPuppiParticles.h"
#include "UserCode/TopFromHeavyIons/interface/anaZToMuMu.h"

#include <TList.h>
#include <TChain.h>
#include <TFile.h>

//#include "CollectFiles.C"

#include <iostream>

using namespace std;

Bool_t doPuppi         = kTRUE;//kFALSE;

void analyzePuppi(std::vector<std::string> urls, const char *outname = "eventObjects.root", Long64_t nentries = 20, Int_t firstF = -1, Int_t lastF = -1) {

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

  //TChain *jetTree = new TChain("akPu3PFJetAnalyzer/t");
  TChain *jetTree = new TChain("akPu2PFJetAnalyzer10/t");
  for(size_t i=firstFile; i<lastFile; i++) jetTree->Add(urls[i].c_str());
  chain->AddFriend(jetTree);
  Printf("jetTree done");

  TChain *genTree = new TChain("HiGenParticleAna/hi");
  for(size_t i=firstFile; i<lastFile; i++) genTree->Add(urls[i].c_str());
  chain->AddFriend(genTree);
  Printf("genTree done");
  
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

  genParticleProducer *p_gen = new genParticleProducer("genParticleProd");
  p_gen->SetInput(chain);
  p_gen->SetGenParticlesName("genParticles");
  p_gen->SetEventObjects(fEventObjects);

  lwJetFromForestProducer *p_PUJet = new lwJetFromForestProducer("lwJetForestProd");
  p_PUJet->SetInput(chain);
  p_PUJet->SetJetContName("aktPUR030");
  p_PUJet->SetEventObjects(fEventObjects);
  p_PUJet->SetRadius(0.3);
  
  //---------------------------------------------------------------
  //analysis modules
  //

  //handler to which all modules will be added
  anaBaseTask *handler = new anaBaseTask("handler","handler");

  //analysis modules which also produce
  anaPuppiProducer *pupProd = new anaPuppiProducer("pupProd","pupProd");
  pupProd->ConnectEventObject(fEventObjects);
  pupProd->SetHiEvtName("hiEventContainer");
  pupProd->SetPFPartName("pfParticles");
  pupProd->SetPuppiPartName("puppiParticles");
  pupProd->SetJetsName("aktPUR030");
  pupProd->SetAddMetricType(anaPuppiProducer::kMass);
  if(doPuppi) handler->Add(pupProd);

  //anti-kt jet finder on reconstructed PUPPI particles ptmin=0
  LWJetProducer *lwjakt = new LWJetProducer("LWJetProducerAKTR030Puppi","LWJetProducerAKTR030Puppi");
  lwjakt->ConnectEventObject(fEventObjects);
  lwjakt->SetJetType(LWJetProducer::kAKT);
  lwjakt->SetRadius(0.3);
  lwjakt->SetGhostArea(0.005);
  lwjakt->SetPtMinConst(0.);
  lwjakt->SetParticlesName("puppiParticles");
  lwjakt->SetJetContName("JetsAKTR030Puppi");
  if(doPuppi) handler->Add(lwjakt);

  //anti-kt jet finder on reconstructed PUPPI particles ptmin=1
  LWJetProducer *lwjaktpt100 = new LWJetProducer("LWJetProducerAKTR030PuppiPtMin100","LWJetProducerAKTR030PuppiPtMin100");
  lwjaktpt100->ConnectEventObject(fEventObjects);
  lwjaktpt100->SetJetType(LWJetProducer::kAKT);
  lwjaktpt100->SetRadius(0.3);
  lwjaktpt100->SetGhostArea(0.005);
  lwjaktpt100->SetPtMinConst(1.);
  lwjaktpt100->SetParticlesName("puppiParticles");
  lwjaktpt100->SetJetContName("JetsAKTR030PuppiPtMin100");
  if(doPuppi) handler->Add(lwjaktpt100);

  //anti-kt jet finder on reconstructed PUPPI particles ptmin=2
  LWJetProducer *lwjaktpt200 = new LWJetProducer("LWJetProducerAKTR030PuppiPtMin200","LWJetProducerAKTR030PuppiPtMin200");
  lwjaktpt200->ConnectEventObject(fEventObjects);
  lwjaktpt200->SetJetType(LWJetProducer::kAKT);
  lwjaktpt200->SetRadius(0.3);
  lwjaktpt200->SetGhostArea(0.005);
  lwjaktpt200->SetPtMinConst(2.);
  lwjaktpt200->SetParticlesName("puppiParticles");
  lwjaktpt200->SetJetContName("JetsAKTR030PuppiPtMin200");
  if(doPuppi) handler->Add(lwjaktpt200);

  //anti-kt jet finder on generated particles
  LWJetProducer *lwjaktGen = new LWJetProducer("LWGenJetProducerAKTR030","LWGenJetProducerAKTR030");
  lwjaktGen->ConnectEventObject(fEventObjects);
  lwjaktGen->SetJetType(LWJetProducer::kAKT);
  lwjaktGen->SetRadius(0.3);
  lwjaktGen->SetGhostArea(0.005);
  lwjaktGen->SetPtMinConst(0.);
  lwjaktGen->SetParticlesName("genParticles");
  lwjaktGen->SetJetContName("GenJetsAKTR030");
  handler->Add(lwjaktGen);
 
  //Initialization of all analysis modules
  anaPuppiParticles *pupAna = new anaPuppiParticles("pupAna","pupAna");
  pupAna->ConnectEventObject(fEventObjects);
  pupAna->SetHiEvtName("hiEventContainer");
  pupAna->SetParticlesName("pfParticles");
  pupAna->SetJetsName("aktPUR030");
  if(doPuppi) handler->Add(pupAna);

  //particle-detector-level jet matching
  anaJetMatching *match = new anaJetMatching("jetMatching","jetMatching");
  match->ConnectEventObject(fEventObjects);
  match->SetHiEvtName("hiEventContainer");
  match->SetJetsNameBase("JetsAKTR030Puppi");
  match->SetJetsNameTag("GenJetsAKTR030");
  if(doPuppi) handler->Add(match);

  anaJetMatching *matchPt100 = new anaJetMatching("jetMatchingPtMin100","jetMatchingPtMin100");
  matchPt100->ConnectEventObject(fEventObjects);
  matchPt100->SetHiEvtName("hiEventContainer");
  matchPt100->SetJetsNameBase("JetsAKTR030PuppiPtMin100");
  matchPt100->SetJetsNameTag("GenJetsAKTR030");
  if(doPuppi) handler->Add(matchPt100);

  anaJetMatching *matchPt200 = new anaJetMatching("jetMatchingPtMin200","jetMatchingPtMin200");
  matchPt200->ConnectEventObject(fEventObjects);
  matchPt200->SetHiEvtName("hiEventContainer");
  matchPt200->SetJetsNameBase("JetsAKTR030PuppiPtMin200");
  matchPt200->SetJetsNameTag("GenJetsAKTR030");
  if(doPuppi) handler->Add(matchPt200);
  
  //---------------------------------------------------------------
  //Event loop
  //---------------------------------------------------------------	  
  Long64_t entries_tot =  chain->GetEntriesFast(); //93064
  if(nentries<0) nentries = chain->GetEntries();
  // Long64_t nentries = 20;//chain->GetEntriesFast();
  Printf("nentries: %lld  tot: %lld",nentries,entries_tot);
  for (Long64_t jentry=0; jentry<nentries;jentry++) {

    //Run producers
    Printf("produce hiEvent");
    p_evt->Run(jentry);   //hi event properties
    Printf("produce pf particles");
    p_pf->Run(jentry);    //pf particles
    Printf("produce gen particles");
    p_gen->Run(jentry);   //generated particles
    Printf("produce PU jets");
    p_PUJet->Run(jentry); //forest jets
	    
    //Execute all analysis tasks
    // handler->ExecuteTask();
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
