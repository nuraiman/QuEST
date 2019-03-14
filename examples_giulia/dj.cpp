#include "dj.hpp"

int main(int narg, char *varg[]) {
    //load quest
    QuESTEnv env = createQuESTEnv();

    dj(env);

    destroyQuESTEnv(env);
    return 0;
}
