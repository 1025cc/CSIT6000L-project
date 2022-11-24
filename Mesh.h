//
// Created by cccccci on 2022/11/24.
//

#ifndef CSIT6000L_PROJECT_MESH_H
#define CSIT6000L_PROJECT_MESH_H
#include <cstdlib>
#include <fstream>
#include <iostream>
#include <sstream>
#include <vecmath.h>
#include <vector>
#if WIN32
#include "GL/freeglut.h"
#endif
#if __APPLE__
#include <GLUT/glut.h>
#else
#include <GL/glut.h>
#endif

struct NewVertex {
    // position
    Vector3f Position;
    // normal
    Vector3f Normal;
    // texCoords
    Vector2f TexCoords;
};
class Mesh {
public:
    Mesh();
    // list of vertices from the OBJ file
    // in the "bind pose"
    std::vector<NewVertex> vertices;

    // each face has 3 indices
    // referencing 3 vertices
    // This is the list of faces (indices into vecv and vecn)
    std::vector<unsigned > faces;

    // This is the list of points (3D vectors)
    std::vector<Vector3f> vecv;

// This is the list of normals (also 3D vectors)
    std::vector<Vector3f> vecn;

    std::vector<Vector2f> vect;
// This is the list of faces (indices into vecv and vecn)
    std::vector<std::vector<unsigned> > vecf;


    //load() should populate bindVertices, currentVertices, and faces
    void load(const char *filename);

    //draw the current mesh.
    void draw();
};


#endif //CSIT6000L_PROJECT_MESH_H
