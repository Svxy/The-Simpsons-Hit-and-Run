//===========================================================================
// Copyright (C) 2000 Radical Entertainment Ltd.  All rights reserved.
//
// Component:   CGuiScreen
//
// Description: 
//              
//
// Authors:     Darwin Chau
//              Tony Chu
//
// Revisions		Date			Author	    Revision
//                  2000/09/20		DChau		Created
//                  2002/06/06      TChu        Modified for SRR2
//
//===========================================================================

#ifndef GUISCREEN_H
#define GUISCREEN_H

//===========================================================================
// Nested Includes
//===========================================================================
#include <presentation/gui/guiwindow.h>

//===========================================================================
// Forward References
//===========================================================================
class CGuiManager;
class tMultiController;

namespace Scrooby
{
    class Screen;
    class Page;
    class Layer;
    class Group;
    class Drawable;
    class Sprite;
    class Text;
    class Polygon;
    class Pure3dObject;
}

//class tCameraAnimationController;

const int MAX_FOREGROUND_LAYERS = 8;
const int MAX_BACKGROUND_LAYERS = 2;

enum eScreenEffect
{
    SCREEN_FX_ALL = ~0,
    SCREEN_FX_NONE = 0,

    SCREEN_FX_FADE = 1,
    SCREEN_FX_ZOOM = 2,
    SCREEN_FX_SLIDE_X = 4,
    SCREEN_FX_SLIDE_Y = 8,
    SCREEN_FX_IRIS = 16,

    NUM_SCREEN_FX
};

enum eButtonIcon
{
    BUTTON_ICON_ACCEPT,
    BUTTON_ICON_BACK,

    NUM_BUTTON_ICONS
};

//===========================================================================
// Interface Definitions
//===========================================================================
class CGuiScreen : public CGuiWindow
{
    public:

        CGuiScreen( Scrooby::Screen* pScroobyScreen,
                    CGuiEntity* pParent,
                    eGuiWindowID id,
                    unsigned int screenFX = SCREEN_FX_FADE );

        virtual ~CGuiScreen();

        virtual void HandleMessage( eGuiMessage message, 
			                        unsigned int param1 = 0,
							        unsigned int param2 = 0 );

        void SetScroobyScreen( Scrooby::Screen* pScreen ) { m_pScroobyScreen = pScreen; }
        Scrooby::Screen* GetScroobyScreen() const { return m_pScroobyScreen; }

        virtual CGuiMenu* HasMenu() { return NULL; }

        void SetFadingEnabled( bool enable );
        void SetZoomingEnabled( bool enable );
        void SetSlidingEnabled( eScreenEffect slideType, bool enable );
        void SetIrisWipeEnabled( bool enable, bool autoOpenIris = false );

        bool IsEffectEnabled( eScreenEffect effect ) const;

        static void Reset3dFEMultiController();

        void SetIngoreControllerInputs( bool ignore ) { m_ignoreControllerInputs = ignore; }
        bool IsIgnoringControllerInputs() const { return m_ignoreControllerInputs; }

        void SetButtonVisible( eButtonIcon button, bool isVisible );
        bool IsButtonVisible( eButtonIcon button ) const;

        void StartTransitionAnimation( int startFrame = -1,
                                       int endFrame = -1,
                                       bool lastTransition = true );

        void ReloadScreen();
        void RestoreScreenCover();
        void RestoreButtons();

        static bool IsWideScreenDisplay();
        static void ApplyWideScreenCorrectionScale( Scrooby::Drawable* drawable );

#ifdef RAD_WIN32
        virtual eFEHotspotType CheckCursorAgainstHotspots( float x, float y );
#endif

	protected:

        //---------------------------------------------------------------------
        // Protected Functions
        //---------------------------------------------------------------------

        void SetFadeTime( float fadeTime ) { m_fadeTime = fadeTime; }
        void RestoreDefaultFadeTime();

        void SetZoomTime( float zoomTime ) { m_zoomTime = zoomTime; }
        void RestoreDefaultZoomTime();

        void IrisWipeOpen();

        void SetAlphaForLayers( float alpha,
                                Scrooby::Layer** layers,
                                int numLayers );

        void AutoScaleFrame( Scrooby::Page* pPage );
        #ifdef DEBUGWATCH
            virtual const char* GetWatcherName() const;
        #endif

        //---------------------------------------------------------------------
        // Protected Data
        //---------------------------------------------------------------------

        CGuiManager* m_guiManager;

		Scrooby::Screen* m_pScroobyScreen;
        Scrooby::Layer* m_screenCover;
        Scrooby::Pure3dObject* m_p3dObject;

        static tMultiController* s_p3dMultiController;

        enum eIrisState
        {
            IRIS_STATE_IDLE,
            IRIS_STATE_CLOSING,
            IRIS_STATE_CLOSED,
            IRIS_STATE_OPENING,

            NUM_IRIS_STATES
        };

        Scrooby::Pure3dObject* m_p3dIris;
        tMultiController* m_irisController;
        eIrisState m_currentIrisState;
        bool m_autoOpenIris : 1;

        Scrooby::Layer* m_foregroundLayers[ MAX_FOREGROUND_LAYERS ];
        int m_numForegroundLayers;

        Scrooby::Layer* m_backgroundLayers[ MAX_BACKGROUND_LAYERS ];
        int m_numBackgroundLayers;

        Scrooby::Group* m_buttonIcons[ NUM_BUTTON_ICONS ];

        bool m_ignoreControllerInputs : 1;
        bool m_inverseFading : 1;

	private:

        //---------------------------------------------------------------------
        // Private Functions
        //---------------------------------------------------------------------

        // No copying or asignment. Declare but don't define.
        //
        CGuiScreen( const CGuiScreen& );
        CGuiScreen& operator= ( const CGuiScreen& );

        // Screen Fade In/Out Effects
        void FadeIn( float elapsedTime );
        void FadeOut( float elapsedTime );

        // Screen Zoom In/Out Effects
        void ZoomIn( float elapsedTime );
        void ZoomOut( float elapsedTime );

        // Screen Slide In/Out Effects
        void SlideIn( float elapsedTime );
        void SlideOut( float elapsedTime );

        // Iris Wipe Closed
        //
        void OnIrisWipeClosed();

        //---------------------------------------------------------------------
        // Private Data
        //---------------------------------------------------------------------

        static float s_numIrisFrames;

        unsigned int m_screenFX; // bit mask for screen effects

        float m_fadeTime;
        float m_elapsedFadeTime;

        float m_zoomTime;
        float m_elapsedZoomTime;

        float m_slideTime;
        float m_elapsedSlideTime;

        bool m_playTransitionAnimationLast : 1;

};

#endif // GUISCREEN_H
