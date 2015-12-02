#include "UserCode/TopFromHeavyIons/interface/genParticleProducer.h"
#include "UserCode/TopFromHeavyIons/interface/hiEventProducer.h"
#include "UserCode/TopFromHeavyIons/interface/lwMuonProducer.h"
#include "UserCode/TopFromHeavyIons/interface/pfParticleProducer.h"
#include "UserCode/TopFromHeavyIons/interface/LWJetProducer.h"
#include "UserCode/TopFromHeavyIons/interface/lwJetContainer.h"
#include "UserCode/TopFromHeavyIons/interface/lwJetFromForestProducer.h"
#include "UserCode/TopFromHeavyIons/interface/anaBaseTask.h"
#include "UserCode/TopFromHeavyIons/interface/anaJetQA.h"
#include "UserCode/TopFromHeavyIons/interface/anaRhoProducer.h"
#include "UserCode/TopFromHeavyIons/interface/anaMET.h"
#include "UserCode/TopFromHeavyIons/interface/anaMuonIsolation.h"
#include "UserCode/TopFromHeavyIons/interface/anaMuonMatcher.h"
#include "UserCode/TopFromHeavyIons/interface/anaPuppiProducer.h"
#include "UserCode/TopFromHeavyIons/interface/anaZToMuMu.h"

#include <TList.h>
#include <TChain.h>
#include <TFile.h>

//#include "CollectFiles.C"

#include <iostream>

using namespace std;

void analyzeZToMuMu(std::vector<std::string> urls, const char *outname = "eventObjects.root", Long64_t nentries = 20, Int_t firstF = -1, Int_t lastF = -1) {

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

  TChain *muTree = new TChain("hltMuTree/HLTMuTree");
  for(size_t i=firstFile; i<lastFile; i++) muTree->Add(urls[i].c_str());
  chain->AddFriend(muTree);
  Printf("muTree done");

    TList *fEventObjects = new TList();


  //---------------------------------------------------------------
  // producers
  //
  hiEventProducer *p_evt = new hiEventProducer("hiEvtProd");
  p_evt->SetInput(chain);
  p_evt->SetHIEventContName("hiEventContainer");
  p_evt->SetEventObjects(fEventObjects);

  lwMuonProducer *p_mu = new lwMuonProducer("lwMuonProd");
  p_mu->SetInput(chain);
  p_mu->SetlwMuonsRecoName("lwMuonsReco");
  p_mu->SetlwMuonsGeneName("");
  p_mu->SetEventObjects(fEventObjects);

  //---------------------------------------------------------------
  //analysis modules
  //

  //handler to which all modules will be added
  anaBaseTask *handler = new anaBaseTask("handler","handler");

  //Z to mumu
  anaZToMuMu *ZToMuMu = new anaZToMuMu("ZToMuMu","ZToMuMu");
  ZToMuMu->ConnectEventObject(fEventObjects);
  ZToMuMu->SetHiEvtName("hiEventContainer");
  //matchingPFCaloJet->DoPFJet80(true);
  //matchingPFCaloJet->DoExcludePhoton30(true);
  ZToMuMu->SetMuonsName("lwMuonsReco");
  handler->Add(ZToMuMu);

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
    p_mu->Run(jentry);    //muons
    
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
