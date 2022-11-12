
#include "pendulumSystem.h"

//generate a random value from [start,end]
float randomF(float start,float end){
    float randomValue = (end - start)*rand()/RAND_MAX + start;
    return randomValue;
}

PendulumSystem::PendulumSystem(int numParticles):ParticleSystem(numParticles)
{
	m_numParticles = numParticles;
    m_vVecState = vector<Vector3f>(2*numParticles);
    springList = vector<Spring>(numParticles-1);
	// fill in code for initializing the state based on the number of particles
	for (int i = 0; i < m_numParticles; i++) {
		// for this system, we care about the position and the velocity
        //no motion
        Vector3f velocity(0,0,0);
        Vector3f position;
        if(i == 0){
            //fix the first particle
            position = Vector3f(0,1,0);
        }else{
            //create a spring to connect the previous particle
            Spring spring(i-1,i,restLength,stiffness);
            springList[i-1] = spring;
            //random position according to the previous position
            Vector3f prePosition = m_vVecState[2*(i-1)];
            position = Vector3f(prePosition[0]+randomF(-1,1),prePosition[1]-randomF(-1,1),prePosition[2]+randomF(-1,1));
        }
        m_vVecState[2*i] = position;
        m_vVecState[2*i+1] = velocity;
	}
}


// TODO: implement evalF
// for a given state, evaluate f(X,t)
vector<Vector3f> PendulumSystem::evalF(vector<Vector3f> state)
{
    int particleNum = state.size()/2;
	vector<Vector3f> f;
    //fix the first particle
    f.push_back(Vector3f(0.0f));
    f.push_back(Vector3f(0.0f));

    // gravity force is constant->m*g
    Vector3f fGravity = mass * gravity;
    for(int i = 1;i<particleNum;i++){
        Vector3f position = state[2*i];
        Vector3f velocity = state[2*i+1];
        //drag force
        Vector3f fDrag = -1 * dampling * velocity;
        //spring force
        Vector3f fSpring;
        //spring1 between i-1 and i
        float restLenth1 = springList[i-1].restLength;
        float stiffness1 = springList[i-1].stiffness;
        Vector3f distance1 = position - state[2*(i-1)];
        fSpring = stiffness1*(restLenth1-distance1.abs()) * (distance1/distance1.abs());
        //not the last one
        if(i != particleNum-1){
            //spring2 between i and i+1
            float restLenth2 = springList[i].restLength;
            float stiffness2 = springList[i].stiffness;
            Vector3f distance2 = position - state[2*(i+1)];
            fSpring += stiffness2*(restLenth2-distance2.abs()) * (distance2/distance2.abs());
        }
        //net force
        Vector3f fNet = fGravity+fDrag+fSpring;
        Vector3f acceleration = fNet/mass;
        f.push_back(velocity);
        f.push_back(acceleration);
    }

	return f;
}

// render the system (ie draw the particles)
void PendulumSystem::draw()
{
	for (int i = 0; i < m_numParticles; i++) {
		Vector3f pos = m_vVecState[i*2];//  position of particle i. YOUR CODE HERE
		glPushMatrix();
		glTranslatef(pos[0], pos[1], pos[2] );
		glutSolidSphere(0.075f,10.0f,10.0f);
		glPopMatrix();
	}


}
