#include "ClothSystem.h"
#include <cmath>

ClothSystem::ClothSystem(int width, int height, int systemState[])
{
	this->wind = 0;
	this->wireframe = true;
	this->width = width;
	this->height = height;
	this->systemState = systemState[0];
	this->m_numParticles = width * height;
	this->fixed_points = vector<bool>(width);
	this->normals = vector<Vector3f>(m_numParticles);
	//fix points
	fixed_points[0] = 1;
	fixed_points[width - 1] = 1;
	int step = width / 4;
	for (int i = step; i < width - 1; i += step) {
		fixed_points[i] = 1;
	}
	//no motion
	Vector3f velocity(0, 0, 0);
	Vector3f originPos(-3, 5, -13.1);
	originPos.x() += systemState[1] * 0.2f;
	originPos.y() += systemState[2] * 0.2f;
	originPos.z() += systemState[3] * 0.2f;
	for (int i = 0; i < height; i++) {
		for (int j = 0; j < width; j++) {
			Vector3f position(j * particleDistance, -i * particleDistance, 0);
			position += originPos;
			//initialize state
			m_vVecState.push_back(position);
			m_vVecState.push_back(velocity);
		}
	}
	// Define cloth faces
	for (int i = 0; i < width; i++) {
		for (int j = 0; j < height; j++) {
			if (i > 0 && j > 0) {
				// counter-clockwise winding for front-facing rendering
				faces.push_back(Vector3f(indexOf(i - 1, j - 1), indexOf(i - 1, j), indexOf(i, j - 1)));
				faces.push_back(Vector3f(indexOf(i, j), indexOf(i, j - 1), indexOf(i - 1, j)));
			}
		}
	}
}

//helper function
// given index i,j into a n x n cloth, returns the linear index into vector of particles
int ClothSystem::indexOf(int i, int j) {
	return i * width + j;
}

void ClothSystem::move() {
	for (int j = 0; j < fixed_points.size(); j++) {
		bool fixed = fixed_points[j];
		if (fixed) {
			int pin = indexOf(0, j);
			float z = m_vVecState[2 * pin].z();
			//limit the moving range
			if (z > moveUpperBound) {
				moveDir = Vector3f(0, 0, -1);
			}
			else if (z < moveLowerBound) {
				moveDir = Vector3f(0, 0, 1);
			}
			m_vVecState[2 * pin] += moveDir * Vector3f(0, 0, 0.06);
		}
	}
}

//helper function
// compare between f and frictionForce
float ClothSystem::compareForce(float f, float friction) {
	if (abs(f) <= abs(friction)) {
		return 0;
	}
	else {
		if ((f < 0 && friction < 0) || (f > 0 && friction > 0)) {
			return f - friction;
		}
		else {
			return f + friction;
		}
	}
}

// for a given state, evaluate f(X,t)
vector<Vector3f> ClothSystem::evalF(vector<Vector3f> state)
{
	vector<Vector3f> f;
	//gravity force is constant->m*g
	Vector3f fGravity = mass * gravity;
	Vector3f ball = Vector3f(0, -0.1f * height, -0.5f*height);
	float epsilon = 0.125f;
	float rad = 0.5f * particleDistance * width;
	float windforce = 0.5f;
	for (int i = 0; i < height; i++) {
		for (int j = 0; j < width; j++) {
			
			//fix multiple particles - hang the cloth
			if (systemState == 0) {
				if (i == 0 && fixed_points[j]) {
					f.push_back(Vector3f(0.0f));
					f.push_back(Vector3f(0.0f));
					continue;
				}
			}
			int curIndex = indexOf(i, j);
			Vector3f position = state[2 * curIndex];
			Vector3f velocity = state[2 * curIndex + 1];
			//drag force
			Vector3f fDrag = -1 * dampling * velocity;
			//structural spring force
			Vector3f fStructuralSpring;
			//means exist a neighbor on the top
			bool top = i > 0;
			bool bottom = i < height - 1;
			bool left = j > 0;
			bool right = j < width - 1;
			//top
			if (top) {
				int newIndex = indexOf(i - 1, j);
				Vector3f distance = position - state[2 * newIndex];
				fStructuralSpring += structuralStiffness * (structuralRestLength - distance.abs()) * (distance / distance.abs());
			}
			//bottom
			if (bottom) {
				int newIndex = indexOf(i + 1, j);
				Vector3f distance = position - state[2 * newIndex];
				fStructuralSpring += structuralStiffness * (structuralRestLength - distance.abs()) * (distance / distance.abs());
			}
			//left
			if (left) {
				int newIndex = indexOf(i, j - 1);
				Vector3f distance = position - state[2 * newIndex];
				fStructuralSpring += structuralStiffness * (structuralRestLength - distance.abs()) * (distance / distance.abs());
			}
			//right
			if (right) {
				int newIndex = indexOf(i, j + 1);
				Vector3f distance = position - state[2 * newIndex];
				fStructuralSpring += structuralStiffness * (structuralRestLength - distance.abs()) * (distance / distance.abs());
			}

			//shear spring force
			Vector3f fShearSpring;
			//left top
			if (left & top) {
				int newIndex = indexOf(i - 1, j - 1);
				Vector3f distance = position - state[2 * newIndex];
				fShearSpring += shearStiffness * (shearRestLength - distance.abs()) * (distance / distance.abs());
			}
			//left bottom
			if (left & bottom) {
				int newIndex = indexOf(i + 1, j - 1);
				Vector3f distance = position - state[2 * newIndex];
				fShearSpring += shearStiffness * (shearRestLength - distance.abs()) * (distance / distance.abs());
			}
			//right top
			if (right & top) {
				int newIndex = indexOf(i - 1, j + 1);
				Vector3f distance = position - state[2 * newIndex];
				fShearSpring += shearStiffness * (shearRestLength - distance.abs()) * (distance / distance.abs());
			}
			//right bottom
			if (right & bottom) {
				int newIndex = indexOf(i + 1, j + 1);
				Vector3f distance = position - state[2 * newIndex];
				fShearSpring += shearStiffness * (shearRestLength - distance.abs()) * (distance / distance.abs());
			}
			//flex spring force
			Vector3f fFlexSpring;
			//top
			if (i > 1) {
				int newIndex = indexOf(i - 2, j);
				Vector3f distance = position - state[2 * newIndex];
				fFlexSpring += flexStiffness * (flexRestLength - distance.abs()) * (distance / distance.abs());
			}
			//bottom
			if (i < height - 2) {
				int newIndex = indexOf(i + 2, j);
				Vector3f distance = position - state[2 * newIndex];
				fFlexSpring += flexStiffness * (flexRestLength - distance.abs()) * (distance / distance.abs());
			}
			//left
			if (j > 1) {
				int newIndex = indexOf(i, j - 2);
				Vector3f distance = position - state[2 * newIndex];
				fFlexSpring += flexStiffness * (flexRestLength - distance.abs()) * (distance / distance.abs());
			}
			//right
			if (j < width - 2) {
				int newIndex = indexOf(i, j + 2);
				Vector3f distance = position - state[2 * newIndex];
				fFlexSpring += flexStiffness * (flexRestLength - distance.abs()) * (distance / distance.abs());
			}

			//friction force
			float frictionForce;
			int newIndex = indexOf(i, j);
			if (systemState == 2) {
				if ((m_vVecState[2 * newIndex] - ball).abs() <= (rad + epsilon)) {
					frictionForce = frictionRate;
				}
				else {
					frictionForce = 0;
				}
			}

			//net force
			Vector3f fNet = fGravity + fDrag + fStructuralSpring + fShearSpring + fFlexSpring;
			if (systemState == 2) {
				fNet.x() = compareForce(fNet.x(), frictionForce);
				fNet.y() = compareForce(fNet.y(), frictionForce);
				fNet.z() = compareForce(fNet.z(), frictionForce);
			}
			if (wind%3!=0) {
				if (wind % 3 == 2) {
					windforce = 2.0f;
				}
				fNet += Vector3f(0.0f, 0.0f, windforce * (rand()) / (static_cast <float> (RAND_MAX)));
			}
			Vector3f acceleration = fNet / mass;
			f.push_back(velocity);
			f.push_back(acceleration);
		}
	}
	return f;



}
Vector3f triangleNormal(Vector3f v1, Vector3f v2, Vector3f v3) {
	return Vector3f::cross(v2 - v1, v3 - v1).normalized();
}

void ClothSystem::draw()
{
	//draw the ball
	glColorMaterial(GL_FRONT_AND_BACK, GL_DIFFUSE);
	glEnable(GL_COLOR_MATERIAL);
	glColor3f(0.0f, 1.0f, 0.8f);
	Vector3f ball = Vector3f(0, -0.1f * height, -0.5f*height);
	float rad = 0.5f * particleDistance * width;
	float epsilon = 0.125f;
	glPushMatrix();
	glTranslatef(ball.x(), ball.y(), ball.z());
	glutSolidSphere(rad, 40.0f, 40.0f);
	glPopMatrix();
	glDisable(GL_COLOR_MATERIAL);
	for (int n = 0; n < m_numParticles; n++) {
		if ((m_vVecState[2 * n] - ball).abs() <= (rad + epsilon)) {
			m_vVecState[2 * n] = (ball + (rad + epsilon) * (m_vVecState[2 * n] - ball).normalized());
		}
	}

	if(!wireframe){// display wireframe
		glEnable(GL_COLOR_MATERIAL);
		glColor3f(1.0f, 1.0f, 1.0f);
		glDisable(GL_COLOR_MATERIAL);
		for (int i = 0; i < height; i++) {
			for (int j = 0; j < width; j++) {
				//draw particle
				Vector3f pos(m_vVecState[2 * indexOf(i, j)]);
				//draw structural springs
				glLineWidth(2);
				glBegin(GL_LINES);
				float groundHeight = -0.5f * height;
				//not in the last col
				if (j < width - 1) {
					Vector3f rightPos(m_vVecState[2 * indexOf(i, j + 1)]);
					glVertex3f(pos[0], pos[1] < groundHeight ? groundHeight : pos[1], pos[2]);
					glVertex3f(rightPos[0], rightPos[1] < groundHeight ? groundHeight : rightPos[1], rightPos[2]);
				}
				//not in the last row
				if (i < height - 1) {
					Vector3f bottomPos(m_vVecState[2 * indexOf(i + 1, j)]);
					glVertex3f(pos[0], pos[1] < groundHeight ? groundHeight : pos[1], pos[2]);
					glVertex3f(bottomPos[0], bottomPos[1] < groundHeight ? groundHeight : bottomPos[1], bottomPos[2]);
				}
				glEnd();
			}
		}
	}
	

	else if(wireframe){
		//compute vertex normals
		glEnable(GL_COLOR_MATERIAL);
		glColor3f(1.0f, 1.0f, 1.0f);
		glDisable(GL_COLOR_MATERIAL);
		for (int i = 0; i < width; i++) {
			for (int j = 0; j < height; j++) {
				Vector3f normal(0, 0, 0);
				if (i > 0 && j > 0) {
					normal += triangleNormal(m_vVecState[2 * indexOf(i, j - 1)], m_vVecState[2 * indexOf(i - 1, j)], m_vVecState[2 * indexOf(i, j)]);
				}
				if (i < width - 1 && j > 0) {
					normal += triangleNormal(m_vVecState[2 * indexOf(i, j - 1)], m_vVecState[2 * indexOf(i, j)], m_vVecState[2 * indexOf(i + 1, j - 1)]);
					normal += triangleNormal(m_vVecState[2 * indexOf(i, j)], m_vVecState[2 * indexOf(i + 1, j)], m_vVecState[2 * indexOf(i + 1, j - 1)]);
				}
				if (i > 0 && j < height - 1) {
					normal += triangleNormal(m_vVecState[2 * indexOf(i, j)], m_vVecState[2 * indexOf(i - 1, j)], m_vVecState[2 * indexOf(i - 1, j + 1)]);
					normal += triangleNormal(m_vVecState[2 * indexOf(i, j)], m_vVecState[2 * indexOf(i - 1, j + 1)], m_vVecState[2 * indexOf(i, j + 1)]);
				}
				if (i < width - 1 && j < height - 1) {
					normal += triangleNormal(m_vVecState[2 * indexOf(i + 1, j)], m_vVecState[2 * indexOf(i, j)], m_vVecState[2 * indexOf(i, j + 1)]);
				}
				normals[indexOf(i, j)] = normal.normalized();
			}
		}

		//draw triangles 
		for (size_t i = 0; i < this->faces.size(); i++) {
			Vector3f face = faces[i];

			Vector3f Va = m_vVecState[2 * ((int)face[0])];
			Vector3f Vb = m_vVecState[2 * ((int)face[1])];
			Vector3f Vc = m_vVecState[2 * ((int)face[2])];
			Vector3f NorVa = this->normals[(int)face[0]];
			Vector3f NorVb = this->normals[(int)face[1]];
			Vector3f NorVc = this->normals[(int)face[2]];

			glBegin(GL_TRIANGLES);
			glNormal3fv(NorVa);
			glVertex3fv(Va);
			glNormal3fv(NorVb);
			glVertex3fv(Vb);
			glNormal3fv(NorVc);
			glVertex3fv(Vc);
			glEnd();//front side

			glBegin(GL_TRIANGLES);
			glNormal3fv(-NorVa);
			glVertex3fv(Va);
			glNormal3fv(-NorVc);
			glVertex3fv(Vc);
			glNormal3fv(-NorVb);
			glVertex3fv(Vb);
			glEnd();//back side
		}
	}
	
}

