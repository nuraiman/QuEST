#include "shor.hpp"

int main(int narg, char *varg[]) {
    //load quest
    QuESTEnv env = createQuESTEnv();
    // command lines
    cxxopts::Options desc(varg[0], "Allowed Options");
    desc.add_options()
        ("help", "produce help message")
        ("N,number", "Number to factorize.", cxxopts::value<qInt>()->default_value("21"))
        ("r,repetitions", "Number of repetitions", cxxopts::value<int>()->default_value("1"));

    auto vm = desc.parse(narg, varg);

    if (vm.count("help")) {
        std::cout << desc.help() << std::endl;
        return 1;
    }

    const qInt N = vm["N"].as<qInt>();
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
        factors.clear();
        syncQuESTEnv(env);
        auto start = std::chrono::steady_clock::now();
        factors = ShorFactoring(env, N,rng);
        syncQuESTEnv(env);
        auto end = std::chrono::steady_clock::now();
        auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(end - start).count();
        times.push_back(duration);
    }

    if (env.rank == 0) {
        std::sort(factors.begin(), factors.end());
        // output the prime factors that are found
        std::cout << "Factors are: ";
        for (const auto& f : factors) {
            std::cout << f << "; ";
        }
        std::cout << std::endl;

        // output the duration vector over repetitions
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
