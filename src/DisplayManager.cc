#include <iostream>
#include <fstream>
#include <stdlib.h>

#include "DataFormats/ParticleFlowReco/interface/PFBlock.h"
#include "RecoParticleFlow/PFBlockAlgo/interface/PFGeometry.h"
#include "DataFormats/ParticleFlowReco/interface/PFBlockElement.h"
#include "DataFormats/ParticleFlowReco/interface/PFClusterFwd.h"

#include "DataFormats/Math/interface/Point3D.h"

#include "RecoParticleFlow/PFRootEvent/interface/DisplayManager.h"
#include "RecoParticleFlow/PFRootEvent/interface/GPFRecHit.h"
#include "RecoParticleFlow/PFRootEvent/interface/GPFCluster.h"
#include "RecoParticleFlow/PFRootEvent/interface/GPFTrack.h"
#include "RecoParticleFlow/PFRootEvent/interface/GPFSimParticle.h"
#include "RecoParticleFlow/PFRootEvent/interface/GPFBase.h"

#include <TTree.h>
#include <TVector3.h>
#include <TH2F.h>
#include <TEllipse.h>
#include "TLine.h"
#include "TLatex.h"
#include "TList.h"
#include "TColor.h"
#include <TMath.h>
#include <TApplication.h>

//____________________________________________________________________________________________
DisplayManager::DisplayManager(PFRootEventManager *em) : em_(em), 
                                                     maxERecHitEcal_(-1),
						     maxERecHitHcal_(-1),
						     isGraphicLoaded_(false),
						     shiftId_(27)      
{
  getDisplayOptions();
  eventNb_  = em_->getEventIndex();
  //TODOLIST: re_initialize if new option file- better in em
  maxEvents_= em_->tree_->GetEntries();  
  
  createCanvas();
  
  //TODO :  Clusterlines don't work anymore
  vectGClusterLines_.resize(NViews);
  vectClusLNb_.resize(NViews);

}
//________________________________________________________
DisplayManager::~DisplayManager()
{
 reset();
} 
//________________________________________________________
void DisplayManager::createCanvas()
{

  //TODOLIST: better TCanvas *displayView_[4]
  displayView_.resize(NViews);
  displayHist_.resize(NViews);
   
  // TODOLIST:Canvases size  
  // Add menu to mofify canvas size
  // Add menu of views to be drawn
  
        
  displayView_[XY] = new TCanvas("displayXY_", "XY view",viewSize_[0], viewSize_[1]);
  displayView_[RZ] = new TCanvas("displayRZ_", "RZ view",viewSize_[0], viewSize_[1]);
  displayView_[EPE] = new TCanvas("displayEPE_", "eta/phi view, ECAL",viewSize_[0], viewSize_[1]);
  displayView_[EPH] = new TCanvas("displayEPH_", "eta/phi view, HCAL",viewSize_[0], viewSize_[1]);
  
  
  for (int viewType=0;viewType<NViews;++viewType) {
    displayView_[viewType]->SetGrid(0, 0);
    displayView_[viewType]->SetLeftMargin(0.12);
    displayView_[viewType]->SetBottomMargin(0.12);
    displayView_[viewType]->ToggleToolBar();
  } 
    
  // Draw support histogram
  double zLow = -500.;
  double zUp  = +500.;
  double rLow = -300.;
  double rUp  = +300.;
  displayHist_[XY] = new TH2F("hdisplayHist_XY", "", 500, rLow, rUp, 500, rLow, rUp);
  displayHist_[XY]->SetXTitle("X");
  displayHist_[XY]->SetYTitle("Y");
  
  displayHist_[RZ] = new TH2F("hdisplayHist_RZ", "",500, zLow, zUp, 500, rLow, rUp); 
  displayHist_[RZ]->SetXTitle("Z");
  displayHist_[RZ]->SetYTitle("R");
  
  displayHist_[EPE] = new TH2F("hdisplayHist_EP", "", 500, -5, 5, 500, -3.5, 3.5);
  displayHist_[EPE]->SetXTitle("#eta");
  displayHist_[EPE]->SetYTitle("#phi");
  
  displayHist_[EPH] = displayHist_[EPE];
  
  for (int viewType=0;viewType<NViews;++viewType){
    displayHist_[viewType]->SetStats(kFALSE);
  }  

  // Draw ECAL front face
  frontFaceECALXY_.SetX1(0);
  frontFaceECALXY_.SetY1(0);
  frontFaceECALXY_.SetR1(PFGeometry::innerRadius(PFGeometry::ECALBarrel));
  frontFaceECALXY_.SetR2(PFGeometry::innerRadius(PFGeometry::ECALBarrel));
  frontFaceECALXY_.SetFillStyle(0);
     
  // Draw HCAL front face
  frontFaceHCALXY_.SetX1(0);
  frontFaceHCALXY_.SetY1(0);
  frontFaceHCALXY_.SetR1(PFGeometry::innerRadius(PFGeometry::HCALBarrel));
  frontFaceHCALXY_.SetR2(PFGeometry::innerRadius(PFGeometry::HCALBarrel));
  frontFaceHCALXY_.SetFillStyle(0);
  
  // Draw ECAL side
  frontFaceECALRZ_.SetX1(-1.*PFGeometry::innerZ(PFGeometry::ECALEndcap));
  frontFaceECALRZ_.SetY1(-1.*PFGeometry::innerRadius(PFGeometry::ECALBarrel));
  frontFaceECALRZ_.SetX2(PFGeometry::innerZ(PFGeometry::ECALEndcap));
  frontFaceECALRZ_.SetY2(PFGeometry::innerRadius(PFGeometry::ECALBarrel));
  frontFaceECALRZ_.SetFillStyle(0);

}
//________________________________________________________________________________________________________  
void DisplayManager::createGCluster(const reco::PFCluster& cluster,int ident, double phi0)
{
  double eta = cluster.positionXYZ().Eta();
  double phi = cluster.positionXYZ().Phi();
  

//   int type = cluster.type();
//   if(algosToDisplay_.find(type) == algosToDisplay_.end() )
//     return;
  
//   TCutG* cutg = (TCutG*) gROOT->FindObject("CUTG");  
//   if( cutg && !cutg->IsInside( eta, phi ) ) return;


  int color = 4;
  if( em_->getDisplayColorClusters() ) color = 2;

  const math::XYZPoint& xyzPos = cluster.positionXYZ();
  
  for (int viewType=0;viewType<4;viewType++){

    switch(viewType) {
    case XY:
      graphicMap_.insert(pair<int,GPFBase *> (ident,new GPFCluster(this,viewType,ident,&cluster,xyzPos.X(),xyzPos.Y(),color)));
      break;
    case RZ:
     {
      double sign = 1.;
      if (cos(phi0 - phi) < 0.)
	 sign = -1.;
      graphicMap_.insert(pair<int,GPFBase *> (ident,new GPFCluster(this,viewType,ident,&cluster,xyzPos.z(),sign*xyzPos.Rho(),color)));
     } 
     break;
     case EPE:
      {
       if( cluster.layer()<0 ) {
         graphicMap_.insert(pair<int,GPFBase *> (ident,new GPFCluster(this,viewType,ident,&cluster,eta,phi,color)));
	 //createGClusterLines(cluster,viewType); 
       }
      } 
     break;
     case EPH:
      {
       if( cluster.layer()>0 ) {
	 graphicMap_.insert(pair<int,GPFBase *> (ident,new GPFCluster(this,viewType,ident,&cluster,eta,phi,color)));
	 //createGClusterLines(cluster,viewType);
       }
      } 
      break;
     default :break; 
    } 
  }      
}
//________________________________________________________________________________________
void DisplayManager::createGClusterLines(const reco::PFCluster& cluster,int viewType)
{
  
  int color = 2;

  const math::XYZPoint& xyzPos = cluster.positionXYZ();
  double eta = xyzPos.Eta(); 
  double phi = xyzPos.Phi(); 
  
  const std::vector< reco::PFRecHitFraction >& rhfracs = 
    cluster.recHitFractions();
    

//   int color = cluster.type();

  // draw a line from the cluster to each of the rechits
  int nbhits=0;
 
  for(unsigned i=0; i<rhfracs.size(); i++) {

    // rechit index 
    // unsigned rhi = rhfracs[i].recHitIndex();

    const reco::PFRecHit& rh = *(rhfracs[i].recHitRef());


    double rheta = rh.positionXYZ().Eta();
    double rhphi = rh.positionXYZ().Phi();
    
    TLine l(eta,phi,rheta,rhphi);
    l.SetLineColor(color);
    vectGClusterLines_[viewType].push_back(l);
    ++nbhits;
    
  }
  vectClusLNb_[viewType].push_back(nbhits);
}
//_______________________________________________________________________________________________
void DisplayManager::createGPart( const reco::PFSimParticle &ptc,
                                      const std::vector<reco::PFTrajectoryPoint>& points, 
                                      int ident,double pt,double phi0, double sign, bool displayInitial,
                                      int markerstyle)
{
  //bool inside = false; 
 //TCutG* cutg = (TCutG*) gROOT->FindObject("CUTG");
  
   for (int viewType=0;viewType<4;++viewType) {
     // reserving space. nb not all trajectory points are valid
     vector<double> xPos;
     xPos.reserve( points.size() );
     vector<double> yPos;
     yPos.reserve( points.size() );
  
     for(unsigned i=0; i<points.size(); i++) {
      if( !points[i].isValid() ) continue;
	
      const math::XYZPoint& xyzPos = points[i].positionXYZ();      
      double eta = xyzPos.Eta();
      double phi = xyzPos.Phi();
    
      if( !displayInitial && 
	  points[i].layer() == reco::PFTrajectoryPoint::ClosestApproach ) {
         const math::XYZTLorentzVector& mom = points[i].momentum();
         eta = mom.Eta();
         phi = mom.Phi();
      }
    
      //if( !cutg || cutg->IsInside( eta, phi ) ) 
      //  inside = true;
         
      switch(viewType) {
       case XY:
        xPos.push_back(xyzPos.X());
        yPos.push_back(xyzPos.Y());
       break;
       case RZ:
        xPos.push_back(xyzPos.Z());
        yPos.push_back(sign*xyzPos.Rho());
       break;
       case EPE:
       case EPH:
        xPos.push_back( eta );
        yPos.push_back( phi );
       break;
       default:break;
      }
    }  

    /// no point inside graphical cut.
    //if( !inside ) return;
    graphicMap_.insert(pair<int,GPFBase *> (ident,new GPFSimParticle(this,viewType,ident,&ptc,xPos.size(),&xPos[0],&yPos[0],pt,markerstyle,"pl")));
  }
}
//____________________________________________________________________
void DisplayManager::createGRecHit(reco::PFRecHit& rh,int ident, double maxe, double phi0, int color)
{

  double me = maxe;
  double thresh = 0;
  int layer = rh.layer();
  

  switch(layer) {
    case PFLayer::ECAL_BARREL:
      thresh = em_->clusterAlgoECAL_.threshBarrel();
    break;     
    case PFLayer::ECAL_ENDCAP:
      thresh = em_->clusterAlgoECAL_.threshEndcap();
    break;     
    case PFLayer::HCAL_BARREL1:
    case PFLayer::HCAL_BARREL2:
      thresh = em_->clusterAlgoHCAL_.threshBarrel();
    break;           
    case PFLayer::HCAL_ENDCAP:
      thresh = em_->clusterAlgoHCAL_.threshEndcap();
    break;           
    case PFLayer::PS1:
    case PFLayer::PS2:
      me = -1;
      thresh = em_->clusterAlgoPS_.threshBarrel();
    break;
    default:
      {
       cerr<<"DisplayManager::createGRecHit : manage other layers."
	<<" GRechit notcreated."<<endl;
       return;
      }
  }
  if( rh.energy() < thresh ) return;
  
  //loop on all views
  for(int viewType=0;viewType<4;++viewType) {
  
     if(  viewType == EPH && 
        ! (layer == PFLayer::HCAL_BARREL1 || 
	  layer == PFLayer::HCAL_BARREL2 || 
	  layer == PFLayer::HCAL_ENDCAP ) ) {
	    continue;
     }
     // on EPE view, draw only HCAL and preshower
     if(  viewType == EPE && 
        (layer == PFLayer::HCAL_BARREL1 || 
	 layer == PFLayer::HCAL_BARREL2 || 
	 layer == PFLayer::HCAL_ENDCAP ) ) {
         continue;
     }
     double rheta = rh.positionREP().Eta();
     double rhphi = rh.positionREP().Phi();

     double sign = 1.;
     if (cos(phi0 - rhphi) < 0.) sign = -1.;


     double etaSize[4];
     double phiSize[4];
     double x[5];
     double y[5];
     double z[5];
     double r[5];
     double eta[5];
     double phi[5];
     double xprop[5];
     double yprop[5];
     double etaprop[5];
     double phiprop[5];

  
     const std::vector< math::XYZPoint >& corners = rh.getCornersXYZ();
     assert(corners.size() == 4);
     double propfact = 0.95; // so that the cells don't overlap ? 
     double ampl=0;
     if(me>0) ampl = (log(rh.energy() + 1.)/log(me + 1.));
     
     for ( unsigned jc=0; jc<4; ++jc ) { 
       // cout<<"corner "<<jc<<" "<<corners[jc].Eta()<<" "<<corners[jc].Phi()<<endl;
       phiSize[jc] = rhphi-corners[jc].Phi();
       etaSize[jc] = rheta-corners[jc].Eta();
       if ( phiSize[jc] > 1. ) phiSize[jc] -= 2.*TMath::Pi();  // this is strange...
       if ( phiSize[jc] < -1. ) phiSize[jc]+= 2.*TMath::Pi();
       phiSize[jc] *= propfact;
       etaSize[jc] *= propfact;

       math::XYZPoint cornerposxyz = corners[jc];

       x[jc] = cornerposxyz.X();
       y[jc] = cornerposxyz.Y();
       z[jc] = cornerposxyz.Z();
       r[jc] = sign*cornerposxyz.Rho();
       eta[jc] = rheta - etaSize[jc];
       phi[jc] = rhphi - phiSize[jc];
   

    // cell area is prop to log(E)
    // not drawn for preshower. 
    // otherwise, drawn for eta/phi view, and for endcaps in xy view
       if( layer != PFLayer::PS1 && 
	   layer != PFLayer::PS2 && 
	  ( viewType == EPE || 
	    viewType == EPH || 
	    ( viewType == XY &&  
	    ( layer == PFLayer::ECAL_ENDCAP || 
	      layer == PFLayer::HCAL_ENDCAP ) ) ) ) {
      
      
         math::XYZPoint centreXYZrot = rh.positionXYZ();

         math::XYZPoint centertocorner(x[jc] - centreXYZrot.X(), 
				    y[jc] - centreXYZrot.Y(),
				    0 );

         math::XYZPoint centertocornerep(eta[jc] - centreXYZrot.Eta(), 
				      phi[jc] - centreXYZrot.Phi(),
				      0 );
      

        // centertocorner -= centreXYZrot;
         xprop[jc] = centreXYZrot.X() + centertocorner.X()*ampl;
         yprop[jc] = centreXYZrot.Y() + centertocorner.Y()*ampl;

         etaprop[jc] = centreXYZrot.Eta() + centertocornerep.X()*ampl;
         phiprop[jc] = centreXYZrot.Phi() + centertocornerep.Y()*ampl;
      }
    }//loop on jc 
  
    if(layer == PFLayer::ECAL_BARREL  || 
       layer == PFLayer::HCAL_BARREL1 || 
       layer == PFLayer::HCAL_BARREL2 || viewType == RZ) {

    // we are in the barrel. Determining which corners to shift 
    // away from the center to represent the cell energy
    
      int i1 = -1;
      int i2 = -1;

      if(fabs(phiSize[1]-phiSize[0]) > 0.0001) {
        if (viewType == XY) {
	  i1 = 2;
	  i2 = 3;
        } else if (viewType == RZ) {
	  i1 = 1;
	  i2 = 2;
        }
      } else {
        if (viewType == XY) {
	  i1 = 1;
	  i2 = 2;
        } else if (viewType == RZ) {
	  i1 = 2;
	  i2 = 3;
        }
      }

      x[i1] *= 1+ampl/2.;
      x[i2] *= 1+ampl/2.;
      y[i1] *= 1+ampl/2.;
      y[i2] *= 1+ampl/2.;
      z[i1] *= 1+ampl/2.;
      z[i2] *= 1+ampl/2.;
      r[i1] *= 1+ampl/2.;
      r[i2] *= 1+ampl/2.;
   }
   x[4]=x[0];
   y[4]=y[0]; // closing the polycell
   z[4]=z[0];
   r[4]=r[0]; // closing the polycell
   eta[4]=eta[0];
   phi[4]=phi[0]; // closing the polycell

   int npoints=5;
  
   switch( viewType ) {
   case  XY:
     {
      if(layer == PFLayer::ECAL_BARREL || 
	 layer == PFLayer::HCAL_BARREL1 || 
	 layer == PFLayer::HCAL_BARREL2) {
	 graphicMap_.insert(pair<int,GPFBase *> (ident,new GPFRecHit(this, viewType,ident,&rh,npoints,x,y,color,"f")));

      } else {
	 graphicMap_.insert(pair<int,GPFBase *> (ident,new GPFRecHit(this, viewType,ident,&rh,npoints,x,y,color,"l")));
	if( ampl>0 ) { // not for preshower
	  xprop[4]=xprop[0];
	  yprop[4]=yprop[0]; // closing the polycell    
	 graphicMap_.insert(pair<int,GPFBase *> (ident,new GPFRecHit(this, viewType,ident,&rh,npoints,xprop,yprop,color,"f")));
	}
      }
     } 
    break;
   
    case RZ:
      graphicMap_.insert(pair<int,GPFBase *> (ident,new GPFRecHit(this, viewType,ident,&rh,npoints,z,r,color,"f")));
    break;
    
    case EPE:
    {
      graphicMap_.insert(pair<int,GPFBase *> (ident,new GPFRecHit(this, viewType,ident,&rh,npoints,eta,phi,color,"l")));
      
      if( ampl>0 ) { // not for preshower
	etaprop[4]=etaprop[0];
	phiprop[4]=phiprop[0]; // closing the polycell    
        graphicMap_.insert(pair<int,GPFBase *> (ident,new GPFRecHit(this, viewType,ident,&rh,npoints,etaprop,phiprop,color,"f")));
      }
    }  
    break;
    case EPH:
    {      
     graphicMap_.insert(pair<int,GPFBase *> (ident,new GPFRecHit(this, viewType,ident,&rh,npoints,eta,phi,color,"l")));
     
     if( ampl>0 ) { // not for preshower
	etaprop[4]=etaprop[0];
	phiprop[4]=phiprop[0]; // closing the polycell    
        graphicMap_.insert(pair<int,GPFBase *> (ident,new GPFRecHit(this, viewType,ident,&rh,npoints,etaprop,phiprop,color,"f")));
     }
    } 
    break;
    
    default: break;
  }//switch end
    
 } //loop on views
}

//_________________________________________________________________________________________
void DisplayManager::createGTrack( reco::PFRecTrack &tr,
                                        const std::vector<reco::PFTrajectoryPoint>& points, 
                                        int ident,double pt,double phi0, double sign, bool displayInitial,
                                        int linestyle) 
{
      
  bool inside = false; 
  //TCutG* cutg = (TCutG*) gROOT->FindObject("CUTG");
  
  for (int viewType=0;viewType<4;++viewType) {
    // reserving space. nb not all trajectory points are valid
    vector<double> xPos;
    xPos.reserve( points.size() );
    vector<double> yPos;
    yPos.reserve( points.size() );
    
    for(unsigned i=0; i<points.size(); i++) {
      if( !points[i].isValid() ) continue;
      
      const math::XYZPoint& xyzPos = points[i].positionXYZ();      
      double eta = xyzPos.Eta();
      double phi = xyzPos.Phi();
    
      if( !displayInitial && 
	  points[i].layer() == reco::PFTrajectoryPoint::ClosestApproach ) {
          const math::XYZTLorentzVector& mom = points[i].momentum();
          eta = mom.Eta();
          phi = mom.Phi();
      }
    
    //if( !cutg || cutg->IsInside( eta, phi ) ) 
    //  inside = true;
    

      switch(viewType) {
       case XY:
        xPos.push_back(xyzPos.X());
        yPos.push_back(xyzPos.Y());
        break;
       case RZ:
        xPos.push_back(xyzPos.Z());
        yPos.push_back(sign*xyzPos.Rho());
        break;
       case EPE:
       case EPH:
        xPos.push_back( eta );
        yPos.push_back( phi );
        break;
      }
    }  
    /// no point inside graphical cut.
    //if( !inside ) return;
  
    //fill map with graphic objects
    graphicMap_.insert(pair<int,GPFBase *> (ident,new GPFTrack(this,viewType,ident,&tr,xPos.size(),&xPos[0],&yPos[0],pt,linestyle,"pl")));
 }   
}
//________________________________________________________
void DisplayManager::display(int ientry)
{
 if (ientry<0 || ientry>maxEvents_) {
   std::cerr<<"DisplayManager::no event matching criteria"<<std::endl;
   return;
 }  
 reset();
 em_->processEntry(ientry);  
 eventNb_= em_->getEventIndex();
 loadGraphicObjects();
 isGraphicLoaded_= true;
 displayAll();
}
//________________________________________________________________________________
void DisplayManager::displayAll()
{
  if (!isGraphicLoaded_) {
    std::cout<<" no Graphic Objects to draw"<<std::endl;
    return;
  }
  if (!redrawWithoutHits_) {  
    for (int viewType=0;viewType<NViews;++viewType) {
      displayView_[viewType]->cd();
      gPad->Clear();
    } 
    //TODOLIST: add test on view to draw 
    displayCanvas();
  }  
 
  std::multimap<int,GPFBase *>::iterator p;
   
  for (p=graphicMap_.begin();p!=graphicMap_.end();p++) {
    int ident=p->first;
    int type=ident >> shiftId_;
    int view = p->second->getView();
    switch (type) {
       case CLUSTERECALID: case CLUSTERHCALID: case  CLUSTERPSID: case CLUSTERIBID:
         {
	   if (drawClus_)
	     if (p->second->getEnergy() > clusEnMin_) {
	           displayView_[view]->cd();
                   p->second->draw();
	     }	   
	 }
	 break;
       case RECHITECALID: case  RECHITHCALID: case RECHITPSID:
         {
	   if (redrawWithoutHits_) break; 
	   if (drawHits_) 
	     if(p->second->getEnergy() > hitEnMin_)  {
	        displayView_[view]->cd();
                p->second->draw();
	     }
	   break;  
	 }  
       case RECTRACKID:
         {
	   if (drawTracks_) 
	     if (p->second->getPt() > trackPtMin_) {
	        displayView_[view]->cd();
                p->second->draw();
	     }
	 }
	 break;
       case SIMPLEPARTID:
         {
	   if (drawParticles_)
	     if (p->second->getPt() > particlePtMin_) {
	        displayView_[view]->cd();
                p->second->draw();
	     }
	 }
	 break;
      default : std::cout<<"DisplayManager::displayAll()-- unknown object "<<std::endl;	     	    
    }  //switch end
  }   //for end
  for (int i=0;i<NViews;i++) {
    displayView_[i]->cd();
    gPad->Modified();
    displayView_[i]->Update();
  }  
}
//___________________________________________________________________________________
void DisplayManager::displayCanvas()
{
 double zLow = -500.;
 double zUp  = +500.;
 double rUp  = +300.;
 
 //TODOLIST : test wether view on/off
 //if (!displayView_[viewType] || !gROOT->GetListOfCanvases()->FindObject(displayView_[viewType]) ) {
 //   assert(viewSize_.size() == 2);
  
 for (int viewType=0;viewType<NViews;++viewType) {
   displayView_[viewType]->cd();
   displayHist_[viewType]->Draw();
   switch(viewType) {
     case XY: 
        frontFaceECALXY_.Draw();
        frontFaceHCALXY_.Draw();
	break;
     case RZ:	
       {// Draw lines at different etas
        TLine l;
        l.SetLineColor(1);
        l.SetLineStyle(3);
        TLatex etaLeg;
        etaLeg.SetTextSize(0.02);
        float etaMin = -3.;
        float etaMax = +3.;
        float etaBin = 0.2;
        int nEtas = int((etaMax - etaMin)/0.2) + 1;
        for (int iEta = 0; iEta <= nEtas; iEta++) {
	  float eta = etaMin + iEta*etaBin;
	  float r = 0.9*rUp;
          TVector3 etaImpact;
	  etaImpact.SetPtEtaPhi(r, eta, 0.);
	  etaLeg.SetTextAlign(21);
	  if (eta <= -1.39) {
	    etaImpact.SetXYZ(0.,0.85*zLow*tan(etaImpact.Theta()),0.85*zLow);
	    etaLeg.SetTextAlign(31);
	  } else if (eta >= 1.39) {
	    etaImpact.SetXYZ(0.,0.85*zUp*tan(etaImpact.Theta()),0.85*zUp);
	    etaLeg.SetTextAlign(11);
	  }
	  l.DrawLine(0., 0., etaImpact.Z(), etaImpact.Perp());
	  etaLeg.DrawLatex(etaImpact.Z(), etaImpact.Perp(), Form("%2.1f", eta));
        }
        frontFaceECALRZ_.Draw();
       }	
       break;
     default: break;
   } //end switch
  }
}
//________________________________________________________________________________
void DisplayManager::displayNext()
{
 int eventNb_=em_->getEventIndex();
 display(++eventNb_);
}
//_________________________________________________________________________________
void DisplayManager::displayNextInteresting(int ientry)
{
  bool ok=false;
  while (!ok && ientry<em_->tree_->GetEntries() ) {
   ok = em_->processEntry(ientry);
   ientry++;
  }
  eventNb_ = em_->getEventIndex();
  if (ok) {
    reset();
    loadGraphicObjects();
    isGraphicLoaded_= true;
    displayAll();
  }
  else 
   std::cerr<<"DisplayManager::dislayNextInteresting : no event matching criteria"<<std::endl;
}
//_________________________________________________________________________________
void DisplayManager::displayPrevious()
{
 int eventNb_=em_->getEventIndex();
 display(--eventNb_);
}
//______________________________________________________________________________
void DisplayManager::redraw()
{
  int size = selectedGObj_.size();
  bool toInitial=true;
  int color=0;
  for (int i=0;i<size;i++) 
    drawGObject(selectedGObj_[i],color,toInitial);
}
//_______________________________________________________________________________
void DisplayManager::displayPFBlock(int blockNb) 
{
 redraw();
 selectedGObj_.clear();
 if (!drawPFBlocks_) return;
 int color=1;
  multimap<int,int>::const_iterator p;
  p= blockIdentsMap_.find(blockNb);
  if (p !=blockIdentsMap_.end()) {
    do {
      int ident=p->second;
      drawGObject(ident,color,false);
      p++;
    } while (p!=blockIdentsMap_.upper_bound(blockNb));
  }
  else 
    cout<<"DisplayManager::displayPFBlock :not found"<<endl;    
}  
//_______________________________________________________________________________
void DisplayManager::drawGObject(int ident,int color,bool toInitial) 
{
   typedef std::multimap<int,GPFBase *>::const_iterator iter;
   iter p;
   std::pair<iter, iter > result = graphicMap_.equal_range(ident);
   if(result.first == graphicMap_.end()) {
	return;
   }
   p=result.first;
   while (p != result.second) {
     int view=p->second->getView();
     displayView_[view]->cd();
     if (toInitial) p->second->setInitialColor();
     else p->second->setColor(color);
     p->second->draw();
     gPad->Modified();
     displayView_[view]->Update();
     if (!toInitial) selectedGObj_.push_back(ident);
     p++; 
   }
}
//______________________________________________________________________________
void DisplayManager::enableDrawPFBlock(bool state)
{
  drawPFBlocks_=state;
}  
//_______________________________________________________________________________
void DisplayManager::findAndDraw(int ident) 
{

  int type=ident >> shiftId_;
  int color=1;
  if (type>8) {
    std ::cout<<"DisplayManager::findAndDraw :object Type unknown"<<std::endl;
    return;
  }  
  if (drawPFBlocks_==0  || type<3 || type==8) {
    redraw();
    selectedGObj_.clear();
    bool toInitial=false;
    drawGObject(ident,color,toInitial);
    if (type<3) {
      redrawWithoutHits_=true;
      displayAll();
      redrawWithoutHits_=false;
    }
  }     
}
//___________________________________________________________________________________
void DisplayManager::findBlock(int ident) 
{
  int blockNb=-1;
  multimap<int,int>::const_iterator p;
  for (p=blockIdentsMap_.begin();p!=blockIdentsMap_.end();p++) {
    int id=p->second;
    if (id == ident) {
      blockNb=p->first;
      break;
    }   
  }
  if (blockNb > -1) {
    std::cout<<"this object belongs to PFblock nb "<<blockNb<<std::endl;
    displayPFBlock(blockNb);
  }   
}  
//_________________________________________________________________________________
void DisplayManager::getDisplayOptions()
{
  //todolist
  //recuperer les options de viewsize dans eventManager ?
  
  viewSizeEtaPhi_= em_->getViewSizeEtaPhi();
  viewSize_      = em_->getViewSize();    

  hitEnMin_      = em_->getDisplayRecHitsEnMin();
  clusEnMin_     = em_->getDisplayClustersEnMin();
  trackPtMin_    = em_->getDisplayRecTracksPtMin();
  particlePtMin_ = em_->getDisplayTrueParticlesPtMin();
  
  drawTracks_    = em_->getDisplayRecTracks();
  drawParticles_ = em_->getDisplayTrueParticles();
  drawClusterL_  = em_->getDisplayClusterLines();
  drawClus_      = em_->getDisplayClusters();
  drawHits_      = em_->getDisplayRecHits();
  drawPFBlocks_  = true;
  redrawWithoutHits_=false;
  
  zoomFactor_    = em_->getDisplayZoomFactor();
  

}
//____________________________________________________________________________________________________
double DisplayManager::getMaxE(int layer) const
{

  double maxe = -9999;

  // in which vector should we look for these rechits ?

  const reco::PFRecHitCollection* vec = 0;
  switch(layer) {
  case PFLayer::ECAL_ENDCAP:
  case PFLayer::ECAL_BARREL:
    vec = &(em_->rechitsECAL_);
    break;
  case PFLayer::HCAL_ENDCAP:
  case PFLayer::HCAL_BARREL1:
  case PFLayer::HCAL_BARREL2:
    vec = &(em_->rechitsHCAL_);
    break;
  case PFLayer::PS1:
  case PFLayer::PS2:
    vec = &(em_->rechitsPS_);
    break;
  default:
    cerr<<"DisplayManager::getMaxE : manage other layers"<<endl;
    return maxe;
  }

  for( unsigned i=0; i<vec->size(); i++) {
    if( (*vec)[i].layer() != layer ) continue;
    if( (*vec)[i].energy() > maxe)
      maxe = (*vec)[i].energy();
  }

  return maxe;
}
//____________________________________________________________________________
double DisplayManager::getMaxEEcal() {
  
  if( maxERecHitEcal_<0 ) {
    double maxeec = getMaxE( PFLayer::ECAL_ENDCAP );
    double maxeb =  getMaxE( PFLayer::ECAL_BARREL );
    maxERecHitEcal_ = maxeec > maxeb ? maxeec:maxeb; 
    // max of both barrel and endcap
  }
  return  maxERecHitEcal_;
}
//_______________________________________________________________________________
double DisplayManager::getMaxEHcal() {

  if(maxERecHitHcal_ < 0) {
    double maxeec = getMaxE( PFLayer::HCAL_ENDCAP );
    double maxeb =  getMaxE( PFLayer::HCAL_BARREL1 );
    maxERecHitHcal_ =  maxeec>maxeb  ?  maxeec:maxeb;
  }
  return maxERecHitHcal_;
} 
//____________________________________________________________________________  
void DisplayManager::loadGClusters()
{
 double phi0=0;
  
 for(unsigned i=0; i<em_->clustersECAL_->size(); i++){
   //int clusId=(i<<shiftId_) | CLUSTERECALID;
   int clusId=(CLUSTERECALID<<shiftId_) | i;
   createGCluster( (*(em_->clustersECAL_))[i],clusId, phi0);
 }    
 for(unsigned i=0; i<em_->clustersHCAL_->size(); i++) {
   //int clusId=(i<<shiftId_) | CLUSTERHCALID;
   int clusId=(CLUSTERHCALID<<shiftId_) | i;
   createGCluster( (*(em_->clustersHCAL_))[i],clusId, phi0);
 }    
 for(unsigned i=0; i<em_->clustersPS_->size(); i++){ 
   //int clusId=(i<<shiftId_) | CLUSTERPSID;
   int clusId=(CLUSTERPSID<<shiftId_) | i;
   createGCluster( (*(em_->clustersPS_))[i],clusId,phi0);
 }
 for(unsigned i=0; i<em_->clustersIslandBarrel_.size(); i++) {
   int layer = PFLayer::ECAL_BARREL;
   //int clusId=(i<<shiftId_) | CLUSTERIBID;
   int clusId=(CLUSTERIBID<<shiftId_) | i;
   
   reco::PFCluster cluster( layer, 
			    em_->clustersIslandBarrel_[i].energy(),
			    em_->clustersIslandBarrel_[i].x(),
			    em_->clustersIslandBarrel_[i].y(),
			    em_->clustersIslandBarrel_[i].z() ); 
   createGCluster( cluster,clusId, phi0);
 }  
}
//_____________________________________________________________________
void DisplayManager::loadGPFBlocks()
{
  int size = em_->pfBlocks_->size();
  for (int ibl=0;ibl<size;ibl++) {
    int elemNb=((*(em_->pfBlocks_))[ibl].elements()).size();
    //std::cout<<"block "<<ibl<<":"<<elemNb<<" elements"<<std::flush<<std::endl;
    edm::OwnVector< reco::PFBlockElement >::const_iterator iter;
    for( iter =((*(em_->pfBlocks_))[ibl].elements()).begin();
         iter != ((*(em_->pfBlocks_))[ibl].elements()).end();iter++) {
         //std::cout<<"elem index "<<(*iter).index()<<"-type:"
    	 //         <<(*iter).type()<<std::flush<<std::endl;
         int ident=-1;		  	     
	 switch ((*iter).type()) {
	   case reco::PFBlockElement::NONE :
	     std::cout<<"unknown PFBlock element"<<std::endl;
             break;
	   case reco::PFBlockElement::TRACK:
	   {
	    reco::TrackRef trackref =(*iter).trackRef();  
	    assert( !trackref.isNull() );
            //std::cout<<" - key "<<trackref.key()<<std::flush<<std::endl<<std::endl;
	    ident=(RECTRACKID <<shiftId_) |trackref.key();
	   }
	   break;
	   case reco::PFBlockElement::PS1:
	    {
	     reco::PFClusterRef clusref=(*iter).clusterRef();
	     assert( !clusref.isNull() );
             //std::cout<<"- key "<<clusref.key()<<std::flush<<std::endl<<std::endl;
	     ident=(CLUSTERPSID <<shiftId_) |clusref.key();
	    }
	    break;
	   case reco::PFBlockElement::PS2:
	    {
	     reco::PFClusterRef clusref=(*iter).clusterRef();
	     assert( !clusref.isNull() );
             //std::cout<<"key "<<clusref.key()<<std::flush<<std::endl<<std::endl;
	     ident=(CLUSTERPSID <<shiftId_) |clusref.key();
	    }
	    break;
	   case reco::PFBlockElement::ECAL:
	    {
	     reco::PFClusterRef clusref=(*iter).clusterRef();
	     assert( !clusref.isNull() );
             //std::cout<<"key "<<clusref.key()<<std::flush<<std::endl<<std::endl;
	     ident=(CLUSTERECALID <<shiftId_) |clusref.key();
	    }
	    break;
	   case reco::PFBlockElement::HCAL:
	    {
	     reco::PFClusterRef clusref=(*iter).clusterRef();
	     assert( !clusref.isNull() );
             //std::cout<<"key "<<clusref.key()<<std::flush<<std::endl<<std::endl;
	     ident=(CLUSTERHCALID <<shiftId_) |clusref.key();
	    }
	    break;
	   case reco::PFBlockElement::MUON:
	     break;
	   default: 
	     std::cout<<"unknown PFBlock element"<<std::endl;
	     break; 
	 } //end switch 
	 if (ident != -1) blockIdentsMap_.insert(pair<int,int> (ibl,ident));	     
    }   //end for elements
  }   //end for blocks
   
}
//__________________________________________________________________________________
void DisplayManager::loadGraphicObjects()
{
 loadGClusters();
 loadGRecHits();
 loadGRecTracks();
 loadGTrueParticles();
 loadGPFBlocks();
}
//________________________________________________________
void DisplayManager::loadGRecHits()
{
 double phi0=0;
   
 double maxee = getMaxEEcal();
 double maxeh = getMaxEHcal();
 double maxe = maxee>maxeh ? maxee : maxeh;
 
 int color = TColor::GetColor(210,210,210);
 int seedcolor = TColor::GetColor(145,145,145);
 int specialcolor = TColor::GetColor(255,140,0);
   
 for(unsigned i=0; i<em_->rechitsECAL_.size(); i++) { 
   int rhcolor = color;
   if( unsigned col = em_->clusterAlgoECAL_.color(i) ) {
     switch(col) {
       case PFClusterAlgo::SEED: rhcolor = seedcolor; break;
       case PFClusterAlgo::SPECIAL: rhcolor = specialcolor; break;
       default:
         cerr<<"DisplayManager::loadGRecHits: unknown color"<<endl;
     }
   }
   //int recHitId=(i<<shiftId_) | RECHITECALID;
   int recHitId=i;
   createGRecHit(em_->rechitsECAL_[i],recHitId, maxe, phi0, rhcolor);
 }
   
 for(unsigned i=0; i<em_->rechitsHCAL_.size(); i++) { 
   int rhcolor = color;
   if(unsigned col = em_->clusterAlgoHCAL_.color(i) ) {
     switch(col) {
       case PFClusterAlgo::SEED: rhcolor = seedcolor; break;
       case PFClusterAlgo::SPECIAL: rhcolor = specialcolor; break;
       default:
	   cerr<<"DisplayManager::loadGRecHits: unknown color"<<endl;
     }
   }
   //int recHitId=(i<<shiftId_) | RECHITHCALID;
   int recHitId=(RECHITHCALID <<shiftId_) | i;
   createGRecHit(em_->rechitsHCAL_[i],recHitId, maxe, phi0, rhcolor);
 }
  
 for(unsigned i=0; i<em_->rechitsPS_.size(); i++) { 
   int rhcolor = color;
   if( unsigned col = em_->clusterAlgoPS_.color(i) ) {
     switch(col) {
       case PFClusterAlgo::SEED: rhcolor = seedcolor; break;
       case PFClusterAlgo::SPECIAL: rhcolor = specialcolor; break;
       default:
	cerr<<"DisplayManager::loadGRecHits: unknown color"<<endl;
     }
   }
   //int recHitId=(i<<shiftId_) | RECHITPSID;
   int recHitId=(RECHITPSID<<shiftId_) | i;
   
   createGRecHit(em_->rechitsPS_[i],recHitId, maxe, phi0, rhcolor);
 }
} 
//________________________________________________________________________
void DisplayManager::loadGRecTracks()
{
 double phi0=0;
 
 int ind=-1;
 std::vector<reco::PFRecTrack>::iterator itRecTrack;
 for (itRecTrack = em_->recTracks_.begin(); itRecTrack != em_->recTracks_.end();itRecTrack++) {
   double sign = 1.;
   const reco::PFTrajectoryPoint& tpinitial 
         = itRecTrack->extrapolatedPoint(reco::PFTrajectoryPoint::ClosestApproach);
   double pt = tpinitial.momentum().Pt();
   //if( pt<em_->displayRecTracksPtMin_ ) continue;

    const reco::PFTrajectoryPoint& tpatecal 
      = itRecTrack->trajectoryPoint(itRecTrack->nTrajectoryMeasurements() +
				    reco::PFTrajectoryPoint::ECALEntrance );
    
    if ( cos(phi0 - tpatecal.momentum().Phi()) < 0.)
      sign = -1.;

    const std::vector<reco::PFTrajectoryPoint>& points = 
      itRecTrack->trajectoryPoints();

    int    linestyle = itRecTrack->algoType(); 
    ind++;
    //int recTrackId=(ind<<shiftId_) | RECTRACKID;
    int recTrackId=(RECTRACKID <<shiftId_) | ind; 

    createGTrack(*itRecTrack,points,recTrackId, pt, phi0, sign, false,linestyle);
  }
}
//___________________________________________________________________________
void DisplayManager::loadGTrueParticles()
{
  double phi0=0;
  
  unsigned trueParticlesVSize = em_->trueParticles_.size();

  for(unsigned i=0; i<trueParticlesVSize; i++) {
    
    const reco::PFSimParticle& ptc = em_->trueParticles_[i];
    
    const reco::PFTrajectoryPoint& tpinitial 
      = ptc.extrapolatedPoint( reco::PFTrajectoryPoint::ClosestApproach );
    
    double pt = tpinitial.momentum().Pt();
    //if( pt<em_->getDisplayTrueParticlesPtMin()) continue;

    double sign = 1.;
    
    const reco::PFTrajectoryPoint& tpatecal 
      = ptc.trajectoryPoint(ptc.nTrajectoryMeasurements() +
			    reco::PFTrajectoryPoint::ECALEntrance );
    
    if ( cos(phi0 - tpatecal.momentum().Phi()) < 0.)
      sign = -1.;

    const std::vector<reco::PFTrajectoryPoint>& points = 
      ptc.trajectoryPoints();
      
    int markerstyle;
    switch( abs(ptc.pdgCode() ) ) {
    case 22:   markerstyle = 3 ;   break; // photons
    case 11:   markerstyle = 5 ;   break; // electrons 
    case 13:   markerstyle = 2 ;   break; // muons 
    case 130:  
    case 321:  markerstyle = 24;  break; // K
    case 211:  markerstyle = 25;  break; // pi+/pi-
    case 2212: markerstyle = 26;  break; // protons
    case 2112: markerstyle = 27;  break; // neutrons  
    default:   markerstyle = 30;  break; 
    }
   
    //int    color = 4;
    //int    linestyle = 2;
    //double markersize = 0.8;
    bool displayInitial=true;
    if( ptc.motherId() < 0 ) displayInitial=false;
    
    //int partId=(i<<shiftId_) | SIMPLEPARTID;
    int partId=(SIMPLEPARTID << shiftId_) | i; 
    createGPart(ptc, points,partId, pt, phi0, sign, displayInitial,markerstyle);
  }
}
//_____________________________________________________________________________
void DisplayManager::lookForMaxRecHit(bool ecal)
{
  // look for the rechit with max e in ecal or hcal
  double maxe = -999;
  reco::PFRecHit* maxrh = 0;

  reco::PFRecHitCollection* rechits = 0;
  if(ecal) rechits = &(em_->rechitsECAL_);
  else rechits = &(em_->rechitsHCAL_);
  assert(rechits);

  for(unsigned i=0; i<(*rechits).size(); i++) {

    double energy = (*rechits)[i].energy();

    if(energy > maxe ) {
      maxe = energy;
      maxrh = &((*rechits)[i]);
    }      
  }
  
  if(!maxrh) return;

  // center view on this rechit


  // get the cell size to set the eta and phi width 
  // of the display window from one of the cells
  
  double phisize = -1;
  double etasize = -1;
  maxrh->size(phisize, etasize);
  
  double etagate = zoomFactor_ * etasize;
  double phigate = zoomFactor_ * phisize;
  
  double eta =  maxrh->positionREP().Eta();
  double phi =  maxrh->positionREP().Phi();
  
  if(displayHist_[EPE]) {
    displayHist_[EPE]->GetXaxis()->SetRangeUser(eta-etagate, eta+etagate);
    displayHist_[EPE]->GetYaxis()->SetRangeUser(phi-phigate, phi+phigate);
    displayView_[EPE]->Modified();
    displayView_[EPE]->Update();
  }
  
  if(displayHist_[EPH]) {
    displayHist_[EPH]->GetXaxis()->SetRangeUser(eta-etagate, eta+etagate);
    displayHist_[EPH]->GetYaxis()->SetRangeUser(phi-phigate, phi+phigate);
    displayView_[EPH]->Modified();
    displayView_[EPH]->Update();
  }
}
//________________________________________________________________________________
void DisplayManager::lookForGenParticle(unsigned barcode) {
  
  const HepMC::GenEvent* event = em_->MCTruth_.GetEvent();
  if(!event) {
    cerr<<"no GenEvent"<<endl;
    return;
  }
  
  const HepMC::GenParticle* particle = event->barcode_to_particle(barcode);
  if(!particle) {
    cerr<<"no particle with barcode "<<barcode<<endl;
    return;
  }

  math::XYZTLorentzVector momentum(particle->momentum().px(),
				   particle->momentum().py(),
				   particle->momentum().pz(),
				   particle->momentum().e());

  double eta = momentum.Eta();
  double phi = momentum.phi();

  double phisize = 0.05;
  double etasize = 0.05;
  
  double etagate = zoomFactor_ * etasize;
  double phigate = zoomFactor_ * phisize;
  
  if(displayHist_[EPE]) {
    displayHist_[EPE]->GetXaxis()->SetRangeUser(eta-etagate, eta+etagate);
    displayHist_[EPE]->GetYaxis()->SetRangeUser(phi-phigate, phi+phigate);
    displayView_[EPE]->Modified();
    displayView_[EPE]->Update();
    
  }
  if(displayHist_[EPH]) {
    displayHist_[EPH]->GetXaxis()->SetRangeUser(eta-etagate, eta+etagate);
    displayHist_[EPH]->GetYaxis()->SetRangeUser(phi-phigate, phi+phigate);
    displayView_[EPH]->Modified();
    displayView_[EPH]->Update();
  }
}
//_______________________________________________________________________
void DisplayManager::printDisplay(const char* sdirectory ) const
{
  string directory = sdirectory;
  if( directory.empty() ) {   
    directory = "Event_";
  }
  char num[10];
  sprintf(num,"%d", eventNb_);
  directory += num;

  string mkdir = "mkdir "; mkdir += directory;
  int code = system( mkdir.c_str() );

  if( code ) {
    cerr<<"cannot create directory "<<directory<<endl;
    return;
  }
  
  cout<<"Event display printed in directory "<<directory<<endl;

  directory += "/";
  
  for(unsigned iView=0; iView<displayView_.size(); iView++) {
    if( !displayView_[iView] ) continue;
    
    string name = directory;
    name += displayView_[iView]->GetName();

    cout<<displayView_[iView]->GetName()<<endl;

    string eps = name; eps += ".eps";
    displayView_[iView]->SaveAs( eps.c_str() );
    
    string png = name; png += ".png";
    displayView_[iView]->SaveAs( png.c_str() );
  }
  
  string txt = directory;
  txt += "event.txt";
  ofstream out( txt.c_str() );
  if( !out ) 
    cerr<<"cannot open "<<txt<<endl;
  em_->print( out );
}
//_____________________________________________________________________________
void DisplayManager::reset()
{
 maxERecHitEcal_=-1;
 maxERecHitHcal_=-1;
 isGraphicLoaded_= false;
 for (int i=0;i<NViews;i++) {
   //vectGHits_[i].clear();
   //vectGClus_[i].clear();
  // vectGTracks_[i].clear();
   //vectGParts_[i].clear();
   vectGClusterLines_[i].clear();
   vectClusLNb_[i].clear();
 }
 std::multimap<int,GPFBase *>::iterator p;
 for (p=graphicMap_.begin();p!=graphicMap_.end();p++)
   delete p->second;
 graphicMap_.clear();
 
 blockIdentsMap_.clear(); 
 selectedGObj_.clear();
  
}
//_______________________________________________________________________________
void DisplayManager::unZoom()
{
  for( unsigned i=0; i<displayHist_.size(); i++) {
    // the corresponding view was not requested
    if( ! displayHist_[i] ) continue;
    displayHist_[i]->GetXaxis()->UnZoom();
    displayHist_[i]->GetYaxis()->UnZoom();
  }
  updateDisplay();
}
//_________________________________________________________________________________
void DisplayManager::updateDisplay()
{
 for( unsigned i=0; i<displayView_.size(); i++) {
    if( gROOT->GetListOfCanvases()->FindObject(displayView_[i]) ) {
      displayView_[i]->Modified();
      displayView_[i]->Update();
    }  
  }
}
//_____________________________________________________________________________________________________
/*void DisplayManager::drawClusters(int viewType,double enmin)
{
  int size=vectGClus_[viewType].size();
  if (size) {
    if (enmin == 0) {
      int istart=0;
      for (int i=0;i<size;i++) {
        (vectGClus_[viewType])[i].Draw();
        if (drawClusterL_ && (viewType == EPE || viewType == EPH)) {
          drawClusterLines(i,viewType,istart);
        }	
      }
    }
  }
  else {
      int istart=0;
      for (int i=0;i<size;i++) {
        if ((vectGClus_[viewType])[i].getEnergy()>=enmin) {
	  (vectGClus_[viewType])[i].Draw();
          if (drawClusterL_ && (viewType == EPE || viewType == EPH)) {
            drawClusterLines(i,viewType,istart);
          }	
        }
     }
  }
}
//______________________________________________________________________________________________
void DisplayManager::drawClusterLines(int clusIndex,int viewType,int &istart)
{
    int i;
    for (i=istart;i<istart+(vectClusLNb_[viewType])[clusIndex];i++) 
        (vectGClusterLines_[viewType])[i].Draw();
    istart=i;
}
*/
