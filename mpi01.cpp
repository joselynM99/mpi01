#include<iostream>
#include<mpi.h>

int sumar_datos(int *datos, int size) {
    int suma = 0;
    for (int i = 0; i < size; i++) {
        suma = suma + datos[i];
    }
    return suma;
}

int main(int argc, char **argv) {
    //inicializar MPI
    MPI_Init(&argc, &argv);

    //obtener el rank y numero de procesos
    int rank, nprocs;

    MPI_Comm_rank(MPI_COMM_WORLD, &rank); //rank
    MPI_Comm_size(MPI_COMM_WORLD, &nprocs); //procesos

    if (rank == 0) {

        int data[100];

        for (int i = 0; i < 100; i++) {
            data[i] = i;
        }

        int bloque = 100 / nprocs;

        for (int nRank = 1; nRank < nprocs; nRank++) {
            MPI_Send(&data[(nRank-1) * bloque], bloque, MPI_INT, nRank, 0, MPI_COMM_WORLD);
        }

        int suma0 = sumar_datos(&data[bloque*(nprocs-1)], 100-(nprocs-1)*bloque);
        int sumaTotal = suma0;

        std::printf("[Rank-0] MPI group size %d\n", nprocs);
        for (int nRank = 1; nRank < nprocs; nRank++) {
            int sumaTmp;
            MPI_Recv(&sumaTmp, 1, MPI_INT, nRank, MPI_ANY_TAG, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
            std::printf("[Rank-0] Resultado rank-%d: %d\n", nRank, sumaTmp);
            sumaTotal = sumaTotal + sumaTmp;
        }

        std::printf("[Rank-0] Suma total: %d\n", sumaTotal);

    } else {
        int bloque = 100 / nprocs;

        int *dataTmp = new int[bloque];

        std::printf("Soy el rank %d, recibiendo datos ...\n", rank);
        MPI_Recv(dataTmp, bloque, MPI_INT, MPI_ANY_SOURCE/*0*/, MPI_ANY_TAG/*0*/, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
        std::string str = "";
        for (int i = 0; i < bloque; i++) {
            str = str + std::to_string(dataTmp[i]) + ", ";

        }
        std::printf("[RANK-%d] %d datos recibidos: [%s]\n", rank, bloque, str.c_str());

        //sumar datos

        int suma = sumar_datos(dataTmp, bloque);


        MPI_Send(&suma, 1, MPI_INT, 0, 0, MPI_COMM_WORLD);
        delete[] dataTmp;
    }

    MPI_Finalize();

}