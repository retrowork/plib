#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <string.h>
#ifdef WIN32
#include <windows.h>
#else
#include <unistd.h>
#endif
#include <math.h>
#include <plib/ssg.h>
#include <plib/ssgAux.h>
#include <GL/glut.h>

#define GUI_BASE      80
#define VIEW_GUI_BASE 20
#define FONT_COLOUR   1,1,1,1

ssgRoot            *scene        = NULL ;
ssgaWaveSystem     *ocean        = NULL ;
ssgSimpleState     *sea_state    = NULL ;
ssgSimpleState     *cube_state   = NULL ;

ssgaWaveTrain trains [ 3 ] ;

ssgTransform   *cube     [ 10 ] ;
sgParticle     *particle [ 10 ] ;
sgSpringDamper *spring   [ 20 ] ;
int num_particles ;
int num_springs ;

void initSMD ()
{
  particle [ 0 ] = new sgParticle ( 1.0f,  2.0f, -4.0f, 4.0f ) ;
  particle [ 1 ] = new sgParticle ( 1.0f,  2.4f,  0.0f, 6.0f ) ;
  particle [ 2 ] = new sgParticle ( 1.0f,  2.0f,  4.0f, 4.0f ) ;
  particle [ 3 ] = new sgParticle ( 1.0f,  4.0f, -4.0f, 0.0f ) ;
  particle [ 4 ] = new sgParticle ( 1.0f,  0.0f, -4.0f, 0.0f ) ;
  particle [ 5 ] = new sgParticle ( 1.0f,  4.0f,  4.0f, 0.0f ) ;
  particle [ 6 ] = new sgParticle ( 1.0f,  0.0f,  4.0f, 0.0f ) ;

  num_particles = 7 ;

  int s=0;

  spring [s++] = new sgSpringDamper ( particle[0], particle[1], 50.0f, 1.0f ) ;
  spring [s++] = new sgSpringDamper ( particle[0], particle[2], 50.0f, 1.0f ) ;
  spring [s++] = new sgSpringDamper ( particle[0], particle[3], 50.0f, 1.0f ) ;
  spring [s++] = new sgSpringDamper ( particle[0], particle[4], 50.0f, 1.0f ) ;
  spring [s++] = new sgSpringDamper ( particle[0], particle[5], 50.0f, 1.0f ) ;
  spring [s++] = new sgSpringDamper ( particle[1], particle[2], 50.0f, 1.0f ) ;
  spring [s++] = new sgSpringDamper ( particle[2], particle[3], 50.0f, 1.0f ) ;
  spring [s++] = new sgSpringDamper ( particle[2], particle[5], 50.0f, 1.0f ) ;
  spring [s++] = new sgSpringDamper ( particle[2], particle[6], 50.0f, 1.0f ) ;
  spring [s++] = new sgSpringDamper ( particle[3], particle[4], 50.0f, 1.0f ) ;
  spring [s++] = new sgSpringDamper ( particle[3], particle[5], 50.0f, 1.0f ) ;
  spring [s++] = new sgSpringDamper ( particle[4], particle[6], 50.0f, 1.0f ) ;
  spring [s++] = new sgSpringDamper ( particle[5], particle[6], 50.0f, 1.0f ) ;
  spring [s++] = new sgSpringDamper ( particle[3], particle[6], 50.0f, 1.0f ) ;
  spring [s++] = new sgSpringDamper ( particle[4], particle[5], 50.0f, 1.0f ) ;

  spring [s++] = new sgSpringDamper ( particle[2], particle[4], 50.0f, 1.0f ) ;
  spring [s++] = new sgSpringDamper ( particle[0], particle[6], 50.0f, 1.0f ) ;

  num_springs = s ;
  
  for ( int i = 0 ; i < num_particles ; i++ )
  {
    ssgaCube *cb = new ssgaCube () ;
    cb -> setKidState ( cube_state ) ;
    cb -> regenerate () ;

    cube [ i ] = new ssgTransform () ;
    cube [ i ] -> addKid ( cb ) ;

    scene -> addKid ( cube[i] ) ;
  }
}

void updateSMD ( float dt )
{
  for ( int i = 0 ; i < num_particles ; i++ )
  {
    sgVec3 friction ;

    sgScaleVec3 ( friction, particle[i]->getVel (), 0.1 ) ;

    if ( particle [ i ] -> getPos()[2] <= 0.0f )
      particle [ i ] -> zeroForce () ;
    else
      particle [ i ] -> gravityOnly () ;

    particle [ i ] -> subForce ( friction ) ;
  }

  for ( int i = 0 ; i < num_springs ; i++ )
    spring [ i ] -> update () ;

  for ( int i = 0 ; i < num_particles ; i++ )
  {
    if ( particle [ i ] -> getPos()[2] <= 0.0f )
    {
      particle [ i ] -> getPos  ()[2] = 0.0f ;
      particle [ i ] -> getVel  ()[2] = 0.0f ;
    }

    particle [ i ] -> update ( dt ) ;
  }

  for ( int i = 0 ; i < num_particles ; i++ )
    cube [ i ] -> setTransform ( particle[i] -> getPos () ) ;
}


sgCoord campos = { { 0, -20, 8 }, { 0, -20, 0 } } ;

void update_motion ( int frameno )
{
  ssgSetCamera ( & campos ) ;

  ocean -> setWindDirn ( 25.0 * sin ( frameno / 10.0 ) ) ;

  static ulClock ck ; ck . update () ;

  float t  = ck . getAbsTime   () ;
  float dt = ck . getDeltaTime () ;

  if ( dt > 0.05f ) dt = 0.05f ;

  ocean -> updateAnimation ( t ) ;

  updateSMD ( dt ) ;
}



/*
  The GLUT window reshape event
*/

static void reshape ( int w, int h )
{
  glViewport ( 0, 0, w, h ) ;
}



/*
  The GLUT keyboard/mouse events
*/


static void keyboard ( unsigned char key, int, int )
{
  switch ( key )
  {
    case 0x03 : exit ( 0 ) ;
  }
}


static void specialfn ( int key, int x, int y )
{
}


static void motionfn ( int x, int y )
{
}

static void mousefn ( int button, int updown, int x, int y )
{
}



/*
  The GLUT redraw event
*/

void redraw ()
{
static int frameno = 0 ;
frameno++ ;

  update_motion ( frameno % 2000 ) ;

  glClear  ( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT ) ;

  ssgCullAndDraw ( scene ) ;

  glPolygonMode ( GL_FRONT_AND_BACK, GL_FILL ) ;

  glutPostRedisplay () ;
  glutSwapBuffers () ;
}



void init_graphics ()
{
  int   fake_argc = 1 ;
  char *fake_argv[3] ;
  fake_argv[0] = "ssgExample" ;
  fake_argv[1] = "Simple Scene Graph : Example Program." ;
  fake_argv[2] = NULL ;

  /*
    Initialise GLUT
  */

  glutInitWindowPosition ( 0, 0 ) ;
  glutInitWindowSize     ( 640, 480 ) ;
  glutInit               ( &fake_argc, fake_argv ) ;
  glutInitDisplayMode    ( GLUT_RGB | GLUT_DOUBLE | GLUT_DEPTH ) ;
  glutCreateWindow       ( fake_argv[1] ) ;
  glutDisplayFunc        ( redraw   ) ;
  glutReshapeFunc        ( reshape  ) ;
  glutKeyboardFunc       ( keyboard ) ;
  glutSpecialFunc        ( specialfn ) ;
  glutMouseFunc          ( mousefn   ) ;
  glutMotionFunc         ( motionfn  ) ;
  glutPassiveMotionFunc  ( motionfn  ) ;

  ssgInit () ;

  /*
    Some basic OpenGL setup
  */

  glClearColor ( 0.2f, 0.7f, 1.0f, 1.0f ) ;
  glEnable ( GL_DEPTH_TEST ) ;

  /*
    Set up the viewing parameters
  */

  ssgSetFOV     ( 60.0f, 0.0f ) ;
  ssgSetNearFar ( 1.0f, 700.0f ) ;

  /*
    Set up the Sun.
  */

  sgVec3 sunposn ;
  sgSetVec3 ( sunposn, 0.1f, -1.0f, 0.1f ) ;
  ssgGetLight ( 0 ) -> setPosition ( sunposn ) ;
}


void init_states ()
{
  cube_state = new ssgSimpleState () ;
  cube_state -> setTexture        ( "data/mg.rgb" ) ;
  cube_state -> enable            ( GL_TEXTURE_2D ) ;
  cube_state -> setShadeModel     ( GL_SMOOTH ) ;
  cube_state -> enable            ( GL_CULL_FACE ) ;
  cube_state -> enable            ( GL_BLEND ) ;
  cube_state -> enable            ( GL_LIGHTING ) ;
  cube_state -> setColourMaterial ( GL_AMBIENT_AND_DIFFUSE ) ;
  cube_state -> setMaterial       ( GL_EMISSION, 0, 0, 0, 1 ) ;
  cube_state -> setMaterial       ( GL_SPECULAR, 1, 1, 1, 1 ) ;
  cube_state -> setShininess      ( 2 ) ;

  sea_state = new ssgSimpleState () ;
  sea_state -> setTexture        ( "data/bw.rgb" ) ;
  sea_state -> enable            ( GL_TEXTURE_2D ) ;
  sea_state -> setShadeModel     ( GL_SMOOTH ) ;
  sea_state -> enable            ( GL_CULL_FACE ) ;
  sea_state -> disable           ( GL_BLEND ) ;
  sea_state -> enable            ( GL_LIGHTING ) ;
  sea_state -> setColourMaterial ( GL_AMBIENT_AND_DIFFUSE ) ;
  sea_state -> setMaterial       ( GL_EMISSION, 0, 0, 0, 1 ) ;
  sea_state -> setMaterial       ( GL_SPECULAR, 1, 1, 1, 1 ) ;
  sea_state -> setShininess      (  5 ) ;
}



void load_database ()
{
  /* Set up the path to the data files */

  ssgModelPath   ( "data" ) ;
  ssgTexturePath ( "data" ) ;

  /* Load the states */

  init_states () ;

  sgVec4  WHITE  = { 1.0, 1.0, 1.0, 1.0 } ;

  /* Set up some interesting defaults. */

  trains[0] . setSpeed      (  3.1f ) ;
  trains[0] . setLength     (  0.8f ) ;
  trains[0] . setLambda     (  0.6f ) ;
  trains[0] . setHeading    ( 47.0f ) ;
  trains[0] . setWaveHeight (  0.2f ) ;

  trains[1] . setSpeed      (  4.6f ) ;
  trains[1] . setLength     (  0.8f ) ;
  trains[1] . setLambda     (  1.0f ) ;
  trains[1] . setHeading    ( 36.0f ) ;
  trains[1] . setWaveHeight (  0.1f ) ;

  trains[2] . setSpeed      (  8.5f ) ;
  trains[2] . setLength     (  0.6f ) ;
  trains[2] . setLambda     (  1.0f ) ;
  trains[2] . setHeading    ( 65.0f ) ;
  trains[2] . setWaveHeight (  0.1f ) ;

  ocean   =  new ssgaWaveSystem ( 10000 ) ;
  ocean   -> setColour        ( WHITE ) ;
  ocean   -> setSize          ( 100 ) ;
  ocean   -> setTexScale      ( 3, 3 ) ;
  ocean   -> setKidState      ( sea_state ) ;
  ocean   -> setWindSpeed     ( 10.0f ) ;
  ocean   -> setWaveTrain     ( 0, & trains[0] ) ;
  ocean   -> setWaveTrain     ( 1, & trains[1] ) ;
  ocean   -> setWaveTrain     ( 2, & trains[2] ) ;
  ocean   -> regenerate       () ;

  /* Build the scene graph */

  scene    =  new ssgRoot ;
  scene    -> addKid          ( ocean    ) ;

  initSMD () ;
}


/*
  The works.
*/

int main ( int, char ** )
{
  init_graphics () ;
  load_database () ;
  glutMainLoop  () ;
  return 0 ;
}



