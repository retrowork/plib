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

// ssgLoaderWriterStuff.cxx
// Here you will find classes and functions you can use to 
// implement loaders and writers for ssg
// Also, there is the parser for loading ASCII files, which 
// has its own file ssgParser.cxx and there are functions like
// the stripifier that are usefull not only for loaders/writers.
//
// 1. Version written by Wolfram Kuss (Wolfram.Kuss@t-online.de) 
// in Nov of 2000
// Distributed with Steve Bakers plib under the LGPL licence

#include  "ssgLocal.h"
#include "ssgLoaderWriterStuff.h"

// need a prototype for alloca:
#if defined(__sgi) || defined(__MWERKS__)
#include <alloca.h>
#endif
#if defined(_MSC_VER)
#include <malloc.h>
#endif

#undef ABS
#undef MIN
#undef MAX
#define ABS(x) ((x) >= 0 ? (x) : -(x))
#define MIN(a,b) ((a) <= (b) ? (a) : (b))
#define MAX(a,b) ((a) >= (b) ? (a) : (b))
#define MIN3(a,b,c) ((a) <= (b) ? MIN(a,c) : MIN(b,c))
#define MAX3(a,b,c) ((a) >= (b) ? MAX(a,c) : MAX(b,c))

// texture coord epsilon
#define TC_EPSILON 0.01


sgVec4 currentDiffuse;

// ***********************************************************************
// ********************  small utility functions  ************************
// ***********************************************************************

void ssgAccumVerticesAndFaces( ssgEntity* node, sgMat4 transform, ssgVertexArray* vertices,
																ssgIndexArray*  indices, SGfloat epsilon, ssgSimpleStateArray* ssa,
																ssgIndexArray*  materialIndices, ssgTexCoordArray *texCoordArray) 
// Accumulates all vertices and Faces (indexes of vertices making up faces)
// from node and any node below.
// Calls itself recursively.
// If indices is NULL, no face info is accumulated
// if epsilon is < 0.0, it is ignored. Else vertices are only accumulated if 
// there is no vertex inside epsilon yet
{

	assert( vertices != NULL );
	assert( (epsilon < 0.0) || (indices == NULL) ); // sorry: using epsilon AND using indices not implemented
	if ( ssa != NULL ) { assert( indices != NULL ); }
	assert ( ((ssa==NULL) && (materialIndices==NULL)) || ((ssa!=NULL) && (materialIndices!=NULL)));
  if ( node->isAKindOf( ssgTypeTransform() ) ) {
    sgMat4 local_transform;
    ssgTransform *t_node = (ssgTransform*)node;
    
    t_node->getTransform(local_transform);
    sgPostMultMat4( local_transform, transform );

    for (ssgEntity* kid = t_node->getKid(0); kid != NULL; 
	 kid = t_node->getNextKid()) {
      ssgAccumVerticesAndFaces( kid, local_transform, vertices, indices, epsilon, ssa, materialIndices, texCoordArray);
    }
  } else if ( node->isAKindOf( ssgTypeBranch() ) ) {
    ssgBranch *b_node = (ssgBranch*)node;
    for (ssgEntity* kid = b_node->getKid(0); kid != NULL; 
	 kid = b_node->getNextKid()) {
      ssgAccumVerticesAndFaces( kid, transform, vertices, indices, epsilon, ssa, materialIndices, texCoordArray);
    }    
  } else if ( node->isAKindOf( ssgTypeLeaf() ) ) {
    ssgLeaf* l_node = (ssgLeaf*)node;
    int i, vert_low = vertices->getNum();
		
		int useTexture = FALSE;
		if ( texCoordArray )
			if ( l_node->getState() )
        if (l_node->getState()->isAKindOf(ssgTypeSimpleState())) 
				{ ssgSimpleState * ss = (ssgSimpleState *) l_node->getState();
					if ( ss->isEnabled ( GL_TEXTURE_2D ) )
// wk kludge!!!						if ( l_node -> getNumTexCoords () == l_node -> getNumVertices() )
							useTexture = TRUE;
				}
    
		for (i = 0; i < l_node->getNumVertices(); i++) {
      sgVec3 new_vertex;
      sgXformVec3(new_vertex, l_node->getVertex(i), transform);

			if ( epsilon < 0.0 )
			{
        vertices->add(new_vertex);
				if ( useTexture )
					texCoordArray ->add ( l_node->getTexCoord(i) );
				else if ( texCoordArray )
					texCoordArray ->add ( _ssgTexCoord00 );
			}
			else
			{ int j, bFound = FALSE, nv1 = vertices -> getNum ();
				for ( j = 0; j < nv1; j++)
				{	float *oldvertex = vertices -> get(j);
					if (( new_vertex[0] - oldvertex[0] > -epsilon) &&
							( new_vertex[0] - oldvertex[0] < epsilon) &&
							( new_vertex[1] - oldvertex[1] > -epsilon) &&
							( new_vertex[1] - oldvertex[1] < epsilon) &&
							( new_vertex[2] - oldvertex[2] > -epsilon) &&
							( new_vertex[2] - oldvertex[2] < epsilon))
					{ float *f;
					  if ( useTexture )
						{	assert( texCoordArray ); // if texCoordArray would be NULL, useTexture would not be set.
							f = texCoordArray -> get(j);
						}
						if ( !useTexture || ((l_node->getTexCoord(i)[0] == f[0]) &&
							         (l_node->getTexCoord(i)[1] == f[1])))
						{
							bFound = TRUE;
							break;
						}
					}
				}
				if ( ! bFound )
				{
					vertices -> add ( new_vertex );
					if ( useTexture )
						texCoordArray ->add ( l_node->getTexCoord(i) );
					else if ( texCoordArray )
						texCoordArray ->add ( _ssgTexCoord00 );
				}
			}
    }

		if ( indices != NULL )
		{ int index=-1;
			if ( ssa != NULL )
			{ 
				ssgState *s = l_node->getState();
				if ( s != NULL )
				{ index = ssa->findIndex (reinterpret_cast <class ssgSimpleState *> (s) );
				  if ( index < 0 )
					{ ssa -> add(reinterpret_cast <class ssgSimpleState *> (s) );
					  index = ssa->getNum()-1;
					}
				}
			}
			for (i = 0; i < l_node->getNumTriangles(); i++) {
				short v1, v2, v3;
				l_node->getTriangle(i, &v1, &v2, &v3);
				indices->add( vert_low + v1 );
				indices->add( vert_low + v2 );
				indices->add( vert_low + v3 );
				if ( materialIndices != NULL )
					materialIndices->add(index); // index is -1 for leafs without state
			}
		}
	}
	if ( texCoordArray )
	{	assert( vertices->getNum() == texCoordArray->getNum() );
	}
} ;










void ssgFindOptConvertTexture( char * filepath, char * tfname ) 
// Find and optionally (= if necessary) convert texture
{
	char tmp[1024], *extension ;

	strcpy( tmp, tfname);
	extension = strrchr(tmp, '.');
	if ( extension == NULL )
	{ strcpy( filepath, tfname );
	  return ;
	}
	extension[1] = 'r';
	extension[2] = 'g';
	extension[3] = 'b';
	extension[4] = 0;

	ssgGetCurrentOptions () -> makeTexturePath ( filepath, tmp ) ;
	if ( ulFileExists ( filepath ) )
		return; // found *.rgb-file

	// look for original, non-rgb - file
	ssgGetCurrentOptions () -> makeTexturePath ( filepath, tfname ) ;
	if ( !ulFileExists ( filepath ) )
		return; // found *.rgb nor original file

	// found original file. convert it.
	strcpy( tmp, filepath );

	extension = strrchr(tmp, '.');
	if ( extension == NULL )
	{ strcpy( filepath, tfname );
	  return ;
	}
	extension[1] = 'r';
	extension[2] = 'g';
	extension[3] = 'b';
	extension[4] = 0;

#ifdef WIN32
        char command [ 1024 ] ;
	sprintf(command, "convert -verbose %s sgi:%s", filepath, tmp);
	unsigned int ui = WinExec(command, SW_HIDE );	
	if ( ui < 32 )
		ulSetError(UL_WARNING, "Couldn't convert texture. Did you install ImageMagick?");
#else
  ulSetError(UL_WARNING, "Converting textures not yet implemented. Please convert %s manually.",
		    filepath);
	//sprintf(command, "-verbose %s sgi:%s", filepath, tmp);
	//execlp ( "convert", "convert",  command, NULL ) ;

#endif
	// Kludge; warning? NIV135
	strcpy( filepath, tmp );
}


/*
  ssgTriangulate - triangulate a simple polygon.
*/

static int triangulate_concave(sgVec3 *coords, int *w, int n, int x, int y, int *tris)
{
   struct Vtx {
      int index;
      float x, y;
      Vtx *next;
   };

   Vtx *p0, *p1, *p2, *m0, *m1, *m2, *t;
   int i, chk, num_tris;
   float a0, a1, a2, b0, b1, b2, c0, c1, c2;

   /* construct a circular linked list of the vertices */
   p0 = (Vtx *) alloca(sizeof(Vtx));
   p0->index = w ? w[0] : 0;
   p0->x = coords[p0->index][x];
   p0->y = coords[p0->index][y];
   p1 = p0;
   p2 = 0;
   for (i = 1; i < n; i++) {
      p2 = (Vtx *) alloca(sizeof(Vtx));
      p2->index = w ? w[i] : i;
      p2->x = coords[p2->index][x];
      p2->y = coords[p2->index][y];
      p1->next = p2;
      p1 = p2;
   }
   p2->next = p0;

   m0 = p0;
   m1 = p1 = p0->next;
   m2 = p2 = p1->next;
   chk = 0;
   num_tris = 0;

   while (p0 != p2->next) {
      if (chk && m0 == p0 && m1 == p1 && m2 == p2) {
	 /* no suitable vertex found.. */
         ulSetError(UL_WARNING, "ssgTriangulate: Self-intersecting polygon.");	 
         return 0;
      }
      chk = 1;

      a0 = p1->y - p2->y;
      a1 = p2->y - p0->y;
      a2 = p0->y - p1->y;
      b0 = p2->x - p1->x;
      b1 = p0->x - p2->x;
      b2 = p1->x - p0->x;
      
      if (b0 * a2 - b2 * a0 < 0) {
	 /* current angle is concave */
         p0 = p1;
         p1 = p2;
         p2 = p2->next;
      }
      else {
	 /* current angle is convex */
         float xmin = MIN3(p0->x, p1->x, p2->x);
         float xmax = MAX3(p0->x, p1->x, p2->x);
         float ymin = MIN3(p0->y, p1->y, p2->y);
         float ymax = MAX3(p0->y, p1->y, p2->y);
         
	 c0 = p1->x * p2->y - p2->x * p1->y;
	 c1 = p2->x * p0->y - p0->x * p2->y;
	 c2 = p0->x * p1->y - p1->x * p0->y;

         for (t = p2->next; t != p0; t = t->next) {
	    /* see if the triangle contains this vertex */
            if (xmin <= t->x && t->x <= xmax && 
                ymin <= t->y && t->y <= ymax &&
		a0 * t->x + b0 * t->y + c0 > 0 &&
		a1 * t->x + b1 * t->y + c1 > 0 &&		   
		a2 * t->x + b2 * t->y + c2 > 0)
	       break;
	 }

         if (t != p0) {
            p0 = p1;
            p1 = p2;
            p2 = p2->next;
         }
         else {
	    /* extract this triangle */
	    tris[3 * num_tris + 0] = p0->index;
	    tris[3 * num_tris + 1] = p1->index;
	    tris[3 * num_tris + 2] = p2->index;
	    num_tris++;
            
	    p0->next = p1 = p2;
	    p2 = p2->next;
            
            m0 = p0;
            m1 = p1;
            m2 = p2;
            chk = 0;
         }
      }
   }

   tris[3 * num_tris + 0] = p0->index;
   tris[3 * num_tris + 1] = p1->index;
   tris[3 * num_tris + 2] = p2->index;
   num_tris++;

   return num_tris;
}

int _ssgTriangulate(sgVec3 *coords, int *w, int n, int *tris)
{
   float *a, *b;
   int i, x, y;

   /* trivial case */
   if (n <= 3) {
      if (n == 3) {
	 tris[0] = w ? w[0] : 0;
	 tris[1] = w ? w[1] : 1;
	 tris[2] = w ? w[2] : 2;
	 return 1;
      }
      ulSetError(UL_WARNING, "ssgTriangulate: Invalid number of vertices (%d).", n);
      return 0;
   }

   /* compute areas */
   {
      float s[3], t[3];
      int swap;

      s[0] = s[1] = s[2] = 0;
      b = coords[w ? w[n - 1] : n - 1];

      for (i = 0; i < n; i++) {
	 a = b;
	 b = coords[w ? w[i] : i];
	 s[0] += a[1] * b[2] - a[2] * b[1];
	 s[1] += a[2] * b[0] - a[0] * b[2];
	 s[2] += a[0] * b[1] - a[1] * b[0];
      }
   
      /* select largest area */
      t[0] = ABS(s[0]);
      t[1] = ABS(s[1]);
      t[2] = ABS(s[2]);
      i = t[0] > t[1] ? t[0] > t[2] ? 0 : 2 : t[1] > t[2] ? 1 : 2;
      swap = (s[i] < 0); /* swap coordinates if clockwise */
      x = (i + 1 + swap) % 3;
      y = (i + 2 - swap) % 3;
   }

   /* concave check */
   {
      float x0, y0, x1, y1;

      a = coords[w ? w[n - 2] : n - 2];
      b = coords[w ? w[n - 1] : n - 1];
      x1 = b[x] - a[x];
      y1 = b[y] - a[y];

      for (i = 0; i < n; i++) {
	 a = b;
	 b = coords[w ? w[i] : i];
	 x0 = x1;
	 y0 = y1;
	 x1 = b[x] - a[x];
	 y1 = b[y] - a[y];
	 if (x0 * y1 - x1 * y0 < 0)
	    return triangulate_concave(coords, w, n, x, y, tris);
      }
   }

   /* convert to triangles */
   {
      int v0 = 0, v1 = 1, v = n - 1; 
      int even = 1;
      for (i = 0; i < n - 2; i++) {
	 if (even) {
	    tris[3 * i + 0] = w ? w[v0] : v0;
	    tris[3 * i + 1] = w ? w[v1] : v1;
	    tris[3 * i + 2] = w ? w[v] : v;
	    v0 = v1;
	    v1 = v;
	    v = v0 + 1;
	 }
	 else {
	    tris[3 * i + 0] = w ? w[v1] : v1;
	    tris[3 * i + 1] = w ? w[v0] : v0;
	    tris[3 * i + 2] = w ? w[v] : v;
	    v0 = v1;
	    v1 = v;
	    v = v0 - 1;
	 }
	 even = !even;
      }
   }
   return n - 2;
}



// ***********************************************************************
// ******************** class ssgLoaderWriterMesh ************************
// ***********************************************************************
 


void ssgLoaderWriterMesh::ReInit(void)
{
  theVertices					= NULL ; 
	materialIndexes			= NULL ; 
	theFaces						= NULL ; 
	tCPFAV							= NULL ; 
	theMaterials				= NULL ;
	tCPV								= NULL ;
	bTCs_are_per_vertex	= TRUE ;
}
	
ssgLoaderWriterMesh::ssgLoaderWriterMesh()
{ ReInit();
}

ssgLoaderWriterMesh::~ssgLoaderWriterMesh()
{}

void ssgLoaderWriterMesh::deleteTCPFAV()
{}

// creation stuff:

void ssgLoaderWriterMesh::ThereAreNVertices( int n ) 
{
	assert( theVertices == NULL );
	theVertices = new ssgVertexArray ( n );
}

void ssgLoaderWriterMesh::addVertex ( sgVec3 v ) 
{
	assert(theVertices!=NULL);
	theVertices->add ( v );
}


void ssgLoaderWriterMesh::ThereAreNFaces( int n ) 
{
	assert( theFaces == NULL );
	theFaces = new ssgListOfLists ( n );
}

void ssgLoaderWriterMesh::addFace ( ssgIndexArray **ia ) 
{
	assert(theFaces!=NULL);
	theFaces->add ( (ssgSimpleList **)ia );
}


void ssgLoaderWriterMesh::ThereAreNTCPFAV( int n ) 
{
	assert( tCPFAV == NULL );
	tCPFAV = new ssgListOfLists ( n );
}

void ssgLoaderWriterMesh::addTCPFAV ( ssgTexCoordArray **tca ) 
{
	assert(tCPFAV!=NULL);
	tCPFAV->add ( (ssgSimpleList **)tca );
}

void ssgLoaderWriterMesh::ThereAreNTCPV( int n ) 
{
	assert( tCPV == NULL );
	tCPV = new ssgTexCoordArray ( n );
}

void ssgLoaderWriterMesh::addTCPV ( sgVec2 tc ) 
{
	assert(tCPV!=NULL);
	tCPV->add ( tc );
}



void ssgLoaderWriterMesh::ThereAreNMaterialIndexes( int n ) 
{
	assert( materialIndexes == NULL );
	materialIndexes = new ssgIndexArray ( n );
}

void ssgLoaderWriterMesh::addMaterialIndex ( short mi ) 
{
	assert(materialIndexes!=NULL);
	materialIndexes->add ( mi );
}


void ssgLoaderWriterMesh::ThereAreNMaterials( int n ) 
{
	assert( theMaterials == NULL );
	theMaterials = new ssgSimpleStateList ( n );
}

void ssgLoaderWriterMesh::addMaterial ( class ssgSimpleState **ss ) 
{
	assert(theMaterials!=NULL);
	theMaterials->add ( ss );
}

static void recalcNormals( ssgIndexArray* il, ssgVertexArray* vl, ssgNormalArray *nl ) 
{
//  DEBUGPRINT( "Calculating normals." << std::endl);
  sgVec3 v1, v2, n;

  for (int i = 0; i < il->getNum() / 3; i++) {
    short ix0 = *il->get(i*3    );
    short ix1 = *il->get(i*3 + 1);
    short ix2 = *il->get(i*3 + 2);

    sgSubVec3(v1, vl->get(ix1), vl->get(ix0));
    sgSubVec3(v2, vl->get(ix2), vl->get(ix0));
    
    sgVectorProductVec3(n, v1, v2);
		SGfloat f= static_cast<SGfloat> (sqrt(n[0]*n[0]+n[1]*n[1]+n[2]*n[2]));
		if (f>0.00001)
		{ f=1.0F/f;
		  n[0] *= f; n[1] *= f; n[2] *= f;
			assert(((n[0]*n[0]+n[1]*n[1]+n[2]*n[2])-1)<0.001);
			assert(((n[0]*n[0]+n[1]*n[1]+n[2]*n[2])-1)>-0.001);
		}
		sgCopyVec3( nl->get(ix0), n );
    sgCopyVec3( nl->get(ix1), n );
    sgCopyVec3( nl->get(ix2), n );
  }
}

void ssgLoaderWriterMesh::AddFaceFromCArray(int nNoOfVerticesForThisFace, 
																						int *vertices)
{
	int j;
	class ssgIndexArray *oneFace = new ssgIndexArray( nNoOfVerticesForThisFace ); 
	oneFace->ref();
	for(j=0;j<nNoOfVerticesForThisFace;j++)
		oneFace->add(vertices[j]);
	assert(theFaces!=NULL);
	theFaces->add( (ssgSimpleList **) &oneFace ); 

}


void ssgLoaderWriterMesh::AddOneNode2SSGFromCPFAV(class ssgVertexArray *theVertices, 
						class ssgListOfLists *theTCPFAV,
						class ssgListOfLists *theFaces,
						class ssgSimpleState *currentState,// Pfusch, kludge. NIV135
						class ssgLoaderOptions* current_options,
						class ssgBranch *curr_branch_)

{ int i, j;

	assert(theVertices!=NULL);
	assert(theFaces!=NULL);
	// note: I am changing theVertices here, but that is allowed.
	class ssgTexCoordArray *tcArray = new ssgTexCoordArray(theVertices->getNum());
	sgVec2 unUsed;
	unUsed[0]=-99999; // FixMe: It would be nicer to have an extra array of booleans
	unUsed[1]=-99999;
	for(i=0;i<theVertices->getNum();i++)
		tcArray->add(unUsed); 
	for(i=0;i<theFaces->getNum();i++)
	{
		class ssgIndexArray *oneFace = *((class ssgIndexArray **) theFaces->get( i )); 
		class ssgTexCoordArray *textureCoordsForOneFace = *((ssgTexCoordArray **) theTCPFAV->get ( i ));
		if ( textureCoordsForOneFace  != NULL ) // It is allowed that some or even all faces are untextured.
		{
			for(j=0;j<oneFace->getNum();j++)
			{ short *ps = oneFace->get(j);
				float *newTC = textureCoordsForOneFace->get(j);
				float *oldTC = tcArray->get(*ps);
					
				assert( oldTC != NULL );
				if ((oldTC[0]==-99999) && (oldTC[1]==-99999)) // tc unused until now. Use it
				{ sgVec2 pv; // FixMe: mem leak?
					pv[0]=newTC[0];
					pv[1]=newTC[1];
					tcArray->set(pv, *ps);
				}
				else
				{ // can we simply use the "old" value?
					if ( TC_EPSILON < ABS ( newTC[0]-oldTC[0] ) +
								            ABS ( newTC[1]-oldTC[1] ))
					{ // NO, we can't. Duplicate vertex
						// not allowed: theVertices->add(theVertices->get(*ps)); // create duplicate 3D. FixMe: clone needed?
						float * f = theVertices->get(*ps);
						sgVec3 v;
						v[0] = f[0]; v[1] = f[1]; v[2] = f[2]; 
						theVertices->add(v);
						sgVec2 pv;
						pv[0]=newTC[0];
						pv[1]=newTC[1];
						tcArray->add(pv); // create duplicate 2D
						*ps=theVertices->getNum()-1;  // use duplicate
						assert ( *oneFace->get(j) == theVertices->getNum()-1);
					}
				}
			}
		}
	}
	AddOneNode2SSGFromCPV(theVertices, tcArray, theFaces, currentState, 
			            current_options, curr_branch_);
}

void ssgLoaderWriterMesh::AddOneNode2SSGFromCPV(class ssgVertexArray *theVertices, 
	class ssgTexCoordArray *theTC,
	class ssgListOfLists *theFaces,
	class ssgSimpleState *currentState,// kludge NIV135 
	class ssgLoaderOptions* current_options,
	class ssgBranch *curr_branch_)

{ int i, j;
		//start Normals, FixMe, kludge NIV135

	ssgNormalArray *nl = new ssgNormalArray(theVertices->getNum());
	sgVec3 kludge;
	for (i=0;i<theVertices->getNum();i++)
		nl->add(kludge); //currentMesh.vl->get(i));


	class ssgIndexArray* il = new ssgIndexArray ( theFaces->getNum() * 3 ) ; // there are MINIMAL n * 3 indexes

	for(i=0;i<theFaces->getNum();i++)
	{
		class ssgIndexArray *oneFace = *((class ssgIndexArray **) theFaces->get( i )); 
		if ( oneFace->getNum() >= 3 )
		{	for(j=0;j<oneFace->getNum();j++)
			{ 
				if (j<3)
					il->add(*oneFace->get(j));
				else // add a complete triangle
				{ il->add(*oneFace->get(0));
					il->add(*oneFace->get(j-1));
					il->add(*oneFace->get(j));
				}
			}
		}
	}
  recalcNormals(il,theVertices, nl); // Fixme, NIV14: only do this if there are no normals in the file
	
	ssgColourArray* cl = NULL ;
  
  if ( currentState -> isEnabled ( GL_LIGHTING ) )
  {
    if ( cl == NULL )
    {
      cl = new ssgColourArray ( 1 ) ;
      cl -> add ( currentDiffuse ) ;
    }
  }

	ssgVtxArray* leaf = new ssgVtxArray ( GL_TRIANGLES,
		theVertices, nl , theTC, cl, il ) ;
	leaf -> setCullFace ( TRUE ) ;
	leaf -> setState ( currentState ) ;

	ssgEntity *model=	current_options -> createLeaf ( leaf, NULL)  ;
	assert( model != NULL );
	curr_branch_->addKid(model);
}


void ssgLoaderWriterMesh::add2SSG(
		class ssgSimpleState *currentState,// FixMe, kludge. NIV135
		class ssgLoaderOptions* current_options,
		class ssgBranch *curr_branch_)
{ int i, j, k;
  unsigned short oldVertexIndex, newVertexIndex;
	class ssgIndexArray *thisFace;

  
#ifdef WRITE_MESH_TO_STDOUT	
	if ( theMaterials == NULL )
		fprintf(stdout, "( theMaterials == NULL )\n");
	else
	{	
		fprintf(stdout, "%d Materials:\n", theMaterials->getNum());
		for(i=0;i<theMaterials->getNum();i++)
		{ fprintf(stdout, "%ld\n", (long)theMaterials->get(i));
		}
	}
	if ( materialIndexes == NULL )
		fprintf(stdout, "( materialIndexes == NULL )\n");
	else
	{
		fprintf(stdout, "%d Material Indexes:\n", materialIndexes->getNum());
		for(i=0;i<materialIndexes->getNum();i++)
		{ short s=*(materialIndexes->get(i));
			fprintf(stdout, "%ld\n", (long)s);
		}
	}
#endif
	if ( theMaterials == NULL )
	{ if ( tCPFAV == NULL )
		  AddOneNode2SSGFromCPV(theVertices, tCPV /* may be NULL */, theFaces, currentState, current_options, curr_branch_);
		else
			AddOneNode2SSGFromCPFAV(theVertices, tCPFAV, theFaces, currentState, 
			                current_options, curr_branch_);
	}
	else
	{	assert(theVertices!=NULL);
		assert(theFaces!=NULL);
		// FixMe: What about faces without state? They should have material -1
		for(i=0;i<theMaterials->getNum();i++)
		{	
			// I often allocate too much; This is wastefull on memory, but fast since it never "resizes":
			class ssgVertexArray *newVertices = new ssgVertexArray ( theVertices->getNum() );
			class ssgListOfLists *newFaces = new ssgListOfLists ( theFaces->getNum() );
			class ssgIndexArray *oldVertexInd2NewVertexInd = new ssgIndexArray ( theVertices->getNum() );
		  class ssgListOfLists *newTCPFAV = NULL;
			class ssgTexCoordArray *newTCPV = NULL;
	
			if ( tCPFAV != NULL )
				newTCPFAV = new ssgListOfLists();
			if ( tCPV != NULL )
				newTCPV = new ssgTexCoordArray();

			
			for (j=0;j<theVertices->getNum();j++)
				oldVertexInd2NewVertexInd->add ( short(0xFFFF) ); // 0xFFFF stands for "unused in new Mesh"

			// Go through all the old Faces, look for the correct material and copy those
			// faces and indexes into the new
			// FixMe, 2do, NIV135: if the Materials just differ through the colour, one would not need
			// several meshes, but could use the colour array. However, this is not possible,
			// if they differ by for example the texture
			assert(materialIndexes!=NULL);
			for (j=0;j<theFaces->getNum();j++)
				if ( i == *(materialIndexes->get(
					       // for *.x-files, there may be less materialIndexes than faces. I then simply repeat 
								 // the last index all the time:
				 	            j<materialIndexes->getNum() ? j : materialIndexes->getNum()-1 )) 
				 							)
				{ 
					// take this face
					thisFace = *((class ssgIndexArray **) theFaces->get( j )); 
					newFaces->add( (class ssgSimpleList **)&thisFace); 
					//thisFace = *((class ssgIndexArray **) newFaces->get( newFaces->getNum()-1 )); 
					
					if ( tCPFAV != NULL )
						newTCPFAV ->add ( tCPFAV -> get( j ) );
					 
					for(k=0;k<thisFace->getNum();k++)
					{ oldVertexIndex = * thisFace->get(k);
					  newVertexIndex = *oldVertexInd2NewVertexInd->get(oldVertexIndex);
					  if ( 0xFFFF == newVertexIndex )
						{ newVertexIndex = newVertices->getNum();
						  newVertices->add ( theVertices->get(oldVertexIndex) );
							if ( tCPV != NULL )
								newTCPV -> add( tCPV->get(oldVertexIndex) );
							oldVertexInd2NewVertexInd->set(newVertexIndex, oldVertexIndex);
						}
						// From here on the indexes in thisFace are only valid in relation to
						// newVertices and newTCPV. Since this face will not be used for any 
						// further material, this doesn't lead to problems.
				    thisFace->set(newVertexIndex, k);
					}	
				}
#ifdef WRITE_MESH_TO_STDOUT	
			fprintf(stdout, "NumVert: %d\n", newVertices->getNum());
			for(j=0;j<newVertices->getNum();j++)
			{ float *f=newVertices->get(j);
			  fprintf(stdout, "%f, %f, %f\n",f[0], f[1], f[2]);
			}
			for(j=0;j<newFaces->getNum();j++)
			{
				thisFace = *((class ssgIndexArray **) newFaces->get( j )); 	
				fprintf(stdout, "%d EP:", thisFace->getNum());
				for(k=0;k<thisFace->getNum();k++)
				{ oldVertexIndex = * thisFace->get(k);
				  fprintf(stdout, "%d, ", oldVertexIndex);
				}
				fprintf(stdout, "\n");
			}
#endif	
			if ( newFaces->getNum() > 0 )
			{
				currentState = *theMaterials->get(i);
				if ( tCPFAV == NULL )
					// FixMe: tCPV-indices are not compatible to newVertices-indices?!?
					AddOneNode2SSGFromCPV(newVertices, newTCPV /* may be NULL */, newFaces, currentState, current_options, curr_branch_);
				else
					AddOneNode2SSGFromCPFAV(newVertices, newTCPFAV, newFaces, currentState, 
			                current_options, curr_branch_);
			}
		}
	}
}
	
int ssgLoaderWriterMesh::checkMe()
// returns TRUE; if ok.
// Writes out errors by calling ulSetError with severity UL_WARNING,
// and a bit of debug info as UL_DEBUG
// May stop on first error.

// FixMe; todo: tCPV and tCPFAV. NIV135
{ int i, oneIndex;
  class ssgIndexArray * vertexIndsForOneFace;
	class ssgTexCoordArray * textureCoordsForOneFace;

  // **** check theVertices *****
	if ( theVertices == NULL )
	{ if (( materialIndexes == NULL ) &&
				(theFaces == NULL ) &&
				( tCPFAV == NULL ))
		{	ulSetError( UL_DEBUG, "LoaderWriterMesh::checkMe(): The mesh is empty\n");
			return TRUE;
		}
		else
		{	ulSetError( UL_WARNING, "LoaderWriterMesh::checkMe(): No theVertices is NULL; but not the rest!\n");
			return FALSE;
		}

	}
	// **** check materialIndexes and theMaterials *****
	/* FixMe; kludge: 2do. NIV135
	// one index per face:
	class ssgIndexArray *materialIndexes; 

	theMaterials
	*/
	if ((( theMaterials == NULL ) && ( materialIndexes != NULL )) ||
		  (( theMaterials != NULL ) && ( materialIndexes == NULL )))
	{	ulSetError( UL_WARNING, "LoaderWriterMesh::checkMe(): "
	                     "One of theMaterials and materialIndexes was NULL and the other != NULL!\n");
		return FALSE;
	}
	if ( materialIndexes != NULL ) 
	{ for (i=0;i<materialIndexes->getNum();i++)
		{ oneIndex = *materialIndexes->get(i);
			assert(theMaterials!=NULL);
	    if (( oneIndex < 0 ) || ( oneIndex >= theMaterials->getNum()))
			{	ulSetError( UL_WARNING, "LoaderWriterMesh::checkMe(): "
													 "Material index out of range. Index = %d, "
													 "theMaterials->getNum() = %d.\n",
													 oneIndex, theMaterials->getNum());
				return FALSE;
			}
		}
	}


	// **** check theFaces *****
	// Each sublist is of type ssgIndexArray and contains the indexes of the vertices:
	if ( theFaces == NULL )
	{	ulSetError( UL_WARNING, "LoaderWriterMesh::checkMe(): There are vertices but no faces.\n");
		return FALSE;
	}
	for(i=0;i<theFaces->getNum();i++)
	{ vertexIndsForOneFace = *((ssgIndexArray **) theFaces->get ( i ));
	  if ( vertexIndsForOneFace == NULL )
		{	ulSetError( UL_WARNING, "LoaderWriterMesh::checkMe(): the vertexindexes for one face are NULL!\n");
			return FALSE;
		}
	}
	// **** check textureCoordinates *****
  // Each sublist is of type ssgTexCoordArray and contains the texture coordinates
	if ( tCPFAV != NULL ) // may be NULL
	{ if ( theFaces->getNum() != tCPFAV->getNum())
		{	ulSetError( UL_WARNING, "LoaderWriterMesh::checkMe(): "
		              "There must be as many faces in theFaces as in textureCoordinates. But "
		              "theFaces->getNum() =%d, tCPFAV->getNum() = %d!\n",
									theFaces->getNum(), tCPFAV->getNum());
			return FALSE;
		}
		for(i=0;i<tCPFAV->getNum();i++)
		{ textureCoordsForOneFace = *((ssgTexCoordArray **) tCPFAV->get ( i ));
			if ( textureCoordsForOneFace  != NULL ) // It is allowed that some or even all faces are untextured.
			{ vertexIndsForOneFace = *((ssgIndexArray **) theFaces->get ( i ));
			  if ( textureCoordsForOneFace->getNum() != vertexIndsForOneFace ->getNum())
				{	ulSetError( UL_WARNING, "LoaderWriterMesh::checkMe(): Face %d: "
											"Each face must have as many texture corrdinates (or none) as vertices. But "
											"textureCoordsForOneFace->getNum() =%d, vertexIndsForOneFace ->getNum() = %d!\n",
											i, textureCoordsForOneFace->getNum(), vertexIndsForOneFace ->getNum());
					return FALSE;
				}
			}
		}
	}
	return TRUE; // success
}
