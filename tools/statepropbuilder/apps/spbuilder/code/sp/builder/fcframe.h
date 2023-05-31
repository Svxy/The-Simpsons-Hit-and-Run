//---------------------------------------------------------------------------


#ifndef fcframeH
#define fcframeH
//---------------------------------------------------------------------------
#include <Classes.hpp>
#include <Controls.hpp>
#include <StdCtrls.hpp>
#include <Forms.hpp>
#include <ExtCtrls.hpp>
//---------------------------------------------------------------------------
class TFrame1 : public TFrame
{
__published:	// IDE-managed Components
        TPanel *Panel1;
        TLabel *FrameControllerNameLabel;
        TLabel *Label1;
        TEdit *MinFrameEditBox;
        TLabel *Label3;
        TEdit *MaxFrameEditBox;
        TCheckBox *CyclicCheckBox;
        TLabel *Label4;
        TEdit *CurrentFrameLabel;
    TCheckBox *holdFrameCheckbox;
    TEdit *cycleForEditBox;
    TEdit *speedEditBox;
    TLabel *Label5;
        void __fastcall CyclicCheckBoxClick(TObject *Sender);
        void __fastcall MaxFrameEditBoxChange(TObject *Sender);
        void __fastcall MinFrameEditBoxChange(TObject *Sender);
    void __fastcall holdFrameCheckboxClick(TObject *Sender);
    void __fastcall speedEditBoxChange(TObject *Sender);
    void __fastcall cycleForEditBoxChange(TObject *Sender);
    void __fastcall EditBoxExit(TObject *Sender);
private:	// User declarations
        int m_FCIndex;
public:		// User declarations
        __fastcall TFrame1(TComponent* Owner , int fcindex);
};
//---------------------------------------------------------------------------
extern PACKAGE TFrame1 *Frame1;
//---------------------------------------------------------------------------
#endif
