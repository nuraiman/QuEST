//
// Created by giuli on 08/03/2019.
//

#pragma once
#include "math_util.hpp"

// Implementation of the Quantum Fourier Transform
// (e.g., this is used in Shor's factoring algorithm)

// Implementation of a SWAP gate between two qubits
void swap(Qureg &qr, const int qubit1, const int qubit2){
    controlledNot(qr, qubit1, qubit2);
    controlledNot(qr, qubit2, qubit1);
    controlledNot(qr, qubit1, qubit2);
}


// Generic textbook approach for implementing the QFT
// Require N = 2^n, where n = number of qubits
void QFT(Qureg &qr, int numQubits, int smallest=0){
    // qft circuit
    for (int j = smallest; j < numQubits ; ++j) {
        for (int k = smallest; k < j; ++k) {
            controlledPhaseShift(qr, (numQubits-1)-j, (numQubits-1)-k, pi/(1<<(j-k)));
        }
        hadamard(qr, (numQubits-1)-j);
    }
    //reverse the order of the qubits
    for (int i = 0; i < numQubits/2 ; ++i) {
        swap(qr, i, (numQubits-1)-i);
    }
}
// Implementation of the inverse QFT (Same circuit as QFT, just reversed order)
void invQFT(Qureg &qr, int numQubits, int smallest=0){
    //reverse the order of the qubits
    for (int i = 0; i < numQubits/2 ; ++i) {
        swap(qr, i, (numQubits-1)-i);
    }
    // qft circuit
    for (int j = smallest; j < numQubits ; ++j) {
        for (int k = smallest; k < j; ++k) {
            controlledPhaseShift(qr, k, j, pi/(1<<(j-k)));
        }
        hadamard(qr, j);
    }

}

// Kitaev's approach for implementing the QFT

// If only the classical information of the QFT
// (such as the period r) is of interest, 2n qubit
// subject to a QFT can be replaced by a single qubit.
// (However, this approach requires qubit recycling (specifically, in-sequence single-qubit readout and
// state reinitialization) paired with feed-forward behavior to compensate for the reduced system size.)
void KQFT(Qureg &qr){
    // ...not yet clear how the general circuit looks like, and if it scales better for larger number of qubits
}
