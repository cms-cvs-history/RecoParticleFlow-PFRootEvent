#ifndef Demo_PFRootEvent_MyPFRootEventManager_h
#define Demo_PFRootEvent_MyPFRootEventManager_h

#include "RecoParticleFlow/PFRootEvent/interface/PFRootEventManager.h"

/* 
Template for a personal analysis class to study particle flow. 
WARNING: Do not edit this file !
create an analysis class for you using this script: 
MyPFRootEventManager/createAnalysisClass.pl

run: 
createAnalysisClass.pl -h 
for more information.
 */ 

class MyPFRootEventManager : public PFRootEventManager {
  
 public:
  MyPFRootEventManager(const char* file);

  ~MyPFRootEventManager();

  bool processEntry(int entry);
  void write();


 private:
  
};

#endif