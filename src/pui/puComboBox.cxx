/*
     PLIB - A Suite of Portable Game Libraries
     Copyright (C) 2001  Steve Baker
 
     This library is free software; you can redistribute it and/or
     modify it under the terms of the GNU Library General Public
     License as published by the Free Software Foundation; either
     version 2 of the License, or (at your option) any later version.
 
     This library is distributed in the hope that it will be useful,
     but WITHOUT ANY WARRANTY; without even the implied warranty of
     MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
     Library General Public License for more details.
 
     You should have received a copy of the GNU Library General Public
     License along with this library; if not, write to the Free
     Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307 USA
 
     For further information visit http://plib.sourceforge.net

     $Id$
*/

#include "puLocal.h"

void puComboBox::handle_arrow ( puObject *arrow )
{
  puComboBox *cbox = (puComboBox *) arrow -> getUserData () ;

  if ( ! cbox -> popup_menu -> isVisible() )
    cbox -> popup_menu -> reveal() ;
  else
    cbox -> popup_menu -> hide() ;

  /* arrow -> setStyle ( - arrow -> getStyle () ) ; */
}

void puComboBox::handle_popup ( puObject *popupm )
{
  puComboBox *cbox = (puComboBox *) popupm -> getUserData () ;

  cbox -> setCurrentItem ( popupm->getLegend () );
}


void puComboBox::update_widgets ( void )
  {
    if ( curr_item >= 0 )
    /* There are items */
    {
      setValue ( list[curr_item] ) ;

      down_arrow -> activate () ;
    }
    else
    /* There aren't any items */
    {
      setValue ( "" ) ;

      down_arrow -> greyOut () ;
    }
  }


void puComboBox::newList ( char ** _list )
{
  list = _list ;

  if ( list == NULL )
    num_items = 0 ;
  else
  {
    int dummy, h ;
    int i ;

    for ( num_items = 0 ; list[num_items] != NULL ; num_items++ )
      /* Count number of items */ ;

    popup_menu -> empty () ;

    for ( i = (num_items - 1) ; i >= 0 ; i-- )
      {
        puObject *menu_item = popup_menu->add_item(list[i], handle_popup) ;

        menu_item -> setUserData ( this ) ;
        menu_item -> getSize ( &dummy, &h ) ;
        menu_item -> setSize ( abox.max[0] - abox.min[0], h ) ;
      }

    popup_menu -> close() ;

    popup_menu -> getSize ( &dummy, &h ) ;

    if ( (abox.min[1] - h) >= 0 )
      popup_menu -> setPosition ( 0, 0 - h ) ;
    else
    {
      input -> getSize ( &dummy, &h ) ;
      popup_menu -> setPosition ( 0, h ) ;
    }
  }

  setCurrentItem ( num_items > 0 ? 0 : - 1 ) ;
}

int puComboBox::getCurrentItem ( void )
{
  if ( strcmp ( list[curr_item], getStringValue() ) )
  /* The user typed in an arbitrary string.
     Let's see if it is one of our entries ... */
  {
    int i ;

    for ( i = 0 ; i < num_items ; i++ )
    {
      if ( !strcmp ( list[i], getStringValue() ) )
        /* ... yes, it its ! */
        return i ;
    }

    /* ... no, it isn't. */
    return -1 ;
  }
  else
    return curr_item ;
}

void puComboBox::setCurrentItem ( const char *item_ptr )
{
  int i ;

  for ( i = 0 ; i < num_items ; i++ )
  {
    if ( list[i] == item_ptr )
    {
      setCurrentItem ( i ) ;

      break ;
    }
  }
}


void puComboBox::draw ( int dx, int dy )
{
  draw_label ( dx, dy ) ;

  puGroup::draw ( dx, dy ) ;
}

puComboBox::puComboBox ( int minx, int miny, int maxx, int maxy,
                         char **entries, int editable ) :
  puGroup( minx, miny )
  {
    type |= PUCLASS_COMBOBOX ;

    char *stringval ;
    int arrow_width = (int) ( (maxy-miny) / 1.5 ) ;

    input = new puInput ( 0, 0, maxx-minx - arrow_width, maxy-miny ) ;
    input -> setStyle ( PUSTYLE_SMALL_SHADED ) ;

    if ( ! editable )
      input -> disableInput () ;

    /* Share 'string' value with input box */
    input -> getValue ( &stringval ) ;
    setValuator ( stringval ) ;

    down_arrow = new puArrowButton ( maxx-minx - arrow_width, 0,
                                     maxx-minx, maxy-miny,
                                     PUARROW_DOWN ) ;
    down_arrow -> setUserData ( this ) ;
    down_arrow -> setCallback ( handle_arrow ) ;

    popup_menu = new puPopupMenu ( 0, 0 ) ;

    newList ( entries ) ;

    close () ;
  }
