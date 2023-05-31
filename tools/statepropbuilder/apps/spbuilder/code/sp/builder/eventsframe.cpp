//---------------------------------------------------------------------------

#include <vcl.h>
#pragma hdrstop
#include <sp/interface.hpp>
#include "eventsframe.h"
#include "main.h"

extern float g_ForceDataUpdate;
extern bool  g_IsDataSaved;

//---------------------------------------------------------------------------
#pragma package(smart_init)
#pragma resource "*.dfm"
TFrame3 *Frame3;
//---------------------------------------------------------------------------
__fastcall TFrame3::TFrame3(TComponent* Owner , int EventIndex ) :
    TFrame(Owner),
    m_EventIndex( EventIndex )
{
}
//---------------------------------------------------------------------------
void __fastcall TFrame3::EventComboBoxChange(TObject *Sender)
{
    EventData eventData;
    SPGetEventData( SPGetCurrentState() , m_EventIndex , &eventData );
    int index = EventComboBox->ItemIndex;
    AnsiString eventname = EventComboBox->Items->Strings[index];
    SPEditEvent( SPGetCurrentState() , m_EventIndex , eventname.c_str() , index , eventData.toState );
    g_IsDataSaved = false;
}
//---------------------------------------------------------------------------

void __fastcall TFrame3::ToFrameEditBoxChange(TObject *Sender)
{
    EventData eventData;
    SPGetEventData( SPGetCurrentState() , m_EventIndex , &eventData );

    int index = EventComboBox->ItemIndex;
    AnsiString eventname = EventComboBox->Items->Strings[index];
    AnsiString toState = ToFrameEditBox->Text;
    int state = 0;
    try
    {
        state = toState.ToInt();
    }
    catch (Exception&)
    {
        state = eventData.toState + 1;
    }
    if ( state > SPGetNumberOfStates()  || state == 0 )
    {
        state = eventData.toState + 1;
        g_ForceDataUpdate = true;
    }

    SPEditEvent( SPGetCurrentState() , m_EventIndex , eventname.c_str() , index , state - 1 );
    g_IsDataSaved = false;
}
//---------------------------------------------------------------------------

void __fastcall TFrame3::Button1Click(TObject *Sender)
{
    SPDeleteEvent( SPGetCurrentState() , m_EventIndex  );
    g_ForceDataUpdate = true;
    g_IsDataSaved = false;
}
//---------------------------------------------------------------------------

void __fastcall TFrame3::ToFrameEditBoxExit(TObject *Sender)
{
    g_ForceDataUpdate = true;
}
//---------------------------------------------------------------------------

