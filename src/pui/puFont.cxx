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

#ifdef _PU_USE_GLUT_FONTS

/*
  A set of puFont's to implement the GLUT BitMap set...
*/

puFont PUFONT_8_BY_13        ( GLUT_BITMAP_8_BY_13        ) ;
puFont PUFONT_9_BY_15        ( GLUT_BITMAP_9_BY_15        ) ;
puFont PUFONT_TIMES_ROMAN_10 ( GLUT_BITMAP_TIMES_ROMAN_10 ) ;
puFont PUFONT_TIMES_ROMAN_24 ( GLUT_BITMAP_TIMES_ROMAN_24 ) ;
puFont PUFONT_HELVETICA_10   ( GLUT_BITMAP_HELVETICA_10   ) ;
puFont PUFONT_HELVETICA_12   ( GLUT_BITMAP_HELVETICA_12   ) ;
puFont PUFONT_HELVETICA_18   ( GLUT_BITMAP_HELVETICA_18   ) ;

#else  // No GLUT fonts, try some corresponding textured fonts

fntTexFont PUFONT_TXF_TYPEWRITER ;
fntTexFont PUFONT_TXF_TIMES      ;
fntTexFont PUFONT_TXF_HELVETICA  ;

puFont PUFONT_8_BY_13        ( &PUFONT_TXF_TYPEWRITER, 13 ) ;
puFont PUFONT_9_BY_15        ( &PUFONT_TXF_TYPEWRITER, 15 ) ;
puFont PUFONT_TIMES_ROMAN_10 ( &PUFONT_TXF_TIMES, 10 )      ;
puFont PUFONT_TIMES_ROMAN_24 ( &PUFONT_TXF_TIMES, 24 )      ;
puFont PUFONT_HELVETICA_10   ( &PUFONT_TXF_HELVETICA, 10 )  ;
puFont PUFONT_HELVETICA_12   ( &PUFONT_TXF_HELVETICA, 12 )  ;
puFont PUFONT_HELVETICA_18   ( &PUFONT_TXF_HELVETICA, 18 )  ;

#endif // #ifdef _PU_USE_GLUT_FONTS


int puFont::getStringWidth ( const char *str )
{
  if ( str == NULL )
    return 0 ;

#ifdef _PU_USE_GLUT_FONTS
  if ( glut_font_handle != (GlutFont) 0 )
  {
    int res = 0 ;

    while ( *str != '\0' )
    {
      res += glutBitmapWidth ( glut_font_handle, *str ) ;
      str++ ;
    }

    return res ;
  }
#endif // #ifdef _PU_USE_GLUT_FONTS

  if ( fnt_font_handle != NULL )
  {
    float r, l ;
    fnt_font_handle -> getBBox ( str, pointsize, slant, &l, &r, NULL, NULL ) ;
    return (int) ( r - l ) ;
  }

  return 0 ;
}


#ifdef _PU_USE_GLUT_FONTS
static int getGLUTStringHeight ( GlutFont glut_font_handle )
{
  if ( glut_font_handle == GLUT_BITMAP_8_BY_13        ) return  9 ;
  if ( glut_font_handle == GLUT_BITMAP_9_BY_15        ) return 10 ;
  if ( glut_font_handle == GLUT_BITMAP_TIMES_ROMAN_10 ) return  7 ;
  if ( glut_font_handle == GLUT_BITMAP_TIMES_ROMAN_24 ) return 17 ;
  if ( glut_font_handle == GLUT_BITMAP_HELVETICA_10   ) return  8 ;
  if ( glut_font_handle == GLUT_BITMAP_HELVETICA_12   ) return  9 ;
  if ( glut_font_handle == GLUT_BITMAP_HELVETICA_18   ) return 14 ;
  return 0 ;
}
#endif // #ifdef _PU_USE_GLUT_FONTS

int puFont::getStringHeight ( const char *s )
{
  /* Height *excluding* descender */
   
#ifdef _PU_USE_GLUT_FONTS
  if ( glut_font_handle != (GlutFont) 0 )
  {
    int i = getGLUTStringHeight ( glut_font_handle ) ;
    int num_lines = 1 ;

    for ( const char *p = s ; *p != '\0' ; p++ )
      if ( *p == '\n' )
        num_lines++ ;

    return i * num_lines ;
  }
#endif // #ifdef _PU_USE_GLUT_FONTS

  if ( fnt_font_handle != NULL )
  {
    float t, b ;
    fnt_font_handle -> getBBox ( s, pointsize, slant, NULL, NULL, &b, &t ) ;
    return (int) ( t - b ) ;
  }

  return 0 ;
}


int puFont::getStringHeight ()
{
  return getStringHeight ( "K" ) ;
}


int puFont::getStringDescender ()
{
#ifdef _PU_USE_GLUT_FONTS
  if ( glut_font_handle != (GlutFont) 0 )
  {
    if ( glut_font_handle == GLUT_BITMAP_8_BY_13        ) return 2 ;
    if ( glut_font_handle == GLUT_BITMAP_9_BY_15        ) return 3 ;
    if ( glut_font_handle == GLUT_BITMAP_TIMES_ROMAN_10 ) return 2 ;
    if ( glut_font_handle == GLUT_BITMAP_TIMES_ROMAN_24 ) return 5 ;
    if ( glut_font_handle == GLUT_BITMAP_HELVETICA_10   ) return 2 ;
    if ( glut_font_handle == GLUT_BITMAP_HELVETICA_12   ) return 3 ;
    if ( glut_font_handle == GLUT_BITMAP_HELVETICA_18   ) return 4 ;
  }
#endif // #ifdef _PU_USE_GLUT_FONTS

  if ( fnt_font_handle != NULL )
  {
    float b ;
    fnt_font_handle -> getBBox ( "y", pointsize, slant, NULL, NULL, &b, NULL ) ;
    return (int) -b ;
  }

  return 0 ;
}


void puFont::drawString ( const char *str, int x, int y )
{
  if ( str == NULL )
    return ;

#ifdef _PU_USE_GLUT_FONTS
  if ( glut_font_handle != (GlutFont) 0 )
  {
    glRasterPos2f( (float)x, (float)y ) ; 

    while ( *str != '\0' )
    {
      if (*str == '\n')
      {
        y -= getStringHeight() * 4 / 3 ;
    	glRasterPos2f( (float)x, (float)y ) ; 
      }
      else
        glutBitmapCharacter ( glut_font_handle, *str ) ;

      str++ ;
    }
    return ;
  }
#endif // #ifdef _PU_USE_GLUT_FONTS

  if ( fnt_font_handle != NULL )
  {
    sgVec3 curpos ;
    sgSetVec3 ( curpos, (float)x, (float)y, 0.0f ) ;

    glPushAttrib( GL_COLOR_BUFFER_BIT ); // NHV
      glEnable    ( GL_ALPHA_TEST   ) ;
      glEnable    ( GL_BLEND        ) ;
      glAlphaFunc ( GL_GREATER, 0.1f ) ;
      glBlendFunc ( GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA ) ;

      fnt_font_handle -> begin () ;
        fnt_font_handle -> puts ( curpos, pointsize, slant, str ) ;
      fnt_font_handle -> end () ;

    glPopAttrib () ;
    glDisable ( GL_TEXTURE_2D ) ;
    return ;
  }
}
