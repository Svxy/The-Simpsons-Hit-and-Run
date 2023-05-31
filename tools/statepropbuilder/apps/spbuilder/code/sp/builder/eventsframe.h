//---------------------------------------------------------------------------


#ifndef eventsframeH
#define eventsframeH
//---------------------------------------------------------------------------
#include <Classes.hpp>
#include <Controls.hpp>
#include <StdCtrls.hpp>
#include <Forms.hpp>
#include <ExtCtrls.hpp>
//---------------------------------------------------------------------------
class TFrame3 : public TFrame
{
__published:	// IDE-managed Components
        TPanel *Panel1;
        TLabel *Label1;
        TComboBox *EventComboBox;
        TLabel *Label2;
        TEdit *ToFrameEditBox;
        TButton *Button1;
    void __fastcall EventComboBoxChange(TObject *Sender);
    void __fastcall ToFrameEditBoxChange(TObject *Sender);
    void __fastcall Button1Click(TObject *Sender);
    void __fastcall ToFrameEditBoxExit(TObject *Sender);
private:	// User declarations
    int m_EventIndex;
public:		// User declarations
        __fastcall TFrame3(TComponent* Owner , int EventIndex );
};
//---------------------------------------------------------------------------
extern PACKAGE TFrame3 *Frame3;
//---------------------------------------------------------------------------
#endif
