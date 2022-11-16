#include "ClothSystem.h"

ClothSystem::ClothSystem(int width, int height)
{
    this->width = width;
    this->height = height;
    this->m_numParticles = width * height;
    //no motion
    Vector3f velocity(0,0,0);
    Vector3f originPos(0,2,0);
    for(int i = 0;i<height;i++){
        for(int j = 0;j<width;j++){
            Vector3f position(j*particleDistance,-i*particleDistance,0);
            position += originPos;
            //initialize state
            m_vVecState.push_back(position);
            m_vVecState.push_back(velocity);
        }
    }
}

//helper function
// given index i,j into a n x n cloth, returns the linear index into vector of particles
int ClothSystem::indexOf(int i, int j) {
    return i * width + j;
}

// for a given state, evaluate f(X,t)
vector<Vector3f> ClothSystem::evalF(vector<Vector3f> state)
{
    vector<Vector3f> f;
    //gravity force is constant->m*g
    Vector3f fGravity = mass * gravity;
    for(int i = 0;i<height;i++){
        for(int j = 0;j<width;j++){
            //fix 2 particles - hang the cloth
            if(i == 0 && j == 0){
                f.push_back(Vector3f(0.0f));
                f.push_back(Vector3f(0.0f));
                continue;
            }
            if(i == 0 && j == width-1){
                f.push_back(Vector3f(0.0f));
                f.push_back(Vector3f(0.0f));
                continue;
            }
            int curIndex = indexOf(i,j);
            Vector3f position = state[2*curIndex];
            Vector3f velocity = state[2*curIndex+1];
            //drag force
            Vector3f fDrag = -1 * dampling * velocity;
            //structural spring force
            Vector3f fStructuralSpring;
            //means exist a neighbor on the top
            bool top = i>0;
            bool bottom = i<height-1;
            bool left = j>0;
            bool right = j<width-1;
            //top
            if(top){
                int newIndex = indexOf(i-1,j);
                Vector3f distance = position - state[2*newIndex];
                fStructuralSpring += structuralStiffness*(structuralRestLength-distance.abs()) * (distance/distance.abs());
            }
            //bottom
            if(bottom){
                int newIndex = indexOf(i+1,j);
                Vector3f distance = position - state[2*newIndex];
                fStructuralSpring += structuralStiffness*(structuralRestLength-distance.abs()) * (distance/distance.abs());
            }
            //left
            if(left){
                int newIndex = indexOf(i,j-1);
                Vector3f distance = position - state[2*newIndex];
                fStructuralSpring += structuralStiffness*(structuralRestLength-distance.abs()) * (distance/distance.abs());
            }
            //right
            if(right){
                int newIndex = indexOf(i,j+1);
                Vector3f distance = position - state[2*newIndex];
                fStructuralSpring += structuralStiffness*(structuralRestLength-distance.abs()) * (distance/distance.abs());
            }

            //shear spring force
            Vector3f fShearSpring;
            //left top
            if(left & top){
                int newIndex = indexOf(i-1,j-1);
                Vector3f distance = position - state[2*newIndex];
                fShearSpring += shearStiffness*(shearRestLength-distance.abs()) * (distance/distance.abs());
            }
            //left bottom
            if(left & bottom){
                int newIndex = indexOf(i+1,j-1);
                Vector3f distance = position - state[2*newIndex];
                fShearSpring += shearStiffness*(shearRestLength-distance.abs()) * (distance/distance.abs());
            }
            //right top
            if(right & top){
                int newIndex = indexOf(i-1,j+1);
                Vector3f distance = position - state[2*newIndex];
                fShearSpring += shearStiffness*(shearRestLength-distance.abs()) * (distance/distance.abs());
            }
            //right bottom
            if(right & bottom){
                int newIndex = indexOf(i+1,j+1);
                Vector3f distance = position - state[2*newIndex];
                fShearSpring += shearStiffness*(shearRestLength-distance.abs()) * (distance/distance.abs());
            }
            //flex spring force
            Vector3f fFlexSpring;
            //top
            if(i>1){
                int newIndex = indexOf(i-2,j);
                Vector3f distance = position - state[2*newIndex];
                fFlexSpring += flexStiffness*(flexRestLength-distance.abs()) * (distance/distance.abs());
            }
            //bottom
            if(i<height-2){
                int newIndex = indexOf(i+2,j);
                Vector3f distance = position - state[2*newIndex];
                fFlexSpring += flexStiffness*(flexRestLength-distance.abs()) * (distance/distance.abs());
            }
            //left
            if(j>1){
                int newIndex = indexOf(i,j-2);
                Vector3f distance = position - state[2*newIndex];
                fFlexSpring += flexStiffness*(flexRestLength-distance.abs()) * (distance/distance.abs());
            }
            //right
            if(j<width-2){
                int newIndex = indexOf(i,j+2);
                Vector3f distance = position - state[2*newIndex];
                fFlexSpring += flexStiffness*(flexRestLength-distance.abs()) * (distance/distance.abs());
            }
            //net force
            Vector3f fNet = fGravity+fDrag+fStructuralSpring+fShearSpring+fFlexSpring;
            Vector3f acceleration = fNet/mass;
            f.push_back(velocity);
            f.push_back(acceleration);
        }
    }
	return f;



}

void ClothSystem::draw()
{
    for(int i = 0;i<height;i++){
        for(int j = 0;j<width;j++){
            //draw particle
            Vector3f pos(m_vVecState[2* indexOf(i,j)]);
            //draw structural springs
            glLineWidth(2);
            glBegin(GL_LINES);
            //not in the last col
            if(j<width-1){
                Vector3f rightPos(m_vVecState[2* indexOf(i,j+1)]);
                glVertex3f(pos[0],pos[1],pos[2]);
                glVertex3f(rightPos[0],rightPos[1],rightPos[2]);
            }
            //not in the last row
            if(i<height-1){
                Vector3f bottomPos(m_vVecState[2* indexOf(i+1,j)]);
                glVertex3f(pos[0],pos[1],pos[2]);
                glVertex3f(bottomPos[0],bottomPos[1],bottomPos[2]);
            }
            glEnd();
        }
    }
}

