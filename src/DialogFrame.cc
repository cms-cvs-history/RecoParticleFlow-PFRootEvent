#include <iostream>
#include <fstream>
#include <stdlib.h>

#include "RecoParticleFlow/PFBlockAlgo/interface/PFGeometry.h"
#include "RecoParticleFlow/PFRootEvent/interface/DialogFrame.h"
#include "RecoParticleFlow/PFRootEvent/interface/DisplayCommon.h"
#include <TTree.h>
#include "TLine.h"
#include "TList.h"
#include "TCanvas.h"
#include "TButton.h"
#include "TGraph.h"
#include "TMarker.h"
#include "TText.h"
#include <TApplication.h>
//#include "TVirtualPadEditor.h"




DialogFrame::DialogFrame(PFRootEventManager *evman,DisplayManager *dm,const TGWindow *p,UInt_t w,UInt_t h)
  :TGMainFrame(p, w, h),evMan_(evman),display_(dm),attrView_(0),
   thisClusPattern_(0),trackPattern_(0),simplePartPattern_(0)
{
  
  mainFrame_= new TGCompositeFrame(this,200,300,kVerticalFrame);
  //char *action="win->modifyAttr()";
  //accept_ = new TButton("accept",action,0.2,0.2,0.8,0.3);
  //maxEvents_=evMan_->tree_->GetEntries();
  createCmdFrame();
  AddFrame(mainFrame_, new TGLayoutHints(kLHintsLeft | kLHintsExpandY));
  
  // Set a name to the main frame
  SetWindowName("PFRootEvent Dialog");
  // Map all subwindows of main frame
  MapSubwindows();
  // Initialize the layout algorithm
  Resize(mainFrame_->GetDefaultSize());
  // Map main frame
  MapWindow();
    
}
//__________________________________________________________________________________________________
void DialogFrame::closeCanvasAttr()
{
 if (gROOT->GetListOfCanvases()->FindObject("graphic Attributes")) 
    attrView_->Close();
 attrView_=0;
 thisClusPattern_=0;
 trackPattern_=0;
 simplePartPattern_=0;
}
//__________________________________________________________________________________________________
void DialogFrame::createCanvasAttr()
{
  //creates an attributes canvas which enable modifications
  //of cluster and track graphic attributes 
  
 closeCanvasAttr(); 
 attrView_=0;
 attrView_ = new TCanvas("graphic Attributes","graphic Attributes",180,300);
 char *action1="win->DialogFrame::modifyAttr()";
 char *action2="win->DialogFrame::closeCanvasAttr()";
 TButton *accept_ = new TButton("modify",action1,0.1,0.2,0.5,0.3);
 TButton *cancel_ = new TButton("cancel",action2,0.54,0.2,0.9,0.3);
 double x[3];
 double y[3];
 x[0]=0.1;x[1]=0.3;x[2]=0.5;
 y[0]=0.6;y[1]=0.6;y[2]=0.6;
 thisClusPattern_= new TMarker(0.3,0.8,display_->clusPattern_->GetMarkerStyle());
 thisClusPattern_->SetMarkerColor(display_->clusPattern_->GetMarkerColor());
 thisClusPattern_->SetMarkerSize(display_->clusPattern_->GetMarkerSize());
 thisClusPattern_->Draw();
 TText * tt=new TText(0.6,0.78,"clusters");
 tt->SetTextSize(.08);
 tt->Draw();
 trackPattern_= new TGraph(3,x,y);
 trackPattern_->SetLineColor(display_->trackPatternL_->GetLineColor());
 trackPattern_->SetMarkerColor(display_->trackPatternM_->GetMarkerColor());
 trackPattern_->SetMarkerStyle(display_->trackPatternM_->GetMarkerStyle());
 trackPattern_->SetMarkerSize(display_->trackPatternM_->GetMarkerSize());
 trackPattern_->Draw("pl");
 TText *tt2= new TText(0.6,0.58,"recTracks");
 tt2->SetTextSize(.08);
 tt2->Draw();
 
 //TButton *accept_ = new TButton("modify",action1,0.2,0.2,0.8,0.3);
 //TButton *cancel_ = new TButton("cancel",action2,0.84,0.2,0.99,0.3);
 accept_->Draw();
 cancel_->Draw();
 attrView_->Update();
}
//__________________________________________________________________________________________________
void DialogFrame::createCmdFrame() 
{
  TGCompositeFrame *h1Frame1 = new TGCompositeFrame(mainFrame_, 100, 100, kHorizontalFrame | kRaisedFrame);
  mainFrame_->AddFrame(h1Frame1,new TGLayoutHints(kLHintsExpandX | kLHintsExpandY));
  TGLayoutHints *lo;
  lo = new TGLayoutHints(kLHintsLeft|kLHintsExpandX |kLHintsExpandY ,5,2,5,2);
  
  //create object selection buttons
  TGGroupFrame *gr1= new TGGroupFrame(h1Frame1,"Draw Selection",kVerticalFrame); 
  gr1->SetLayoutManager(new TGMatrixLayout(gr1,6,3,5));
  
  selectObject_[0] = new TGCheckButton(gr1,"Hits");
  selectObject_[0]->SetState(display_->drawHits_ ? kButtonDown :kButtonUp);
  selectObject_[0]->Connect("Clicked()","DialogFrame",this,"doModifyOptions(=0)");
  selectObject_[1] = new TGCheckButton(gr1,"Clusters");
  selectObject_[1]->SetState(display_->drawClus_ ? kButtonDown :kButtonUp);
  selectObject_[1]->Connect("Clicked()","DialogFrame",this,"doModifyOptions(=1)");
  selectObject_[2] = new TGCheckButton(gr1,"Tracks");
  selectObject_[2]->SetState(display_->drawTracks_ ? kButtonDown :kButtonUp);
  selectObject_[2]->Connect("Clicked()","DialogFrame",this,"doModifyOptions(=2)");
  selectObject_[3] = new TGCheckButton(gr1,"Particles");
  selectObject_[3]->SetState(display_->drawParticles_ ? kButtonDown :kButtonUp);
  selectObject_[3]->Connect("Clicked()","DialogFrame",this,"doModifyOptions(=3)");
  selectObject_[4] = new TGCheckButton(gr1,"PFBlock visible");
  selectObject_[4]->SetState(display_->drawPFBlocks_ ? kButtonDown :kButtonUp);
  selectObject_[4]->Connect("Clicked()","DialogFrame",this,"isPFBlockVisible()");

  // create threshold fields
  TGNumberFormat::ELimit lim = TGNumberFormat::kNELLimitMinMax;  
  for (int i=0;i<4;++i){
    thresholdS_[i] = new TGHSlider(gr1,100,kSlider1,ENER+i);
    thresholdS_[i]->Associate(this);
    thresholdS_[i]->SetRange(0,10);
   
    threshEntry_[i] = new TGNumberEntryField(gr1,EN+i,0);
    threshEntry_[i]->Associate(this);
    threshEntry_[i]->SetLimits(lim,0,10);
    threshEntry_[i]->SetFormat((TGNumberFormat::EStyle)0);
  }
  thresholdS_[0]->SetPosition((long) display_->hitEnMin_);
  thresholdS_[1]->SetPosition((long) display_->clusEnMin_);
  thresholdS_[2]->SetPosition((long) display_->trackPtMin_);
  thresholdS_[3]->SetPosition((long) display_->particlePtMin_);
  
  int charw= threshEntry_[0]->GetCharWidth("O");
  int size=charw*4;
  for (int i=0;i<4;++i) {
    threshEntry_[i]->SetIntNumber(thresholdS_[i]->GetPosition());
    threshEntry_[i]->Resize(size,threshEntry_[i]->GetDefaultHeight());
  }
  
  //
  TGLayoutHints *lo1=new TGLayoutHints(kLHintsTop | kLHintsLeft, 2, 2, 2, 2);
  TGLabel *label=new TGLabel(gr1,"  ");
  gr1->AddFrame(label,lo1);
  label=new TGLabel(gr1," En/Pt  Threshold");
  gr1->AddFrame(label,lo1);
  label=new TGLabel(gr1," (Gev) ");  
  gr1->AddFrame(label,lo1);
  
  for (int i=0;i<4;++i) {
    gr1->AddFrame(selectObject_[i],lo1);
    gr1->AddFrame(thresholdS_[i],lo1);
    gr1->AddFrame(threshEntry_[i],lo1);
  }
  gr1->AddFrame(selectObject_[4],lo1);   // no thresh for clusterLines
  h1Frame1->AddFrame(gr1,lo1);
  
  //add options frame
  TGVerticalFrame *optionFrame = new TGVerticalFrame(h1Frame1,10,10,kSunkenFrame);
  
  //print stuff
  TGHorizontalFrame *h2 = new TGHorizontalFrame(optionFrame,10,10);
  TGGroupFrame *printGroup = new TGGroupFrame(h2, "Print", kVerticalFrame);
  printGroup->SetLayoutManager(new TGMatrixLayout(printGroup, 4,1,4));
  printButton_[0] = new TGCheckButton(printGroup,"clusters");
  printButton_[0]->SetState(evMan_->printClusters_ ? kButtonDown :kButtonUp); 
  printButton_[0]->Connect("Clicked()","DialogFrame",this,"selectPrintOption(=0)");
  printButton_[1] = new TGCheckButton(printGroup,"recHits ");
  printButton_[1]->SetState(evMan_->printRecHits_ ? kButtonDown :kButtonUp); 
  printButton_[1]->Connect("Clicked()","DialogFrame",this,"selectPrintOption(=1)");
  printButton_[2] = new TGCheckButton(printGroup,"SimParticles ");
  printButton_[2]->SetState(evMan_->printSimParticles_ ? kButtonDown :kButtonUp); 
  printButton_[2]->Connect("Clicked()","DialogFrame",this,"selectPrintOption(=2)");
  printButton_[3] = new TGCheckButton(printGroup,"PFBlocks");
  printButton_[3]->SetState(evMan_->printPFBlocks_ ? kButtonDown :kButtonUp); 
  printButton_[3]->Connect("Clicked()","DialogFrame",this,"selectPrintOption(=3)");
  for(UInt_t i = 0 ;i<4 ; ++i){
    printGroup->AddFrame(printButton_[i],lo1);
  }
  
  TGTextButton *sendPrintButton = new TGTextButton(h2,"Print");
  sendPrintButton->Connect("Clicked()","DialogFrame",this,"doPrint()");
  
  h2->AddFrame(printGroup,lo1);
  h2->AddFrame(sendPrintButton,new TGLayoutHints(kLHintsLeft|kLHintsCenterY,2,2,2,2));
  
  //printMCTruth stuff
 // TGHorizontalFrame *h3 = new TGHorizontalFrame(optionFrame,10,10);
 // printTrueButton_= new TGCheckButton(h3,"True");
 // printTrueButton_->SetState(kButtonUp);
 // maxLineEntry_= new TGNumberEntryField(h3,MAXL,30);
 // maxLineEntry_->Associate(this);
//  maxLineEntry_->SetFormat((TGNumberFormat::EStyle)0);
 // maxLineEntry_->Resize(charw*3,maxLineEntry_->GetDefaultHeight());
  
 // TGTextButton *printMCTruthButton = new TGTextButton(h3,"PrintGenParticles");
 // printMCTruthButton->Connect("Clicked()","DialogFrame",this,"doPrintGenParticles()");
//----
  //printMCTruth stuff
  TGGroupFrame *printGroup2 = new TGGroupFrame(optionFrame,"GenParticles",kHorizontalFrame);
  printGroup2->SetLayoutManager(new TGMatrixLayout(printGroup2, 2,3,3));
  TGTextButton *printGenPartButton = new TGTextButton(printGroup2,"  Print  ");
  printGenPartButton->Connect("Clicked()","DialogFrame",this,"doPrintGenParticles()");
  TGLabel *maxl = new TGLabel(printGroup2,"max lines:");
  maxLineEntry_= new TGNumberEntryField(printGroup2,MAXL,30);
  maxLineEntry_->Associate(this);
  maxLineEntry_->SetFormat((TGNumberFormat::EStyle)0);
  maxLineEntry_->Resize(charw*3,maxLineEntry_->GetDefaultHeight());
  
  TGTextButton *lookFor = new TGTextButton(printGroup2,"Look for");
  lookFor->Connect("Clicked()","DialogFrame",this,"doLookForGenParticle()");
  TGLabel *genPartNb = new TGLabel(printGroup2,"Gen Particle Nb:");
  particleTypeEntry_ = new TGNumberEntryField(printGroup2,PARTTYPE,1);
  particleTypeEntry_->Associate(this);
  particleTypeEntry_->SetFormat((TGNumberFormat::EStyle)0);
  particleTypeEntry_->Resize(charw*3,particleTypeEntry_->GetDefaultHeight());
  //TGTextButton *printEventButton = new TGTextButton(printGroup2,"DisplayGenParticle");
  //printEventButton->Connect("Clicked()","DialogFrame",this,"doDisplayGenParticle()");
  
  printGroup2->AddFrame(printGenPartButton,lo1);
  printGroup2->AddFrame(maxl,lo1),
  printGroup2->AddFrame(maxLineEntry_,lo1);
  printGroup2->AddFrame(lookFor,lo1);
  printGroup2->AddFrame(genPartNb,lo1),
  printGroup2->AddFrame(particleTypeEntry_,lo1);
  //printGroup2->AddFrame(printEventButton,lo1);
  
//
  optionFrame->AddFrame(h2,lo);
  optionFrame->AddFrame(printGroup2,lo1);
 //h3->AddFrame(printTrueButton_,lo1);
  //h3->AddFrame(maxLineEntry_,lo1);
  // h3->AddFrame(printMCTruthButton,lo1);
  // optionFrame->AddFrame(h3,lo);
  h1Frame1->AddFrame(optionFrame,lo);

  
  // Next/Pevious/exit buttons
  
  TGHorizontalFrame *h1 = new TGHorizontalFrame(mainFrame_,20,30);
  mainFrame_->AddFrame(h1,new TGLayoutHints(kLHintsCenterX,2,2,2,2));
  
  nextButton = new TGTextButton(h1,"Draw Next");
  nextButton->Connect("Clicked()","DialogFrame",this,"doNextEvent()");
  h1->AddFrame(nextButton,new TGLayoutHints(kLHintsBottom|kLHintsCenterX,2,2,2,2));
  
  previousButton = new TGTextButton(h1,"Draw Previous");
  previousButton->Connect("Clicked()","DialogFrame",this,"doPreviousEvent()");
  h1->AddFrame(previousButton,new TGLayoutHints(kLHintsBottom|kLHintsCenterX,2,2,2,2));
  
  TGTextButton *unZoomButton = new TGTextButton(h1,"Unzoom");
  unZoomButton->Connect("Clicked()","DialogFrame",this,"unZoom()");
  h1->AddFrame(unZoomButton,new TGLayoutHints(kLHintsBottom|kLHintsCenterX,2,2,2,2));
  
  reProcessButton = new TGTextButton(h1,"Re-Process");
  reProcessButton->Connect("Clicked()","DialogFrame",this,"doReProcessEvent()");
  h1->AddFrame(reProcessButton,new TGLayoutHints(kLHintsBottom|kLHintsCenterX,2,2,2,2));
  
//Modifie Graphic attributes in option file
//  TGTextButton *newAttr = new TGTextButton(h1,"new GAttr");
//  newAttr->Connect("Clicked()","DialogFrame",this,"modifyGraphicAttributes()");
//  h1->AddFrame(newAttr,new TGLayoutHints(kLHintsBottom|kLHintsCenterX,2,2,2,2));

// Modify graphic attributes online without modify the option file  
  TGTextButton *newAttrBis = new TGTextButton(h1,"Modify Graphic Attr");
  newAttrBis->Connect("Clicked()","DialogFrame",this,"createCanvasAttr()");
  h1->AddFrame(newAttrBis,new TGLayoutHints(kLHintsBottom|kLHintsCenterX,2,2,2,2));
    
  exitButton = new TGTextButton(h1,"&Exit","gApplication->Terminate(0)");
  h1->AddFrame(exitButton,new TGLayoutHints(kLHintsBottom|kLHintsCenterX,2,2,2,2));
}  
  
//________________________________________________________________________________
void DialogFrame::CloseWindow()
{
  //!!!WARNING keep the first letter of the method uppercase.It is an overriden ROOT method  
  gApplication->Terminate(0);
}
//_________________________________________________________________________________
void DialogFrame::doDisplayGenParticle()
{
  std::cout<<"DisplayGenParticle() not yet implemented"<<std::endl;
}
//_________________________________________________________________________________
void DialogFrame::doLookForGenParticle()
{
 int num = particleTypeEntry_->GetIntNumber();
 std::cout<<"display GPFGenParticle nb "<<num<<std::flush<<std::endl;
 display_->lookForGenParticle((unsigned)num);
}

//_________________________________________________________________________________
void DialogFrame::doModifyOptions(unsigned objNb)
{
  switch (objNb) {
  case 0:
    display_->drawHits_ = (selectObject_[0]->IsDown()) ?true :false;
    break;
  case 1:
    display_->drawClus_ = (selectObject_[1]->IsDown()) ?true :false;
    break; 
  case 2:
    display_->drawTracks_ = (selectObject_[2]->IsDown()) ?true :false;
    break;
  case 3: 
    display_->drawParticles_ = (selectObject_[3]->IsDown()) ?true :false;
    break;
  case 4:
    display_->drawClusterL_ = (selectObject_[4]->IsDown()) ?true :false;
    break;
  }
  display_->displayAll();    
}
//_______________________________________________________________________________
DialogFrame::~DialogFrame()
{
  mainFrame_->Cleanup();
}
//________________________________________________________________________________
void DialogFrame::doModifyPtThreshold(unsigned objNb,long pt)
{
  switch(objNb) {
  case 0: 
    display_->hitEnMin_=(double)pt;break;
    break;
  case 1:
    display_->clusEnMin_=(double)pt;break;
    break;
  case 2:
    display_->trackPtMin_=(double)pt;break;
    break;
  case 3:
    display_->particlePtMin_=(double)pt;break;
  default:break;
  }  
  display_->displayAll();
}
//_________________________________________________________________________________
void DialogFrame::doNextEvent()
{
  display_->displayNext();
  //   int eventNumber = evMan_->eventNumber();
  //TODOLIST:display new value of eventNumber in the futur reserve field
} 
//_________________________________________________________________________________
void DialogFrame::doPreviousEvent()
{
  display_->displayPrevious();
  //   int eventNumber = evMan_->eventNumber();
  //TODOLIST:display new value of eventNumber in the futur reserve field
}
//_________________________________________________________________________________
void DialogFrame::doPrint()
{
  std::cout<<"click on Print Button"<<std::endl;
  evMan_->print();
}
//_________________________________________________________________________________
void DialogFrame::doPrintGenParticles()
{
  std::cout<<"appel a printMCTruth"<<std::flush<<std::endl; 
  evMan_->printMCTruth(std::cout,maxLineEntry_->GetIntNumber());
}
//__________________________________________________________________________________
void DialogFrame::doReProcessEvent()
{
  // TODOLIST:evMan_->connect() + nouveau nom de fichier s'il y a lieu ??
  int eventNumber = evMan_->eventNumber();
  display_->display(eventNumber);
}
//_________________________________________________________________________________
void DialogFrame::isPFBlockVisible()
{
  display_->enableDrawPFBlock((selectObject_[4]->IsDown()) ?true :false);
}
//_________________________________________________________________________________
void DialogFrame::selectPrintOption(int opt)
{
  switch (opt) {
  case 0:
    evMan_->printClusters_ = (printButton_[0]->IsDown()) ?true :false;
    break;
  case 1:
    evMan_->printRecHits_ = (printButton_[1]->IsDown()) ?true :false;
    break;
  case 2:
    evMan_->printSimParticles_ = (printButton_[2]->IsDown()) ?true :false;
    break;
  case 3:
    evMan_->printPFBlocks_ = (printButton_[3]->IsDown()) ?true :false;
    break;
  default: break;  
    
  }
} 
//________________________________________________________________________________
void DialogFrame::updateDisplay()
{
  display_->updateDisplay();
}

//________________________________________________________________________________
void DialogFrame::unZoom()
{
  display_->unZoom();
}
//________________________________________________________________________________
/*void DialogFrame::modifyGraphicAttributes()
{
 // readOption avec nom du fichier apres valeurs changees a la main
 std::cout <<"do it yourself in the root input window"<<std::endl;
 std::cout <<"Edit your option file "<<std::endl;
 std::cout <<"modify the clusAttributes, trackAttributes or simpleTrackAttributes "<<std::endl;
 std::cout <<"type :dm->readOptions(opt.c_str();"<<std::endl;
} 
*/
//______________________________________________________________________________________
void DialogFrame::modifyAttr()
{ 
  display_->clusPattern_->SetMarkerStyle(thisClusPattern_->GetMarkerStyle());
  display_->clusPattern_->SetMarkerSize(thisClusPattern_->GetMarkerSize());
  display_->clusPattern_->SetMarkerColor(thisClusPattern_->GetMarkerColor());
  display_->trackPatternL_->SetLineColor(trackPattern_->GetLineColor());
  display_->trackPatternM_->SetMarkerStyle(trackPattern_->GetMarkerStyle());
  display_->trackPatternM_->SetMarkerSize(trackPattern_->GetMarkerSize());
  //trackPattern_->SetMarkerColor(display_->trackAttributes_[0]);
  closeCanvasAttr();
  display_->drawWithNewGraphicAttributes();
}
//_________________________________________________________________________________
Bool_t DialogFrame::ProcessMessage(Long_t msg, Long_t parm1, Long_t parm2)
{ 
  switch (GET_MSG(msg)) {
  case kC_TEXTENTRY:
    switch (GET_SUBMSG(msg)) {
    case kTE_ENTER:
      switch (parm1) {
      case EN :case EN+1: case EN+2: case EN+3:
        {
          //int eventNumber=evMan_->iEvent_;
          long val=threshEntry_[parm1-EN]->GetIntNumber();
          thresholdS_[parm1-EN]->SetPosition(val);
          doModifyPtThreshold(parm1-EN,val);
          break;
        }
      case MAXL:  // print genPart max lines
        doPrintGenParticles();
	break;
      case PARTTYPE:
        doLookForGenParticle();	
       	break;
      default:break;
      }
      break;
    default:break;
    }
    break;
  case kC_HSLIDER:
    switch (GET_SUBMSG(msg)) {
    case kSL_POS:
      switch (parm1) {
      case ENER: case ENER+1: case ENER+2: case ENER+3:
        {
          unsigned index=parm1-ENER;
          threshEntry_[index]->SetIntNumber(parm2);
          fClient->NeedRedraw(threshEntry_[index]);
          break;
        } 
      default:break;
      }
      break;  
    case kSL_RELEASE:
      switch (parm1) {
      case ENER: case ENER+1: case ENER+2: case ENER+3:
        {
          long val = thresholdS_[parm1-ENER]->GetPosition();
          doModifyPtThreshold(parm1-ENER,val);
          break;
        } 
      default:break;    
      }
      break;
    default:break;       
    }
    break;
  default:break;
  }
  return true;   
}             
