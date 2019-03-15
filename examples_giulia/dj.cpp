#include "dj.hpp"

int main(int narg, char *varg[]) {
    //load quest
    QuESTEnv env = createQuESTEnv();

    syncQuESTEnv(env);
    auto start = std::chrono::steady_clock::now();
    dj(env);
    syncQuESTEnv(env);
    auto end = std::chrono::steady_clock::now();
    auto duration = std::chrono::duration_cast<std::chrono::nanoseconds>(end - start).count();
    if (env.rank == 0) {
        std::cout << "Duration [ns] = " << duration << std::endl;
    }

    destroyQuESTEnv(env);
    return 0;
}
