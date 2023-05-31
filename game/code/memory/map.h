#ifndef MAP_H
#define MAP_H

#include <algorithm>
#include <vector>
#include <raddebug.hpp>
#include <memory/stlallocators.h>

// Ian Gipson's Map. 
// Similar to STL map but with the addition of a reserve function that works much 
// like the original stl::vector::reserve.
// I've made 2 minor changes. One is prefixing the Map namespace to the iterators
// that prevent the PS2 version from compiling because of typedef confusion
// with STL's vectors and I've moved the STL onto Darwin's 
// persistent_allocator for memory tracking

//=============================================================================
// ForwardReference
//=============================================================================
template < class KeyClass, class T >
class Map;


//=============================================================================
// MapElement
//=============================================================================
template < class KeyClass, class T >
class MapElement
{
public:
    KeyClass    first;
    T           second;

    bool operator<( const MapElement& right ) const{ return first < right.first;}
    bool operator==( const MapElement& right ) const{ return first == right.first;}
protected:
};

//=============================================================================
// Map
//=============================================================================
template < class KeyClass, class T >
class Map
{
public:
    //=========================================================================
    // iterator
    //=========================================================================
    typedef MapElement< KeyClass, T > MapElementType;
    typedef std::vector< MapElementType, s2alloc< MapElementType > > MapElementVector;
    typedef typename MapElementVector::iterator iterator;
    typedef typename MapElementVector::const_iterator const_iterator;
    //=========================================================================

    Map();

    typename Map::const_iterator  begin() const;
	typename Map::iterator		  begin();
    size_t				          capacity() const;
    void				          clear();
    typename Map::const_iterator  end() const;
    typename Map::iterator        end();
    typename Map::iterator        erase( typename Map::iterator it );
    size_t				          erase( const KeyClass& key );
    typename Map::const_iterator  find( const KeyClass& key ) const;
    typename Map::iterator        find( const KeyClass& key );
    T&                            get( const KeyClass& key );
    void				          insert( const KeyClass& key, const T& element );
    const T&			          operator[]( const KeyClass& key ) const;
    void				          reserve( const size_t size );
    size_t				          size() const;

protected:
    void RefreshIfDirty() const;
    mutable MapElementVector m_Elements;   //oh how i lie about constness...
    mutable bool m_Dirty;
};

//=============================================================================
// Constructor
//=============================================================================
template < class KeyClass, class T >
Map< KeyClass, T >::Map( ):
    m_Dirty( false )
{
    //
    // nothing
    //
}

//=============================================================================
// begin
//=============================================================================
template < class KeyClass, class T >
typename Map< KeyClass, T >::const_iterator 
Map< KeyClass, T >::begin() const
{
    return m_Elements.begin();
}

//=============================================================================
// begin
//=============================================================================
template < class KeyClass, class T >
typename Map< KeyClass, T >::iterator Map< KeyClass, T >::begin()
{
    return m_Elements.begin();
}

//=============================================================================
// capacity
//=============================================================================
template < class KeyClass, class T >
size_t Map< KeyClass, T >::capacity() const
{
    return m_Elements.capacity();
}

//=============================================================================
// clear
//=============================================================================
template < class KeyClass, class T >
void Map< KeyClass, T >::clear()
{
    m_Elements.erase( m_Elements.begin(), m_Elements.end() );
}

//=============================================================================
// end
//=============================================================================
template < class KeyClass, class T >
typename Map< KeyClass, T >::const_iterator Map< KeyClass, T >::end() const
{
    return m_Elements.end();
}

//=============================================================================
// end
//=============================================================================
template < class KeyClass, class T >
typename Map< KeyClass, T >::iterator Map< KeyClass, T >::end()
{
    return m_Elements.end();
}

//=============================================================================
// erase
//=============================================================================
template < class KeyClass, class T >
typename Map< KeyClass, T >::iterator Map< KeyClass, T >::erase( typename Map< KeyClass, T >::iterator it )
{
    return m_Elements.erase( it );
}

//=============================================================================
// erase
//=============================================================================
template < class KeyClass, class T >
size_t Map< KeyClass, T >::erase(  const KeyClass& key )
{
    MapElementVector::iterator it;
    for( it = m_Elements.begin(); it != m_Elements.end(); ++it )
    {
        if( (*it).first == key )
        {
            it = m_Elements.erase( it );
            return 1;
        }
    }
    return 0;
}

//======`=======================================================================
// find
//=============================================================================
template < class KeyClass, class T >
typename Map< KeyClass, T >::const_iterator Map< KeyClass, T >::find( const KeyClass& key ) const
{
    RefreshIfDirty();
    MapElement< KeyClass, T > findme;
    findme.first = key;
    const Map::iterator it = std::lower_bound( m_Elements.begin(), m_Elements.end(), findme );
    return it;
}

//=============================================================================
// find
//=============================================================================
template < class KeyClass, class T >
typename Map< KeyClass, T >::iterator Map< KeyClass, T >::find( const KeyClass& key )
{
    RefreshIfDirty();
    MapElement< KeyClass, T > findme;
    findme.first = key;
    Map::iterator it = std::lower_bound( m_Elements.begin(), m_Elements.end(), findme );
    if( it != m_Elements.end() )
    {
        if( ( *it).first == key )
        {
            return it;
        }
    }
    return m_Elements.end();
}

//=============================================================================
// insert
//=============================================================================
template < class KeyClass, class T >
void Map< KeyClass, T >::insert( const KeyClass& key, const T& element )
{
    //if the key is already in the map, then just replace it
    Map::iterator found = find( key );
    if( found != end() )
    {
        ( *found ).second = element;
    }
    else
    {
        // this function could be faster, not requiring a sort every time
        // something gets inserted. if maps turn out to be slow, make this 
        // adjustment
        MapElement< KeyClass, T > me;
        me.first = key;
        me.second = element;
#ifdef RAD_DEBUG
		size_t capacityBefore = m_Elements.capacity();
#endif
        m_Elements.push_back( me );
#ifdef RAD_DEBUG
		size_t capacityAfter = m_Elements.capacity();
        if( capacityBefore != capacityAfter )
        {
            //rDebugPrintf( "This map should have been RESERVED larger before use\n" );
        }
#endif
        m_Dirty = true;
    }
}

//=============================================================================
// operator[]
//=============================================================================
template < class KeyClass, class T >
const T& Map< KeyClass, T >::operator[]( const KeyClass& key ) const
{
    Map::const_iterator it = find( key );
    rAssert( it != end() );
    return (*it).second;
}

//=============================================================================
// get
//=============================================================================
template < class KeyClass, class T >
T& Map< KeyClass, T >::get( const KeyClass& key )
{
    Map::iterator it = find( key );
    rAssert( it != end() );
    return (*it).second;
}

//=============================================================================
// Refresh the sorting of the map if it is currently dirty
//=============================================================================
template < class KeyClass, class T >
void Map< KeyClass, T >::RefreshIfDirty() const
{
    //THIS FUNCTION DOES NOTHING
    if( m_Dirty )
    {
        std::sort( m_Elements.begin(), m_Elements.end() );
        m_Dirty = false;
    }
}

//=============================================================================
// Reserves space in the map, so that it won't have to autoresize
//=============================================================================
template < class KeyClass, class T >
void Map< KeyClass, T >::reserve( const size_t size )
{
    m_Elements.reserve( size );
}

//=============================================================================
// returns the number of elements in the map
//=============================================================================
template < class KeyClass, class T >
size_t Map< KeyClass, T >::size() const
{
    return m_Elements.size();
}

#endif