#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <complex.h>
#include <time.h>

typedef double complex QState;

// Quantum system structure
typedef struct {
    QState *qubits;
    int num_qubits;
} QuantumSystem;

// Initialize a quantum system with n qubits
QuantumSystem* init_quantum_system(int n) {
    QuantumSystem *qs = (QuantumSystem*)malloc(sizeof(QuantumSystem));
    qs->num_qubits = n;
    qs->qubits = (QState*)malloc((1 << n) * sizeof(QState));
    
    for (int i = 0; i < (1 << n); i++) {
        qs->qubits[i] = (i == 0) ? 1.0 + 0.0*I : 0.0 + 0.0*I;
    }
    
    return qs;
}

void free_quantum_system(QuantumSystem *qs) {
    free(qs->qubits);
    free(qs);
}

void hadamard(QuantumSystem *qs, int qubit) {
    int stride = 1 << qubit;
    int size = 1 << qs->num_qubits;
    double factor = 1.0 / sqrt(2.0);
    
    for (int i = 0; i < size; i += (1 << (qubit + 1))) {
        for (int j = 0; j < stride; j++) {
            int pos0 = i + j;
            int pos1 = pos0 + stride;
            
            QState a = qs->qubits[pos0];
            QState b = qs->qubits[pos1];
            
            qs->qubits[pos0] = factor * (a + b);
            qs->qubits[pos1] = factor * (a - b);
        }
    }
}

void cnot(QuantumSystem *qs, int cqubit, int tqubit) {
    int cstride = 1 << cqubit;
    int tstride = 1 << tqubit;
    int size = 1 << qs->num_qubits;
    
    for (int i = 0; i < size; i++) {
        
        if (i & cstride) {
            int target_pos = i ^ tstride;
            if (target_pos > i) {
                
                QState temp = qs->qubits[i];
                qs->qubits[i] = qs->qubits[target_pos];
                qs->qubits[target_pos] = temp;
            }
        }
    }
}

int measure(QuantumSystem *qs, int qubit) {
    int stride = 1 << qubit;
    double prob0 = 0.0;
   
    for (int i = 0; i < (1 << qs->num_qubits); i++) {
        if ((i & stride) == 0) {
            prob0 += creal(qs->qubits[i] * conj(qs->qubits[i]));
        }
    }
   
    double r = (double)rand() / RAND_MAX;
    
    if (r < prob0) {
        
        for (int i = 0; i < (1 << qs->num_qubits); i++) {
            if (i & stride) {
                qs->qubits[i] = 0.0 + 0.0*I;
            } else {
                qs->qubits[i] /= sqrt(prob0);
            }
        }
        return 0;
    } else {
      
        for (int i = 0; i < (1 << qs->num_qubits); i++) {
            if (i & stride) {
                qs->qubits[i] /= sqrt(1.0 - prob0);
            } else {
                qs->qubits[i] = 0.0 + 0.0*I;
            }
        }
        return 1;
    }
}
void print_state(QuantumSystem *qs) {
    printf("Quantum State:\n");
    for (int i = 0; i < (1 << qs->num_qubits); i++) {
        printf("|");
        for (int j = qs->num_qubits - 1; j >= 0; j--) {
            printf("%d", (i >> j) & 1);
        }
        printf("⟩: %.3f + %.3fi\n", creal(qs->qubits[i]), cimag(qs->qubits[i]));
    }
}
int main() {
    srand(time(NULL));
   
    QuantumSystem *qs = init_quantum_system(2);
    
    printf("Initial state:\n");
    print_state(qs);
    
    hadamard(qs, 0);
    printf("\nAfter Hadamard on qubit 0:\n");
    print_state(qs);
    
    cnot(qs, 0, 1);
    printf("\nAfter CNOT (0->1):\n");
    print_state(qs);
    
    int result0 = measure(qs, 0);
    int result1 = measure(qs, 1);
    printf("\nMeasurement results: qubit0=%d, qubit1=%d\n", result0, result1);
    
    printf("\nFinal state after measurement:\n");
    print_state(qs);
    
    free_quantum_system(qs);
    return 0;
}
