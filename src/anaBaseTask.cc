#include "UserCode/TopFromHeavyIons/interface/anaBaseTask.h"

#include "TClass.h"
#include "TDirectory.h"

ClassImp(anaBaseTask)
   
anaBaseTask::anaBaseTask(const char *name, const char *title) 
:TTask(name,title),
  fEventObjects(0),
  fInitOutput(0),
  fDir(0x0),
  fOutput(0),
  fEvtName(""),
  fHiEvent(),
  fCollSel(1),
  fHBHENoise(1),
  fCentMin(-999),
  fCentMax(999),
  fPFJet80(0),
  fPhoton30(0),
  fPhoton30Excl(0),
  fTrk24(0),
  fTrk45(0)
{

}

//----------------------------------------------------------
void anaBaseTask::Exec(Option_t * /*option*/)
{
  if(!fInitOutput) CreateOutputObjects();

  //Get event properties
  
  if(!fHiEvent && !fEvtName.IsNull()) {
     fHiEvent = dynamic_cast<hiEventContainer*>(fEventObjects->FindObject(fEvtName.Data()));
     if(!fHiEvent) {
       Printf("%s: !!WARNING: Couldn't locate %s branch",GetName(),fEvtName.Data());
       return;
     }
  }

   if(!SelectEvent()) return;
}

//----------------------------------------------------------
void anaBaseTask::CreateOutputObjects() {

  if(!fDir) {
    TString dirName = GetName();
    fDir = new TDirectory(dirName.Data(),dirName.Data());
  }
  fDir->cd();
  if(!fOutput) fOutput = new TList();
  if(!fOutput) {
    Printf("%s: There is a problem. fOutput does not exist",GetName());
    return;
  }
  fOutput->SetName(GetName());
  
  fInitOutput = kTRUE;
}

//----------------------------------------------------------
Bool_t anaBaseTask::CheckInheritance(TClonesArray *c, TString clsName) {

  //check if inheritance is to expected type
  TString objname(c->GetClass()->GetName());
  TClass cls(objname);
  if (!cls.InheritsFrom(clsName)) {
    Printf("%s: Objects of type %s are not inherited from %s!", 
           GetName(), cls.GetName(), clsName.Data()); 
    return kFALSE;
  }
  return kTRUE;
}

//----------------------------------------------------------
bool anaBaseTask::SelectEvent() const {
  //
  //pcollision, hbhenoise
  //triggers

  bool accept = true;
  
  if(fHiEvent) {
    if(fCollSel && !fHiEvent->GetColl()) accept = false;
    if(fHBHENoise && !fHiEvent->GetHBHENoise()) accept = false;

    if(fCentMin>-1) {
      double cent = fHiEvent->GetCentrality();
      if(cent<fCentMin && cent>fCentMax) accept = false;
    }
    
    if(fPFJet80 && !fHiEvent->GetPFJet80()) accept = false;
    if(fPhoton30 && !fHiEvent->GetPhoton30()) accept = false;
    if(fTrk24 && !fHiEvent->GetTrk24()) accept = false;
    if(fTrk45 && !fHiEvent->GetTrk45()) accept = false;

    //special for express stream
    if(fPhoton30Excl && fHiEvent->GetPhoton30()) accept = false;
  }

  return accept;
}
