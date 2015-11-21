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

void analyzePFvsCaloJetsppData(std::vector<std::string> urls, const char *outname = "eventObjects.root", Long64_t nentries = 20, Int_t firstF = -1, Int_t lastF = -1) {

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

  TChain *hltTree = new TChain("hltanalysis/HltTree");
  for(size_t i=firstFile; i<lastFile; i++) hltTree->Add(urls[i].c_str());
  chain->AddFriend(hltTree);
  Printf("hltTree done");

  TChain *skimTree = new TChain("skimanalysis/HltTree");
  for(size_t i=firstFile; i<lastFile; i++) skimTree->Add(urls[i].c_str());
  chain->AddFriend(skimTree);
  Printf("skimTree done");

  //TChain *pileupTree = new TChain("pileup/tree");
  // for(size_t i=firstFile; i<lastFile; i++) pileupTree->Add(urls[i].c_str());
  //chain->AddFriend(pileupTree);
  
  TChain *pfTree = new TChain("pfcandAnalyzer/pfTree");
  for(size_t i=firstFile; i<lastFile; i++) pfTree->Add(urls[i].c_str());
  chain->AddFriend(pfTree);
  Printf("pfTree done");
  
  // TChain *muTree = new TChain("hltMuTree/HLTMuTree");
  // for(size_t i=firstFile; i<lastFile; i++) muTree->Add(urls[i].c_str());
  // chain->AddFriend(muTree);
  // Printf("muTree done");

  TChain *pfJetTree = new TChain("ak4PFJetAnalyzer/t");
  for(size_t i=firstFile; i<lastFile; i++) pfJetTree->Add(urls[i].c_str());
  chain->AddFriend(pfJetTree);
  Printf("pfJetTree done");

  TChain *caloJetTree = new TChain("ak4CaloJetAnalyzer/t");
  for(size_t i=firstFile; i<lastFile; i++) caloJetTree->Add(urls[i].c_str());
  //chain->AddFriend(caloJetTree);
  Printf("caloJetTree done");

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

  lwJetFromForestProducer *p_pfJet = new lwJetFromForestProducer("lwJetForestProd");
  p_pfJet->SetInput(chain);
  p_pfJet->SetJetContName("akt4PF");
  p_pfJet->SetGenJetContName("");//akt4Gen");
  p_pfJet->SetEventObjects(fEventObjects);
  p_pfJet->SetRadius(0.4);
  
  lwJetFromForestProducer *p_caloJet = new lwJetFromForestProducer("lwJetForestProd");
  p_caloJet->SetInput(caloJetTree);
  p_caloJet->SetJetContName("akt4Calo");
  p_caloJet->SetGenJetContName("");
  p_caloJet->SetEventObjects(fEventObjects);
  p_caloJet->SetRadius(0.4);
  
  //---------------------------------------------------------------
  //analysis modules
  //

  //handler to which all modules will be added
  anaBaseTask *handler = new anaBaseTask("handler","handler");

  anaJetMatching *matchingPFCaloJet = new anaJetMatching("matchingPFCaloJet","matchingPFCaloJet");
  matchingPFCaloJet->ConnectEventObject(fEventObjects);
  matchingPFCaloJet->SetHiEvtName("hiEventContainer");
  matchingPFCaloJet->DoPFJet80(true);
  matchingPFCaloJet->DoExcludePhoton30(true);
  matchingPFCaloJet->SetJetsNameBase("akt4PF");
  matchingPFCaloJet->SetJetsNameTag("akt4Calo");
  matchingPFCaloJet->SetNCentBins(1);
  handler->Add(matchingPFCaloJet);  
  
  anaPFvsCaloJet *anaPFCaloJet = new anaPFvsCaloJet("anaPFvsCaloJet","anaPFvsCaloJet");
  anaPFCaloJet->ConnectEventObject(fEventObjects);
  anaPFCaloJet->SetHiEvtName("hiEventContainer");
  anaPFCaloJet->DoPFJet80(true);
  anaPFCaloJet->DoExcludePhoton30(true);
  anaPFCaloJet->SetPFJetsName("akt4PF");
  anaPFCaloJet->SetCaloJetsName("akt4Calo");
  handler->Add(anaPFCaloJet);

  /*
  //PF-GEN matching
  anaJetMatching *matchingGenPFJet = new anaJetMatching("matchingGenPFJet","matchingGenPFJet");
  matchingGenPFJet->ConnectEventObject(fEventObjects);
  matchingGenPFJet->SetHiEvtName("");
  matchingGenPFJet->SetJetsNameBase("akt4Gen");
  matchingGenPFJet->SetJetsNameTag("akt4PF");
  matchingGenPFJet->SetNCentBins(1);
  handler->Add(matchingGenPFJet);  
  
  anaPFvsCaloJet *anaGenPFJet = new anaPFvsCaloJet("anaGenVsPFJet","anaGenVsPFJet");
  anaGenPFJet->ConnectEventObject(fEventObjects);
  anaGenPFJet->SetHiEvtName("");
  anaGenPFJet->SetPFJetsName("akt4Gen");
  anaGenPFJet->SetCaloJetsName("akt4PF");
  handler->Add(anaGenPFJet);
  */
  
  //---------------------------------------------------------------
  //Event loop
  //---------------------------------------------------------------	  
  Long64_t entries_tot =  chain->GetEntriesFast(); //93064
  if(nentries<0) nentries = chain->GetEntries();
  // Long64_t nentries = 20;//chain->GetEntriesFast();
  Printf("nentries: %lld  tot: %lld",nentries,entries_tot);
  for (Long64_t jentry=0; jentry<nentries;jentry++) {
    if (jentry%10000==0) Printf("Processing event %d  %d",(int)(jentry), (int)(nentries));
    //Run producers
    // Printf("produce hiEvent");
    p_evt->Run(jentry);   //hi event properties

    //Printf("produce pf particles");
    //  p_pf->Run(jentry);    //pf particles
    p_pfJet->Run(jentry); //jets
    p_caloJet->Run(jentry); //jets
    
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
