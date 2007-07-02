#include <TGClient.h>
#include <TGButton.h>


class PFRootEventManager;

class MainFrame : public TGMainFrame {

private:
    TGTextButton    *fButton1, *fButton2;
    TGPictureButton *fPicBut;
    TGCheckButton   *fChkBut;
    TGRadioButton   *fRBut1, *fRBut2;
    TGLayoutHints   *fLayout;
    PFRootEventManager* *fEventManager;

public:
    MainFrame(const TGWindow *p, UInt_t w, UInt_t h);
    ~MainFrame() { } // need to delete here created widgets
};
