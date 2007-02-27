{
  // initialize the core of the framework, and load the PFRootEvent 
  // library, which contains the ROOT interface
  gSystem->Load("libFWCoreFWLite.so");
  gSystem->Load("libRecoParticleFlowPFRootEvent.so");
  AutoLibraryLoader::enable();
  gSystem->Load("libCintex.so");
  ROOT::Cintex::Cintex::Enable();
  
  // create a PFRootEventManager
  //PFRootEventManager em("pfRootEvent.opt");
  PFRootEventManager em("tauBenchmark.opt");

  for(int i=0; i < em.tree_->GetEntries(); ++i)
    em.processEntry(i);
  
  //writing output 
  em.write();
  
  //closing root session
  gROOT->ProcessLine(".q");
  
}//RunAlex
