#include "approx-pf.hpp"

// test the functions
int main(int narg, char *varg[]) {
    // load quest
    QuESTEnv env = createQuESTEnv();
    // command lines
    cxxopts::Options desc(varg[0], "Allowed Options");
    desc.add_options()
            ("help", "produce help message")
            ("a,base", "First parameter of the function a^x mod N", cxxopts::value<long long>()->default_value("10"))
            ("N,number", "second parameter of the function a^x mod N", cxxopts::value<long long>()->default_value("21"))
            ("r,repetitions", "Number of repetitions", cxxopts::value<int>()->default_value("100"));

    auto vm = desc.parse(narg, varg);

    if (vm.count("help")) {
        std::cout << desc.help() << std::endl;
        return 1;
    }

    const long long a = vm["a"].as<long long>();
    const long long N = vm["N"].as<long long>();
    const int n_rep = vm["r"].as<int>();

    // counts how many times the algorithm was successful
    int counter = 0;

    long long period = 0;
    for (int i = 0; i < n_rep; ++i) {
        long long r = PeriodFinding(env, a,N);
        counter += (r > -1) ? 1 : 0;

        if (r > -1) { 
            period = std::max(period, r);
        }
    }

    double success_probability = (100.0 * counter) / n_rep;

    std::cout << "The probability of finding the right solution, based on " << n_rep << " repetitions, is approximatelly = " << success_probability << std::endl;

    double target_probability = 100.0 * 4.0/(pi*pi);

    std::cout << "\nTarget probability = " << target_probability << std::endl;

    std::cout << "r = " << period << std::endl;

    destroyQuESTEnv(env);

    return 0;
}
