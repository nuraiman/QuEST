//
// Created by giuli on 25/04/2019.
//

// weak scaling benchmark for single-qubit-gates
#pragma once
#include "math_util.hpp"

void printAvgTimes(std::vector<long long> &times, int steps, int nrep){
    std::vector<long long> tempTimes;
    tempTimes.reserve(nrep-1);
    for (int j = 0; j < steps; ++j) {
        for (int l = 1; l < nrep; ++l) {
            tempTimes.push_back(times[j+l*steps]);
        }
        // output the duration vector over repetitions
        std::cout << "Duration [ms] = ";
        for (int i = 0; i < nrep-1; ++i) {
            std::cout << tempTimes[i];
            if (i < nrep - 2) {
                std::cout << ", ";
            }
        }
        std::cout << std::endl;
        // output statistics
        auto statistics = computeStatistics(tempTimes);
        std::cout << "Avg time [ms] = " << statistics.first << ", Std deviation = " << statistics.second << " (Position k = " << j << ")" << std::endl;
        tempTimes.clear();
    }
    std::cout << std::endl;
}

int main(int argc, char **argv) {
    //load quest
    QuESTEnv env = createQuESTEnv();
  
    /* // command line arguments
    cxxopts::Options desc(varg[0], "Allowed Options");
    desc.add_options()
            ("help", "produce help message")
            ("N,number", "Number of qubits", cxxopts::value<int>()->default_value("13"))
            ("p,position", "Index of the staring position", cxxopts::value<int>()->default_value("0"))
            ("r,repetitions", "Number of repetitions", cxxopts::value<int>()->default_value("4"));

    auto vm = desc.parse(narg, varg);

    if (vm.count("help")) {
        std::cout << desc.help() << std::endl;
        return 1;
    }

    const int n = vm["N"].as<int>();
    const int startPos = vm["p"].as<int>();
    const int nrep = vm["r"].as<int>();
*/

    // command line arguments
    int n = 13;
    int startPos = 0;
    int nrep = 2;
    while ((argc > 1) && (argv[1][0] == '-')){
        switch (argv[1][1]){
            case 'N':  // Number of qubits
                n = std::atoi(argv[2]);
                break;
            case 'p':  // Index of the starting position
                startPos = std::atoi(argv[2]);
                break;
            case 'r':  // Number of repetitions
                nrep = std::atoi(argv[2]);
                break;
            default:
                std::cout << "Wrong Argument: " << argv[1] << std::endl;
        }
        argv += 2;
        argc -= 2;
    }

    int steps = n - startPos;
    if (steps <= 0){
        std::cout << "Error: Index of the starting position p must be smaller than the number of qubits n, p < n!" << std::endl;
        exit(1);
    }

    std::vector<long long> timesH;
    timesH.reserve(steps*nrep);

    std::vector<long long> timesX;
    timesX.reserve(steps*nrep);

    std::vector<long long> timesT;
    timesT.reserve(steps*nrep);

    Qureg qureg = createQureg(n, env);

    for (int i = 0; i < nrep; ++i) {
        // hadamard
        for (int k = startPos; k < n; ++k) {
            initZeroState(qureg);
            syncQuESTEnv(env);
            auto startH = std::chrono::steady_clock::now();
            hadamard(qureg, k);
            syncQuESTEnv(env);
            auto endH = std::chrono::steady_clock::now();
            auto durationH = std::chrono::duration_cast<std::chrono::milliseconds>(endH - startH).count();
            timesH.push_back(durationH);
        }

        // pauli X
        for (int k = startPos; k < n; ++k) {
            initZeroState(qureg);
            syncQuESTEnv(env);
            auto startX = std::chrono::steady_clock::now();
            pauliX(qureg, k);
            syncQuESTEnv(env);
            auto endX = std::chrono::steady_clock::now();
            auto durationX = std::chrono::duration_cast<std::chrono::milliseconds>(endX - startX).count();
            timesX.push_back(durationX);
        }

        // T gate
        for (int k = startPos; k < n; ++k) {
            initZeroState(qureg);
            syncQuESTEnv(env);
            auto startT = std::chrono::steady_clock::now();
            tGate(qureg, k);
            syncQuESTEnv(env);
            auto endT = std::chrono::steady_clock::now();
            auto durationT = std::chrono::duration_cast<std::chrono::milliseconds>(endT - startT).count();
            timesT.push_back(durationT);
        }
    }

    destroyQureg(qureg, env);

    // output data (times)
    if (env.rank == 0) {

        std::cout << "Gate H:" << std::endl;
        printAvgTimes(timesH, steps, nrep);
        std::cout << "Gate X:" << std::endl;
        printAvgTimes(timesX, steps, nrep);
        std::cout << "Gate T:" << std::endl;
        printAvgTimes(timesT, steps, nrep);

    }

    destroyQuESTEnv(env);
    return 0;
}

