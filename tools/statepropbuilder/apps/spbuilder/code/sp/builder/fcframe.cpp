//---------------------------------------------------------------------------

#include <vcl.h>
#include <sp/interface.hpp>
#pragma hdrstop

#include "fcframe.h"
//---------------------------------------------------------------------------
#pragma package(smart_init)
#pragma resource "*.dfm"

extern bool g_IsDataSaved;
extern bool g_ForceDataUpdate;

TFrame1 *Frame1;
//---------------------------------------------------------------------------
__fastcall TFrame1::TFrame1(TComponent* Owner , int fcindex)
        : TFrame(Owner) , m_FCIndex( fcindex )
{
}

//---------------------------------------------------------------------------


void __fastcall TFrame1::CyclicCheckBoxClick(TObject *Sender)
{
    SPSetCyclic( SPGetCurrentState() , m_FCIndex , this->CyclicCheckBox->State != cbUnchecked );
    this->cycleForEditBox->Enabled = CyclicCheckBox->State != cbUnchecked;
    g_IsDataSaved = false;
}
//---------------------------------------------------------------------------

void __fastcall TFrame1::MaxFrameEditBoxChange(TObject *Sender)
{
    FrameControllerData fcData;
    SPGetFrameControllerData( SPGetCurrentState() , m_FCIndex , &fcData );

    AnsiString frame;
    frame = this->MaxFrameEditBox->Text;
    float f = 0.0f;
    try
    {
        f = frame.ToDouble();
    }
    catch (Exception&)
    {
        f = fcData.maxFrame;
    }

    SPSetFrameRange( SPGetCurrentState() , m_FCIndex , fcData.minFrame ,  f );
    g_IsDataSaved = false;
}
//---------------------------------------------------------------------------

void __fastcall TFrame1::MinFrameEditBoxChange(TObject *Sender)
{
    FrameControllerData fcData;
    SPGetFrameControllerData( SPGetCurrentState() , m_FCIndex , &fcData );

    AnsiString frame;
    frame = this->MinFrameEditBox->Text;
    float f = 0.0f;

    try
    {
            f = frame.ToDouble();

    }
    catch (Exception&)
    {
        f = fcData.minFrame;
    }

    SPSetFrameRange( SPGetCurrentState() , m_FCIndex , f , fcData.maxFrame );
    g_IsDataSaved = false;
}
//---------------------------------------------------------------------------

void __fastcall TFrame1::holdFrameCheckboxClick(TObject *Sender)
{
    SPSetHoldFrame( SPGetCurrentState() , m_FCIndex , holdFrameCheckbox->Checked );
}
//---------------------------------------------------------------------------

void __fastcall TFrame1::speedEditBoxChange(TObject *Sender)
{
    FrameControllerData fcData;
    SPGetFrameControllerData( SPGetCurrentState() , m_FCIndex , &fcData );

    AnsiString speed;
    speed = this->speedEditBox->Text;
    float fSpeed = 0.0f;

    try
    {
        fSpeed = speed.ToDouble();
    }
    catch (Exception&)
    {
        fSpeed = fcData.relativeSpeed;
    }

    SPSetRelativeSpeed( SPGetCurrentState() , m_FCIndex , fSpeed );
    g_IsDataSaved = false;
}
//---------------------------------------------------------------------------

void __fastcall TFrame1::cycleForEditBoxChange(TObject *Sender)
{
    FrameControllerData fcData;
    SPGetFrameControllerData( SPGetCurrentState() , m_FCIndex , &fcData );

    AnsiString cycles;
    cycles = cycleForEditBox->Text;
    int iCycles = 0;
    try
    {
        iCycles = cycles.ToInt();
    }
    catch (Exception&)
    {
        iCycles = fcData.numberOfCycles;
    }

    SPSetNumberOfCycles( SPGetCurrentState() , m_FCIndex , iCycles );
    g_IsDataSaved = false;
}
//---------------------------------------------------------------------------

void __fastcall TFrame1::EditBoxExit(TObject *Sender)
{
    //do something here
    g_ForceDataUpdate = true;
}
//---------------------------------------------------------------------------

