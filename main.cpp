#include <iostream>
#include <mpi.h>
#include <cmath>
#include <cstdlib>

static const int cantidadPuntos = 30; // Cantidad de puntos a generar

double distanciaC(int* p1, int* p2) {
    double dx = p1[0] - p2[0];
    double dy = p1[1] - p2[1];
    double dz = p1[2] - p2[2];
    return std::sqrt(dx * dx + dy * dy + dz * dz);
}

int main(int argc, char** argv) {
    MPI_Init(&argc, &argv);

    int rank, nprocs;
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    MPI_Comm_size(MPI_COMM_WORLD, &nprocs);

    int A[cantidadPuntos][3]; // Arreglo de arreglos para almacenar los puntos generados
    int C[3][3];

    int elementosPorProceso = cantidadPuntos / nprocs;  // Dividir los elementos de A equitativamente entre los procesos
    int resto = cantidadPuntos % nprocs;  // Resto de la división

    // Generar puntos aleatorios y guardarlos en el arreglo de arreglos
    for (int i = 0; i < cantidadPuntos; i++) {
        A[i][0] = std::rand() % 10 + 1;
        A[i][1] = std::rand() % 10 + 1;
        A[i][2] = std::rand() % 10 + 1;
    }

    for (int i = 0; i < 3; i++) {
        C[i][0] = std::rand() % 10 + 1;
        C[i][1] = std::rand() % 10 + 1;
        C[i][2] = std::rand() % 10 + 1;
    }

    if (rank == 0) {
        std::cout << "Centros:" << std::endl;
        for (int i = 0; i < 3; i++) {
            std::cout << "(" << C[i][0] << ", " << C[i][1] << ", " << C[i][2] << ")" << std::endl;
        }
        std::cout << std::endl;
    }

    int* indices = new int[elementosPorProceso + (rank < resto ? 1 : 0)];

    int startIndex = elementosPorProceso * rank + (rank < resto ? rank : resto);
    int endIndex = startIndex + elementosPorProceso + (rank < resto ? 1 : 0);

    for (int i = startIndex; i < endIndex; ++i) {
        double distanciaMin = 100.0;
        int minIndex = -1;
        for (int j = 0; j < 3; ++j) {
            double distancia = distanciaC(A[i], C[j]);
            if (distancia < distanciaMin) {
                distanciaMin = distancia;
                minIndex = j;
            }
        }
        indices[i - startIndex] = minIndex;
    }

    for (int i = 0; i < elementosPorProceso + (rank < resto ? 1 : 0); i++) {
        int globalIndex = startIndex + i;
        if (globalIndex < cantidadPuntos) {
            std::cout << "Vector: (" << A[globalIndex][0] << ", " << A[globalIndex][1] << ", " << A[globalIndex][2] << ")";
            std::cout << " Indice: " << indices[i] << std::endl;
        }
    }



    MPI_Finalize();
    return 0;
}