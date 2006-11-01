#include "RecoParticleFlow/PFRootEvent/interface/EffResImp.h"

EffResImp::EffResImp(const char* name, 
		     const char* title, 
		     int nbinsx, double xlow, double xup, 
		     int nbinsy, double ylow, double yup, 
		     int nbinsz, double zlow, double zup) 
  : TH3D( name, title, 
	  nbinsx, xlow, xup, 
	  nbinsy, ylow, yup, 
	  nbinsz, zlow, zup ) {
  
}


void EffResImp::Fill() {

  Reset();
  
  for(unsigned it = 0; it<true_.size(); it++) {
    for(unsigned im = 0; im<meas_.size(); im++) {
      TH3D::Fill(true_[it].eta_, 
		 true_[it].e_, 
		 meas_[it].phi_ - true_[it].phi_ );
    }
  }
}
