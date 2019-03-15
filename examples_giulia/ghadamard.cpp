//
// Created by giuli on 01/03/2019.
//

#include <iostream>
#include <QuEST.h>
#include <ctime>
#include <chrono>
#include "math_util.hpp"

int main(int narg, char *varg[]) {
    // load QuEST
    QuESTEnv env = createQuESTEnv();

    // command lines
    cxxopts::Options desc(varg[0], "Allowed Options");
    desc.add_options()
            ("help", "produce help message")
            ("N,number", "Number of qubits", cxxopts::value<int>()->default_value("5"))
            ("r,repetitions", "Number of repetitions", cxxopts::value<int>()->default_value("1"));

    auto vm = desc.parse(narg, varg);

    if (vm.count("help")) {
        std::cout << desc.help() << std::endl;
        return 1;
    }

    const int n = vm["N"].as<int>();
    const int nrep = vm["r"].as<int>();

    // create n qubits, Method 1
    Qureg qubits1 = createQureg(n, env);
    auto start = std::chrono::steady_clock::now();
    initPlusState(qubits1);
    auto end = std::chrono::steady_clock::now();
    double tdiff1 = std::chrono::duration_cast<std::chrono::milliseconds>(end - start).count(); // divide the 'number of ticks' by 'ticks per second'
    //printf("(Method 1) Elapsed time: %f\n", tdiff1);
    std::cout << "(Method 1) Elapsed time: " << tdiff1 << std::endl;

    // create n qubits, Method 2
    Qureg qubits2 = createQureg(n, env);
    start = std::chrono::steady_clock::now();// start time counting
    initZeroState(qubits2);
    for (int i = 0; i < n; ++i) {
    hadamard(qubits2,i);
    }
    end = std::chrono::steady_clock::now();
    double tdiff2 = std::chrono::duration_cast<std::chrono::milliseconds>(end - start).count();
    // printf("(Method 2) Elapsed time: %f\n", tdiff2);
    std::cout << "(Method 2) Elapsed time: " << tdiff2 << std::endl;

    // get probability amplitudes
    qreal prob1;
    for (int j = 0; j < n ; ++j) {
        prob1 = calcProbOfOutcome(qubits1, j, 0);
        // printf("(1) Probability of outcome 0 for qubit %d: %lf\n", j, prob1);
        std::cout << "(1) Probability of outcome 0 for qubit " << j << ": " << prob1 << std::endl;

    }

    qreal prob2;
    for (int k = 0; k < n ; ++k) {
        prob2 = calcProbOfOutcome(qubits2, k, 0);
        // printf("(2) Probability of outcome 0 for qubit %d: %lf\n", k, prob2);
        std::cout << "(2) Probability of outcome 0 for qubit " << k << ": " << prob2 << std::endl;
    }

    // unload QuEST
    destroyQureg(qubits1, env);
    destroyQureg(qubits2, env);
    destroyQuESTEnv(env);


    return 0;
}