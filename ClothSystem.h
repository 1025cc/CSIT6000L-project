#ifndef CLOTHSYSTEM_H
#define CLOTHSYSTEM_H

#include <vecmath.h>
#include <vector>
#ifdef _WIN32
#include "GL/freeglut.h"
#else
#include <GL/glut.h>
#endif

#include "particleSystem.h"



class ClothSystem : public ParticleSystem
{
    ///ADD MORE FUNCTION AND FIELDS HERE
public:
    ClothSystem();
    ClothSystem(int width, int height, int systemState[]);
    vector<Vector3f> evalF(vector<Vector3f> state);
    int indexOf(int i, int j);
    float compareForce(float f, float friction);
    void draw();
    void move();
    void computeVertexNormals();
    vector<Vector3f> normals;
    vector<Vector3f> faces;

protected:
    //cloth size(particle nums)
    int width = 8;
    int height = 8;
    int systemState = 0;//systemState: 0->fixed particles; 1->the cloth falls; 2-> add friction 
    //initial distance between particles
    float particleDistance = 0.3f;
    //some physical parameters
    float dampling = 0.1f;
    Vector3f gravity = Vector3f(0.0f, -9.8f, 0.0f);
    float mass = 0.04f;
    float structuralStiffness = 25.0f;
    float structuralRestLength = particleDistance;
    float shearStiffness = 15.0f;
    float shearRestLength = particleDistance * sqrt(2);
    float flexStiffness = 4.0f;
    float flexRestLength = particleDistance * 2;
    float frictionRate = 3.0f;
    //move
    float moveUpperBound = 2;
    float moveLowerBound = -2;
    Vector3f moveDir = (0, 0, 1);
    vector<bool> fixed_points;

};


#endif
