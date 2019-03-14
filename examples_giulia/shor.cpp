#include "shor.hpp"

int main(int narg, char *varg[]) {
    //load quest
    QuESTEnv env = createQuESTEnv();

    long long N = 2201;

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
