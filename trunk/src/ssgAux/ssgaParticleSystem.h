
class ssgaParticle
{
public:

  sgVec4 col ;
  sgVec3 pos ;
  sgVec3 vel ;
  sgVec3 acc ;

  float time_to_live ;
  void *userData ;

  void update ( float dt )
  {
    sgAddScaledVec3 ( vel, acc, dt ) ;
    sgAddScaledVec3 ( pos, vel, dt ) ;
    time_to_live -= dt ;
  }

  ssgaParticle ()
  {
    sgSetVec4 ( col, 1, 1, 1, 1 ) ;
    sgZeroVec3 ( pos ) ;
    sgZeroVec3 ( vel ) ;
    sgZeroVec3 ( acc ) ;
    time_to_live = 0 ;
    userData = NULL ;
  } 

} ;


class ssgaParticleSystem ;


typedef void (* ssgaParticleCreateFunc) ( ssgaParticleSystem *ps,
                                          int index,
                                          ssgaParticle *p ) ;

typedef void (* ssgaParticleUpdateFunc) ( float deltaTime,
                                          ssgaParticleSystem *ps,
                                          int index,
                                          ssgaParticle *p ) ;

typedef void (* ssgaParticleDeleteFunc) ( ssgaParticleSystem *ps,
                                          int index,
                                          ssgaParticle *p ) ;

class ssgaParticleSystem : public ssgVtxArray
{
  int num_particles  ;
  int num_verts      ;
  int turn_to_face   ;
  int num_active     ;
  ssgaParticle *particle ;

  float create_error ;
  float create_rate ;

  float size ;

  ssgaParticleCreateFunc particle_create ;
  ssgaParticleUpdateFunc particle_update ;
  ssgaParticleDeleteFunc particle_delete ;

public:

  ssgaParticleSystem ( int num, int initial_num,
                       float _create_rate, int _turn_to_face,
                       float sz, float bsphere_size,
                       ssgaParticleCreateFunc _particle_create,
                       ssgaParticleUpdateFunc _particle_update = NULL,
                       ssgaParticleDeleteFunc _particle_delete = NULL ) ;

  virtual ~ssgaParticleSystem () ;

  void setSize ( float sz ) { size = sz ; }
  float getSize () { return size ; }

  void update ( float t ) ;
  void draw_geometry () ;

  int getNumActiveParticles () { return num_active ; }

} ;

