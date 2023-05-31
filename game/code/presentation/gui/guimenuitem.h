//===========================================================================
// Copyright (C) 2003 Radical Entertainment Ltd.  All rights reserved.
//
// Component:   GuiMenuItem
//
// Description: 
//              
//
// Authors:     Tony Chu
//
// Revisions    Date            Author      Revision
//              2003/03/19      TChu        Created
//
//===========================================================================

#ifndef GUIMENUITEM_H
#define GUIMENUITEM_H

//===========================================================================
// Nested Includes
//===========================================================================
#include <presentation/gui/utility/slider.h>

#include <p3d/p3dtypes.hpp>

// Scrooby
//
#include <text.h>
#include <sprite.h>

//===========================================================================
// Forward References
//===========================================================================
namespace Scrooby
{
    class BoundedDrawable;
    class Sprite;
    class Text;
}

enum eMenuAttribute
{
    ALL_ATTRIBUTES_OFF = 0,

    SELECTION_ENABLED = 1,
    SELECTABLE = 2,
    VALUES_WRAPPED = 4,
    TEXT_OUTLINE_ENABLED = 8,

    ALL_ATTRIBUTES_ON = -1
};

//===========================================================================
// Interface Definitions - GuiMenuItem (Abstract Base Class)
//===========================================================================
struct GuiMenuItem
{
    GuiMenuItem();
    virtual ~GuiMenuItem();

    virtual void SetItem( Scrooby::BoundedDrawable* item ) = 0;
    virtual Scrooby::BoundedDrawable* GetItem() const = 0;

    virtual void SetItemValue( Scrooby::BoundedDrawable* itemValue ) = 0;
    virtual Scrooby::BoundedDrawable* GetItemValue() const = 0;

    virtual void SetItemValueIndex( int index ) = 0;
    virtual int GetItemValueIndex() const = 0;

    virtual void SetDisplayOutline( bool enable ) = 0;
    virtual void SetOutlineColour( tColour colour ) = 0;
    virtual tColour GetOutlineColour() const = 0;

    int m_attributes;
    tColour m_defaultColour;

    Scrooby::Sprite* m_itemValueArrowL;
    Scrooby::Sprite* m_itemValueArrowR;
    int m_itemValueCount;

    ImageSlider m_slider;

};

//===========================================================================
// Interface Definitions - GuiMenuItemText
//===========================================================================
struct GuiMenuItemText : public GuiMenuItem
{
    GuiMenuItemText();

    virtual void SetItem( Scrooby::BoundedDrawable* item );
    virtual Scrooby::BoundedDrawable* GetItem() const;

    virtual void SetItemValue( Scrooby::BoundedDrawable* itemValue );
    virtual Scrooby::BoundedDrawable* GetItemValue() const;

    virtual void SetItemValueIndex( int index );
    virtual int GetItemValueIndex() const;

    virtual void SetDisplayOutline( bool enable );
    virtual void SetOutlineColour( tColour colour );
    virtual tColour GetOutlineColour() const;

    Scrooby::Text* m_item;
    Scrooby::Text* m_itemValue;

};

//===========================================================================
// Inlines - GuiMenuItemText
//===========================================================================

inline Scrooby::BoundedDrawable* GuiMenuItemText::GetItem() const
{
    return m_item;
}

inline Scrooby::BoundedDrawable* GuiMenuItemText::GetItemValue() const
{
    return m_itemValue;
}

//===========================================================================
// Interface Definitions - GuiMenuItemSprite
//===========================================================================
struct GuiMenuItemSprite : public GuiMenuItem
{
    GuiMenuItemSprite();

    virtual void SetItem( Scrooby::BoundedDrawable* item );
    virtual Scrooby::BoundedDrawable* GetItem() const;

    virtual void SetItemValue( Scrooby::BoundedDrawable* itemValue );
    virtual Scrooby::BoundedDrawable* GetItemValue() const;

    virtual void SetItemValueIndex( int index );
    virtual int GetItemValueIndex() const;

    virtual void SetDisplayOutline( bool enable );
    virtual void SetOutlineColour( tColour colour );
    virtual tColour GetOutlineColour() const;

    Scrooby::Sprite* m_item;
    Scrooby::Sprite* m_itemValue;

};

//===========================================================================
// Inlines - GuiMenuItemSprite
//===========================================================================

inline Scrooby::BoundedDrawable* GuiMenuItemSprite::GetItem() const
{
    return m_item;
}

inline Scrooby::BoundedDrawable* GuiMenuItemSprite::GetItemValue() const
{
    return m_itemValue;
}


#endif // GUIMENUITEM_H
