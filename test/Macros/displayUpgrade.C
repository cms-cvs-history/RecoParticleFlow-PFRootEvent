{
// initialize the core of the framework, and load the PFRootEvent 
// library, which contains the ROOT interface
gSystem->Load("libFWCoreFWLite.so");
gSystem->Load("libRecoParticleFlowPFRootEvent.so");
// // AutoLibraryLoader::enable();
gSystem->Load("libCintex.so");
ROOT::Cintex::Cintex::Enable();

// create a PFRootEventManager (upgraded)
// PFRootEventManagerUpgrade em("pfRootEvent.opt");
// create a JetPFRootEventManagerUpgrade to make FWLiteJets
string opt = "pfRootEvent.opt";
PFRootEventManagerUpgrade em( opt.c_str() );

//create a DisplayManager (upgraded)
DisplayManagerUpgrade dm(&em, opt.c_str() );

// display first entry
int i=0;
dm.display(i++);

// look for ECAL rechit with maximum energy
dm.lookForMaxRecHit(true);

// create a dialogFrame (upgraded):
// DialogFrameUpgrade *win = new DialogFrameUpgrade(&em,&dm,gClient->GetRoot(), 200,220);

}
