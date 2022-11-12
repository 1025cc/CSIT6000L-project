
#include "simpleSystem.h"

using namespace std;

SimpleSystem::SimpleSystem()
{
}

// TODO: implement evalF
// for a given state, evaluate f(X,t)
vector<Vector3f> SimpleSystem::evalF(vector<Vector3f> state)
{
	vector<Vector3f> f;
    for(int i = 0;i<state.size();i++){
        Vector3f fi(-state[i].y(),state[i].x(),0);
        f.push_back(fi);
    }
	return f;
}

// render the system (ie draw the particles)
void SimpleSystem::draw()
{
    //this simple system has only one particle
    for(Vector3f pos:m_vVecState){
        glPushMatrix();
        glTranslatef(pos[0], pos[1], pos[2] );
        glutSolidSphere(0.075f,10.0f,10.0f);
        glPopMatrix();
    }
}
