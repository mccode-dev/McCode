/**
\mainpage
Simple Meta-Conic Neutron Raytracer is a framework for raytracing geometries of the form: @f$ r^2=k_1 + k_2 z + k_3 z^2 @f$. 

<h3>General Notes</h3>
To use the software you must make a Scene element using the function makeScene(). You must then add items to this scene element using the various add function (addDisk(), addParaboloid(), etc...). Next you must call the function traceSingleNeutron() for every neutron you would like to trace through the geometry. The maximum number of each geometry you can place in a scene is defined by the MAX_CONICSURF, MAX_DISK and MAX_DETECTOR definitions in the conic.h file.

<h3>TODO</h3>

@todo  
       Name variable for each component <br/>
       Normalize Detector Events by weight of neutron <br/>

<h3>Known Bugs</h3>
@bug  HPPH works incorrectly for M != 1 <br/>
      Neutrons t=1e-11 away from a surface will pass through <br/>

<h3>Note on Pointers</h3>
This framework uses pointers extensivly. It is important to be familiar with how to use pointers.
<br/>Here is a quick overview.

@code
//Making an Element
ConicSurf c = makeParaboloid(...);
int k = 10;

//Making a Pointer from an Element
ConicSurf * c_pointer = &c;            
int * k_pointer = &k;                 
 
//Making an Element from a Pointer
ConicSurf c2 = *c_pointer;             
int ten = *k_pointer;
 
//Getting Item in Element
double k1 = c.k1;

//Getting Item in Element from a Pointer
double k1 = c_pointer->k1;                 

//Functions that have pointers as parameters can modify the element being pointed to.
//Functions that have elements as parameters can not modify the value of the element.
@endcode

<h3>Stand Alone Example Code</h3>
This framework can be used entirely by itself as the following example demonstrates.
@code
/////////////////////////////////////////////////////////////////////////////////
// Giacomo Resta <gresta@mit.edu>
//
// Basic standalone exmaple of a raytracer. It is advised to modify
// the getRandom() function to use a better random number generator (i.e. glib). 
// The systems random generator was used to preserve clarity and conciseness. 
/////////////////////////////////////////////////////////////////////////////////

#include <math.h>
#include <stdio.h>

#include "conic.h"
//#include "w1_conics.h"

#define NUM_NEUTRON 1000000
 
//Function to get random number
double getRandom() {
    return rand01();
}
 
//Function to get new particle from source
_class_particle generate_class_particleFromSource(double radius, double div, double Lambda0,
        double num_neutrons) {

    double chi = 2*M_PI*(getRandom());
    double r = sqrt(getRandom()) * radius;

    double v = 3956.036 / Lambda0;
    
    double theta = sqrt(getRandom())*div;
    double phi = getRandom()*2*M_PI;
    double tan_r = tan(theta);

    double vz = v/sqrt(1+tan_r*tan_r);
    double vr = tan_r * vz;

    return make_class_particle(r*cos(chi),r*sin(chi),0,cos(phi)*vr,sin(phi)*vr,
                vz,0,0.0,0.0,0.0,1.0/num_neutrons);
}

//Function to add items to scene
void addItems(Scene* s, double instr_len,double r,double f,double M,
        double max_mir_len,double m, double mirr_thick) {

    //Change code here to make different Scenes
    PP p = addPPShell(0.0, instr_len, r, f, M, max_mir_len, m, m, mirr_thick, s);
    addDisk(p.p0->zs, 0.0, rConic(p.p0->ze, *p.p0)*p.p0->zs/p.p0->ze, s);
    addDisk(p.p1->zs, rConic(p.p1->zs, *p.p1), 10000,s);
    addDetector(10.0, -0.01, 0.01, -0.01, 0.01, 600, 600, NUM_NEUTRON, "test.txt", s);
}

//Main Function
int main() {
    //seed random generator (starts the generator)
    srand48((long)time(0));

    //Make a new Scene
    Scene s = makeScene();

    //Add Items and initialize Scene
    addItems(&s,10.0,0.068,4.2,1,0.7,3,0.001);
    initSimulation(&s);

    //Raytrace all particles through the Scene
    double i;
    for (i = 0; i < NUM_NEUTRON; i++) {
        _class_particle p = generate_class_particleFromSource(0.005, 0.02422, 4, NUM_NEUTRON);
        traceSingleNeutron(&p,s);
    }

    //Finish Simulation of the Scene
    finishSimulation(&s);

    return 0;
}                                                               
@endcode

*/                        

/**
    @file conic.h
    \brief General Framework for generating and raytracing geometries of the
    form @f$ r = k_1+k_2 z+k_3 z^2 @f$

    @author Giacomo Resta <gresta@mit.edu>
    @version 0.2

    @section LICENSE
    Permission is hereby granted, free of charge, to any person obtaining a
    copy of this software and associated documentation files (the "Software"),
    to deal in the Software without restriction, including without limitation
    the rights to use, copy, modify, merge, publish, distribute, sublicense,
    and/or sell copies of the Software, and to permit persons to whom the Software
    is furnished to do so, subject to the following conditions:

    The above copyright notice and this permission notice shall be included in all
    copies or substantial portions of the Software.

    THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED,
    INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR
    PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE
    FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR
    OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER
    DEALINGS IN THE SOFTWARE.

    @section DESCRIPTION
     General Framework for generating and raytracing geometries of the form
     @f$ r = k_1+k_2 z+k_3 z^2 @f$
*/

/////////////////////////////////////
// Simulation
/////////////////////////////////////

#ifndef MIT_CONICS
#define MIT_CONICS

#include <math.h>
#include <stdio.h>
#include <stdlib.h>

/** @defgroup simgroup Simulator Internals
    Contains items general to the simulation
    @{
*/
//! Max number of ConicSurf allowed in a Scene
#define MAX_FLATSURF 200

//! Max number of ConicSurf allowed in a Scene
#define MAX_CONICSURF 100

//! Max number of Disks allowed in a Scene
#define MAX_DISK 100

//! Max number of Detectors allowed in a Scene
#define MAX_DETECTOR 10

//! If "1" simulator will record z location where neutron with greatest grazing angle reflected for each ConicSurf
/*! The information is stored in the max_ga and max_ga_z0 members of each ConicSurf, which are only present if
    this flag is 1. See source code for clarification.

@note You must use default traceNeutronConic function or implement saving routine yourself */
#define REC_MAX_GA 0

#define V2Q_conic 1.58825361e-3
#define Q2V_conic 629.622368
#define POT_V 46.839498800356665//theta_crit = 4 pi^2 kappa^2/m_n^2
#define m_Si 0.478 // m-value of pure silicon
//! Stucture to represent a point
typedef struct {
    double x; //!< x-axis position of point
    double y; //!< y-axis position of point
    double z; //!< z-axis position of point
} Point;

//! Structure to represent a vector
typedef struct {
    double x; //!< x-axis length of vector
    double y; //!< y-axis length of vector
    double z; //!< z-axis length of vector
} Vec;

//! Structure to represent a particle
/* typedef struct { */
/*     double _x; //!< x axis position of particle */
/*     double _y; //!< y axis position of particle */
/*     double _z; //!< z axis position of particle */
/*     double _vx; //!< x axis components of velocity */
/*     double _vy; //!< y axis components of velocity */
/*     double _vz; //!< z axis components of velocity */
/*     double _sx; //!< x spin vector components  */
/*     double _sy; //!< y spin vector components  */
/*     double _sz; //!< z spin vector components  */
/*     double w; //!< Weight of particle */
/*     int absorb; //!< Absorb flag (0 is not absorbed) */
/*     double _t; //!< Time of flight of particle */
/* } _class_particle; */

/*! \brief Function to make a point

@param x x-axis position
@param y y-axis position
@param z z-axis position
*/
Point makePoint(double x, double y, double z) {
    Point p;
    p.x = x;
    p.y = y;
    p.z = z;

    return p;
}

/*! \brief Function to make a vector

@param x x-axis length
@param y y-axis length
@param z z-axis length
*/
Vec makeVec(double x, double y, double z) {
    Vec p;
    p.x = x;
    p.y = y;
    p.z = z;

    return p;
}

//! Function to compute length of a vector
double getMagVec(Vec v) {
    return sqrt(v.x*v.x+v.y*v.y+v.z*v.z);
}

//! Function to compute dot product of two vectors
double dotVec(Vec v1, Vec v2) {
    return v1.x*v2.x + v1.y*v2.y + v1.z*v2.z;
}

//! Function to compute the sum of two vectors
Vec difVec(Vec v1, Vec v2){
    return makeVec(v1.x-v2.x, v1.y-v2.y, v1.z-v2.z);
}

//! Function to compute the sum of two vectors
Vec sumVec(Vec v1, Vec v2){
    return makeVec(v1.x+v2.x, v1.y+v2.y, v1.z+v2.z);
}

//! Function to compute the sum of two vectors
Vec skalarVec(Vec v1, double k){
    return makeVec(v1.x*k, v1.y*k, v1.z*k);
}
/*! \brief Function to make a particle

@param x x-axis position
@param y y-axis position
@param z z-axis position
@param vx x-axis velocity
@param vy y-axis velocity
@param vz z-axis velocity
@param t time of flight of neutron
@param sx x-axis component of spin vector
@param sy y-axis component of spin vector
@param sz z-axis component of spin vector
@param w weight of particle
*/
_class_particle make_class_particle(double x, double y, double z,
    double vx, double vy, double vz, double t,
    double sx, double sy, double sz, int silicon, double w) {

    _class_particle pa=mcgenstate();

    pa.x = x;
    pa.y = y;
    pa.z = z;

    pa.vx = vx;
    pa.vy = vy;
    pa.vz = vz;

    pa.sx = sx;
    pa.sy = sy;
    pa.sz = sz;

    pa.p = w;
    pa.t = t;

    return pa;
}

//! Function to get position of particle 
Point get_class_particlePos(_class_particle p) {
    return makePoint(p.x,p.y,p.z);
}       

//! Function to get velocity vector of particle 
Vec get_class_particleVel(_class_particle p) {
  return makeVec(p.vx, p.vy, p.vz);
}

/*! \brief Function to move particle a specific time step.

Will not move particle if t < 0. Does not simulate
gravity.

@param t time step to move particle
@param p pointer to particle
*/
void move_class_particleT(double t, _class_particle* p) {
    if (t < 0)
        return;
    if (p->_mctmp_a>0){
        p->x = p->x+p->vx*t;
        p->y = p->y+p->vy*t;
        p->z = p->z+p->vz*t;
        p->t = p->t+t;
	p->p *= exp(-t*98900/52.338);//ugly hard coding in the penetration depth of silicon at 989 m/s
        //absorb_class_particle(p);
    }
    else{
        p->x = p->x+p->vx*t;
        p->y = p->y+p->vy*t;
        p->z = p->z+p->vz*t;
        p->t = p->t+t;
    }
}

/*! \brief Function to move particle to position z.

Will not move particle if moving particle to z
position would require negative time.
Does not simulate gravity.

@param z z-position to move particle
@param p pointer to particle
*/
void move_class_particleZ(double z, _class_particle* p) {
    double t = (z-p->z)/p->vz;
    move_class_particleT(t, p);
}

/*! \brief Function to compute new position of particle
without modifying the position of the actual particle.

Will not move particle if t < 0. Does not simulate gravity.

@param t timestep to move particle
@param p particle
*/
_class_particle copyMove_class_particleT(double t, _class_particle p) {
    _class_particle p2 = p;
    move_class_particleT(t,&p2);
    return p2;
}

/*! \brief Function to move particle to position z
without modifying the position of the actual particle.

Will not move particle if moving particle to z
position would require negative time.
Does not simulate gravity.

@param z z-position to move particle
@param p pointer to particle
*/
_class_particle copyMove_class_particleZ(double z, _class_particle p) {
    _class_particle p2 = p;
    move_class_particleZ(z,&p2);
    return p2;
}

/*! \brief Mathematical Aid for Snell's Law for reflection.

Does not take into account grazing angle constrictions.
Only computes mathematical reflection.

@param n Normal vector
@param p Pointer to particle
*/
void reflect_class_particle(Vec n, _class_particle* p) {
    double vn = dotVec(get_class_particleVel(*p),n);

    p->vx = p->vx-2*vn*n.x;
    p->vy = p->vy-2*vn*n.y; 
    p->vz = p->vz-2*vn*n.z; 
}

/*! \brief Function to mark particle as absorbed

@param p Pointer to particle to be absorbed */
void absorb_class_particle(_class_particle* p)  {
    p->vx = 0;
    p->vy = 0; 
    p->vz = 0;
    p->p = 0;
    p->_absorbed = 1;
}

/*! \brief Function to set weight of particle.

Will set the weight of the particle to w.  */
void setWeight_class_particle(double w, _class_particle* pa) {
    pa->p = w;
}

/*! \brief Function to solve quadratic equations for smallest positive result.

If no positive result returns -1. Parameters are coefficents such that
@f$ 0=A z^2 + B z + C @f$

@return Will return smallest positive value or -1 if no smallest positive value
*/
double solveQuad(double A, double B, double C) {
    if (fabs(A) < 1e-11 && B != 0)           //FIXME: 1e-11 cutoff may cause problems
        return -C/B;
    else {
        double det = B*B - 4*A*C;
        if (det < 0)
            return -1;
        else {
            double sdet = sqrt(det);
            double s1 = (-B+sdet)/(2*A);
            double s2 = (-B-sdet)/(2*A);

            if (fabs(s1) < 1e-11) s1=0.0;     //FIXME: 1e-11 cutoff may cause problems
            if (fabs(s2) < 1e-11) s2=0.0;     //FIXME: 1e-11 cutoff may cause problems

            if (s1 > 0.0) {
                if (s2 > 0.0) {
                    if (s1 > s2)
                        return s2;
                    return s1;
                }
                else
                    return s1;
           }
           if (s2 > 0.0)
               return s2;
        }
    }
    return -1;
}

//! Returns sign of x, either -1 or 1
int sign(double x) {
    if (x < 0)
        return -1;
    return 1;
}

/** @} */ //end of simgroup

/*! @ingroup detectorgroup
\brief Structure for representing inline detectors

@warning Do not directly modify this structure*/
typedef struct {
    double z0; //!< z-axis position of detector
    double xmin; //!< Smallest x value to detect
    double xmax; //!< Largest x value to detect
    double xstep; //!< x size of subsampling pixel
    double ymin; //!< Smallest y value to detect
    double ymax; //!< Largest y value to detect
    double ystep; //!< y size of subsampling pixel
    int nrows;    //!< Number of pixels along y axis
    int ncols;    //!< Number of pixels along x axis
    double num_particles; //!< Number of particles being emitted from source
    double *num_count; //!< Pointer to the number of particles that hit detector
    double **data; //!< Pointer to 2d data, pixel_x_y = data[x][y]
    char* filename; //!< Name of output file of detector (should end in .txt)
} Detector;

/*! @ingroup diskgroup
\brief Structure for representing Disk geometry

 Creates a doughnut with inner radius r0 and outer radus r1 at position z0.
Neutrons between r0 and r1 are absorbed. */
typedef struct {
    double r0; //!< Inner radius of doughnut
    double r1; //!< Outer radius of doughnut
    double z0; //!< z-axis position of Disk
    int absorb; // !< Flag to inicate if disk absorbs or not
} Disk;

/*! @ingroup conicgroup */
enum ConicType {
    PARA,
    HYPER,
    ELLIP
};


/*! @ingroup conicgroup
\brief Structure to contain z-axis symetric conic sections

Contains any geometry that can be expressed as
@f$ r^2=k_1 + k_2 z + k_3 z^2 @f$

@warning Do not directly modify values in this structure directly */
typedef struct {
    double k1; //!< @f$ k_1 @f$ in equation below
    double k2; //!< @f$ k_2 @f$ in equation below
    double k3; //!< @f$ k_3 @f$ in equation below
    double zs; //!< z-axis position of start of mirror
    double ze; //!< z-axis position of end of mirror
    double m;  //!< m value for mirror (1.0 for Nickel)
    double Qc; //!< m value for mirror (0.0219 AA-1 for Nickel substrate)
    double R0; //!< R0 reflectivity for mirror (0.99 for Nickel substrate)
    double alpha; //!< alpha, slope of reflectivity (6.07 AA for m=2 mirror)
    double W;     //!< Width of supermirror cut-off, 0.003 AA-1 for m=2 mirror
  
    //Only for reference
    double f1; //!< z-axis position of first focus
    double f2; //!< z-axis position of second focus, for paraboloid this is unassigned
    double a;  //!< Value of a, specific to geometry type
    double c;  //!< Value of c, for paraboloid this is unassigned
    enum ConicType type; //!< Type of mirror geometry

    #if REC_MAX_GA
    double max_ga; //!< Max Grazing Angle of Reflected Neutron (Exists only if REC_MAX_GA)
    double max_ga_z0; //!< Collision point of Max Grazing Neutron (Exists only if REC_MAX_GA)
    #endif

} ConicSurf;


/*! @ingroup flatgroup
\brief Structure to contain z-axis symetric flat sections

Contains flat geometries which can be expressed as
@f$ x^2 = k_1 + k_2 z + k_3 z^2 @f$ or
@f$ y^2 = k_1 + k_2 z + k_3 z^2 @f$

@warning Do not directly modify values in this structure directly */
typedef struct {
    double k1; //!< @f$ k_1 @f$ in equation below
    double k2; //!< @f$ k_2 @f$ in equation below
    double k3; //!< @f$ k_3 @f$ in equation below
    double zs; //!< z-axis position of start of mirror
    double ze; //!< z-axis position of end of mirror
    double ll; //!< left/lower limit of mirror along translational symmetry
    double rl; //!< right/upper limit of mirror along translational symmetry
    double m;  //!< m value for mirror (1.0 for Nickel)
    double Qc; //!< m value for mirror (0.0219 AA-1 for Nickel substrate)
    double R0; //!< R0 reflectivity for mirror (0.99 for Nickel substrate)
    double alpha; //!< alpha, slope of reflectivity (6.07 AA for m=2 mirror)
    double W;     //!< Width of supermirror cut-off, 0.003 AA-1 for m=2 mirror

    //Only for reference
    double f1; //!< z-axis position of first focus
    double f2; //!< z-axis position of second focus, for paraboloid this is unassigned
    double a;  //!< Value of a, specific to geometry type
    double c;  //!< Value of c, for paraboloid this is unassigned
    //enum FlatType type; //!< Type of mirror geometry
    int doubleReflections; // will determine whether the geometry allows double reflections
    #if REC_MAX_GA
    double max_ga; //!< Max Grazing Angle of Reflected Neutron (Exists only if REC_MAX_GA)
    double max_ga_z0; //!< Collision point of Max Grazing Neutron (Exists only if REC_MAX_GA)
    #endif

} FlatSurf;
/*! @ingroup simgroup
\brief Structure to hold all scene geometry

The number of possible ConicSurf, Disk and Detector in the Scene are
determined by MAX_CONICSURF, MAX_DISK and MAX_DETECTOR.
*/
typedef struct {
    FlatSurf f[MAX_FLATSURF]; //!< Array of all ConicSurf in Scene
    int num_f;                  //!< Number of ConicSurf in Scene

    ConicSurf c[MAX_CONICSURF]; //!< Array of all ConicSurf in Scene
    int num_c;                  //!< Number of ConicSurf in Scene

    Disk di[MAX_DISK];          //!< Array of all Disk in Scene
    int num_di;                 //!< Number of Disk in Scene

    Detector d[MAX_DETECTOR];  //!< Array of all Detector in Scene
    int num_d;                 //!< Number of Detector in Scene


} Scene;

/////////////////////////////////////
// Inline Detector
/////////////////////////////////////

/** @defgroup detectorgroup Detector
    Contains code related to the inline detectors
    @{
*/

/*! \brief Function to make Detector

@param z0 z-axis position of detector
@param xmin Smallest x value to detect
@param xmax Largest x value to detect
@param ymin Smallest y value to detect
@param ymax Largest y value to detect
@param xres Number of pixels along x axis
@param yres Number of pixels along y axis
@param num_particles Total number of particles being emitted
@param filename Name of output file of detector (should end in .txt)
*/
Detector makeDetector(double z0,double xmin, double xmax, double ymin, double ymax, int xres,
    int yres, double num_particles, char* filename) {

    Detector d;
    d.z0 = z0;
    d.xmin = xmin;
    d.xmax = xmax;
    d.xstep = (xmax-xmin)/xres;

    d.ymin = ymin;
    d.ymax = ymax;
    d.ystep = (ymax-ymin)/yres;

    d.ncols = xres;
    d.nrows = yres;
    d.filename = filename;
    d.num_particles = num_particles;

    d.num_count = (double*)malloc(sizeof(double));
    if (d.num_count == NULL) {
        fprintf(stderr, "MEMORY ALLOCATION PROBLEM\n");
        exit(-1);
    }
    (*d.num_count) = 0;

    d.data = (double**)malloc(d.ncols*sizeof(double *));
    if (d.data == NULL) {
        fprintf(stderr, "MEMORY ALLOCATION PROBLEM\n");
        exit(-1);
    }
    int x;
    for(x = 0; x  < d.ncols; x++) {
        d.data[x] = (double*)malloc(d.ncols*sizeof(double));
        if (d.data[x] == NULL) {
            fprintf(stderr, "MEMORY ALLOCATION PROBLEM\n");
            exit(-1);
        }
        (*d.data[x]) = 0;
    }

    return d;
}

/*! \brief Function to make and add Detector

@param z0 z-axis position of detector
@param xmin Smallest x value to detect
@param xmax Largest x value to detect
@param ymin Smallest y value to detect
@param ymax Largest y value to detect
@param xres Number of pixels along x axis
@param yres Number of pixels along y axis
@param num_particles Total number of particles being emitted
@param filename Name of output file of detector (should end in .txt)
@param s Scene to add Detector to
*/
Detector* addDetector(double z0, double xmin, double xmax, double ymin, double ymax, double xres,
    double yres, double num_particles, char* filename, Scene* s) {
    if (s->num_d >= MAX_DETECTOR-1) {
        fprintf(stderr,"TOO MANY DETECTORS IN SCENE");
        exit(-1);
    }
    s->d[s->num_d] = makeDetector(z0,xmin,xmax,ymin,ymax,xres,yres,num_particles,filename);
    s->num_d++;
    return &s->d[s->num_d-1];
}

/*! \brief Function to compute time of first collision for a Detector.

@param p _class_particle to consider
@param d Detector to consider

@return Time until the propogation or -1 if particle will not hit detector
*/
double getTimeOfFirstCollisionDetector(_class_particle p, Detector d) {
    double t = (d.z0-p.z)/p.vz;
    if (t <= 0)
        return -1;
    _class_particle p2 = copyMove_class_particleT(t,p);
    if (p2.x > d.xmax || p2.x < d.xmin || p2.y > d.ymax || p2.y < d.ymin)
        return -1;
    return t;
}

/*! \brief Function to raytrace Detector

@param p Pointer to particle to be traced
@param d Detector to be traced
*/
void traceNeutronDetector(_class_particle* p, Detector d) {
    double t = getTimeOfFirstCollisionDetector(*p, d);
    if (t < 0)
        return;
    move_class_particleT(t,p);
    d.data[(int)floor((p->x-d.xmin)/d.xstep)][(int)floor((p->y-d.ymin)/d.ystep)] += p->p;
    (*d.num_count) += p->p;
}

/*! \brief Function to finalize detector

Will write data and free data array.

@param d Detector to finalize
*/
void finishDetector(Detector d) {
    int x,y;
    if (strcmp(d.filename,"")) {
        FILE *file;
        file = fopen(d.filename,"w");
	if (file) {
	  double intensity = (*d.num_count);
	  fprintf(file, "#I=%e I_ERR=%e xmin=%f xmax=%f ymin=%f ymax=%f ncols=%i nrows=%i\n",
		  intensity, sqrt(intensity/d.num_particles), d.xmin, d.xmax, d.ymin, d.ymax, d.ncols, d.nrows); //FIXME: check I_ERR sqrt(I/num_particles)

	  //Write data
	  for (x=0; x < d.ncols; x++) {
            for (y=0; y < d.nrows; y++)
	      fprintf(file, "%e ", d.data[x][y]);
            fprintf(file, "\n");
	  }
	  fclose(file);
	} else {
	  fprintf(stderr,"Warning from conic.h: Could not open file %s in write mode. No data written!\n",d.filename);
	}
    }
    for (x=0; x < d.ncols; x++)
        free(d.data[x]);
    free(d.data);
    free(d.num_count);
    return;
}

/** @} */ //end of detectorgroup

/////////////////////////////////////
// Geometry Types
/////////////////////////////////////

/////////////////////////////////////
// Disks
/////////////////////////////////////

/** @defgroup diskgroup Disk
    Contains code related to Disks
    @{
*/

/*! \brief Function for creating a Disk structure

@param z0 z-axis position of Disk
@param r0 Inner radius of doughnut
@param r1 Outer radius of doughnut

@see Disk
*/
Disk makeDisk(double z0, double r0, double r1) {
    Disk d;

    d.r0 = r0;
    d.z0 = z0;
    d.r1 = r1;
    d.absorb = 1; // By default, absorb on propagation to disk
    return d;
}

/*! \brief Function for making and adding Disk to Scene

@param z0 z-axis position of Disk
@param r0 Inner radius of doughnut
@param r1 Outer radius of doughnut
@param s Scene to add Disk to

@see Disk
*/
Disk* addDisk(double z0, double r0, double r1, Scene* s) {
    if (s->num_di >= MAX_DISK-1) {
        fprintf(stderr,"TOO MANY DISKS IN SCENE");
        exit(-1);
    }
    s->di[s->num_di] = makeDisk(z0, r0, r1);
    s->num_di++;
    return &s->di[s->num_di -1];
}

/*! \brief Function for making and adding non-absorbing end-Disk to Scene

@param z0 z-axis position of Disk
@param r0 Inner radius of doughnut
@param r1 Outer radius of doughnut
@param s Scene to add Disk to

@see Disk
*/
Disk* addEndDisk(double z0, double r0, double r1, Scene* s) {
    if (s->num_di >= MAX_DISK-1) {
        fprintf(stderr,"TOO MANY DISKS IN SCENE");
        exit(-1);
    }
    s->di[s->num_di] = makeDisk(z0, r0, r1);
    s->di[s->num_di].absorb = 0;
    s->num_di++;
    return &s->di[s->num_di -1];
}

/*! \brief Function to compute time of first collision for a disk

@param p _class_particle to consider
@param d Disk to consider
@return Time until the propogation or -1 if particle will not hit disk
*/
double getTimeOfFirstCollisionDisk(_class_particle p, Disk d) {
    double tz = (d.z0-p.z)/p.vz;
    if (tz <= 0)
        return -1;
    _class_particle p2 = copyMove_class_particleT(tz, p);
    double rp = sqrt(p2.x*p2.x+p2.y*p2.y);
    if (rp > d.r0 && rp < d.r1 && fabs(p2.z-d.z0) < 1e-11)
        return (d.z0-p.z)/p.vz; 
    return -1;
}

/*! \brief Function to raytrace Disks

@param p Pointer to particle to be traced
@param d Disk to be traced
*/
void traceNeutronDisk(_class_particle* p, Disk d) {
    double t = getTimeOfFirstCollisionDisk(*p, d);

    if (t <= 0)
        return;

    move_class_particleT(t, p);
	
	SCATTER_func(p);
	
    if (d.absorb)
      absorb_class_particle(p);
}

/** @} */ //end of diskgroup

/////////////////////////////////////
// Z-Axis Symetric Conic Sections
/////////////////////////////////////

/** @defgroup conicgroup ConicSurf
    Contains code related to ConicSurfs
    @{
*/

/*! \brief Function to return radius of ConicSurf at a z-axis position.

Will return radius even if z is outside the bounds of zs and ze
for the particular ConicSurf.

@param z z-axis position to compute radius
@param s ConicSurf to compute radius of
*/
double rConic(double z, ConicSurf s) {
    return sqrt(s.k1+s.k2*z+s.k3*z*z);
}

/*! \brief Function to return width of FlatSurf at a z-axis position.

Will return width even if z is outside the bounds of zs and ze
for the particular FlatSurf.

@param z z-axis position to compute radius
@param s FlatSurf to compute width of
*/
double wFlat(double z, FlatSurf s) {
    return sqrt(s.k1+s.k2*z+s.k3*z*z);
}

/*! \brief Function for generating Hyperboloid ConicSurf.

@param f1 z position of focus closest to actual mirror surface
@param f2 z position of focus furthest from actual mirror surface
@param p A Point on the actual surface of the mirror
@param zstart z position of start of mirror surface
@param zend z position of end of mirror surface
@param m m value for reflectivity of the surface

@see ConicSurf
*/
ConicSurf makeHyperboloid(double f1, double f2, Point p,
			  double zstart, double zend, double m, double R0, double Qc, double alpha, double W) {
    ConicSurf s;
    s.zs = zstart;
    s.ze = zend;

    double r2 = p.x*p.x+p.y*p.y;
    double c = (f1-f2)/2;

    double u = p.z+c-f1;
    double a = sqrt(((u*u+c*c+r2)-sqrt(pow(u*u+c*c+r2,2)-4*c*c*u*u))/2);

    s.k3 = c*c/(a*a)-1;
    s.k2 = 2*s.k3*(c-f1);
    s.k1 = (s.k3)*(c-f1)*(c-f1)-c*c+a*a;

    s.m = m;
    s.R0 = R0;
    s.Qc = Qc;
    s.alpha = alpha;
    s.W = W;
    s.f1 = f1;
    s.f2 = f2;
    s.a = a;
    s.c = c;

    s.type = HYPER;

    #if REC_MAX_GA
    s.max_ga = -1;
    s.max_ga_z0 = -1;
    #endif

    return s;
}

/*! \brief Function for generating Ellipsoid ConicSurf.

@param f1 z position of focus closest to actual mirror surface
@param f2 z position of focus furthest from actual mirror surface
@param p A Point on the actual surface of the mirror
@param zstart z position of start of mirror surface
@param zend z position of end of mirror surface
@param m m value for reflectivity of the surface

@see ConicSurf
*/     
ConicSurf makeEllipsoid(double f1, double f2, Point p, 
			double zstart, double zend, double m, double R0, double Qc, double alpha, double W) {
    ConicSurf s;
    s.zs = zstart;
    s.ze = zend;

    double r2 = p.x*p.x+p.y*p.y;
    double c = (f1-f2)/2;

    double u = p.z+c-f1;
    double a = sqrt(((u*u+c*c+r2)+sqrt(pow(u*u+c*c+r2,2)-4*c*c*u*u))/2);

    s.k3 = c*c/(a*a)-1;
    s.k2 = 2*s.k3*(c-f1);
    s.k1 = (s.k3)*(c-f1)*(c-f1)-c*c+a*a;

    s.m = m;
    s.R0 = R0;
    s.Qc = Qc;
    s.alpha = alpha;
    s.W = W;
    s.f1 = f1;
    s.f2 = f2;
    s.a = a;
    s.c = c;   

    s.type = ELLIP;

    #if REC_MAX_GA
    s.max_ga = -1;
    s.max_ga_z0 = -1;
    #endif

    return s;
}


/*! \brief Function for generating Flat Ellipse with symmetry along the vertical y.

@param f1 z position of focus closest to actual mirror surface
@param f2 z position of focus furthest from actual mirror surface
@param b the short half axis of the ellipse, positive for translational symmetry along y, negative for translational symmetry along x
@param p A Point on the actual surface of the mirror
@param zstart z position of start of mirror surface
@param zend z position of end of mirror surface
@param ll the left/lower limit of the ellipse surface (in either y or x for b > 0 or b < 0
@param rl the right/upper limit of the ellipse surface (in either y or x for b > 0 or b < 0
@param m m value for reflectivity of the surface


@see ConicSurf
*/
FlatSurf makeFlatEllipse(double f1, double f2, Point p, double zstart, double zend, double ll, double rl, double m, double R0, double Qc, double alpha, double W) {
    FlatSurf s;
    s.zs = zstart;
    s.ze = zend;

    s.ll = ll;
    s.rl = rl;
    double r2 = p.x*p.x + p.y*p.y;
    double c = (f1-f2)/2;
    double u = p.z+c-f1;
    double a = sqrt(((u*u+c*c+r2)+sqrt(pow(u*u+c*c+r2,2)-4*c*c*u*u))/2);

    s.k3 = c*c/(a*a)-1;
    s.k2 = 2*s.k3*(c-f1);
    s.k1 = (s.k3)*(c-f1)*(c-f1)-c*c+a*a;

    s.m = m;
    s.R0 = R0;
    s.Qc = Qc;
    s.alpha = alpha;
    s.W = W;
    s.f1 = f1;
    s.f2 = f2;
    s.a = a;
    s.c = c;

    //s.type = ELLIP;

    #if REC_MAX_GA
    s.max_ga = -1;
    s.max_ga_z0 = -1;
    #endif

    // PW Initializer added from linter hints...
    s.doubleReflections=0;

    return s;
}

/*! \brief Function for generating Paraboloid ConicSurf.

@param f z position of focus closest to actual mirror surface
@param p A Point on the actual surface of the mirror
@param zstart z position of start of mirror surface
@param zend z position of end of mirror surface
@param m m value for reflectivity of the surface

@see ConicSurf
*/
ConicSurf makeParaboloid(double f, Point p, double zstart,
			 double zend, double m, double R0, double Qc, double alpha, double W) {

    ConicSurf s;
    s.zs = zstart;
    s.ze = zend;

    double r2 = p.x*p.x+p.y*p.y;
    double a = (-(p.z-f)+sign(p.z-f)*sqrt((p.z-f)*(p.z-f)+r2))/2;

    s.k3 = 0.0;
    s.k2 = 4*a;
    s.k1 = s.k2*(a-f);

    s.m = m;
    s.R0 = R0;
    s.Qc = Qc;
    s.alpha = alpha;
    s.W = W;
    s.f1 = f;
    s.a = a;

    s.type = PARA;

    #if REC_MAX_GA
    s.max_ga = -1;
    s.max_ga_z0 = -1;
    #endif

    // PW Initializer added from linter hints...
    s.f2=0;
    s.c=0;

    return s;
}

/*! \brief Function for generating Flat Parabola for FlatSurf.

@param f z position of focus closest to actual mirror surface
@param p A Point on the actual surface of the mirror, putting one of x or y to 0 results in the surface being parallel to said coordinate
@param zstart z position of start of mirror surface
@param zend z position of end of mirror surface
@param ll the left/lower limit of the ellipse surface (in either y or x for b > 0 or b < 0
@param rl the right/upper limit of the ellipse surface (in either y or x for b > 0 or b < 0
@param m m value for reflectivity of the surface
@param doubleReflections wether double reflections are allowed

@see FlatSurf
*/
FlatSurf makeFlatparbola(
    double f,
    Point p,
    double zstart,
    double zend,
    double ll,
    double rl,
    double m,
    double R0, double Qc, double alpha, double W) {

    FlatSurf s;
    s.zs = zstart;
    s.ze = zend;

    double r2 = p.x*p.x+p.y*p.y;
    double a = (-(p.z-f)+sign(p.z-f)*sqrt((p.z-f)*(p.z-f)+r2))/2;

    s.k3 = 0.0;
    s.k2 = 4*a;
    s.k1 = s.k2*(a-f);

    s.m = m;
    s.R0 = R0;
    s.Qc = Qc;
    s.alpha = alpha;
    s.W = W;
    s.f1 = f;
    s.a = a;
    s.ll = ll;
    s.rl = rl;
    //s.type = PARA;

    #if REC_MAX_GA
    s.max_ga = -1;
    s.max_ga_z0 = -1;
    #endif

    // PW Initializer added from linter hints...
    s.doubleReflections=0;
    s.f2=0;
    s.c=0;

    return s;
}

/*! \brief Function for generating and adding Paraboloid ConicSurf.

@param f1 z position of focus closest to actual mirror surface
@param p A Point on the actual surface of the mirror
@param zstart z position of start of mirror surface
@param zend z position of end of mirror surface
@param m m value for reflectivity of the surface
@param s Scene to add Paraboloid to

@see ConicSurf
*/
ConicSurf* addParaboloid(double f1, Point p, double zstart, double zend,
			 double m, double R0, double Qc, double alpha, double W, Scene* s) {
    if (s->num_c >= MAX_CONICSURF-1) {
        fprintf(stderr,"TOO MANY CONICSURF IN SCENE");
        exit(-1);
    }
    s->c[s->num_c] = makeParaboloid(f1,p,zstart,zend,m,R0,Qc,alpha,W);
    s->num_c++;
    return &s->c[s->num_c-1];
}

/*! \brief Function for generating and adding a flat Parabolic FlatSurf.

@param f1 z position of focus closest to actual mirror surface
@param p A Point on the actual surface of the mirror
@param zstart z position of start of mirror surface
@param zend z position of end of mirror surface
@param ll the lower bound of the mirror
@param rl the upper bound of the mirror
@param m m value for reflectivity of the surface
@param s Scene to add Ellipsoid to
@param doubleReflections wether double reflections can occur
@see FlatSurf
*/
FlatSurf* addFlatParabola(
    double f,
    Point p,
    double zstart,
    double zend,
    double ll,
    double rl,
    double m,
    double R0, double Qc, double alpha, double W,
    Scene* s) {
    if (s->num_f >= MAX_FLATSURF-1) {
        fprintf(stderr,"TOO MANY FLATSURF IN SCENE");
        exit(-1);
    }
    s->f[s->num_f] = makeFlatparbola(f,p,zstart,zend,ll,rl,m, R0, Qc, alpha, W);
    s->num_f++;
    return &s->f[s->num_f-1];
}

/*! \brief Function for generating and adding Hyperboloid ConicSurf.

@param f1 z position of focus closest to actual mirror surface
@param f2 z position of focus furthest from actual mirror surface
@param p A Point on the actual surface of the mirror
@param zstart z position of start of mirror surface
@param zend z position of end of mirror surface
@param m m value for reflectivity of the surface
@param s Scene to add Hyperboloid to

@see ConicSurf
*/
ConicSurf* addHyperboloid(double f1, double f2, Point p, double zstart,
    double zend, double m, double R0, double Qc, double alpha, double W, Scene* s) {
    if (s->num_c >= MAX_CONICSURF-1) {
        fprintf(stderr,"TOO MANY CONICSURF IN SCENE");
        exit(-1);
    }
    s->c[s->num_c] = makeHyperboloid(f1,f2,p,zstart,zend,m,R0,Qc,alpha,W);
    s->num_c++;
    return &s->c[s->num_c-1];
}

/*! \brief Function for generating and adding Ellipsoid ConicSurf.

@param f1 z position of focus closest to actual mirror surface
@param f2 z position of focus furthest from actual mirror surface
@param p A Point on the actual surface of the mirror
@param zstart z position of start of mirror surface
@param zend z position of end of mirror surface
@param m m value for reflectivity of the surface
@param s Scene to add Ellipsoid to

@see ConicSurf
*/
ConicSurf* addEllipsoid(double f1, double f2, Point p, double zstart,
    double zend, double m, double R0, double Qc, double alpha, double W, Scene* s) {
    if (s->num_c >= MAX_CONICSURF-1) {
        fprintf(stderr,"TOO MANY CONICSURF IN SCENE");
        exit(-1);
    }
    s->c[s->num_c] = makeEllipsoid(f1,f2,p,zstart,zend,m,R0,Qc,alpha,W);
    s->num_c++;
    return &s->c[s->num_c-1];
}

/*! \brief Function for generating and adding a flat Ellipse FlatSurf.

@param f1 z position of focus closest to actual mirror surface
@param f2 z position of focus furthest from actual mirror surface
@param p A Point on the actual surface of the mirror
@param zstart z position of start of mirror surface
@param zend z position of end of mirror surface
@param m m value for reflectivity of the surface
@param s Scene to add Ellipsoid to

@see ConicSurf
*/
FlatSurf* addFlatEllipse(
    double f1,
    double f2,
    Point p,
    double zstart,
    double zend,
    double ll,
    double rl,
    double m,
    double R0, double Qc, double alpha, double W,
    Scene* s) {
    if (s->num_f >= MAX_FLATSURF-1) {
        fprintf(stderr,"TOO MANY FLATSURF IN SCENE");
        exit(-1);
    }
    s->f[s->num_f] = makeFlatEllipse(f1,f2,p,zstart,zend,ll,rl,m,R0,Qc,alpha,W);
    s->num_f++;
    return &s->f[s->num_f-1];
}
//!TODO
double getGrazeAngleConic(_class_particle p, ConicSurf s) {
    /*
    double v = sqrt(dotVec(get_class_particleVel(p),get_class_particleVel(p)));
    double vn = dotVec(get_class_particleVel(p),n);
    return fabs(acos(vn/v)) - M_PI/2;
    */

    /* Even though this function does absolutely nothing, better 
       add return value... Negative to indicate something went wrong */
    return -1;
}

/*! \brief Function for returning normal vector of ConicSurf at Point p

Will compute vector even if p is not on surface.
MAKE SURE p IS ON SURFACE

@param p Point to compute normal vector
@param s ConicSurf to compute normal vector of
*/
Vec getNormConic(Point p, ConicSurf s) {
    double det = s.k2*s.k2+4*s.k3*(p.x*p.x+p.y*p.y-s.k1);
    if (det <= 0.){

        return makeVec(-p.x/sqrt(p.x*p.x + p.y*p.y),-p.y/(p.x*p.x + p.y*p.y),0);
    }
    double den = sqrt(det);
    double nx = -2*p.x/den;
    double ny = -2*p.y/den;
    double nz = sign(2*s.k3*p.z+s.k2);
    double n = sqrt(nx*nx+ny*ny+nz*nz);
    return makeVec(nx/n,ny/n,nz/n);
}

/*! \brief Function for returning normal vector of FlatSurf at Point p

Will compute vector even if p is not on surface.
MAKE SURE p IS ON SURFACE

@param p Point to compute normal vector
@param s FlatSurf to compute normal vector of; for s.b > 0 surface posseses translation symmetry along y direction
*/
Vec getNormFlat(Point p, FlatSurf s) {
    double r;
    //if(s.b > 0){
    r = p.x;
    //else{
    //    r = p.y;
    //};
    double den;
    double det = s.k2*s.k2+4*s.k3*(r*r-s.k1);
    if (det > 0){
        den = sqrt(det);
    }
    else{
        return makeVec(-1,0,0); // if the neutron hits the apex of the ellipse we run into a divide by zero problem
    }
    double nx = -2*p.x/den;
    double ny = 0;
    double nz = sign(2*s.k3*p.z+s.k2);
    double n = sqrt(nx*nx+ny*ny+nz*nz);
    return makeVec(nx/n,ny/n,nz/n);
}

/*! \brief Function to compute time of first collision for a ConicSurf

@param p _class_particle to consider
@param s ConicSurf to consider
@return Time until the propogation or -1 if particle will not hit disk
*/ 
double getTimeOfFirstCollisionConic(_class_particle p, ConicSurf s) {
    double tz = (s.zs-p.z)/p.vz;
    if (tz < 0) {
       tz = 0;
       if (p.z > s.ze)
            return -1;
    }

    _class_particle p2 = copyMove_class_particleT(tz,p);

    double A = p2.vx*p2.vx+p2.vy*p2.vy-s.k3*p2.vz*p2.vz;
    double B = 2*(p2.vx*p2.x+p2.vy*p2.y-s.k3*p2.vz*p2.z)-s.k2*p2.vz;
    double C = p2.x*p2.x+p2.y*p2.y-s.k3*p2.z*p2.z-s.k2*p2.z-s.k1;
    
    double t = solveQuad(A,B,C);

    if (t <= 0 || p2.vz*t+p2.z > s.ze || p2.vz*t+p2.z < s.zs)  
        return -1;
    return t+tz;
}

/*! \brief Function to compute time of first collision for a FlatSurf

@param p Particle to consider
@param s FlatSurf to consider
@return Time until the propogation or -1 if particle will not hit surface
*/
//TODO
double getTimeOfFirstCollisionFlat(_class_particle p, FlatSurf s) {
    double tz = (s.zs-p.z)/p.vz;
    if (tz < 0) {
       tz = 0;
       if (p.z > s.ze)
            return -1;
    }

    _class_particle p2 = copyMove_class_particleT(tz,p);
    double vs = 0;//the vector important for calculating the intersection with the ellipse
    double s0 = 0;
    double vt = 0;//the other component only important for testing whether the mirror is hit
    double t0 = 0;
    //if(s.b > 0){//obsolete iteration allowing to rotate by 90 deg with out rotation in McStas, not really needed
    vs = p2.vx;
    s0 = p2.x;
    vt = p2.vy;
    t0 = p2.y;

    //}
    /*else{
    vs = p2.vy;
    s0 = p2.y;
    vt = p2.vx;
    t0 = p2.x;
    };
    */
    double A = vs*vs-s.k3*p2.vz*p2.vz;
    double B = 2*(vs*s0-s.k3*p2.vz*p2.z)-s.k2*p2.vz;
    double C = s0*s0-s.k3*p2.z*p2.z-s.k2*p2.z-s.k1;

    double t = solveQuad(A,B,C);

    if (t <= 0 || p2.vz*t+p2.z > s.ze || p2.vz*t+p2.z < s.zs||vt*t+t0 < s.ll||vt*t +t0 > s.rl)
        return -1;
    return t+tz;
}
/*! \brief Function to rotate a vector (by D. Liu)

@param v vector to rotate
@param angle rotation angle
@param axis rotation according to this axis
*/
Vec rotateM(Vec v, double angle, Vec axis) {
    Vec p;
	double miu = 1-cos(angle);
	double ux = axis.x, uy = axis.y, uz=axis.z;
	
    p.x = v.x*(cos(angle)+ux*ux*miu) + v.y*(ux*uy*miu-uz*sin(angle)) + v.z*(ux*uz*miu+uy*sin(angle));
    p.y = v.x*(uy*ux*miu+uz*sin(angle)) + v.y*(cos(angle)+uy*uy*miu) + v.z*(uy*uz*miu-ux*sin(angle));
    p.z = v.x*(uz*ux*miu-uy*sin(angle)) + v.y*(uz*uy*miu+ux*sin(angle)) + v.z*(cos(angle)+uz*uz*miu);

    return p;
}

/*! \brief Function to handle supermirror reflectivity copied from mcstas.
@note Uses only m-value for calculating reflectivity curve TODO more sophisticated formulae in the future

@param q k_i - k_f momentum transfer of the neutron at the super mirror surface
@param m supermirror m-value
@param R_0 low angle reflectivity
@param Q_c critical momentum transfer of the super mirror

@return p weight reduction of the neutron for further simulation
*/
double calcSupermirrorReflectivity(double q, double m, double R_0, double Q_c){
    double arg;
    double beta = 0;//values fitting supermirror data from sn
    double alpha = 2.5;
    double W = 0.004;
    double weight = 1.0; //neutron weight to be transformed
    q = fabs(q);
    if (m >= 10){
        weight = 1.0;
        return weight;
    }
    if (W==0 && alpha==0) {
      m=m*0.9853+0.1978;
      W=-0.0002*m+0.0022;
      alpha=0.2304*m+5.0944;
      beta=-7.6251*m+68.1137;
      if (m<=3) {
	    alpha=m;
	    beta=0;
        }
    }
    arg = W > 0 ? (q - m*Q_c)/W : 11;
    if (arg > 10 || m <= 0 || Q_c <=0 || R_0 <= 0) {
      weight = 0.0;
      return weight;
    }

    if (m < 1) { Q_c *= m; m=1; }

    if(q <= Q_c) {
      weight = R_0;
      return weight;
    }


    weight = R_0*0.5*(1 - tanh(arg))*(1 - alpha*(q - Q_c) + beta*(q - Q_c)*(q - Q_c));
    return weight;
}

/*! \brief Function to handle reflection of neutron for a ConicSurf.

@note Uses step function for reflectivity

@warning Make sure particle has been moved to surface of mirror
before computing reflection

@param p Pointer of particle to reflect
@param s ConicSurf to use

@return Value of critical angle of the neutron or -1 if neutron is absorbed

@see traceNeutronConic()
*/
double reflectNeutronConic(_class_particle* _particle, ConicSurf s) {
    Vec n = getNormConic(get_class_particlePos(*_particle),s);
    Vec pv = get_class_particleVel(*_particle);
	
	int disp = 0;
	if (disp>0) {
	printf("\n n = %f, %f, %f",n.x,n.y,n.z);
	printf("\n pv = %f, %f, %f",pv.x,pv.y,pv.z);
	}
	//add figure error by D. Liu
	Vec n_p = makeVec(-n.y/sqrt(n.x*n.x+n.y*n.y),n.x/sqrt(n.x*n.x+n.y*n.y),0); //define a vector perpendicular to norm
	//Vec n_p = makeVec(0,-n.z/sqrt(n.z*n.z+n.y*n.y),n.y/sqrt(n.z*n.z+n.y*n.y)); //define a vector perpendicular to norm
	//Vec n_p = makeVec(-n.z/sqrt(n.z*n.z+n.x*n.x),0,n.x/sqrt(n.z*n.z+n.x*n.x)); //define a vector perpendicular to norm
	if (disp>0) {
	printf("\n n_p %f, %f, %f", n_p.x, n_p.y, n_p.z);
	}
	double len_p2 = sqrt((n.x*n.x+n.y*n.y)*(n.x*n.x+n.y*n.y+n.z*n.z));
	Vec n_p2 = makeVec(-n.x*n.z/len_p2, -n.y*n.z/len_p2, (n.x*n.x+n.y*n.y)/len_p2); //define a vector perpendicular to both n and n_p
	
	//double phi, theta = rand01()*2*M_PI;
	double phi1, theta1, sigma1;
	//sigma = 37.7e-6;
	sigma1 = 1e-6; // sigma1 is half-sigma (unit in rad): sigma1=5e-6 means the FWHM is 23.6 urad
	phi1 = sigma1*randnorm();//randgaussian(0.0,sigma1);
	theta1 = 250*sigma1*randnorm();//randgaussian(0.0,sigma1);
	if (disp>0) {
	printf("\n phi and theta %f, %f", phi1, theta1);
	}
	
	Vec R1 = rotateM(n,phi1,n_p);	//rotate n by phi according to n_p
	if (disp>0) {
	printf("\n R1 %f, %f, %f", R1.x, R1.y, R1.z);
	}
	Vec R2 = rotateM(R1,theta1,n_p2);	//rotate R1 by theta according to n_p2
	//Vec R2 = rotateM(R1,theta1,n);	//rotate R1 by theta according to n_p2
	n = R2;
	//
	//n.y = n.y+theta1;
	//n.z = n.z;
	
	// end of adding figure error

    double v = getMagVec(pv);
    double vn = dotVec(pv,n);
    
    //Hitting shell from outside
    if (vn > 0) {
        absorb_class_particle(_particle);
        return -1;
    }

    double ga = fabs(acos(vn/v)) - M_PI/2;
    double gc = 6.84459399932*s.m/v;
    double ref=1.0;
    if (ga > gc) {
        absorb_class_particle(_particle);
        return -1;
    }
    else {
        _particle->vx = _particle->vx-2*vn*n.x;
        _particle->vy = _particle->vy-2*vn*n.y; 
        _particle->vz = _particle->vz-2*vn*n.z; 
        double q = V2Q*(-2)*vn/sqrt(pv.x*pv.x + pv.y*pv.y + pv.z*pv.z);
        double par[5] = {s.R0, s.Qc, s.alpha, s.m, s.W};
        StdReflecFunc(q, par, &ref);
	_particle->p = _particle->p * ref;
    if (disp>0) {
		printf("\n pv final = %f, %f, %f",pv.x,pv.y,pv.z);
	}
	}
    return ga;
}

/*! \brief Function to handle refraction of neutron.

@warning Make sure particle has been moved to surface of mirror
before computing reflection

@param p Pointer of particle to reflect/refract
@param n Normal vector
@param m1 m-value of the material we come from
@param m2 m-value of the goal material we go to

@return Value of critical angle of the neutron or -1 if neutron is absorbed

@see traceNeutronConic()
*/
void refractNeutronFlat(_class_particle* p, Vec n, double m1, double m2) {
    Vec pv = get_class_particleVel(*p);
    //printf("incoming %.14g %.14g %.14g\n", pv.x, pv.y, pv.z);
    //printf("normal %.14g %.14g %.14g\n", n.x, n.y, n.z);
    double v = getMagVec(pv);
    double vn = dotVec(pv, n);
    //printf("vn %.9g", vn);
    Vec v_p = difVec(pv, skalarVec(n, vn));

    double k2_perp = POT_V*(m1*m1-m2*m2)+vn*vn;
    //printf("the magnitude %f\n", k2_perp);
    if (k2_perp>0){//refraction
        
        k2_perp = sqrt(k2_perp)*sign(vn);
        p->vx = v_p.x + n.x*k2_perp;
        p->vy = v_p.y + n.y*k2_perp;
        p->vz = v_p.z + n.z*k2_perp;
        p->_mctmp_a *= -1;// from silicon to air or vice versa
        //printf("resulting vector %f %f %f\n", p->vx, p->vy, p->vz);
    }else{//total reflection, no change in material
        p->vx = p->vx-2*vn*n.x;
        p->vy = p->vy-2*vn*n.y;
        p->vz = p->vz-2*vn*n.z;
        //no change in material
    }

    
}

/*! \brief Function to handle reflection of neutron for a FlatSurf.

@note Uses step function for reflectivity

@warning Make sure particle has been moved to surface of mirror
before computing reflection

@param p Pointer of particle to reflect
@param s FlatSurf to use

@return Value of critical angle of the neutron or -1 if neutron is absorbed

@see traceNeutronConic()
*/
double reflectNeutronFlat(_class_particle* _particle, FlatSurf s) {
    Vec n = getNormFlat(get_class_particlePos(*_particle),s);
    Vec pv = get_class_particleVel(*_particle);
    //printf("nothing");
    double v = getMagVec(pv);
    double vn = dotVec(pv,n);
    //printf("before %f \n", p->w);
    //Hitting shell from outside For FlatSurface this has to be checked
    // make it able to reflect from the outside
    double ga = fabs(acos(vn/v)) - M_PI/2;
    double gc = 6.84459399932*s.m/v;
    double ref=1.0;

    double q = V2Q*(-2)*vn/sqrt(pv.x*pv.x + pv.y*pv.y + pv.z*pv.z);
    double par[5] = {s.R0, s.Qc, s.alpha, s.m, s.W};
    StdReflecFunc(q, par, &ref);

    _particle->p = _particle->p * ref;

    if (ref < 0) {
        printf("this happens?");
        absorb_class_particle(_particle);
        return -1;
    } else {//here we need to implement the refraction
        if (rand01() <= ref){//to be updated to use the mcstas random function or quasoi deterministic model
            //printf("oh a reflections\n");
            //printf("this total reflection?");
            _particle->vx = _particle->vx-2*vn*n.x;
            _particle->vy = _particle->vy-2*vn*n.y;
            _particle->vz = _particle->vz-2*vn*n.z;
            return ga;
        }
        else{//
            //printf("enter the refraction process");
            double m1 = 0;
            double m2 = 0;
            //if no mirrorwidth is specified no refraction has to be calc
            if (_particle->_mctmp_a == 0){
                return ga;
            }
            if (_particle->_mctmp_a == 1){
                m1 = m_Si;
                m2 = 0;
            }
            if (_particle->_mctmp_a == -1){
                m1 = 0;
                m2 = m_Si;                
            }
            //printf("k1 %f k2 %f k3 %f x %f y %f z %f\n", s.k1, s.k2, s.k3, p->_x, p->_y, p->_z);
            refractNeutronFlat(_particle, n, m1, m2);// this can still lead to total reflection, we miss the supermirror, but are still reflected by the silicon takes care of change of material for refraction
            return ga;
        }
    }
    return ga;
}


/*! \brief Function to handle raytracing of neutron for a ConicSurf.

@param p Pointer of particle to reflect
@param c ConicSurf to use

*/
void traceNeutronConic(_class_particle* _particle, ConicSurf c) {
    double t = getTimeOfFirstCollisionConic(*_particle, c);
    if (t < 0)
        return;
    else {
        move_class_particleT(t, _particle);
		SCATTER_func(_particle);
        double ga = reflectNeutronConic(_particle, c);
#if REC_MAX_GA
        if (ga > c.max_ga) {
            c.max_ga = ga;
            c.max_ga_z0 = _particle->z;
        }
#endif
    }
}

/*! \brief Function to handle raytracing of neutron for a FlatSurf.

@param p Pointer of particle to reflect
@param f FlatSurf to use

*/
void traceNeutronFlat(_class_particle* _particle, FlatSurf f) {
    double t = getTimeOfFirstCollisionFlat(*_particle, f);
    if (t < 0)
        return;
    else {

        move_class_particleT(t, _particle);
		SCATTER_func(_particle);
        double ga = reflectNeutronFlat(_particle, f);

#if REC_MAX_GA
        if (ga > f.max_ga) {
            f.max_ga = ga;
            f.max_ga_z0 = p->_z;
        }
#endif
    }
}
/** @} */ //end of conicgroup
/////////////////////////////////////
// Scene Functions
/////////////////////////////////////
/** @ingroup simgroup
    @{
*/
enum GEO {
    NONE,
    DETECTOR,
    DISK,
    CONIC,
    FLAT
};

//! Function to generate an empty Scene
Scene makeScene() {
    Scene s;
    s.num_f = 0;
    s.num_c = 0;
    s.num_di = 0;
    s.num_d = 0;
    return s;
}

//! Function to init simulation items
/*! Should be called after all items
have been added to scene but before
neutrons are traced.

@param s Pointer of Scene to init
*/
void initSimulation(Scene* s) {
    //
}

/*! \brief Function to raytrace single neutron through geometries specified by d, di and c.

@param p Pointer of particle to trace
@param s Scene to trace
*/
void traceSingleNeutron(_class_particle* _particle, Scene s) {
   
    int contact = 1;
    do {
        double t;
        enum  GEO type = NONE;
        int index = -1;
        int i;

        for (i = 0; i < s.num_c; i++) {
            double t2 = getTimeOfFirstCollisionConic(*_particle,s.c[i]);

            if (t2 <= 0)
                continue;
            if (index == -1 || t2 < t) {
                type = CONIC;
                index = i;
                t = t2;
            }
        }

        for (i = 0; i < s.num_f; i++) {
            double t2 = getTimeOfFirstCollisionFlat(*_particle,s.f[i]);

            if (t2 <= 0)
                continue;
            if (index == -1 || t2 < t) {
                type = FLAT;
                index = i;
                t = t2;
            }
        }

        for (i = 0; i < s.num_di; i++)  {
            double t2 = getTimeOfFirstCollisionDisk(*_particle,s.di[i]);

            if (t2 <= 0)
                continue;
            else if (index == -1 || t2 < t) {
                type = DISK;
                index = i;
                t = t2;
            }
        }

        for (i = 0; i < s.num_d; i++) {
            double t2 = getTimeOfFirstCollisionDetector(*_particle,s.d[i]);

            if (t2 <= 0)
                continue;
            else if (index == -1 || t2 < t) {
                type = DETECTOR;
                index = i;
                t = t2;
            }
        }

        switch (type) {
            case DETECTOR:
                traceNeutronDetector(_particle, s.d[index]);
                break;
            case FLAT:
	        traceNeutronFlat(_particle, s.f[index]);
                break;
            case DISK:
                traceNeutronDisk(_particle, s.di[index]);
                break;
            case CONIC:
                traceNeutronConic(_particle, s.c[index]);
                break;
            default:
                contact = 0;
                break;
        }
    } while (contact && !_particle->_absorbed);

}

//!Finishes tracing the scene
/*! This function should be called after all of the
particles have been raytraced.

@param s Pointer of Scene to finish tracing
*/
void finishSimulation(Scene* s) {
    int i;

    //Finish Detectors
    for (i=0; i < s->num_d; i++)
        finishDetector(s->d[i]);
}

/** @} */ //end of ingroup simgroup

#endif
