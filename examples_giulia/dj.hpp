//
// Created by giuli on 06/03/2019.
//
#pragma once
#include "math_util.hpp"
void dj(QuESTEnv& env, int n) {

    // Create a type of function (either constant or balanced) at random (oracle)
    int oracleType = (int) sample_uniformly();
    int oracleValue = (int) sample_uniformly();
    std::cout << "oracleType = " << oracleType << ", oracleValue = " << oracleValue << std::endl;
    std::string type = oracleType == 0 ? "the constant value" : "a balanced function";
    std::cout << "The oracle returns " << type << std::endl;

    // choose a balanced function at random and represent it by the long long value f
    long long f = sample_uniformly(1, (1 << n) - 1); 
    std::cout << "f = " << f << std::endl;

    // Construct the circuit
    // creating registers
    Qureg qureg = createQureg(n+1, env); // create quantum register with n qubits and 1 ancilla qubit
    initPlusState(qureg);// initialize all qubits to an equal superposition state
    pauliZ(qureg, n); // necessary in order to apply the phase oracle

    //implement the bit oracle
    if (oracleType == 0){ // If the oracleType is "0" (constant function), the oracle returns oracleValue for all input
        if (oracleValue == 1){
            pauliX(qureg, n);
        }
    }
    else {
        for (int i = 0; i < n ; ++i) { // this represents a way to define balanced functions which are represented by f
            if (f & (1 << i)){
                controlledNot(qureg,i,n);
            }

        }

    }

    // Apply again hadamard on all qubits
    for (int i = 0; i < n; ++i) {
        hadamard(qureg,i);
    }

    // measure the qubits
    qreal realprob = getProbAmp(qureg, 0) + getProbAmp(qureg,1<<n); // get the probability amplitude of state |00.....0>
    // TODO: find a better way to ignore the ancilla qubit!
    // NOTE: If the algorithm runs with real hardware backend, need to change the above line and measure all qubits separately.

    //int prob = int(realprob);


    if (realprob > 0.5){
        std::cout << "The DJ Algorithm finds the function f to be constant with value " << oracleValue << std::endl;
    }
    else {
        std::cout << "The DJ Algorithm finds the function f to be balanced." << std::endl;
    }

    // unload quest
    destroyQureg(qureg, env);
}
