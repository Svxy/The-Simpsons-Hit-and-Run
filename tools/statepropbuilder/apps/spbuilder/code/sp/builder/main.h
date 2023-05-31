//---------------------------------------------------------------------------

#ifndef mainH
#define mainH
//---------------------------------------------------------------------------
#include <Classes.hpp>
#include <Controls.hpp>
#include <StdCtrls.hpp>
#include <Forms.hpp>
#include <AppEvnts.hpp>
#include <ExtCtrls.hpp>
#include <ComCtrls.hpp>
#include <Menus.hpp>
#include <ActnList.hpp>
#include <ImgList.hpp>
#include <Dialogs.hpp>
#include <CheckLst.hpp>
#include "fcframe.h"
#include "visframe.h"
#include "eventsframe.h"
#include "callbacksframe.h"

//---------------------------------------------------------------------------
class TMainForm : public TForm
{
__published:	// IDE-managed Components
        TApplicationEvents *ApplicationEvents1;
        TSplitter *Splitter2;
        TPanel *Panel2;
        TPanel *Panel1;
        TSplitter *Splitter1;
        TMainMenu *MainMenu1;
        TMenuItem *N1;
        TMenuItem *Open1;
        TMenuItem *Save1;
        TActionList *ActionList1;
        TImageList *ImageList1;
        TAction *FileOpen;
        TAction *FileSave;
        TAction *FileExit;
        TAction *HelpAbout;
        TMenuItem *Exit1;
        TMenuItem *Edit1;
        TMenuItem *Help1;
        TMenuItem *About1;
        TOpenDialog *OpenDialog1;
        TPanel *Panel3;
        TLabel *Label1;
        TLabel *PropNameLabel;
        TLabel *Label2;
        TLabel *NumPropStatesLabel;
        TLabel *Label3;
        TLabel *CurrentStateLabel;
        TButton *PlayButton;
        TButton *ForwardStepButton;
        TButton *BackStepButton;
        TEdit *CurrentFrameWindow;
        TButton *PrevStatebutton;
        TButton *NextStateButton;
        TLabel *Label4;
        TPageControl *PageControl1;
        TTabSheet *TabSheet1;
        TTabSheet *TabSheet2;
        TTabSheet *TabSheet3;
        TPanel *EventsPanel;
    TButton *Button1;
    TPanel *Panel7;
    TPanel *Panel8;
    TButton *Button2;
    TPanel *Panel9;
    TPanel *P3DClientPanel;
    TPanel *P3DPanel;
    TPanel *Panel10;
    TLabel *Label5;
    TLabel *PropNameLabelTop;
    TLabel *NumPropStatesLabelTop;
    TLabel *Label9;
    TLabel *CurrentStateLabelTop;
    TTabSheet *TabSheet4;
    TPanel *Panel11;
    TLabel *Label10;
    TLabel *StateStateLabel;
    TCheckBox *AutoTransitionCheckBox;
    TLabel *Label6;
    TEdit *ToStateEditBox;
    TLabel *Label8;
    TEdit *OnFrameEditBox;
    TButton *Button4;
    TButton *Button3;
    TLabel *Label7;
    TPanel *Panel12;
    TButton *Button6;
    TButton *Button7;
    TScrollBox *Panel5;
    TScrollBox *Panel6;
    TScrollBox *CallbackPanel;
    TScrollBox *Panel4;
    TMenuItem *B1;
    TColorDialog *ColorDialog1;
    TCheckBox *PDDICheckBox;
    TMenuItem *P3DBackgroundArt1;
        void __fastcall ApplicationEvents1Idle(TObject *Sender,
          bool &Done);
        void __fastcall FormShow(TObject *Sender);
        void __fastcall FormClose(TObject *Sender, TCloseAction &Action);
        void __fastcall P3DClientPanelResize(TObject *Sender);
        void __fastcall FileOpenExecute(TObject *Sender);
        void __fastcall P3DPanelMouseDown(TObject *Sender,
          TMouseButton Button, TShiftState Shift, int X, int Y);
        void __fastcall P3DPanelMouseMove(TObject *Sender,
          TShiftState Shift, int X, int Y);
        void __fastcall P3DPanelMouseUp(TObject *Sender,
          TMouseButton Button, TShiftState Shift, int X, int Y);
        void __fastcall FormKeyPress(TObject *Sender, char &Key);
        void __fastcall PlayButtonClick(TObject *Sender);
        void __fastcall PrevStatebuttonClick(TObject *Sender);
        void __fastcall NextStateButtonClick(TObject *Sender);
        void __fastcall ForwardStepButtonClick(TObject *Sender);
        void __fastcall BackStepButtonClick(TObject *Sender);
    void __fastcall Button1Click(TObject *Sender);
    void __fastcall Button2Click(TObject *Sender);
    void __fastcall Save1Click(TObject *Sender);
    void __fastcall FileSaveExecute(TObject *Sender);
    void __fastcall AutoTransitionCheckBoxClick(TObject *Sender);
    void __fastcall ToStateEditBoxChange(TObject *Sender);
    void __fastcall OnFrameEditBoxChange(TObject *Sender);
    void __fastcall Button4Click(TObject *Sender);
    void __fastcall Button3Click(TObject *Sender);
    void __fastcall Button6Click(TObject *Sender);
    void __fastcall Button7Click(TObject *Sender);
    void __fastcall B1Click(TObject *Sender);
    void __fastcall PDDICheckBoxClick(TObject *Sender);
    void __fastcall ToStateEditBoxExit(TObject *Sender);
    void __fastcall OnFrameEditBoxExit(TObject *Sender);
    void __fastcall P3DBackgroundArt1Click(TObject *Sender);
private:	// User declarations
        int m_CurrentPropState;
        int m_numFCPanels;
        TFrame1* m_FCPanels[250];
        int m_numDrawablePanels;
        TFrame2* m_DrawablePanels[250];
        int m_numEvents;
        TFrame3* m_EventPanels[250];
        int m_numCallbacks;
        TFrame4* m_CallbackPanels[250];
        int m_numForces;

        void UpdateStateInfo();
        void UpdateFrameControllers();
        void UpdateDrawables();
        void UpdateEvents();
        void UpdateCallbacks();
         void SaveData();

public:		// User declarations
        __fastcall TMainForm(TComponent* Owner);

        AnsiString FileName;
};
//---------------------------------------------------------------------------
extern PACKAGE TMainForm *MainForm;
//---------------------------------------------------------------------------
#endif
