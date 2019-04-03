#include "grover.hpp"

int main(int narg, char *varg[]) {
    //load quest
    QuESTEnv env = createQuESTEnv();
    //command lines
    cxxopts::Options desc(varg[0], "Allowed Options");
    desc.add_options()
            ("help", "produce help message")
            ("N,number", "Size of the array with one marked element", cxxopts::value<qInt>()->default_value("10000"))
            ("r,repetitions", "Number of repetitions", cxxopts::value<int>()->default_value("1"));

    auto vm = desc.parse(narg, varg);

    if (vm.count("help")) {
        std::cout << desc.help() << std::endl;
        return 1;
    }

    const qInt N = vm["N"].as<qInt>();
    const int nrep = vm["r"].as<int>();

    // Create RNG
    RandomGen rng;

    unsigned long int seed =1;
    seedQuEST(&seed,1);

    // times vector for multiple repetitions of the code
    std::vector<long long> times;
    times.reserve(nrep);

    std::vector<qInt> factors;

    qInt outputresult = 0;
    qreal success = 0;
    rng.setSeed(0);
    for (int i = 0; i < nrep; ++i) {
        syncQuESTEnv(env);
        auto start = std::chrono::steady_clock::now();
        // find smallest natural number n such that N <= 2^n
        int n = int(ceil(log2(N)));

        //std::cout << "n = " << n << std::endl;
        // choose marked element at random
        qInt x0 = rng.sampleUniformly(0,N-1);
        std::cout << "Rep "<< i+1 << ": Test x0 = " << x0 << std::endl;


        // Create quantumregister and initialize in the |++...+> state
        Qureg qureg = createQureg(n, env);
        initPlusState(qureg);

        // calculate optimal number T of iterations
        int T = int(round(pi*0.25*sqrt(1<<n)-0.5));
        std::cout << "T = " << T << std::endl;

        // Grover algorithm
        for (int j = 0; j < T ; ++j) {
        PhaseOracle(qureg, x0);
        GroverStep(qureg, n);
        }

        // measure all the qubits and save their values in the variable 'result'
        success = getProbAmp(qureg, x0);

        qInt result = 0;
        int a;
        for (int j = 0; j < n ; ++j) {
            a = measure(qureg,j);
            if (a==1){
                result = result | (1 << j);
            }
        }

        syncQuESTEnv(env);
        auto end = std::chrono::steady_clock::now();
        auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(end - start).count();
        times.push_back(duration);
        outputresult = result;
        std::cout << "Found x0 = " << outputresult << " with success probability " << success << std::endl;
        destroyQureg(qureg, env);
    }

//    std::cout << "The algorithm found x0 = " << outputresult << " with success probability " << success << std::endl;

    // output the duration vector over repetitions
    if (env.rank == 0) {
        std::cout << "Duration [ms] = ";
        for (int i = 0; i < nrep; ++i) {
            std::cout << times[i];
            if (i < nrep - 1) {
               std::cout << ", ";
            }
        }
        std::cout << std::endl;

        auto statistics = computeStatistics(times);
        std::cout << "Avg time [ms] = " << statistics.first << ", Std deviation = " << statistics.second << std::endl;
    }

    destroyQuESTEnv(env);
    return 0;
}
