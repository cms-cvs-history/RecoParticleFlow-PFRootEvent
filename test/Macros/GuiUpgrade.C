{
// initialize the core of the framework, and load the PFRootEvent 
// library, which contains the ROOT interface
gSystem->Load("libFWCoreFWLite.so");
gSystem->Load("libRecoParticleFlowPFRootEvent.so");
//AutoLibraryLoader::enable();
gSystem->Load("libCintex.so");
ROOT::Cintex::Cintex::Enable();

// create a PFRootEventManager (upgraded)
PFRootEventManagerUpgrade em("tauBenchmark.opt");
// create a DisplayManager (upgraded)
DisplayManagerUpgrade dm(&em);

int i=0;
dm.display(i++);
// look for ECAL rechit with maximum energy
//dm.lookForMaxRecHit(true);

//create a dialogFrame (upgraded):
// DialogFrameUpgrade *win = new DialogFrameUpgrade(&em,&dm,gClient->GetRoot(), 200,220);
}
