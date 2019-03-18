#include <stdio.h>
#include <QuEST.h>

#include "shor.hpp"

int main(int narg, char *varg[]) {

    // load QuEST
    QuESTEnv env = createQuESTEnv();

    // choose number of qubits
    int n = 2;

    // create 2 qubits in the hadamard state
    Qureg qubits = createQureg(n, env);
    initPlusState(qubits);

    // apply circuit
    hadamard(qubits, 0);
    controlledNot(qubits, 0, 1);
    //measure(qubits, 1);

    // get probability amplitudes
    qreal prob1 = getProbAmp(qubits, 0);
    qreal prob2 = getProbAmp(qubits, 2);
    printf("Probability amplitude of |00>: %lf\n", prob1);
    printf("Probability amplitude of |10>: %lf\n", prob2);

    // get prob of outcome
    prob1 = calcProbOfOutcome(qubits, 0, 1);
    prob2 = calcProbOfOutcome(qubits, 1, 1);
    printf("Probability of qubit 0 being in state 1: %f\n", prob1);
    printf("Probability of qubit 1 being in state 1: %f\n", prob2);

    // unload QuEST
    destroyQureg(qubits, env);

    long long x = gcdForShor(175,4,21843958);
    std::cout << "x = " << x << std::endl;
    destroyQuESTEnv(env);
    return 0;
}
