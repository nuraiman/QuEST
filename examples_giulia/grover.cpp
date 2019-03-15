#include "grover.hpp"

int main(int narg, char *varg[]) {
    //load quest
    QuESTEnv env = createQuESTEnv();
    //command lines
    cxxopts::Options desc(varg[0], "Allowed Options");
    desc.add_options()
            ("help", "produce help message")
            ("N,number", "Size of the array with one marked element", cxxopts::value<long long>()->default_value("1<<13"))
            ("r,repetitions", "Number of repetitions", cxxopts::value<int>()->default_value("1"));

    auto vm = desc.parse(narg, varg);

    if (vm.count("help")) {
        std::cout << desc.help() << std::endl;
        return 1;
    }

    const long long N = vm["N"].as<long long>();
    const int nrep = vm["r"].as<int>();

    syncQuESTEnv(env);
    auto start = std::chrono::steady_clock::now();
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

    syncQuESTEnv(env);
    auto end = std::chrono::steady_clock::now();
    auto duration = std::chrono::duration_cast<std::chrono::nanoseconds>(end - start).count();
    std::cout << "The algorithm found x0 = " << result << " with success probability " << success << std::endl;
    std::cout << "Duration [ns] = " << duration << std::endl;

    destroyQureg(qureg, env);
    destroyQuESTEnv(env);
    return 0;
}
