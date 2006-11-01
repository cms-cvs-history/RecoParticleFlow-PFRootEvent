#ifndef RecoParticleFlow_PFRootEvent_EffResImp_h
#define RecoParticleFlow_PFRootEvent_EffResImp_h


#include <TH3.h>

class EffResImp : public TH3D {

 public:
  EffResImp(const char* name, 
	    const char* title, 
	    int nbinsx, double xlow, double xup, 
	    int nbinsy, double ylow, double yup, 
	    int nbinsz, double zlow, double zup);

  void addTrue( double eta, double phi, double e ) 
    {true_.push_back( Point(eta,phi,e) ); }
  void addMeas( double eta, double phi, double e )
    {meas_.push_back( Point(eta,phi,e) ); }
  void Fill();

  class Point {
  public:
    Point() : eta_(-10), phi_(-10), e_(-10) {}
    Point(double eta, double phi, double e) :
      eta_(eta), phi_(phi), e_(e) {} 
  private:
    double eta_;
    double phi_;
    double e_;    
    
    friend class EffResImp;
  };

 private:
  std::vector< EffResImp::Point > true_;
  std::vector< EffResImp::Point > meas_;
  

};

#endif
