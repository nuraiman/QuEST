#include "dj.hpp"

int main(int narg, char *varg[]) {
    //load quest
    QuESTEnv env = createQuESTEnv();
    // command lines
    cxxopts::Options desc(varg[0], "Allowed Options");
    desc.add_options()
            ("help", "produce help message")
            ("N,number", "Number of qubits / length of the boolean function", cxxopts::value<int>()->default_value("13"))
            ("r,repetitions", "Number of repetitions", cxxopts::value<int>()->default_value("1"));

    auto vm = desc.parse(narg, varg);

    if (vm.count("help")) {
        std::cout << desc.help() << std::endl;
        return 1;
    }

    const int n = vm["N"].as<int>();
    const int nrep = vm["r"].as<int>();

    // create RNG
    RandomGen rng;
    unsigned long int seed =1;
    seedQuEST(&seed,1);

    std::vector<long long> times;
    times.reserve(nrep);

    std::vector<qInt> factors;

    for (int i = 0; i < nrep; ++i) {
        rng.setSeed(1);
        syncQuESTEnv(env);
        auto start = std::chrono::steady_clock::now();
        dj(env, n, rng);
        syncQuESTEnv(env);
        auto end = std::chrono::steady_clock::now();
        auto duration = std::chrono::duration_cast<std::chrono::nanoseconds>(end - start).count();
        times.push_back(duration);
    }

    if (env.rank == 0) {
        // output the duration vector over repetitions
        std::cout << "Duration [ns] = ";
        for (int i = 0; i < nrep; ++i) {
            std::cout << times[i];
            if (i < nrep - 1) {
                std::cout << ", ";
            }
        }
        std::cout << std::endl;

        auto statistics = computeStatistics(times);
        std::cout << "Avg time [ns] = " << statistics.first << ", Std deviation = " << statistics.second << std::endl;
    }

    destroyQuESTEnv(env);
    return 0;
}
