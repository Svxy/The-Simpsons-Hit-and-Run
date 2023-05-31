//---------------------------------------------------------------------------

#include <vcl.h>
#pragma hdrstop

#include <sp/interface.hpp>
#include "callbacksframe.h"
//---------------------------------------------------------------------------
#pragma package(smart_init)
#pragma resource "*.dfm"

extern bool g_ForceDataUpdate;
extern bool  g_IsDataSaved;

TFrame4 *Frame4;
//---------------------------------------------------------------------------
__fastcall TFrame4::TFrame4(TComponent* Owner,int cbindex) :
    TFrame(Owner),
    m_CBIndex( cbindex )
{
}
//---------------------------------------------------------------------------
void __fastcall TFrame4::CallbackComboBoxChange(TObject *Sender)
{
    CallbackData cbData;
    SPGetCallbackData( SPGetCurrentState() , m_CBIndex , &cbData );

    int index = CallbackComboBox->ItemIndex;
    AnsiString eventname = CallbackComboBox->Items->Strings[index];
    SPEditCallback( SPGetCurrentState() , m_CBIndex , eventname.c_str() , index , cbData.onFrame );
    g_IsDataSaved = false;
}
//---------------------------------------------------------------------------

void __fastcall TFrame4::OnFrameEditBoxChange(TObject *Sender)
{
    CallbackData cbData;
    SPGetCallbackData( SPGetCurrentState() , m_CBIndex , &cbData );

    AnsiString onFrame = OnFrameEditBox->Text;
    float frame = 0.f;
    try
    {
        frame = onFrame.ToDouble();
    }
    catch (Exception&)
    {
        frame = cbData.onFrame;
        AnsiString oldFrame;
        oldFrame.sprintf("%0.3f" , frame);
        OnFrameEditBox->Text = oldFrame;
    }

    SPEditCallback( SPGetCurrentState() , m_CBIndex , cbData.callbackName , cbData.callbackID , frame );
    g_IsDataSaved = false;
}
//---------------------------------------------------------------------------

void __fastcall TFrame4::Button1Click(TObject *Sender)
{
    SPDeleteCallback( SPGetCurrentState() , m_CBIndex  );
    g_ForceDataUpdate = true;
    g_IsDataSaved = false;
}
//---------------------------------------------------------------------------


void __fastcall TFrame4::OnFrameEditBoxExit(TObject *Sender)
{
    g_ForceDataUpdate = true;    
}
//---------------------------------------------------------------------------

