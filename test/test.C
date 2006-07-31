{

gSystem->Load("libFWCoreFWLite.so");
gSystem->Load("libRecoParticleFlowPFRootEvent.so");
AutoLibraryLoader::enable();
gSystem->Load("libCintex.so");
ROOT::Cintex::Cintex::Enable();

PFRootEventManager em("pfRootEvent.opt");
int i=0;
em.display(i++);
em.lookForMaxRecHit(true);
}
