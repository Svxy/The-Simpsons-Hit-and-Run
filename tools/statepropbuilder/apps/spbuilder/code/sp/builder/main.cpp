//---------------------------------------------------------------------------

#include <vcl.h>
#include <sp/interface.hpp>
#include <direct.h>

#include <errno.h>
#include <dos.h>
#include <io.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#pragma hdrstop

#include "main.h"
#include "sp/builder/fcframe.h"
#include "sp/builder/eventsframe.h"

float g_ForceDataUpdate = false;
bool g_IsDataSaved = true;

/* returns the attributes of a DOS file */
int get_file_attrib(char *filename)
{
   return(_rtl_chmod(filename, 0));
}
//---------------------------------------------------------------------------

void FVUShiftStateToMouse(TShiftState shiftState, int& buttons, int& shift)
{
    buttons = 0;
    if (shiftState.Contains(ssLeft))
        buttons |= MK_LBUTTON;
    if (shiftState.Contains(ssRight))
        buttons |= MK_RBUTTON;
    if (shiftState.Contains(ssMiddle))
        buttons |= MK_MBUTTON;

    shift = 0;
    if (shiftState.Contains(ssShift))
        shift |= MK_SHIFT;
    if (shiftState.Contains(ssCtrl))
        shift |= MK_CONTROL;
}
//---------------------------------------------------------------------------
#pragma package(smart_init)
#pragma resource "*.dfm"
TMainForm *MainForm;
//---------------------------------------------------------------------------
__fastcall TMainForm::TMainForm(TComponent* Owner) :
    TForm(Owner),
    m_numFCPanels(0),
    m_numDrawablePanels(0),
    m_numEvents(0),
    m_numCallbacks(0),
    m_CurrentPropState(-1)
{
}
//---------------------------------------------------------------------------

void __fastcall TMainForm::ApplicationEvents1Idle(TObject *Sender,
      bool &Done)
{
    int i;
    SPContextAdvance();
    SPContextDisplay();

    //update frames
    float currentFrame = SPGetBaseFrameControllerFrame();
    AnsiString frame;
    frame.sprintf("%0.3f",currentFrame);
    CurrentFrameWindow->Text = frame;

    for ( i = 0; i < m_numFCPanels; i++ )
    {
            frame.sprintf("%0.3f",SPGetFrameControllerFrame( i ));
            m_FCPanels[i]->CurrentFrameLabel->Text = frame;
    }

    Save1->Enabled = ! g_IsDataSaved;

    //check for a new state
    int newState = SPGetCurrentState();
    if ( m_CurrentPropState != newState || g_ForceDataUpdate )
    {
        UpdateStateInfo();
        UpdateFrameControllers();
        UpdateDrawables();
        UpdateEvents();
        UpdateCallbacks();
        m_CurrentPropState = newState;
        g_ForceDataUpdate = false;
    }

    static bool bgloaded = false;
    if ( ! bgloaded )
    {
        //load the events list
        AnsiString fname = Application->ExeName;
        fname.SetLength( fname.Length() - 13 );
        fname.cat_sprintf("data\\init.txt");
        TComboBox* cbox = new TComboBox( this );
        cbox->Parent = Panel4;
        cbox->Items->LoadFromFile(fname);
        if ( cbox->Items->Count >= 1 )
        {
            AnsiString bgfilename = cbox->Items->Strings[0];
            SPLoadBackground( bgfilename.c_str() );
        }

        delete cbox;
        bgloaded = true;
    }

    Done = false;
}
//---------------------------------------------------------------------------

void __fastcall TMainForm::FormShow(TObject *Sender)
{
    SPPlatformOpen( Handle );

    HWND desktopHwnd = GetDesktopWindow();
    RECT rect;
    GetWindowRect(desktopHwnd, &rect);

    P3DPanel->Width = rect.right - rect.left;
    P3DPanel->Height = rect.bottom - rect.top;
    SPContextOpen(P3DPanel->Handle);
    SPContextViewResize(P3DClientPanel->ClientWidth, P3DClientPanel->ClientHeight);
}
//---------------------------------------------------------------------------

void __fastcall TMainForm::FormClose(TObject *Sender, TCloseAction &Action)
{
    SPContextClose();
    SPPlatformClose();
}
//---------------------------------------------------------------------------

void __fastcall TMainForm::P3DClientPanelResize(TObject *Sender)
{
    SPContextViewResize(P3DClientPanel->ClientWidth, P3DClientPanel->ClientHeight);
    SPContextDisplay();
}
//---------------------------------------------------------------------------

void __fastcall TMainForm::FileOpenExecute(TObject *Sender)
{
    if (OpenDialog1->Execute())
    {
        int i;
        SPLoad( OpenDialog1->FileName.c_str() );
        FileName = OpenDialog1->FileName;
        m_CurrentPropState = -1;

        //Remove old panels
        for (i = 0; i < m_numFCPanels; i++)
        {
            delete m_FCPanels[i];
             m_FCPanels[i] = NULL;
        }
        m_numFCPanels = 0;

        for (i = 0; i < m_numDrawablePanels; i++)
        {
             delete m_DrawablePanels[i];
             m_DrawablePanels[i] = NULL;
        }
        m_numDrawablePanels = 0;

        //Set up new panels
        int numfc = SPGetNumFrameControllers();
        m_numFCPanels = numfc;
        for (i = 0; i < numfc; i++ )
        {
            AnsiString framename;
            framename.sprintf("fcframe%i", i );
            TFrame1* f = new TFrame1( this , i );
            f->Parent = Panel4;
            f->Name = framename;
            m_FCPanels[i] = f;
        }
        Panel2->Height = numfc * 33 + 35 + 32;
        int panel4height = numfc * 33;
        if ( panel4height < 6 * 33 )
        {
            Panel4->Height = numfc * 33;
        }
        else
        {
            Panel4->Height = 6 * 33;
        }

        for (i = numfc - 1; i >= 0; i-- )
        {
            m_FCPanels[i]->Align = alTop;
        }

        m_numDrawablePanels = SPGetNumDrawables();
        for (i = 0; i < m_numDrawablePanels; i++ )
        {
            AnsiString framename;
            framename.sprintf("drawframe%i", i );
            TFrame2* f = new TFrame2( this , i );
            f->Parent = Panel5;
            f->Name = framename;
            m_DrawablePanels[i] = f;
        }
        for ( i = m_numDrawablePanels-1; i >= 0; i-- )
        {
            m_DrawablePanels[i]->Align = alTop;
        }
        g_IsDataSaved = true;
    }
}
//---------------------------------------------------------------------------

void __fastcall TMainForm::P3DPanelMouseDown(TObject *Sender,
      TMouseButton Button, TShiftState Shift, int X, int Y)
{
    int buttons, shift;
    FVUShiftStateToMouse(Shift, buttons, shift);
    SPContextMouseDown(buttons, shift, X, Y);
}
//---------------------------------------------------------------------------

void __fastcall TMainForm::P3DPanelMouseMove(TObject *Sender,
      TShiftState Shift, int X, int Y)
{
    int buttons, shift;
    FVUShiftStateToMouse(Shift, buttons, shift);
    SPContextMouseMove(buttons, shift, X, Y);
}
//---------------------------------------------------------------------------

void __fastcall TMainForm::P3DPanelMouseUp(TObject *Sender,
      TMouseButton Button, TShiftState Shift, int X, int Y)
{
    int buttons, shift;
    FVUShiftStateToMouse(Shift, buttons, shift);
    SPContextMouseUp(buttons, shift, X, Y);
}
//---------------------------------------------------------------------------

void __fastcall TMainForm::FormKeyPress(TObject *Sender, char &Key)
{
    switch (Key)
    {
        case '.':
            SPNextState();
            g_ForceDataUpdate = true;
            break;
        case ',':
            SPPrevState();
            g_ForceDataUpdate = true;
            break;
    }
}
//---------------------------------------------------------------------------

void __fastcall TMainForm::PlayButtonClick(TObject *Sender)
{
        static bool play = true;
        SPPause( play );
        play = !play;
        if (play)
        {
                PlayButton->Caption = "| |";
        }
        else
        {
                PlayButton->Caption = ">";
        }
}
//---------------------------------------------------------------------------



void __fastcall TMainForm::PrevStatebuttonClick(TObject *Sender)
{
    SPPrevState();
    g_ForceDataUpdate = true;
}
//---------------------------------------------------------------------------

void __fastcall TMainForm::NextStateButtonClick(TObject *Sender)
{
    SPNextState();
    g_ForceDataUpdate = true;
}
//---------------------------------------------------------------------------

void __fastcall TMainForm::ForwardStepButtonClick(TObject *Sender)
{
        SPAdvanceOneFrame();
}
//---------------------------------------------------------------------------

void __fastcall TMainForm::BackStepButtonClick(TObject *Sender)
{
        SPBackOneFrame();
}
//---------------------------------------------------------------------------

void __fastcall TMainForm::Button1Click(TObject *Sender)
{
    SPAddEvent( "" , 0 , 0 , SPGetCurrentState() );
    UpdateEvents();
    g_IsDataSaved = false;
}
//---------------------------------------------------------------------------

void TMainForm::UpdateStateInfo()
{
    AnsiString propname;
    propname.sprintf( "%s", SPGetPropName() );
    PropNameLabel->Caption = propname;
    PropNameLabelTop->Caption = propname;

    AnsiString numstates;
    numstates.sprintf("%i", SPGetNumberOfStates() );
    NumPropStatesLabel->Caption = numstates;
    NumPropStatesLabelTop->Caption = numstates;

    AnsiString curstate;
    curstate.sprintf("%i" , SPGetCurrentState() + 1 );
    CurrentStateLabel->Caption = curstate;
    CurrentStateLabelTop->Caption = curstate;
    StateStateLabel->Caption = curstate;

    TransitionData transitionData;
    if ( SPGetTransitionData( SPGetCurrentState() , &transitionData ) )
    {
        AutoTransitionCheckBox->Checked = transitionData.autoTransition;

        AnsiString tostate;
        tostate.sprintf("%i", transitionData.toState + 1);
        ToStateEditBox->Text = tostate;
        ToStateEditBox->Enabled = transitionData.autoTransition;

        AnsiString onframe;
        onframe.sprintf("%0.3f", transitionData.onFrame );
        OnFrameEditBox->Text = onframe;
        OnFrameEditBox->Enabled = transitionData.autoTransition;
    }
}
//---------------------------------------------------------------------------

void TMainForm::UpdateFrameControllers()
{
    //Set up fc's
    int i;
    for ( i = 0; i < m_numFCPanels; i++ )
    {
        AnsiString text;
        text.sprintf("%s",SPGetFrameControllerName( i ));
        m_FCPanels[i]->FrameControllerNameLabel->Caption = text;

        FrameControllerData fcData;
        if ( SPGetFrameControllerData( SPGetCurrentState() , i , &fcData ) )
        {
            text.sprintf("%0.3f", fcData.minFrame );
            m_FCPanels[i]->MinFrameEditBox->Text = text;

            text.sprintf("%0.3f",fcData.maxFrame);
            m_FCPanels[i]->MaxFrameEditBox->Text = text;

            text.sprintf("%0.3f", fcData.relativeSpeed );
            m_FCPanels[i]->speedEditBox->Text = text;

            if ( fcData.isCyclic )
            {
                m_FCPanels[i]->CyclicCheckBox->State = cbChecked;
            }
            else
            {
                m_FCPanels[i]->CyclicCheckBox->State = cbUnchecked;
            }
            m_FCPanels[i]->holdFrameCheckbox->Checked = fcData.holdFrame;

            if ( fcData.numberOfCycles == 0 )
            {
                text.sprintf("forever" );
                m_FCPanels[i]->cycleForEditBox->Text = text;
            }
            else
            {
                text.sprintf("%i" , fcData.numberOfCycles);
                m_FCPanels[i]->cycleForEditBox->Text = text;
            }

            m_FCPanels[i]->cycleForEditBox->Enabled = m_FCPanels[i]->CyclicCheckBox->State != cbUnchecked;
        }
    }
}
//---------------------------------------------------------------------------

void TMainForm::UpdateDrawables()
{
    //Set up drawables
    int i;
    for ( i = 0; i < m_numDrawablePanels; i++ )
    {
        AnsiString n;
        n.sprintf("%s",SPGetDrawableName( i ));
        m_DrawablePanels[i]->NameLabel->Caption = n;

        VisibilityData visData;
        if ( SPGetVisibilityData( SPGetCurrentState() , i , &visData ) )
        {
            if ( visData.isVisible )
            {
                m_DrawablePanels[i]->VisibilityCheckBox->State = cbChecked;
            }
            else
            {
                m_DrawablePanels[i]->VisibilityCheckBox->State = cbUnchecked;
            }
        }
    }
}
//---------------------------------------------------------------------------

void TMainForm::UpdateEvents()
{
    int i;
    for ( i = 0; i < m_numEvents; i++ )
    {
        //delete old panels
        delete m_EventPanels[i];
        m_EventPanels[i] = NULL;
    }
    m_numEvents = SPGetNumberOfEvents( SPGetCurrentState() );
    for ( i = 0; i < m_numEvents; i++ )
    {
        //create panels
        AnsiString framename;
        framename.sprintf("eventframe%i", i );
        TFrame3* f = new TFrame3( this , i );
        f->Parent = Panel6;
        f->Name = framename;

        //load the events list
        AnsiString fname = Application->ExeName;
        fname.SetLength( fname.Length() - 13 );
        fname.cat_sprintf("data\\events.txt");
        f->EventComboBox->Items->LoadFromFile(fname);

        //set up events fields
        EventData eventData;
        if ( SPGetEventData( SPGetCurrentState() , i , &eventData ) )
        {
            AnsiString eventname;
            eventname.sprintf( eventData.eventName );
            int eventindex = f->EventComboBox->Items->IndexOf( eventname );
            f->EventComboBox->ItemIndex = eventindex;
            AnsiString toState;
            toState.sprintf("%i", eventData.toState + 1);
            f->ToFrameEditBox->Text = toState;
        }

        //add panel to panel list
        m_EventPanels[i] = f;
    }

    for ( i = m_numEvents-1; i >= 0 ; i-- )
    {
        m_EventPanels[i]->Align = alTop;
    }
}
//---------------------------------------------------------------------------

void TMainForm::UpdateCallbacks()
{
    int i;
    for ( i = 0; i < m_numCallbacks; i++ )
    {
        //delete old panels
        delete m_CallbackPanels[i];
        m_CallbackPanels[i] = NULL;
    }
    m_numCallbacks = SPGetNumberOfCallbacks( SPGetCurrentState() );
    for ( i = 0; i < m_numCallbacks; i++ )
    {
        //create panels
        AnsiString framename;
        framename.sprintf("callbackframe%i", i );
        TFrame4* f = new TFrame4( this , i );
        f->Parent = CallbackPanel;
        f->Name = framename;

        //load the callbacks list
        AnsiString fname = Application->ExeName;
        fname.SetLength( fname.Length() - 13 );
        fname.cat_sprintf("data\\callbacks.txt");
        f->CallbackComboBox->Items->LoadFromFile(fname);

        CallbackData cbData;
        if ( SPGetCallbackData( SPGetCurrentState() , i , &cbData ) )
        {
            //set up events fields
            AnsiString callbackname;
            callbackname.sprintf( cbData.callbackName );
            int cbindex = f->CallbackComboBox->Items->IndexOf( callbackname );
            f->CallbackComboBox->ItemIndex = cbindex;
            AnsiString onFrame;
            onFrame.sprintf("%0.3f", cbData.onFrame );
            f->OnFrameEditBox->Text = onFrame;
        }
        //add panel to panel list
        m_CallbackPanels[i] = f;
    }

    for ( i = m_numCallbacks-1; i >= 0 ; i-- )
    {
        m_CallbackPanels[i]->Align = alTop;
    }
}
//---------------------------------------------------------------------------

void __fastcall TMainForm::Button2Click(TObject *Sender)
{
    SPAddCallback( SPGetCurrentState() , "" , 0 , 0.f );
    UpdateCallbacks();
    g_IsDataSaved = false;
}
//---------------------------------------------------------------------------

void __fastcall TMainForm::Save1Click(TObject *Sender)
{
    SaveData();
}
//---------------------------------------------------------------------------

void TMainForm::SaveData()
{
    char buf[256];
	memcpy( buf , FileName.c_str() , strlen(FileName.c_str()) - 4 );
	buf[strlen(FileName.c_str()) - 4] = '\0';
	strcat(buf , "data.p3d");

    int attrib = get_file_attrib( buf );

    if ( ( attrib & FA_RDONLY ) && ( attrib != -1 ) )
    {
        AnsiString out;
        out.sprintf( "%s is read-only.\n" , FileName.c_str() );
        if ( Application->MessageBox( out.c_str() , "" , MB_RETRYCANCEL ) == IDRETRY )
        {
            SaveData();
        }
    }
    else
    {
        SPExportStatePropData( FileName.c_str() );
        g_IsDataSaved = true;
    }
}
//---------------------------------------------------------------------------

void __fastcall TMainForm::FileSaveExecute(TObject *Sender)
{
    SaveData();
}
//---------------------------------------------------------------------------

void __fastcall TMainForm::AutoTransitionCheckBoxClick(TObject *Sender)
{
    bool autotransition = AutoTransitionCheckBox->State == cbChecked ? true : false;
    SPSetAutoTransition( SPGetCurrentState() , autotransition );
    g_ForceDataUpdate = true;
    g_IsDataSaved = false;
}
//---------------------------------------------------------------------------

void __fastcall TMainForm::ToStateEditBoxChange(TObject *Sender)
{
    AnsiString toState = ToStateEditBox->Text;
    int state = 0;
    try
    {
        state = toState.ToInt();
    }
    catch (Exception&)
    {
        TransitionData tranData;
        SPGetTransitionData( SPGetCurrentState() , &tranData );
        state = tranData.toState + 1;
    }
    if ( state > SPGetNumberOfStates() || state <= 0 )
    {
        state = 1;
    }
    SPSetAutoTransitionToState( SPGetCurrentState() , state - 1 );
    g_IsDataSaved = false;
}
//---------------------------------------------------------------------------

void __fastcall TMainForm::OnFrameEditBoxChange(TObject *Sender)
{
    AnsiString toframe = OnFrameEditBox->Text;
    float frame = 0;
    try
    {
        frame = toframe.ToDouble();
    }
    catch (Exception&)
    {
        TransitionData tranData;
        SPGetTransitionData( SPGetCurrentState() , &tranData );
        frame = tranData.onFrame;
    }

    SPSetAutoTransitionOnFrame( SPGetCurrentState() , frame );
    g_IsDataSaved = false;
}
//---------------------------------------------------------------------------

void __fastcall TMainForm::Button4Click(TObject *Sender)
{
    AnsiString caption;
    caption.sprintf("Delete State");
    AnsiString out;
    out.sprintf( "Delete State: %i\n It won't come back!", SPGetCurrentState() + 1 );
    if ( Application->MessageBox(out.c_str() , caption.c_str() , MB_YESNO) != IDOK )
    {
        SPDeleteState( SPGetCurrentState() );
        if ( SPGetCurrentState() - 1 >= 0 )
        {
            SPSetState( SPGetCurrentState() - 1 );
        }
        g_ForceDataUpdate = true;
    }
}
//---------------------------------------------------------------------------

void __fastcall TMainForm::Button3Click(TObject *Sender)
{
    SPInsertState( SPGetCurrentState() );
    g_ForceDataUpdate = true;
}
//---------------------------------------------------------------------------

//---------------------------------------------------------------------------
void __fastcall TMainForm::Button6Click(TObject *Sender)
{
    SPHideAll( SPGetCurrentState() );
    g_ForceDataUpdate = true;
    g_IsDataSaved = false;
}
//---------------------------------------------------------------------------

void __fastcall TMainForm::Button7Click(TObject *Sender)
{
    SPShowAll( SPGetCurrentState() );
    g_ForceDataUpdate = true;
    g_IsDataSaved = false;
}

//---------------------------------------------------------------------------
void __fastcall TMainForm::B1Click(TObject *Sender)
{
    if ( ColorDialog1->Execute() )
    {
        TColor color = ColorDialog1->Color;
        int blue =   (int)   ((color & 0x00ff0000) >> 16);
        int green = (int) ((color & 0x0000ff00) >> 8);
        int red =  (int)  ((color & 0x000000ff));
        SPSetBackgroundColour( red , green , blue );
    }
}
//---------------------------------------------------------------------------

void __fastcall TMainForm::PDDICheckBoxClick(TObject *Sender)
{
    SPContextSetIsPDDIStatsEnabled( PDDICheckBox->Checked );
}
//---------------------------------------------------------------------------



void __fastcall TMainForm::ToStateEditBoxExit(TObject *Sender)
{
    g_ForceDataUpdate = true;    
}
//---------------------------------------------------------------------------

void __fastcall TMainForm::OnFrameEditBoxExit(TObject *Sender)
{
    g_ForceDataUpdate = true;
}
//---------------------------------------------------------------------------

void __fastcall TMainForm::P3DBackgroundArt1Click(TObject *Sender)
{
    if (OpenDialog1->Execute())
    {
        SPLoadBackground( OpenDialog1->FileName.c_str() );
    }
}
//---------------------------------------------------------------------------

