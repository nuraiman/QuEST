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

    syncQuESTEnv(env);
    auto start = std::chrono::steady_clock::now();
    dj(env,n);
    syncQuESTEnv(env);
    auto end = std::chrono::steady_clock::now();
    auto duration = std::chrono::duration_cast<std::chrono::nanoseconds>(end - start).count();
    if (env.rank == 0) {
        std::cout << "Duration [ns] = " << duration << std::endl;
    }

    destroyQuESTEnv(env);
    return 0;
}
