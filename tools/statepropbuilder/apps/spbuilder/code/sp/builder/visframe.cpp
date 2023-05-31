//---------------------------------------------------------------------------

#include <vcl.h>
#include <sp/interface.hpp>
#pragma hdrstop

#include "visframe.h"
//---------------------------------------------------------------------------
#pragma package(smart_init)
#pragma resource "*.dfm"
TFrame2 *Frame2;
//---------------------------------------------------------------------------
__fastcall TFrame2::TFrame2(TComponent* Owner , int dindex )
        : TFrame(Owner),
          m_DrawableIndex( dindex )
{
}
//---------------------------------------------------------------------------
void __fastcall TFrame2::VisibilityCheckBoxClick(TObject *Sender)
{
    SPSetVisible( SPGetCurrentState() , m_DrawableIndex , this->VisibilityCheckBox->State != cbUnchecked );
}
//---------------------------------------------------------------------------

