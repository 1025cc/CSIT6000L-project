#ifndef PENDULUMSYSTEM_H
#define PENDULUMSYSTEM_H

#include <vecmath.h>
#include <vector>
#ifdef _WIN32
#include "GL/freeglut.h"
#else
#include <GLUT/glut.h>
#endif

#include "particleSystem.h"

class PendulumSystem: public ParticleSystem
{
public:
	PendulumSystem(int numParticles);
	
	vector<Vector3f> evalF(vector<Vector3f> state);
	
	void draw();
    //a spring connects i and j with restLengh L0
    class Spring{
        public:
            Spring(){

            }
            Spring(float i,float j,float restLength,float stiffness){
                this->i = i;
                this->j = j;
                this->restLength = restLength,
                this->stiffness = stiffness;
            }
            float i;
            float j;
            float restLength;
            float stiffness;
    };

protected:
    //some physical parameters
    float dampling = 0.1f;
    Vector3f gravity = Vector3f(0.0f,-9.8f,0.0f);
    float mass = 0.8f;
    float stiffness = 50.0f;
    float restLength =  1.0f;
    vector<Spring> springList;


};

#endif
