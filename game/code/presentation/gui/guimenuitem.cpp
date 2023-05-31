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

//===========================================================================
// Includes
//===========================================================================
#include <presentation/gui/guimenuitem.h>

//===========================================================================
// Global Data, Local Data, Local Classes
//===========================================================================

//===========================================================================
// Public Member Functions - GuiMenuItem
//===========================================================================

GuiMenuItem::GuiMenuItem()
:   m_attributes( ALL_ATTRIBUTES_OFF ),
    m_defaultColour( 0, 0, 0 ),
    m_itemValueArrowL( NULL ),
    m_itemValueArrowR( NULL ),
    m_itemValueCount( 0 )
{
}

GuiMenuItem::~GuiMenuItem()
{
}

//===========================================================================
// Public Member Functions - GuiMenuItemText
//===========================================================================

GuiMenuItemText::GuiMenuItemText()
:   GuiMenuItem(),
    m_item( NULL ),
    m_itemValue( NULL )
{
}

//===========================================================================
// GuiMenuItemText::SetItem
//===========================================================================
// Description: Sets the text drawable menu item.
//
// Parameters:  pointer to a Scrooby text drawable
//
// Return:      n/a
//
//===========================================================================
void
GuiMenuItemText::SetItem( Scrooby::BoundedDrawable* item )
{
    m_item = dynamic_cast<Scrooby::Text*>( item );
    rAssert( m_item != NULL );
}

//===========================================================================
// GuiMenuItemText::SetItemValue
//===========================================================================
// Description: Sets the text drawable menu item value.
//
// Parameters:  pointer to a Scrooby text drawable
//
// Return:      n/a
//
//===========================================================================
void
GuiMenuItemText::SetItemValue( Scrooby::BoundedDrawable* itemValue )
{
    m_itemValue = dynamic_cast<Scrooby::Text*>( itemValue );
    rAssert( m_itemValue != NULL );

    m_itemValueCount = m_itemValue->GetNumOfStrings();
}

//===========================================================================
// GuiMenuItemText::SetItemValueIndex
//===========================================================================
// Description: Sets the current item value index.
//
// Parameters:  text index
//
// Return:      n/a
//
//===========================================================================
void
GuiMenuItemText::SetItemValueIndex( int index )
{
    rAssert( m_itemValue != NULL );
    m_itemValue->SetIndex( index );
}

//===========================================================================
// GuiMenuItemText::GetItemValueIndex
//===========================================================================
// Description: Gets the current item value index.
//
// Parameters:  n/a
//
// Return:      text index
//
//===========================================================================
int
GuiMenuItemText::GetItemValueIndex() const
{
    rAssert( m_itemValue != NULL );
    return m_itemValue->GetIndex();
}

//===========================================================================
// GuiMenuItemText::SetDisplayOutline
//===========================================================================
// Description: Enables/Disables text outlining.
//
// Parameters:  enable/disable flag
//
// Return:      n/a
//
//===========================================================================
void
GuiMenuItemText::SetDisplayOutline( bool enable )
{
    rAssert( m_item != NULL );
    m_item->SetDisplayOutline( enable );

    if( m_itemValue != NULL )
    {
        m_itemValue->SetDisplayOutline( enable );
    }
}

//===========================================================================
// GuiMenuItemText::SetOutlineColour
//===========================================================================
// Description: Sets the current text outline colour.
//
// Parameters:  colour
//
// Return:      n/a
//
//===========================================================================
void
GuiMenuItemText::SetOutlineColour( tColour colour )
{
    rAssert( m_item != NULL );
    m_item->SetOutlineColour( colour );

    if( m_itemValue != NULL )
    {
        m_itemValue->SetOutlineColour( colour );
    }
}

//===========================================================================
// GuiMenuItemText::GetOutlineColour
//===========================================================================
// Description: Gets the current text outline colour.
//
// Parameters:  n/a
//
// Return:      colour
//
//===========================================================================
tColour
GuiMenuItemText::GetOutlineColour() const
{
    rAssert( m_item != NULL );
    return m_item->GetOutlineColour();
}


//===========================================================================
// Public Member Functions - GuiMenuItemSprite
//===========================================================================

GuiMenuItemSprite::GuiMenuItemSprite()
:   GuiMenuItem(),
    m_item( NULL ),
    m_itemValue( NULL )
{
}

//===========================================================================
// GuiMenuItemSprite::SetItem
//===========================================================================
// Description: Sets the text drawable menu item.
//
// Parameters:  pointer to a Scrooby sprite drawable
//
// Return:      n/a
//
//===========================================================================
void
GuiMenuItemSprite::SetItem( Scrooby::BoundedDrawable* item )
{
    m_item = dynamic_cast<Scrooby::Sprite*>( item );
    rAssert( m_item != NULL );
}

//===========================================================================
// GuiMenuItemSprite::SetItemValue
//===========================================================================
// Description: Sets the text drawable menu item value.
//
// Parameters:  pointer to a Scrooby sprite drawable
//
// Return:      n/a
//
//===========================================================================
void
GuiMenuItemSprite::SetItemValue( Scrooby::BoundedDrawable* itemValue )
{
    m_itemValue = dynamic_cast<Scrooby::Sprite*>( itemValue );
    rAssert( m_itemValue != NULL );

    m_itemValueCount = m_itemValue->GetNumOfImages();
}

//===========================================================================
// GuiMenuItemSprite::SetItemValueIndex
//===========================================================================
// Description: Sets the current item value index.
//
// Parameters:  sprite index
//
// Return:      n/a
//
//===========================================================================
void
GuiMenuItemSprite::SetItemValueIndex( int index )
{
    rAssert( m_itemValue != NULL );
    m_itemValue->SetIndex( index );
}

//===========================================================================
// GuiMenuItemSprite::GetItemValueIndex
//===========================================================================
// Description: Gets the current item value index.
//
// Parameters:  n/a
//
// Return:      sprite index
//
//===========================================================================
int
GuiMenuItemSprite::GetItemValueIndex() const
{
    rAssert( m_itemValue != NULL );
    return m_itemValue->GetIndex();
}

//===========================================================================
// GuiMenuItemSprite::SetDisplayOutline
//===========================================================================
// Description: Not applicable for GuiMenuItemSprite's and should never be
//              invoked.
//
// Parameters:  n/a
//
// Return:      n/a
//
//===========================================================================
void
GuiMenuItemSprite::SetDisplayOutline( bool enable )
{
    rAssertMsg( false, "No outlining on sprites!" );
}

//===========================================================================
// GuiMenuItemSprite::SetOutlineColour
//===========================================================================
// Description: Not applicable for GuiMenuItemSprite's and should never be
//              invoked.
//
// Parameters:  n/a
//
// Return:      n/a
//
//===========================================================================
void
GuiMenuItemSprite::SetOutlineColour( tColour colour )
{
    rAssertMsg( false, "No outlining on sprites!" );
}

//===========================================================================
// GuiMenuItemSprite::SetOutlineColour
//===========================================================================
// Description: Not applicable for GuiMenuItemSprite's and should never be
//              invoked.
//
// Parameters:  n/a
//
// Return:      n/a
//
//===========================================================================
tColour
GuiMenuItemSprite::GetOutlineColour() const
{
    rAssertMsg( false, "No outlining on sprites!" );

    return tColour( 0, 0, 0 );
}

