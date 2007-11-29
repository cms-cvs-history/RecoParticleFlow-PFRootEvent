#include "RecoParticleFlow/PFRootEvent/interface/JetPFRootEventManager.h"
#include "RecoParticleFlow/PFRootEvent/interface/PFRootEventManager.h"
#include "RecoParticleFlow/PFRootEvent/interface/IO.h"
#include "RecoParticleFlow/PFRootEvent/interface/Utils.h" 
#include "DataFormats/ParticleFlowReco/interface/PFSimParticle.h"
#include "RecoJets/JetAlgorithms/interface/CMSIterativeConeAlgorithm.h"
#include "RecoJets/JetAlgorithms/interface/ProtoJet.h"
#include "RecoJets/JetAlgorithms/interface/JetRecoTypes.h"
#include "DataFormats/JetReco/interface/CaloJet.h"
#include "DataFormats/JetReco/interface/GenJet.h"
#include "DataFormats/JetReco/interface/PFJet.h"
#include "DataFormats/JetReco/interface/BasicJet.h"
#include "RecoJets/JetAlgorithms/interface/JetMaker.h"
#include "RecoJets/JetAlgorithms/interface/JetAlgoHelper.h"
#include "DataFormats/Candidate/interface/CandidateFwd.h"
#include "RecoJets/JetAlgorithms/interface/JetRecoTypes.h"
#include "FWCore/Framework/interface/EDProducer.h"
#include "DataFormats/Common/interface/EDProductfwd.h"
#include "DataFormats/Candidate/interface/CandidateFwd.h"
#include "DataFormats/RecoCandidate/interface/RecoCandidate.h"
#include "RecoParticleFlow/PFRootEvent/interface/FWLiteJetProducer.h"

#include <TFile.h>
#include <TTree.h>
#include <TCanvas.h>
#include <TPad.h>
#include <TMarker.h>
#include <TH1F.h>
#include <TH2F.h>
#include <TCutG.h>
#include <TPolyLine.h>
#include <TColor.h>
#include "TGraph.h"
#include "TMath.h"
#include "TLine.h"
#include "TLatex.h"
#include "TVector3.h"

#include <iostream>
#include <vector>
using namespace reco;
using namespace JetReco;

//-----------------------------------------------------------
JetPFRootEventManager::JetPFRootEventManager(const char* file):
  PFRootEventManager(file),
  genParticleCand_(new edm::OwnVector<reco::Candidate, edm::ClonePolicy<reco::Candidate> >),
  caloTowersCand_(new edm::OwnVector<reco::Candidate, edm::ClonePolicy<reco::Candidate> >),
  particleFlowCand_(new edm::OwnVector<reco::Candidate, edm::ClonePolicy<reco::Candidate> >),
  reccalojets_(new vector<reco::CaloJet>),   
  recpfjets_(new vector<reco::PFJet>),
  test_(new edm::OwnVector<reco::Candidate, edm::ClonePolicy<reco::Candidate> >){               
        
  options_ = 0; 
  jetMaker_=0;
  readSpecificOptions(file); 
        
}
//-----------------------------------------------------------
void JetPFRootEventManager::reset() {   
  reccalojets_->clear();
  recpfjets_->clear(); 
  pfJets_->clear(); 
  caloTowersCand_->clear();
  particleFlowCand_->clear();   
}
//-----------------------------------------------------------
JetPFRootEventManager::~JetPFRootEventManager() {       
        
}


//-----------------------------------------------------------
void JetPFRootEventManager::print() {   
  if (jetMaker_)  jetMaker_->print();
  cout <<"Opt: jetsDebugCMSSW " << jetsDebugCMSSW_  <<endl; 
  cout <<"Opt: algoType " << algoType_  <<endl; 
} 

//-----------------------------------------------------------
bool JetPFRootEventManager::processEntry(int entry) {
  cout<<"JetPFRootEventManager :processEntry" <<endl;  
  PFRootEventManager::processEntry(entry);
   cout<<"JKetPFRootEventManager: Print Particle Flow Candidates? "<<endl;
   cout<<pfAlgo_<<endl;
    for(unsigned i=0; i<pfCandidates_->size(); i++) {
	//(*particleFlowCand_)[i] = (*pfCandidates_)[i];//   does not work!
      cout<<(*pfCandidates_)[i]<<endl;
    }    
    cout<<endl;
	/// build particleFlowCandidates 

  // Copy PFCandidates into edm::OwnVector<Candidate> format 
  // as input for jet algorithms
  for( std::vector<PFCandidate>::const_iterator it = 
	(*pfCandidates_).begin (); it != (*pfCandidates_).end(); ++it) {
    particleFlowCand_->push_back( it->clone() );
	}
	makePFJets();
	// print new PFJets
    for ( unsigned i = 0; i < pfJets_->size(); i++) {
	const reco::PFJet& pfj = (*pfJets_)[i];
    if ( jetsDebugCMSSW_) {
      cout  << "FWLite PFlow jet " << i
	        << " ET   " <<pfj.et() 
	        << " eta  " << pfj.eta() 
	        << " phi  " << pfj.phi()<< endl;
	cout << pfj.print (); // PFJet print default
    }//debug
	} //pfJet loop		


                
  if(! readFromSimulation(entry) ) return false;
  {
    /* if(reccalojets_.get() ) {
       cout<<"Calo Jets : "<<reccalojets_->size()<<endl;
       } 
       if(recpfjets_.get() ) {
       cout<<"PF Jets : "<<recpfjets_->size()<<endl;
       } 
    */                  
	readCMSSWJets();
    return false;
  }
}

//-----------------------------------------------------------
void JetPFRootEventManager::readSpecificOptions(const char* file, 
						bool refresh) {
  cout<<"JetPFRootEventManager:reading options "<<endl;
  try {
    if( !options_ )
      options_ = new IO(file);
    else if( refresh) {
      delete options_;
      options_ = new IO(file);
    }
  }
  catch( const string& err ) {
    cout<<err<<endl;
  }
  // true particles already there from PFRootEventManager     
  
  //string trueParticlesbranchname;
  //options_->GetOpt("root","trueParticles_branch", trueParticlesbranchname);
  //trueParticlesBranch_ = tree_->GetBranch(trueParticlesbranchname.c_str());
  //if(!trueParticlesBranch_) {
  //  cerr<<"JetPFRootEventManager::ReadOptions : trueParticles_branch not found : "
   //     <<trueParticlesbranchname<< endl;
 // }
  //else {
 //   trueParticlesBranch_->SetAddress(&trueParticles_);   
 // } 

  // GenParticlesCand   
  string genParticleCandBranchName;
  genParticleCandBranch_ = 0;
  options_->GetOpt("root","genParticleCand_branch", genParticleCandBranchName);
  if(!genParticleCandBranchName.empty() ){  
    genParticleCandBranch_= tree_->GetBranch(genParticleCandBranchName.c_str()); 
    if(!genParticleCandBranch_) {
      cerr<<"JetPFRootEventanager::ReadOptions : genParticleCand_branch not found : "
          <<genParticleCandBranchName<< endl;
    }
    else {
      genParticleCandBranch_->SetAddress(genParticleCand_.get());
    }      
  }
        
  // CalotowersCand
  string caloTowersCandBranchName;
  recCaloTowersCandBranch_ = 0;
  options_->GetOpt("root","caloTowersCand_branch", caloTowersCandBranchName);
  if(!caloTowersCandBranchName.empty() ){  
    recCaloTowersCandBranch_ = tree_->GetBranch(caloTowersCandBranchName.c_str()); 
    if(!recCaloTowersCandBranch_) {
      cerr<<"JetPFRootEventanager::ReadOptions : caloTowersCand_branch not found : "
          <<caloTowersCandBranchName<< endl;
    }
    else {      
      recCaloTowersCandBranch_->SetAddress(caloTowersCand_.get());
    }           
  }
        
  // PF Cand
  string ParticleFlowCandBranchName;
  recParticleFlowCandBranch_ = 0;
  options_->GetOpt("root","ParticleFlowCandbranch", ParticleFlowCandBranchName);
  if(!ParticleFlowCandBranchName.empty() ){  
    recParticleFlowCandBranch_ = tree_->GetBranch(ParticleFlowCandBranchName.c_str()); 
    if(!recParticleFlowCandBranch_) {
      cerr<<"JetPFRootEventanager::ReadOptions : ParticleFlowCandbranch not found : "
          <<ParticleFlowCandBranchName<< endl;
    }
    else {
      recParticleFlowCandBranch_->SetAddress(particleFlowCand_.get());
    }           
  }
  /// rec calo jets
  string recCaloJetsBranchname;  
  recCaloJetsBranch_=0;
  options_->GetOpt("root","calojet_branch",recCaloJetsBranchname );
  recCaloJetsBranch_ = tree_->GetBranch(recCaloJetsBranchname.c_str());
  if(! recCaloJetsBranch_) {
    cerr<<"JetPFRootEventManager::ReadOptions : calojet_IC5_branch  not found : "
        <<recCaloJetsBranchname<< endl;
  }
  else {
    recCaloJetsBranch_->SetAddress(reccalojets_.get());
  }
        
  /// rec PFlow jets
  recPFJetsBranch_=0;   
  string recPFJetsBranchname;   
  options_->GetOpt("root","pfjet_branch",recPFJetsBranchname);
  recPFJetsBranch_ = tree_->GetBranch(recPFJetsBranchname.c_str());
  if(! recPFJetsBranch_) {
    cerr<<"JetPFRootEventManager::ReadOptions : pfjet_IC5_branch not found : "
        <<recPFJetsBranchname<< endl;
  }
  else {
    recPFJetsBranch_->SetAddress(recpfjets_.get());
  }
        
  /// jets options ---------------------------------;
  jetsDebugCMSSW_ = false;
  options_->GetOpt("CMSSWjets", "jetsDebugCMSSW", jetsDebugCMSSW_);
  algoType_=3; //FastJet as Default
  options_->GetOpt("CMSSWjets", "Algo", algoType_);
  mEtInputCut_ = 0.5;
  options_->GetOpt("CMSSWjets", "EtInputCut",  mEtInputCut_);           
  mEInputCut_ = 0.;
  options_->GetOpt("CMSSWjets", "EInputCut",  mEInputCut_);  
  seedThreshold_  = 1.0;
  options_->GetOpt("CMSSWjets", "seedThreshold", seedThreshold_);
  coneRadius_ = 0.5;
  options_->GetOpt("CMSSWjets", "coneRadius", coneRadius_);             
  coneAreaFraction_= 1.0;
  options_->GetOpt("CMSSWjets", "coneAreaFraction",  coneAreaFraction_);        
  maxPairSize_=2;
  options_->GetOpt("CMSSWjets", "maxPairSize",  maxPairSize_);  
  maxIterations_=100;
  options_->GetOpt("CMSSWjets", "maxIterations",  maxIterations_);      
  overlapThreshold_  = 0.75;
  options_->GetOpt("CMSSWjets", "overlapThreshold", overlapThreshold_);
  ptMin_ = 10.;
  options_->GetOpt("CMSSWjets", "ptMin",  ptMin_);      
  rparam_ = 1.0;
  options_->GetOpt("CMSSWjets", "rParam",  rparam_);    
 

  if(! jetMaker_) jetMaker_ = new FWLiteJetProducer();
  jetMaker_->setmEtInputCut (mEtInputCut_);
  jetMaker_->setmEInputCut(mEInputCut_); 
  jetMaker_->setSeedThreshold(seedThreshold_); 
  jetMaker_->setConeRadius(coneRadius_);
  jetMaker_->setConeAreaFraction(coneAreaFraction_);
  jetMaker_->setMaxPairSize(maxPairSize_);
  jetMaker_->setMaxIterations(maxIterations_) ;
  jetMaker_->setOverlapThreshold(overlapThreshold_) ;
  jetMaker_->setPtMin (ptMin_);
  jetMaker_->setRParam (rparam_);
  jetMaker_->updateParameter();

  if (jetsDebugCMSSW_) {      
    print();
  }     
}


//-----------------------------------------------------------
bool  JetPFRootEventManager::readFromSimulation(int entry) {
        
  cout<<"CALL JetPFRootEventManager::readFromSimulation"<<endl;
  
  reset();
  if(!tree_) return false; 
  // true particles already there from PFRootEventManager
 // if(trueParticlesBranch_ ) {
 //   trueParticlesBranch_->GetEntry(entry);
 // }
  if(recCaloJetsBranch_) {
    recCaloJetsBranch_->GetEntry(entry);
  }
  if( recPFJetsBranch_ ) {
    recPFJetsBranch_->GetEntry(entry);
  }
  if( recCaloTowersCandBranch_) {
    recCaloTowersCandBranch_->GetEntry(entry);
    if (jetsDebugCMSSW_)cout<<"Got candidate number caloTowersCand_->size()" << caloTowersCand_->size() << endl;                        
  }
  if( recParticleFlowCandBranch_) {
    recParticleFlowCandBranch_->GetEntry(entry);
    if (jetsDebugCMSSW_)cout<<"Got candidate number recPFCandBranch_->size()" << particleFlowCand_->size() << endl;                     
  }
    if( genParticleCandBranch_) {
    genParticleCandBranch_->GetEntry(entry);
    if (jetsDebugCMSSW_)cout<<"Got candidate number genCandBranch_->size()" << genParticleCand_->size() << endl;                     
  }
  
  
  return true;  
}

//-----------------------------------------------------------

//========================================================================
//Run CMSSW Algos in FWLITE ================================
//========================================================================
//-----------------------------------------------------------
void JetPFRootEventManager::makeFWLiteJets(const reco::CandidateCollection& Candidates) {
  // cout<<"!!! Make FWLite Jets  "<<endl;  
  JetReco::InputCollection input;
  vector <ProtoJet> output;
  jetMaker_->applyCuts (Candidates, &input);     
  if (algoType_==1) {// ICone 
    /// Produce jet collection using CMS Iterative Cone Algorithm       
    jetMaker_->makeIterativeConeJets(input, &output);
  }
  if (algoType_==2) {// MCone
    jetMaker_->makeMidpointJets(input, &output);
  }     
  if (algoType_==3) {// Fastjet
    jetMaker_->makeFastJets(input, &output);  
  }
  if((algoType_>3)||(algoType_<0)) {
    cout<<"Unknown Jet Algo ! " <<algoType_ << endl;
  }
  if (jetsDebugCMSSW_)cout<<"Proto Jet Size " <<output.size()<<endl;
  // try reset?
  pfJets_.reset(new PFJetCollection);
  vector <ProtoJet>::const_iterator protojet = output.begin ();
  JetMaker mjet;
  if (jetsDebugCMSSW_){ 
    for  (; protojet != output.end (); protojet++) {
      cout<<"Protojet ET " <<protojet->et()<<endl;
	  // first attemppt to build PFJets from protojets
      pfJets_->push_back(mjet.makePFJet(*protojet));  
//	  cout << protojet->print(); print method does not exist for protojets
//  	  cout << pfJets_.print(); //print method does exist forPFjets
    }
  } 
}
//-----------------------------------------------------------
void JetPFRootEventManager::makeGenJets(){
  makeFWLiteJets(*genParticleCand_);
} 

//-----------------------------------------------------------
void JetPFRootEventManager::makeCaloJets(){
  makeFWLiteJets(*caloTowersCand_);       
}

//-----------------------------------------------------------
void JetPFRootEventManager::makePFJets(){       
  makeFWLiteJets(*particleFlowCand_);
}

//-----------------------------------------------------------
void JetPFRootEventManager::readCMSSWJets(){
  TLorentzVector partTOTMC;
  partTOTMC.SetPxPyPzE(0.0, 0.0, 0.0, 0.0);     
  //MAKING JETS WITH TAU DAUGHTERS
  vector<reco::PFSimParticle> vectPART;
  for ( unsigned i=0;  i < trueParticles_.size(); i++) {
    const reco::PFSimParticle& ptc = trueParticles_[i];
    vectPART.push_back(ptc);
  }//loop       
  for ( unsigned i=0;  i < trueParticles_.size(); i++) {
    const reco::PFSimParticle& ptc = trueParticles_[i];
    const std::vector<int>& ptcdaughters = ptc.daughterIds();
                
    if (abs(ptc.pdgCode()) == 15) {
      for ( unsigned int dapt=0; dapt < ptcdaughters.size(); ++dapt) {                          
        const reco::PFTrajectoryPoint& tpatvtx 
          = vectPART[ptcdaughters[dapt]].trajectoryPoint(0);
        TLorentzVector partMC;
        partMC.SetPxPyPzE(tpatvtx.momentum().Px(),
                          tpatvtx.momentum().Py(),
                          tpatvtx.momentum().Pz(),
                          tpatvtx.momentum().E());
                                
        partTOTMC += partMC;
        if (jetsDebugCMSSW_) {
          //pdgcode
          int pdgcode = vectPART[ptcdaughters[dapt]].pdgCode();
          cout << pdgcode << endl;
          cout << tpatvtx << endl;
          cout << partMC.Px() << " " << partMC.Py() << " " 
               << partMC.Pz() << " " << partMC.E()
               << " PT=" 
               << sqrt(partMC.Px()*partMC.Px()+partMC.Py()*partMC.Py()) 
               << endl;
        }//debug
      }//loop daughter
    }//tau?
  }//loop particles     
  //    if (jetsDebugCMSSW_) {
                
  //////////////////////////////////////////////////////////////////////////
  //CALO TOWER JETS (ECAL+HCAL Towers)
  if ( jetsDebugCMSSW_) {
    if(reccalojets_.get() ) {
      cout<<"CMSSW Calo jets : "<<reccalojets_->size()<<endl;
    } 
  }
  double JetEHTETmax = 0.0;     
  for ( unsigned i = 0; i < reccalojets_->size(); i++) {
    //   TLorentzVector jetmom = (*reccalojets_)[i]. momentum();
    //double jetcalo_pt = (*reccalojets_)[i].pt();
    double jetcalo_et = (*reccalojets_)[i].et();
    if ( jetsDebugCMSSW_) {
      cout  << "CMSSW Calo jet " << (*reccalojets_)[i].px() << " " 
            << " " << (*reccalojets_)[i].pz() 
            << " ET=" << jetcalo_et << endl;
    }//debug
                        
    if (jetcalo_et >= JetEHTETmax) 
      JetEHTETmax = jetcalo_et;
  }//loop MCjets
                
  //   //////////////////////////////////////////////////////////////////
  //   //PARTICLE FLOW JETS
                
  double JetPFETmax = 0.0;
  if ( jetsDebugCMSSW_) {
    if(recpfjets_.get() ) {
      cout<<"CMSSW PFlow Jets : "<<recpfjets_->size()<< endl;      
    }
  } 
  for ( unsigned i = 0; i < recpfjets_->size(); i++) {
    //   TLorentzVector jetmom = (*recpfjets_)[i]. momentum();
    //	double jetpf_pt = (*recpfjets_)[i].pt();
	const reco::PFJet& pfj = (*recpfjets_)[i];
    double jetpf_et = pfj.et();
    if ( jetsDebugCMSSW_) {
      cout  << "CMSSW PFlow jet " << i
	        << " ET   " <<pfj.et() 
	        << " eta  " << pfj.eta() 
	        << " phi  " << pfj.phi()<< endl;
	cout << pfj.print (); // PFJet print default
    }//debug		
    if (jetpf_et >= JetPFETmax) 
      JetPFETmax = jetpf_et;
  }//loop MCjets

}


//-----------------------------------------------------------
void JetPFRootEventManager::write() {

        
}

