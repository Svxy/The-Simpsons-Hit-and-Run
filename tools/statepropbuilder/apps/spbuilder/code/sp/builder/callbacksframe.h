//---------------------------------------------------------------------------


#ifndef callbacksframeH
#define callbacksframeH
//---------------------------------------------------------------------------
#include <Classes.hpp>
#include <Controls.hpp>
#include <StdCtrls.hpp>
#include <Forms.hpp>
#include <ExtCtrls.hpp>
//---------------------------------------------------------------------------
class TFrame4 : public TFrame
{
__published:	// IDE-managed Components
    TPanel *Panel1;
    TLabel *Label2;
    TComboBox *CallbackComboBox;
    TLabel *Label1;
    TEdit *OnFrameEditBox;
    TButton *Button1;
    void __fastcall CallbackComboBoxChange(TObject *Sender);
    void __fastcall OnFrameEditBoxChange(TObject *Sender);
    void __fastcall Button1Click(TObject *Sender);
    void __fastcall OnFrameEditBoxExit(TObject *Sender);
private:	// User declarations
    int m_CBIndex;
public:		// User declarations
    __fastcall TFrame4(TComponent* Owner,int cbindex);
};
//---------------------------------------------------------------------------
extern PACKAGE TFrame4 *Frame4;
//---------------------------------------------------------------------------
#endif
