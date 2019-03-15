#include "approx-pf.hpp"

// test the functions
int main(int narg, char *varg[]) {
    // load quest
    QuESTEnv env = createQuESTEnv();

    // number of repetitions of the algorithms
    int n_rep = 100;

    // counts how many times the algorithm was successful
    int counter = 0;

    long long period = 0;
    for (int i = 0; i < n_rep; ++i) {
        long long r = PeriodFinding(env, 10,21);
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
