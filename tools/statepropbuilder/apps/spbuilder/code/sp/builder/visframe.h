//---------------------------------------------------------------------------


#ifndef visframeH
#define visframeH
//---------------------------------------------------------------------------
#include <Classes.hpp>
#include <Controls.hpp>
#include <StdCtrls.hpp>
#include <Forms.hpp>
#include <ExtCtrls.hpp>
//---------------------------------------------------------------------------
class TFrame2 : public TFrame
{
__published:	// IDE-managed Components
        TPanel *Panel1;
        TCheckBox *VisibilityCheckBox;
        TLabel *NameLabel;
        void __fastcall VisibilityCheckBoxClick(TObject *Sender);
private:	// User declarations
        int m_DrawableIndex;
public:		// User declarations
        __fastcall TFrame2(TComponent* Owner , int dindex );
};
//---------------------------------------------------------------------------
extern PACKAGE TFrame2 *Frame2;
//---------------------------------------------------------------------------
#endif
