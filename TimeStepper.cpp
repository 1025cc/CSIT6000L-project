#include "TimeStepper.hpp"

///TODO: implement Explicit Euler time integrator here
void ForwardEuler::takeStep(ParticleSystem* particleSystem, float stepSize)
{
    vector<Vector3f> X0 = particleSystem->getState();
    //Calculate f(X0,t0)
    vector<Vector3f> f0 = particleSystem->evalF(X0);
    vector<Vector3f> X1;
    //handle each point
    for(int i = 0;i<f0.size();i++){
        Vector3f X0i = X0.at(i);
        Vector3f f0i = f0.at(i);
        Vector3f X1i = X0i + stepSize * f0i;
        X1.push_back(X1i);
    }
    //update
    particleSystem->setState(X1);
}

///TODO: implement Trapzoidal rule here
void Trapzoidal::takeStep(ParticleSystem* particleSystem, float stepSize)
{
    vector<Vector3f> X0 = particleSystem->getState();
    //f0 = f(X0,t0)
    vector<Vector3f> f0 = particleSystem->evalF(X0);
    //X0 + hf0
    vector<Vector3f> X1;
    for(int i = 0;i<X0.size();i++){
        Vector3f X0i = X0[i];
        Vector3f f0i = f0[i];
        Vector3f X1i(X0i + stepSize * f0i);
        X1.push_back(X1i);
    }
    vector<Vector3f> f1 = particleSystem->evalF(X1);
    vector<Vector3f> newState;
    for(int i = 0;i<X0.size();i++){
        Vector3f X0i = X0[i];
        Vector3f f0i = f0[i];
        Vector3f f1i = f1[i];
        Vector3f newStatei = X0i + (stepSize/2.0f) * (f0i + f1i);
        newState.push_back(newStatei);
    }
    //update
    particleSystem->setState(newState);
}
void RK4::takeStep(ParticleSystem* particleSystem, float stepSize) {
    std::vector<Vector3f> state = particleSystem->getState();
    size_t stateSize = state.size();

    // k1
    std::vector<Vector3f> k1 = particleSystem->evalF(state);
    for (size_t i = 0; i < stateSize; ++i) {
        k1[i] *= stepSize;
    }

    // k2
    std::vector<Vector3f> k2Input;
    for (size_t i = 0; i < stateSize; ++i) {
        k2Input.push_back(state[i] + 0.5 * k1[i]);
    }
    std::vector<Vector3f> k2 = particleSystem->evalF(k2Input);
    for (size_t i = 0; i < stateSize; ++i) {
        k2[i] *= stepSize;
    }

    // k3
    std::vector<Vector3f> k3Input;
    for (size_t i = 0; i < stateSize; ++i) {
        k3Input.push_back(state[i] + 0.5 * k2[i]);
    }
    std::vector<Vector3f> k3 = particleSystem->evalF(k3Input);
    for (size_t i = 0; i < stateSize; ++i) {
        k3[i] *= stepSize;
    }

    // k4
    std::vector<Vector3f> k4Input;
    for (size_t i = 0; i < stateSize; ++i) {
        k4Input.push_back(state[i] + k3[i]);
    }
    std::vector<Vector3f> k4 = particleSystem->evalF(k4Input);
    for (size_t i = 0; i < stateSize; ++i) {
        k4[i] *= stepSize;
    }

    // update state
    std::vector<Vector3f> newState;
    for (size_t i = 0; i < state.size(); ++i) {
        newState.push_back(state[i] + (1.0 / 6.0) * (k1[i] + 2 * k2[i] + 2 * k3[i] + k4[i]));
    }

    particleSystem->setState(newState);
}

