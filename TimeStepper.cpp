#include <iostream>
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

void RKF45::takeStep(ParticleSystem *particleSystem, float stepSize) {
    if(step == 0){
        step = stepSize;
    }
    float tolerance = 0.01f;
    std::vector<Vector3f> state = particleSystem->getState();
    size_t stateSize = state.size();
    bool stopAdapting = false;
    std::vector<Vector3f> newState;
    do {
        // k1 = h*f(tk,xk)
        std::vector<Vector3f> k1 = particleSystem->evalF(state);
        for (size_t i = 0; i < stateSize; ++i) {
            k1[i] *= step;
        }
        // k2 = h*f(tk + 1/4h,xk + 1/4k1)
        std::vector<Vector3f> k2State;
        for (size_t i = 0; i < stateSize; ++i) {
            k2State.push_back(state[i] + 1.0/4.0 * k1[i]);
        }
        std::vector<Vector3f> k2 = particleSystem->evalF(k2State);
        for (size_t i = 0; i < stateSize; ++i) {
            k2[i] *= step;
        }
        // k3 = h*f(tk + 3/8h,xk + 3/32k1 + 9/32k2)
        std::vector<Vector3f> k3State;
        for (size_t i = 0; i < stateSize; ++i) {
            k3State.push_back(state[i] + 3.0/32.0 * k1[i] + 9.0/32.0 * k2[i]);
        }
        std::vector<Vector3f> k3 = particleSystem->evalF(k3State);
        for (size_t i = 0; i < stateSize; ++i) {
            k3[i] *= step;
        }
        // k4 = h*f(tk + 12/13h,xk + 1932/2197k1 - 7200/2197k2 +7296/2197k3)
        std::vector<Vector3f> k4State;
        for (size_t i = 0; i < stateSize; ++i) {
            k4State.push_back(state[i] + 1932.0/2197.0 * k1[i] - 7200.0/2197.0 * k2[i] + 7296.0/2197.0 * k3[i]);
        }
        std::vector<Vector3f> k4 = particleSystem->evalF(k4State);
        for (size_t i = 0; i < stateSize; ++i) {
            k4[i] *= step;
        }
        // k5 = h*f(tk + h,xk + 439/216k1 - 8k2 +3680/513k3 -845/4104k4)
        std::vector<Vector3f> k5State;
        for (size_t i = 0; i < stateSize; ++i) {
            k5State.push_back(state[i] + 439.0/216.0 * k1[i] - 8 * k2[i] + 3690.0/513.0 * k3[i] - 845.0/4104.0 * k4[i]);
        }
        std::vector<Vector3f> k5 = particleSystem->evalF(k5State);
        for (size_t i = 0; i < stateSize; ++i) {
            k5[i] *= step;
        }
        // k6 = h*f(tk + 1/2h,xk - 8/27k1 + 2k2 -3544/2565k3 +1859/4104k4 - 11/40k5)
        std::vector<Vector3f> k6State;
        for (size_t i = 0; i < stateSize; ++i) {
            k6State.push_back(state[i] - 8.0/27.0 * k1[i] + 2 * k2[i] - 3544.0/2565.0 * k3[i] + 1859.0/4104.0 * k4[i] - 11.0/40.0 * k5[i]);
        }
        std::vector<Vector3f> k6 = particleSystem->evalF(k6State);
        for (size_t i = 0; i < stateSize; ++i) {
            k6[i] *= step;
        }
        //yk+1 = yk + 25/216 k1 + 1408/2565 k3 + 2197/4101 k4 − 1/5 k5
        //zk+1 = yk + 16/135 k1 + 6656/12,825 k3 + 28,561/56,430 k4 − 9/50 k5 + 2/55 k6
        //error = zk+1-yk+1
        std::vector<Vector3f> xRKF4;
        std::vector<Vector3f> xRKF5;
        std::vector<Vector3f> error;
        float maxError = -99999;
        for (size_t i = 0; i < state.size(); ++i) {
            Vector3f rkf4 = state[i] + (25.0 / 216.0) * k1[i] + 1408.0/2565.0 * k3[i] + 2197.0/4101.0 *k4[i] - 1.0/5.0 * k5[i];
            xRKF4.push_back(rkf4);
            Vector3f rkf5 = state[i] + (16.0 / 135.0) * k1[i] + 6656.0/12825.0 * k3[i] + 28561.0/56430.0 *k4[i] - 9.0/50.0 * k5[i] + 2.0/55.0 * k6[i];
            xRKF5.push_back(rkf5);
            Vector3f e =  rkf4 -rkf5;
            float eAbs = e.abs();
            maxError = eAbs>maxError?eAbs:maxError;
            error.push_back(e);
        }
        //if the error is too large，reduce the step size
        if(maxError > tolerance){
            float scale = 0.84 * pow((tolerance * step)/(maxError), 0.25);
            step = scale * step;
        } else{
            stopAdapting = true;
            newState = xRKF4;
        }
    }while(!stopAdapting);
    cout<<"step = "<<step<<endl;
    particleSystem->setState(newState);
}

