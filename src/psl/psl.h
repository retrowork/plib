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


#include <stdio.h>

enum pslResult
{
  PSL_PROGRAM_END,
  PSL_PROGRAM_PAUSE,
  PSL_PROGRAM_CONTINUE
} ;


#define PSL_COMPILETIME_WARNING 1
#define PSL_COMPILETIME_ERROR   2
#define PSL_RUNTIME_WARNING     3
#define PSL_RUNTIME_ERROR       4

typedef unsigned char pslOpcode ;
class pslContext ;
class pslCompiler  ;
class pslProgram ;


enum pslType
{
  PSL_INT    = 0,
  PSL_FLOAT  = 1,
  PSL_STRING = 3,
  PSL_VOID   = 4
} ;
 

class pslNumber
{
protected:

  union
  {
    int   i ;
    float f ;
  } ;

  char *s ;


  pslType t ;

public:

  pslNumber () { t = PSL_VOID ; s = NULL ; }

  virtual void set ( int                ) = 0 ;
  virtual void set ( float              ) = 0 ;
  virtual void set ( const char *       ) = 0 ;
  virtual void set ( const pslNumber  * ) = 0 ;

  void reset () { t = PSL_VOID ; i = 0 ; delete [] s ; s = NULL ; }

  pslType getType () const          { return t  ; }
  void    setType ( pslType _type ) { t = _type ; }

  int getInt () const
  {
    switch ( t )
    {
      case PSL_INT    : return       i ;
      case PSL_FLOAT  : return (int) f ;
      case PSL_STRING : return atoi(s) ;
      case PSL_VOID   : return       0 ;
    }
    return 0 ;
  }

  float getFloat () const
  {
    switch ( t )
    {
      case PSL_INT    : return (float) i ;
      case PSL_FLOAT  : return         f ;
      case PSL_STRING : return atof( s ) ;
      case PSL_VOID   : return      0.0f ;
    }
    return 0.0f ;
  }

  char *getString () const
  {
    switch ( t )
    {
      case PSL_STRING : return  s   ;
      case PSL_INT    :
      case PSL_FLOAT  :
      case PSL_VOID   : return NULL ;
    }
    return NULL ;
  }

} ;



/*
  psValues can change their type as needed.
*/

class pslValue : public pslNumber
{
public:
  virtual void set ()                { t = PSL_VOID   ; }
  virtual void set ( int         v ) { t = PSL_INT    ; i = v ; }
  virtual void set ( float       v ) { t = PSL_FLOAT  ; f = v ; }
  virtual void set ( const char *v ) { t = PSL_STRING ;
                                         delete [] s ;
                                         s = new char [ strlen(v)+1 ] ;
                                         strcpy ( s, v ) ; }

  virtual void set ( const pslNumber *v )
  {
    t = v -> getType ()  ;

    switch ( t )
    {
      case PSL_INT    : set ( v -> getInt    () ) ; break ;
      case PSL_FLOAT  : set ( v -> getFloat  () ) ; break ;
      case PSL_STRING : set ( v -> getString () ) ; break ;
      case PSL_VOID   : break ;
    }
  }

} ;


/*
  psVariables can change value - but their type is
  fixed once set.
*/

class pslVariable : public pslNumber
{
public:
  virtual void set ( int v )
  {
    switch ( t )
    {
      case PSL_INT    : i = v ; return ;
      case PSL_FLOAT  : f = (float) v ; return ;
      case PSL_STRING :
      case PSL_VOID   : return ;
    }
  }

  virtual void set ( float v )
  {
    switch ( t )
    {
      case PSL_INT    : i = (int) v ; return ;
      case PSL_FLOAT  : f =  v ; return ;
      case PSL_STRING :
      case PSL_VOID   : return ;
    }
  }

  virtual void set ( const char *v )
  {
    switch ( t )
    {
      case PSL_INT    : i = atoi ( v ) ; return ;
      case PSL_FLOAT  : f = atof ( v ) ; return ;
      case PSL_STRING : delete [] s ;
                        s = new char [ strlen(v)+1 ] ;
                        strcpy ( s, v ) ;
                        return ;
      case PSL_VOID   : return ;
    }
  }

  virtual void set ( const pslNumber *v )
  {
    switch ( t )
    {
      case PSL_INT    : set ( v -> getInt    () ) ; return ;
      case PSL_FLOAT  : set ( v -> getFloat  () ) ; return ;
      case PSL_STRING : set ( v -> getString () ) ; return ;
      case PSL_VOID   : return ;
    }
  }
} ;


class pslExtension
{
public:
  const char *symbol ;
  int   argc ;
  pslValue (*func) ( int, pslValue *, pslProgram *p ) ;
} ;



class pslProgram
{
  pslOpcode     *code       ;
  pslContext    *context    ;
  pslCompiler   *compiler   ;
  pslExtension  *extensions ;

  void *userData ;

  char *progName ;

public:

   pslProgram ( pslExtension *ext, char *_progName = NULL ) ;
   pslProgram ( pslProgram   *src, char *_progName = NULL ) ;

  ~pslProgram () ;

  pslContext   *getContext     () const { return context    ; }
  pslOpcode    *getCode        () const { return code       ; }
  pslCompiler  *getCompiler    () const { return compiler   ; }
  pslExtension *getExtensions  () const { return extensions ; }

  char *getProgName () const { return progName ; }

  void  setProgName ( const char *nm )
  {
    delete [] progName ;
    progName = new char [ strlen ( nm ) + 1 ] ;
    strcpy ( progName, nm ) ;
  }

  void      *getUserData () const     { return userData ; }
  void       setUserData ( void *ud ) { userData = ud ; }

  void       dump  () const ;
  int        compile ( const char *fname ) ;
  int        compile ( FILE *fd ) ;
  void       reset () ;
  pslResult  step  () ;
  pslResult  trace () ;
} ;


void pslInit () ;
void pslSetErrorCallback ( void (*CB) ( int, char *, int, char * ) ) ;

