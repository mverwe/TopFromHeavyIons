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

Bool_t doMuonIsolation = kTRUE;//kFALSE;
Bool_t doPuppi         = kFALSE;//kTRUE;//kFALSE;
Bool_t doMuonIsoInBJet = kTRUE;

void analyze(std::vector<std::string> urls, const char *outname = "eventObjects.root", Long64_t nentries = 20, Int_t firstF = -1, Int_t lastF = -1) {

  // std::vector<std::string> urls = CollectFiles(list);

  // Printf("anaFile: %d",anaFile);
  
  std::cout << "nfiles: " << urls.size() << std::endl;
  // for (auto i = urls.begin(); i != urls.end(); ++i)
  //   std::cout << *i << std::endl;

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
  
  TChain *muTree = new TChain("hltMuTree/HLTMuTree");
  for(size_t i=firstFile; i<lastFile; i++) muTree->Add(urls[i].c_str());
  chain->AddFriend(muTree);
  Printf("muTree done");

  TChain *jetTree = new TChain("akPu3PFJetAnalyzer/t");
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

  lwMuonProducer *p_mu = new lwMuonProducer("lwMuonProd");
  p_mu->SetInput(chain);
  p_mu->SetlwMuonsRecoName("lwMuonsReco");
  p_mu->SetlwMuonsGeneName("lwMuonsGene");
  p_mu->SetEventObjects(fEventObjects);

  genParticleProducer *p_gen = new genParticleProducer("genParticleProd");
  p_gen->SetInput(chain);
  p_gen->SetGenParticlesName("genParticles");
  p_gen->SetEventObjects(fEventObjects);
  
  lwJetFromForestProducer *p_PUJet = new lwJetFromForestProducer("lwJetForestProd");
  p_PUJet->SetInput(chain);
  p_PUJet->SetJetContName("aktPUR030");
  p_PUJet->SetEventObjects(fEventObjects);

  
  //anti-kt jet finder on reconstructed pf candidates
  LWJetProducer *lwjakt = new LWJetProducer("LWJetProducerAKTR030");
  lwjakt->SetInput(chain);
  lwjakt->SetEventObjects(fEventObjects);
  lwjakt->SetJetType(LWJetProducer::kAKT);
  lwjakt->SetRadius(0.3);
  lwjakt->SetGhostArea(0.005);
  lwjakt->SetPtMinConst(0.);
  lwjakt->SetParticlesName("pfParticles");
  lwjakt->SetJetContName("JetsAKTR030");
  lwjakt->SetCSJetContName("CSJetsAKTR030");
  lwjakt->SetDoConstituentSubtraction(kTRUE);
  lwjakt->SetRhoMapName("rhoMap");
  lwjakt->SetRhoMMapName("rhoMMap");

  //anti-kt jet finder on reconstructed pf candidates
  LWJetProducer *lwjaktGen = new LWJetProducer("LWGenJetProducerAKTR030");
  lwjaktGen->SetInput(chain);
  lwjaktGen->SetEventObjects(fEventObjects);
  lwjaktGen->SetJetType(LWJetProducer::kAKT);
  lwjaktGen->SetRadius(0.3);
  lwjaktGen->SetGhostArea(0.005);
  lwjaktGen->SetPtMinConst(0.);
  lwjaktGen->SetParticlesName("genParticles");
  lwjaktGen->SetJetContName("GenJetsAKTR030");
  lwjaktGen->SetDoConstituentSubtraction(kFALSE);

  //kt jet finder
  LWJetProducer *lwjkt = new LWJetProducer("LWJetProducerKTR020");
  lwjkt->SetInput(chain);
  lwjkt->SetEventObjects(fEventObjects);
  lwjkt->SetJetType(LWJetProducer::kKT);
  lwjkt->SetRadius(0.2);
  lwjkt->SetGhostArea(0.005);
  lwjkt->SetPtMinConst(0.);
  lwjkt->SetParticlesName("pfParticles");
  lwjkt->SetJetContName("JetsKTR020");
  lwjkt->SetDoConstituentSubtraction(kFALSE);
	 

  //---------------------------------------------------------------
  //analysis modules
  //

  //handler to which all modules will be added
  anaBaseTask *handler = new anaBaseTask("handler","handler");

  //analysis modules which also produce
  anaRhoProducer *rhoProd = new anaRhoProducer("anaRhoProducerKTR020","anaRhoProducerKTR020");
  rhoProd->ConnectEventObject(fEventObjects);
  rhoProd->SetJetsName("JetsKTR020");
  rhoProd->SetHiEvtName("hiEventContainer");
  rhoProd->SetNExcl(2);
  rhoProd->SetEtaRangeAll(-5.+0.2,5.-0.2);
  //handler->Add(rhoProd);
	  
  anaPuppiProducer *pupProd = new anaPuppiProducer("pupProd","pupProd");
  pupProd->ConnectEventObject(fEventObjects);
  pupProd->SetHiEvtName("hiEventContainer");
  pupProd->SetPFPartName("pfParticles");
  pupProd->SetJetsName("aktPUR030");
  if(doPuppi) handler->Add(pupProd);

  //Initialization of all analysis modules
  anaJetQA *jetQA = new anaJetQA("anaJetQAAKTPUR030","anaJetQAAKTPUR030");
  jetQA->ConnectEventObject(fEventObjects);
  jetQA->SetJetsName("aktPUR030");
  handler->Add(jetQA);
	  
  anaMuonMatcher *muMatchGen = new anaMuonMatcher("muMatchGen","muMatchGen");
  muMatchGen->ConnectEventObject(fEventObjects);
  muMatchGen->SetMuonsName("lwMuonsReco");
  muMatchGen->SetMatchName("lwMuonsGene");
  muMatchGen->SetMatchType(anaMuonMatcher::kGen);
  handler->Add(muMatchGen);

  anaMuonMatcher *muMatchPF = new anaMuonMatcher("muMatchPF","muMatchPF");
  muMatchPF->ConnectEventObject(fEventObjects);
  muMatchPF->SetMuonsName("lwMuonsReco");
  muMatchPF->SetMatchName("pfParticles");
  muMatchPF->SetMatchType(anaMuonMatcher::kPF);
  handler->Add(muMatchPF);

  //Muon isolation
	 
  anaMuonIsolation *muonIsoRaw = new anaMuonIsolation("muonIsoRaw","muonIsoRaw");
  muonIsoRaw->ConnectEventObject(fEventObjects);
  muonIsoRaw->SetHiEvtName("hiEventContainer");
  muonIsoRaw->SetMuonsName("lwMuonsReco");
  muonIsoRaw->SetMuonsGenName("lwMuonsGene");
  muonIsoRaw->SetPFPartName("pfParticles");
  muonIsoRaw->SetIsolationType(anaMuonIsolation::kRaw);
	  
  anaMuonIsolation *muonIsoVS = new anaMuonIsolation("muonIsoVS","muonIsoVS");
  muonIsoVS->ConnectEventObject(fEventObjects);
  muonIsoVS->SetHiEvtName("hiEventContainer");
  muonIsoVS->SetMuonsName("lwMuonsReco");
  muonIsoVS->SetMuonsGenName("lwMuonsGene");
  muonIsoVS->SetPFPartName("pfParticles");
  muonIsoVS->SetIsolationType(anaMuonIsolation::kVS);
	  
  anaMuonIsolation *muonIsoArea = new anaMuonIsolation("muonIsoArea","muonIsoArea");
  muonIsoArea->ConnectEventObject(fEventObjects);
  muonIsoArea->SetHiEvtName("hiEventContainer");
  muonIsoArea->SetMuonsName("lwMuonsReco");
  muonIsoArea->SetMuonsGenName("lwMuonsGene");
  muonIsoArea->SetPFPartName("pfParticles");
  muonIsoArea->SetIsolationType(anaMuonIsolation::kArea);
  muonIsoArea->SetRhoMapName("rhoMap");
	 
  anaMuonIsolation *muonIsoCS = new anaMuonIsolation("muonIsoCS","muonIsoCS");
  muonIsoCS->ConnectEventObject(fEventObjects);
  muonIsoCS->SetHiEvtName("hiEventContainer");
  muonIsoCS->SetMuonsName("lwMuonsReco");
  muonIsoCS->SetMuonsGenName("lwMuonsGene");
  muonIsoCS->SetPFPartName("pfParticles");
  muonIsoCS->SetIsolationType(anaMuonIsolation::kCS);
  muonIsoCS->SetRhoMapName("rhoMap");
  muonIsoCS->SetRhoMMapName("rhoMMap");

  anaMuonIsolation *muonIsoPuppi = new anaMuonIsolation("muonIsoPuppi","muonIsoPuppi");
  muonIsoPuppi->ConnectEventObject(fEventObjects);
  muonIsoPuppi->SetHiEvtName("hiEventContainer");
  muonIsoPuppi->SetMuonsName("lwMuonsReco");
  muonIsoPuppi->SetMuonsGenName("lwMuonsGene");
  muonIsoPuppi->SetPFPartName("pfParticles");
  muonIsoPuppi->SetIsolationType(anaMuonIsolation::kPuppi);

  // anaMuonIsolation *muonIsoGen = new anaMuonIsolation("muonIsoGen","muonIsoGen");
  // muonIsoGen->ConnectEventObject(fEventObjects);
  // muonIsoGen->SetHiEvtName("hiEventContainer");
  // muonIsoGen->SetMuonsName("lwMuonsGene");
  // muonIsoGen->SetMuonsGenName("");
  // muonIsoGen->SetPFPartName("genParticles");
  // muonIsoGen->SetIsolationType(anaMuonIsolation::kGen);
	  
  if(doMuonIsolation) {
    handler->Add(muonIsoRaw);
    handler->Add(muonIsoVS);
    handler->Add(muonIsoArea);
    handler->Add(muonIsoCS);
    if(doPuppi) handler->Add(muonIsoPuppi);
  }

  if(doMuonIsoInBJet) {
    anaMuonIsolation *muonIsoRawBjet = new anaMuonIsolation("muonIsoRawBjet","muonIsoRawBjet");
    muonIsoRawBjet->ConnectEventObject(fEventObjects);
    muonIsoRawBjet->SetHiEvtName("hiEventContainer");
    muonIsoRawBjet->SetMuonsName("lwMuonsReco");
    muonIsoRawBjet->SetMuonsGenName("lwMuonsGene");
    muonIsoRawBjet->SetPFPartName("pfParticles");
    muonIsoRawBjet->SetIsolationType(anaMuonIsolation::kRaw);
    muonIsoRawBjet->SetCheckBjet(kTRUE);
    muonIsoRawBjet->SetJetContName("aktPUR030");
	    
    anaMuonIsolation *muonIsoVSBjet = new anaMuonIsolation("muonIsoVSBjet","muonIsoVSBjet");
    muonIsoVSBjet->ConnectEventObject(fEventObjects);
    muonIsoVSBjet->SetHiEvtName("hiEventContainer");
    muonIsoVSBjet->SetMuonsName("lwMuonsReco");
    muonIsoVSBjet->SetMuonsGenName("lwMuonsGene");
    muonIsoVSBjet->SetPFPartName("pfParticles");
    muonIsoVSBjet->SetIsolationType(anaMuonIsolation::kVS);
    muonIsoVSBjet->SetCheckBjet(kTRUE);
    muonIsoVSBjet->SetJetContName("aktPUR030");
	    
    anaMuonIsolation *muonIsoAreaBjet = new anaMuonIsolation("muonIsoAreaBjet","muonIsoAreaBjet");
    muonIsoAreaBjet->ConnectEventObject(fEventObjects);
    muonIsoAreaBjet->SetHiEvtName("hiEventContainer");

    muonIsoAreaBjet->SetPFPartName("pfParticles");
    muonIsoAreaBjet->SetMuonsName("lwMuonsReco");
    muonIsoAreaBjet->SetMuonsGenName("lwMuonsGene");
    muonIsoAreaBjet->SetIsolationType(anaMuonIsolation::kArea);
    muonIsoAreaBjet->SetRhoMapName("rhoMap");
    muonIsoAreaBjet->SetCheckBjet(kTRUE);
    muonIsoAreaBjet->SetJetContName("aktPUR030");
	    
    anaMuonIsolation *muonIsoCSBjet = new anaMuonIsolation("muonIsoCSBjet","muonIsoCSBjet");
    muonIsoCSBjet->ConnectEventObject(fEventObjects);
    muonIsoCSBjet->SetHiEvtName("hiEventContainer");
    muonIsoCSBjet->SetMuonsName("lwMuonsReco");
    muonIsoCSBjet->SetMuonsGenName("lwMuonsGene");
    muonIsoCSBjet->SetPFPartName("pfParticles");
    muonIsoCSBjet->SetIsolationType(anaMuonIsolation::kCS);
    muonIsoCSBjet->SetRhoMapName("rhoMap");
    muonIsoCSBjet->SetRhoMMapName("rhoMMap");
    muonIsoCSBjet->SetCheckBjet(kTRUE);
    muonIsoCSBjet->SetJetContName("aktPUR030");
	    
    anaMuonIsolation *muonIsoPuppiBjet = new anaMuonIsolation("muonIsoPuppiBjet","muonIsoPuppiBjet");
    muonIsoPuppiBjet->ConnectEventObject(fEventObjects);
    muonIsoPuppiBjet->SetHiEvtName("hiEventContainer");
    muonIsoPuppiBjet->SetMuonsName("lwMuonsReco");
    muonIsoPuppiBjet->SetMuonsGenName("lwMuonsGene");
    muonIsoPuppiBjet->SetPFPartName("pfParticles");
    muonIsoPuppiBjet->SetIsolationType(anaMuonIsolation::kPuppi);
    muonIsoPuppiBjet->SetCheckBjet(kTRUE);
    muonIsoPuppiBjet->SetJetContName("aktPUR030");

    handler->Add(muonIsoRawBjet);
    handler->Add(muonIsoVSBjet);
    handler->Add(muonIsoAreaBjet);
    handler->Add(muonIsoCSBjet);
    if(doPuppi) handler->Add(muonIsoPuppiBjet);
  }

  anaMET *metPFRaw = new anaMET("metPFRaw","metPFRaw");
  metPFRaw->ConnectEventObject(fEventObjects);
  metPFRaw->SetHiEvtName("hiEventContainer");
  metPFRaw->SetParticlesName("pfParticles");
  metPFRaw->SetMetType(anaMET::kPFRaw);
  handler->Add(metPFRaw);

  anaMET *metVS = new anaMET("metVS","metVS");
  metVS->ConnectEventObject(fEventObjects);
  metVS->SetHiEvtName("hiEventContainer");
  metVS->SetParticlesName("pfParticles");
  metVS->SetMetType(anaMET::kVS);
  handler->Add(metVS);

  anaMET *metPuppi = new anaMET("metPuppi","metPuppi");
  metPuppi->ConnectEventObject(fEventObjects);
  metPuppi->SetHiEvtName("hiEventContainer");
  metPuppi->SetParticlesName("pfParticles");
  metPuppi->SetMetType(anaMET::kPuppi);
  if(doPuppi) handler->Add(metPuppi);

  anaMET *metGen = new anaMET("metGen","metGen");
  metGen->ConnectEventObject(fEventObjects);
  metGen->SetHiEvtName("hiEventContainer");
  metGen->SetParticlesName("genParticles");
  metGen->SetMetType(anaMET::kGen);
  handler->Add(metGen);

  //MET with minimum pT cut
  anaMET *metPFRawPtMin = new anaMET("metPFRawPtMin","metPFRawPtMin");
  metPFRawPtMin->ConnectEventObject(fEventObjects);
  metPFRawPtMin->SetHiEvtName("hiEventContainer");
  metPFRawPtMin->SetParticlesName("pfParticles");
  metPFRawPtMin->SetMetType(anaMET::kPFRaw);
  metPFRawPtMin->SetMinPt(3.);
  handler->Add(metPFRawPtMin);

  anaMET *metVSPtMin = new anaMET("metVSPtMin","metVSPtMin");
  metVSPtMin->ConnectEventObject(fEventObjects);
  metVSPtMin->SetHiEvtName("hiEventContainer");
  metVSPtMin->SetParticlesName("pfParticles");
  metVSPtMin->SetMetType(anaMET::kVS);
  metVSPtMin->SetMinPt(3.);
  handler->Add(metVSPtMin);

  anaMET *metPuppiPtMin = new anaMET("metPuppiPtMin","metPuppiPtMin");
  metPuppiPtMin->ConnectEventObject(fEventObjects);
  metPuppiPtMin->SetHiEvtName("hiEventContainer");
  metPuppiPtMin->SetParticlesName("pfParticles");
  metPuppiPtMin->SetMetType(anaMET::kPuppi);
  metPuppiPtMin->SetMinPt(3.);
  if(doPuppi) handler->Add(metPuppiPtMin);

  anaMET *metGenPtMin = new anaMET("metGenPtMin","metGenPtMin");
  metGenPtMin->ConnectEventObject(fEventObjects);
  metGenPtMin->SetHiEvtName("hiEventContainer");
  metGenPtMin->SetParticlesName("genParticles");
  metGenPtMin->SetMetType(anaMET::kGen);
  metGenPtMin->SetMinPt(3.);
  handler->Add(metGenPtMin);

  //MET from jets
  anaMET *metJet = new anaMET("metJet","metJet");
  metJet->ConnectEventObject(fEventObjects);
  metJet->SetHiEvtName("hiEventContainer");
  metJet->SetParticlesName("aktPUR030");//aktPUR030
  metJet->SetMetType(anaMET::kPFRaw);
  metJet->SetMinPt(30.);
  handler->Add(metJet);

  anaMET *metCSJet = new anaMET("metCSJet","metCSJet");
  metCSJet->ConnectEventObject(fEventObjects);
  metCSJet->SetHiEvtName("hiEventContainer");
  metCSJet->SetParticlesName("CSJetsAKTR030");
  metCSJet->SetMetType(anaMET::kPFRaw);
  metCSJet->SetMinPt(30.);
  handler->Add(metCSJet);

  anaMET *metGenJet = new anaMET("metGenJet","metGenJet");
  metGenJet->ConnectEventObject(fEventObjects);
  metGenJet->SetHiEvtName("hiEventContainer");
  metGenJet->SetParticlesName("GenJetsAKTR030");
  metGenJet->SetMetType(anaMET::kGen);
  metGenJet->SetMinPt(30.);
  handler->Add(metGenJet);
	  
  //Z to mumu
  anaZToMuMu *ZToMuMu = new anaZToMuMu("ZToMuMu","ZToMuMu");
  ZToMuMu->ConnectEventObject(fEventObjects);
  ZToMuMu->SetHiEvtName("hiEventContainer");
  ZToMuMu->SetMuonsName("lwMuonsReco");
  handler->Add(ZToMuMu);

  anaZToMuMu *ZToMuMuGen = new anaZToMuMu("ZToMuMuGen","ZToMuMuGen");
  ZToMuMuGen->ConnectEventObject(fEventObjects);
  ZToMuMuGen->SetHiEvtName("hiEventContainer");
  ZToMuMuGen->SetMuonsName("lwMuonsGene");
  ZToMuMuGen->SetCheckPid(kTRUE);
  handler->Add(ZToMuMuGen);

  anaZToMuMu *ZToMuMuGenAll = new anaZToMuMu("ZToMuMuGenAll","ZToMuMuGenAll");
  ZToMuMuGenAll->ConnectEventObject(fEventObjects);
  ZToMuMuGenAll->SetHiEvtName("hiEventContainer");
  ZToMuMuGenAll->SetMuonsName("genParticles");
  ZToMuMuGenAll->SetCheckPid(kTRUE);
  handler->Add(ZToMuMuGenAll);
	  
  //---------------------------------------------------------------
  //output tree
  //  TTree *tree_out = new TTree("tree_out","lwTree");
  //tree_out->Branch("EventObjects",&fEventObjects);
	  
  Long64_t entries_tot =  chain->GetEntriesFast(); //93064
  if(nentries<0) nentries = chain->GetEntries();
  // Long64_t nentries = 20;//chain->GetEntriesFast();
  Printf("nentries: %lld  tot: %lld",nentries,entries_tot);
  for (Long64_t jentry=0; jentry<nentries;jentry++) {

    //Run producers
    p_evt->Run(jentry);   //hi event properties
    p_pf->Run(jentry);    //pf particles
    p_mu->Run(jentry);    //muons
    p_gen->Run(jentry);    //generated particles
    p_PUJet->Run(jentry); //forest jets
	    
    lwjkt->FindJets();   //kt jets
    lwjaktGen->FindJets(); //akt gen jets
	   
    //akt jets with constituent subtraction
    rhoProd->Exec();
    lwjakt->FindJets();
         
    //Execute all analysis tasks
    handler->ExecuteTask();

    //tree_out->Fill();
  }
    
  fEventObjects->Print();

  TFile *out = new TFile(outname,"RECREATE");
  rhoProd->GetOutput()->Write(rhoProd->GetName(),TObject::kSingleKey);
  TList *tasks = handler->GetListOfTasks();
  TIter next(tasks);
  anaBaseTask *obj;
  while ((obj = dynamic_cast<anaBaseTask*>(next()) ))
    obj->GetOutput()->Write(obj->GetName(),TObject::kSingleKey);
  
  out->Write();
  out->Close();
  
}
