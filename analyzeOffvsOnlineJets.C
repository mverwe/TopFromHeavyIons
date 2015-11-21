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
#include "UserCode/TopFromHeavyIons/interface/anaPFvsCaloJet.h"
#include "UserCode/TopFromHeavyIons/interface/anaPuppiProducer.h"
#include "UserCode/TopFromHeavyIons/interface/anaPuppiParticles.h"
#include "UserCode/TopFromHeavyIons/interface/anaZToMuMu.h"

#include <TList.h>
#include <TChain.h>
#include <TFile.h>

//#include "CollectFiles.C"

#include <iostream>

using namespace std;

void analyzeOffvsOnlineJets(std::vector<std::string> urls, const char *outname = "eventObjects.root", Long64_t nentries = 20, Int_t firstF = -1, Int_t lastF = -1, Int_t firstEvent = 0) {

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

  Int_t lastEvent = nentries;
  if(firstEvent>0) {
    lastEvent = firstEvent + nentries;
  }
  std::cout << "firstEvent: " << firstEvent << std::endl;
  
  //add files to chain
  TChain *chain = NULL;
  //chain = new TChain("hiEvtAnalyzer/HiTree");
  //for(size_t i=firstFile; i<lastFile; i++) chain->Add(urls[i].c_str());
  Printf("hiTree done");

  // TChain *pfTree = new TChain("pfcandAnalyzer/pfTree");
  // for(size_t i=firstFile; i<lastFile; i++) pfTree->Add(urls[i].c_str());
  // chain->AddFriend(pfTree);
  // Printf("pfTree done");
  
  // TChain *muTree = new TChain("hltMuTree/HLTMuTree");
  // for(size_t i=firstFile; i<lastFile; i++) muTree->Add(urls[i].c_str());
  // chain->AddFriend(muTree);
  // Printf("muTree done");

  TChain *caloJetTree = new TChain("hltPuAK4CaloJetsCorrectedIDPassedJetAnalyzer/t");
  chain = caloJetTree;
  for(size_t i=firstFile; i<lastFile; i++) caloJetTree->Add(urls[i].c_str());
  //chain->AddFriend(caloJetTree);
  Printf("caloJetTree done");

  TChain *caloJetCorrKTree = new TChain("hltPuAK4CaloJetsIDPassedCorrKJetAnalyzer/t");
  for(size_t i=firstFile; i<lastFile; i++) caloJetCorrKTree->Add(urls[i].c_str());
  chain->AddFriend(caloJetCorrKTree);
  Printf("caloJetCorrKTree done");

  TChain *caloJetCorrLTree = new TChain("hltPuAK4CaloJetsIDPassedCorrLJetAnalyzer/t");
  for(size_t i=firstFile; i<lastFile; i++) caloJetCorrLTree->Add(urls[i].c_str());
  chain->AddFriend(caloJetCorrLTree);
  Printf("caloJetCorrLTree done");
  
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

  lwJetFromForestProducer *p_caloJet = new lwJetFromForestProducer("lwJetForestProd");
  p_caloJet->SetInput(chain);
  p_caloJet->SetJetContName("akt4Calo");
  p_caloJet->SetGenJetContName("akt4Gen");
  p_caloJet->SetEventObjects(fEventObjects);
  p_caloJet->SetRadius(0.4);

  lwJetFromForestProducer *p_caloCorrKJet = new lwJetFromForestProducer("lwJetForestProd");
  p_caloCorrKJet->SetInput(caloJetCorrKTree);
  p_caloCorrKJet->SetJetContName("akt4CaloCorrK");
  p_caloCorrKJet->SetGenJetContName("");
  p_caloCorrKJet->SetEventObjects(fEventObjects);
  p_caloCorrKJet->SetRadius(0.4);

  lwJetFromForestProducer *p_caloCorrLJet = new lwJetFromForestProducer("lwJetForestProd");
  p_caloCorrLJet->SetInput(caloJetCorrLTree);
  p_caloCorrLJet->SetJetContName("akt4CaloCorrL");
  p_caloCorrLJet->SetGenJetContName("");
  p_caloCorrLJet->SetEventObjects(fEventObjects);
  p_caloCorrLJet->SetRadius(0.4);
  
  //---------------------------------------------------------------
  //analysis modules
  //

  //handler to which all modules will be added
  anaBaseTask *handler = new anaBaseTask("handler","handler");

  //CALO-GEN matching
  anaJetMatching *matchingGenCaloCorrLJet = new anaJetMatching("matchingGenCaloCorrLJet","matchingGenCaloCorrLJet");
  matchingGenCaloCorrLJet->ConnectEventObject(fEventObjects);
  matchingGenCaloCorrLJet->SetHiEvtName("");//hiEventContainer");
  matchingGenCaloCorrLJet->SetJetsNameBase("akt4Gen");
  matchingGenCaloCorrLJet->SetJetsNameTag("akt4CaloCorrL");
  matchingGenCaloCorrLJet->SetNCentBins(1);
  handler->Add(matchingGenCaloCorrLJet);  

  anaPFvsCaloJet *anaGenCaloCorrLJet = new anaPFvsCaloJet("anaGenVsCaloCorrLJet","anaGenVsCaloCorrLJet");
  anaGenCaloCorrLJet->ConnectEventObject(fEventObjects);
  anaGenCaloCorrLJet->SetHiEvtName("");//hiEventContainer");
  anaGenCaloCorrLJet->SetPFJetsName("akt4Gen");
  anaGenCaloCorrLJet->SetCaloJetsName("akt4CaloCorrL");
  handler->Add(anaGenCaloCorrLJet);

  anaJetMatching *matchingCaloCorrKCaloCorrLJet = new anaJetMatching("matchingCaloCorrKCaloCorrLJet","matchingCaloCorrKCaloCorrLJet");
  matchingCaloCorrKCaloCorrLJet->ConnectEventObject(fEventObjects);
  matchingCaloCorrKCaloCorrLJet->SetHiEvtName("");//hiEventContainer");
  matchingCaloCorrKCaloCorrLJet->SetJetsNameBase("akt4CaloCorrK");
  matchingCaloCorrKCaloCorrLJet->SetJetsNameTag("akt4CaloCorrL");
  matchingCaloCorrKCaloCorrLJet->SetNCentBins(1);
  handler->Add(matchingCaloCorrKCaloCorrLJet);  
  
  anaPFvsCaloJet *anaCaloCorrKCaloCorrLJet = new anaPFvsCaloJet("anaCaloCorrKVsCaloCorrLJet","anaCaloCorrKVsCaloCorrLJet");
  anaCaloCorrKCaloCorrLJet->ConnectEventObject(fEventObjects);
  anaCaloCorrKCaloCorrLJet->SetHiEvtName("");//hiEventContainer");
  anaCaloCorrKCaloCorrLJet->SetPFJetsName("akt4CaloCorrK");
  anaCaloCorrKCaloCorrLJet->SetCaloJetsName("akt4CaloCorrL");
  handler->Add(anaCaloCorrKCaloCorrLJet);
  

  //---------------------------------------------------------------
  //Event loop
  //---------------------------------------------------------------	  
  Long64_t entries_tot =  chain->GetEntriesFast(); //93064
  if(nentries<0) nentries = chain->GetEntries();
  // Long64_t nentries = 20;//chain->GetEntriesFast();
  Printf("nentries: %lld  tot: %lld",nentries,entries_tot);
  for (Long64_t jentry=firstEvent; jentry<lastEvent; ++jentry) {
    //Run producers
    // Printf("produce hiEvent");
    //p_evt->Run(jentry);   //hi event properties
    //Printf("produce pf particles");
    //  p_pf->Run(jentry);    //pf particles
    p_caloJet->Run(jentry); //jets
    p_caloCorrKJet->Run(jentry);
    p_caloCorrLJet->Run(jentry); //jets
       
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
