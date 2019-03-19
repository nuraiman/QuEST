#include "shor.hpp"

int main(int narg, char *varg[]) {
    //load quest
    QuESTEnv env = createQuESTEnv();
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

    syncQuESTEnv(env);
    auto start = std::chrono::steady_clock::now();
    std::vector<qInt> factors = ShorFactoring(env, N);
    syncQuESTEnv(env);
    auto end = std::chrono::steady_clock::now();
    auto duration = std::chrono::duration_cast<std::chrono::nanoseconds>(end - start).count();

    if (env.rank == 0) {
        std::sort(factors.begin(), factors.end());
        std::cout << "Factors are: ";
        for (const auto& f : factors) {
            std::cout << f << "; ";
        }
        std::cout << std::endl;

        std::cout << "Duration [ns] = " << duration << std::endl;
    }

    destroyQuESTEnv(env);
    return 0;
}
