//#include "../../interface/PFRootEventManager.h"
#include "MainFrame.h"

MainFrame::MainFrame(const TGWindow *p, UInt_t w, UInt_t h)
  : TGMainFrame(p, w, h)
{
    // Create a main frame with a number of different buttons.

    fButton1 = new TGTextButton(this, "&Print PFRootEvent", 1);
    fButton1->SetCommand("em.print();");
    fButton2 = new TGTextButton(this, "&Exit", 2);
    fButton2->SetCommand(".q" );
    fChkBut = new TGCheckButton(this, "Check Button", 4);
    fRBut1 = new TGRadioButton(this, "Radio Button 1", 5);
    fRBut2 = new TGRadioButton(this, "Radio Button 2", 6);
    fLayout = new TGLayoutHints(kLHintsCenterX | kLHintsCenterY);

    AddFrame(fButton1, fLayout);
    AddFrame(fButton2, fLayout);
    AddFrame(fChkBut, fLayout);
    AddFrame(fRBut1, fLayout);
    AddFrame(fRBut2, fLayout);

    MapSubwindows();

    Layout();

    SetWindowName("Button Example");
    SetIconName("Button Example");

    MapWindow();
}



