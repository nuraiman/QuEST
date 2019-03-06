//
// Created by giuli on 06/03/2019.
//

#include <iostream>
#include <QuEST.h>
#include <ctime>
#include <chrono>

#include <stdlib.h>

int main(int narg, char *varg[]) {
    //load quest
    QuESTEnv env = createQuESTEnv();

    int n = 13; // length of the boolean function f (~ number of qubits)
    //std::cout << "Insert the length of the n-bit string function f: n = " << std::endl;
    //std::cin >> n;

    // Create a type of function (either constant or balanced) at random (oracle)
    double oracleType = drand48(), oracleValue = drand48();
    std::cout << oracleType << " " << oracleValue << std::endl;
    if (oracleValue < 0.5){
        oracleValue = 0;
    }
    else {
        oracleValue = 1;
    }
    long int f = (long int) (1<<n)*drand48(); // choose a balanced function at random and represent it by the long integer f
    std::cout << f << std::endl;
    if (oracleType < 0.5) {
        oracleType = 0;
        std::cout << "The oracle returns the constant value " << oracleValue << std::endl;
    }
    else {
        std::cout << "The oracle returns a balanced function" << std::endl;
        oracleType =1;
    }

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
    qreal realprob = getProbAmp(qureg, 0) + getProbAmp(qureg,1<<n); // get the probability amplitude of state |00.....0> // TODO: find a better way to ignore the ancilla qubit!!
    //int prob = int(realprob);
    //std::cout << realprob << std::endl;
    //std::cout << prob << std::endl;

    if (realprob > 0.5){
        std::cout << "The DJ Algorithm finds the function f to be constant with value " << oracleValue << std::endl;
    }
    else {
        std::cout << "The DJ Algorithm finds the function f to be balanced." << std::endl;
    }

    // unload quest
    destroyQureg(qureg, env);
    destroyQuESTEnv(env);
    return 0;

}
