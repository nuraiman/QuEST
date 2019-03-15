#include "period-finding.hpp"

// What is the best way to
int main(int narg, char *varg[]) {
    //load quest
    QuESTEnv env = createQuESTEnv();

    syncQuESTEnv(env);
    auto start = std::chrono::steady_clock::now();
    long long r = PeriodFinding(env, 7,16,15);
    syncQuESTEnv(env);
    auto end = std::chrono::steady_clock::now();
    auto duration = std::chrono::duration_cast<std::chrono::nanoseconds>(end - start).count();

    std::cout << "r = "<< r << std::endl;
    std::cout << "Duration [ns] = " << duration << std::endl;

    destroyQuESTEnv(env);
    return 0;
}
