#ifndef MAIN_H
#define MAIN_H

#include "..\..\QUIX_Engine\include\QUIX.h"

#include <omp.h>
#include <string>

/* Some physics constants */
#define DAMPING 0.05f // how much to damp the cloth simulation each frame
#define TIME_STEPSIZE2 0.017f*0.017f // how large time step each particle takes each frame
#define CONSTRAINT_ITERATIONS 7 // how many iterations of constraint satisfaction each frame (more is rigid, less is soft)
#define PARTICLE_MASS 0.01f

#include "Particle.h"
#include "Constraint.h"
#include "Cloth.h"
#include "Demo.h"

#endif