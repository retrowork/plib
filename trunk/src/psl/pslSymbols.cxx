/*
     PLIB - A Suite of Portable Game Libraries
     Copyright (C) 1998,2002  Steve Baker

     This library is free software; you can redistribute it and/or
     modify it under the terms of the GNU Library General Public
     License as published by the Free Software Foundation; either
     version 2 of the License, or (at your option) any later version.

     This library is distributed in the hope that it will be useful,
     but WITHOUT ANY WARRANTY; without even the implied warranty of
     MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
     Library General Public License for more details.

     You should have received a copy of the GNU Library General Public
     License along with this library; if not, write to the Free Software
     Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307 USA

     For further information visit http://plib.sourceforge.net

     $Id$
*/


#include "pslLocal.h"


pslAddress pslParser::setVarSymbol ( const char *s )
{
  for ( int i = 0 ; i < next_var ; i++ )
    if ( strcmp ( s, symtab [ i ] . symbol ) == 0 )
    {
      ulSetError ( UL_WARNING, "PSL: Multiple definition of '%s'.", s ) ;
      return symtab [ i ] . address ;
    }

  if ( next_var >= MAX_VARIABLE-1 )
  {
    ulSetError ( UL_WARNING, "PSL: Too many variables." ) ;
    next_var-- ;
  }

  symtab [ next_var ] . set ( s, next_var ) ;

  return symtab [ next_var++ ] . address ;
}



pslAddress pslParser::getVarSymbol ( const char *s )
{
  for ( int i = 0 ; i < next_var ; i++ )
    if ( strcmp ( s, symtab [ i ] . symbol ) == 0 )
      return symtab [ i ] . address ;

  ulSetError ( UL_WARNING, "PSL: Undefined symbol '%s'.", s ) ;

  return setVarSymbol ( s ) ;
}


int pslParser::getExtensionSymbol ( const char *s )
{
  for ( int i = 0 ; extensions [ i ] . symbol != NULL ; i++ )
    if ( strcmp ( s, extensions [ i ] . symbol ) == 0 )
      return i ;

  return -1 ;
}


void pslParser::addFwdRef ( const char *s, pslAddress where )
{
  for ( int i = 0 ; i < next_fwdref ; i++ )
  {
    if ( forward_ref [ i ] . symbol == NULL )
    {
      forward_ref [ i ] . set ( s, where ) ;
      return ;
    }
  }

  if ( next_fwdref >= MAX_SYMBOL )
  {
    ulSetError ( UL_WARNING, "PSL: Too many unresolved forward references." ) ;
    return ;
  }

  forward_ref [ next_fwdref++ ] . set ( s, where ) ;
}



void pslParser::fixup ( const char *s, pslAddress v )
{
  for ( int i = 0 ; i < next_fwdref ; i++ )
  {
    if ( forward_ref [ i ] . matches ( s ) )
    {
      pslAddress p = forward_ref [ i ] . getWhere () ;

      code [  p  ] = v & 0xFF ;
      code [ p+1 ] = (v >> 8) & 0xFF ;

      delete [] forward_ref[i].symbol ;
      forward_ref[i].symbol = NULL ;
    }
  }
}                                                                             



void pslParser::checkUnresolvedSymbols ()
{
  for ( int i = 0 ; i < next_fwdref ; i++ )
  {
    if ( forward_ref [ i ] . symbol != NULL )
    {
      ulSetError ( UL_WARNING, "PSL: '%s' does not exist.",
                                   forward_ref [ i ] . symbol ) ;

      /* Prevent multiple errors for same symbol */

      fixup ( forward_ref [ i ] . symbol, 0 ) ;
    }
  }
}



pslAddress pslParser::getCodeSymbol ( const char *s, pslAddress where )
{
  for ( int i = 0 ; i < next_code_symbol ; i++ )
    if ( strcmp ( s, code_symtab [ i ] . symbol ) == 0 )
      return code_symtab [ i ] . address ;

  /* Symbol is undefined - so make a forward reference to it */

  addFwdRef ( s, where ) ;

  return 0 ;
}



void pslParser::setCodeSymbol ( const char *s, pslAddress v )
{
  for ( int i = 0 ; i < next_code_symbol ; i++ )
    if ( strcmp ( s, code_symtab [ i ] . symbol ) == 0 )
    {
      ulSetError ( UL_WARNING, "PSL: Multiple definition of '%s'.", s ) ;
      return ;
    }

  if ( next_code_symbol >= MAX_VARIABLE-1 )
  {
    ulSetError ( UL_WARNING, "PSL: Too many labels." ) ;
    next_code_symbol-- ;
  }

  code_symtab [ next_code_symbol++ ] . set ( s, v ) ;

  fixup ( s, v ) ;
}



