#include "shor.hpp"
#include "cxxopts/cxxopts.hpp"

int main(int narg, char *varg[]) {
    //load quest
    QuESTEnv env = createQuESTEnv();
    cxxopts::Options desc(argv[0], "Allowed Options");
    desc.add_options()
        ("help", "produce help message")
        ("N,number", "Number to factorize.", cxxopts::value<long long>()->default_value("21"))
        ("r,repetitions", "Number of repetitions", cxxopts::value<int>()->default_value("1"));

    auto vm = desc.parse(argc, argv);

    if (vm.count("help")) {
        std::cout << desc.help() << std::endl;
        return 1;
    }

    const long long N = vm["N"].as<long long>();
    const int r = vm["r"].as<int>();

    syncQuESTEnv(env);
    auto start = std::chrono::steady_clock::now();
    long long factor = ShorFactoring(env, N);
    syncQuESTEnv(env);
    auto end = std::chrono::steady_clock::now();
    auto duration = std::chrono::duration_cast<std::chrono::nanoseconds>(end - start).count();

    if (env.rank == 0) {
        if (factor > 0) {
            std::cout << "Factors found: " << factor << ",  " << N / factor << std::endl;
            std::cout << "Duration [ns] = " << duration << std::endl;
        }
        else {
            std::cout << "Algorithm failed" << std::endl;
        }
    }

    destroyQuESTEnv(env);
    return 0;
}
