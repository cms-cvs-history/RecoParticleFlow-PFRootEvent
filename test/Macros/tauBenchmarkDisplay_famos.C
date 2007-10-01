{
// initialize the core of the framework, and load the PFRootEvent 
// library, which contains the ROOT interface
gSystem->Load("libFWCoreFWLite.so");
gSystem->Load("libRecoParticleFlowPFRootEvent.so");
// AutoLibraryLoader::enable();
gSystem->Load("libCintex.so");
ROOT::Cintex::Cintex::Enable();

// create a PFRootEventManager
// PFRootEventManager em("pfRootEvent.opt");
PFRootEventManager em("tauBenchmark_famos.opt");

// create a DisplayManager
DisplayManager dm(&em);

// display entry number 2
int i=0;
dm.display(i++);

// look for ECAL rechit with maximum energy
dm.lookForGenParticle(1);
//if you want a dialogFrame:
// DialogFrame *win = new DialogFrame(&em,&dm,gClient->GetRoot(), 200,220);

}
