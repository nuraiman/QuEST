//
// Created by giuli on 01/03/2019.
//

#include <stdio.h>
#include <QuEST.h>
#include <time.h>

int main(int narg, char *varg[]) {
    clock_t globalA = clock();
    // load QuEST
    QuESTEnv env = createQuESTEnv();

    // choose number of qubits
    int n = 5;


    // create n qubits, Method 1
    Qureg qubits1 = createQureg(n, env);
    clock_t launch = clock(); // start time counting
    initPlusState(qubits1);
    clock_t elapsed = clock(); // end time counting
    double tdiff1 = (elapsed - launch) / CLOCKS_PER_SEC; // divide the 'number of ticks' by 'ticks per second'
    printf("(Method 1) Elapsed time: %f\n", tdiff1);

    // create n qubits, Method 2
    Qureg qubits2 = createQureg(n, env);
    launch = clock(); // start time counting
    initZeroState(qubits2);
    for (int i = 0; i < n; ++i) {
    hadamard(qubits2,i);
    }
    elapsed = clock();
    double tdiff2 = (elapsed - launch) / CLOCKS_PER_SEC;
    printf("(Method 2) Elapsed time: %f\n", tdiff2);

    // get probability amplitudes
    qreal prob1;
    for (int j = 0; j < n ; ++j) {
        prob1 = calcProbOfOutcome(qubits1, j, 0);
        printf("(1) Probability of outcome 0 for qubit %d: %lf\n", j, prob1);
    }

    qreal prob2;
    for (int k = 0; k < n ; ++k) {
        prob2 = calcProbOfOutcome(qubits2, k, 0);
        printf("(2) Probability of outcome 0 for qubit %d: %lf\n", k, prob2);
    }

    // unload QuEST
    destroyQureg(qubits1, env);
    destroyQureg(qubits2, env);
    destroyQuESTEnv(env);\

    clock_t globalB = clock();
    double global = (globalB - globalA)/ CLOCKS_PER_SEC;
    printf("Global time: %f\n", global);

    return 0;
}