#ifndef MAPPER_HPP
#define MAPPER_HPP

#include <input/controller.h>
#include <input/button.h>

class Mappable;

// Holds mappings from physical inputs to logical inputs 
//
// All inputs from controller system are passed through this table by the base Mappable 
// class before being passed on to derived classes (for a particular gameplay state)
class Mapper 
{
public:
    Mapper( void );

	// Associate sourceButtonID with destButtonID.
	int SetAssociation( int physicalIndex, int logicalIndex);

  	// Remove all mappings.  Set num associations to zero.
    void ClearAssociations( void );

	// Will return the physical button id ("X") associated with the logical
	// input ("Gas").  Useful for controller config.
    // Warning, could be miltiple mappings, it will only return the first one
	int GetPhysicalIndex( int logicalIndex) const;

	// Will return the logical button id ("Gas") associated with the logical
	// input ("X").  Useful for controller config.
	int GetLogicalIndex( int physicalIndex ) const;

private:
	// Association structure and members
    int buttonMap[Input::MaxPhysicalButtons];
};


#endif

