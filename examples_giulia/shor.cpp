#include "shor.hpp"

int main(int narg, char *varg[]) {
    int N = 87;
    int factor = ShorFactoring(N);
    if (factor > 0) {
        std::cout << "Factors found: " << factor << ",  " << N / factor << std::endl;
    }
    else {
        std::cout << "Algorithm failed" << std::endl;
    }

    return 0;
}
