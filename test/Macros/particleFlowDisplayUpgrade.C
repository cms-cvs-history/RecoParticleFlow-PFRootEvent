{
// initialize the core of the framework, and load the PFRootEvent 
// library, which contains the ROOT interface
gSystem->Load("libFWCoreFWLite.so");
gSystem->Load("libRecoParticleFlowPFRootEvent.so");
// AutoLibraryLoader::enable();
gSystem->Load("libCintex.so");
ROOT::Cintex::Cintex::Enable();

string opt = "particleFlowUpgrade.opt";

// create a PFRootEventManagerUpgrade
PFRootEventManagerUpgrade em( opt.c_str() );
 if ( em.ev_ && em.ev_->isValid() ) {
   em.ev_->getTFile()->cd();

   // create a DisplayManager (upgraded)
   DisplayManagerUpgrade dm(&em, opt.c_str() );
   
   // display entry number 2
   int i=0;
   dm.display(i++);
   dm.lookForGenParticle(1);

   //if you want a dialogFrame:
   DialogFrameUpgrade *win = new DialogFrameUpgrade(&em,&dm,gClient->GetRoot(), 200,220);
 }

}
