#include "grover.hpp"

int main(int narg, char *varg[]) {
    //load quest
    QuESTEnv env = createQuESTEnv();

    long long N = 1 << 13;

    // find smallest natural number n such that N <= 2^n
    int n = int(ceil(log2(N)));

    std::cout << "n = " << n << std::endl;
    // choose marked element at random
    long long x0 = sample_uniformly(0,N-1);
    std::cout << "Test x0 = " << x0 << std::endl;


    // Create quantumregister and initialize in the |++...+> state
    Qureg qureg = createQureg(n, env);
    initPlusState(qureg);

    // calculate optimal number T of iterations
    int T = int(round(pi*0.25*sqrt(1<<n)-0.5));
    std::cout << "T = " << T << std::endl;

    // Grover algorithm
    for (int i = 0; i < T ; ++i) {
        PhaseOracle(qureg, x0);
        GroverStep(qureg, n);
    }

    // measure all the qubits and save their values in the variable 'result'
    qreal success = getProbAmp(qureg, x0);

    long long result = 0;
    int a;
    for (int j = 0; j < n ; ++j) {
        a = measure(qureg,j);
        if (a==1){
            result = result | (1 << j);
        }
    }

    std::cout << "The algorithm found x0 = " << result << " with success probability " << success << std::endl;


    destroyQureg(qureg, env);
    destroyQuESTEnv(env);
    return 0;
}
