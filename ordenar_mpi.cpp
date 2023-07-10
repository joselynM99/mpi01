#include <iostream>
#include <mpi.h>
#include <fstream>
#include <string>
#include <vector>
#include <algorithm>


static int rank, nprocs;
static std::vector<int> vector = {8, 23, 19, 67, 45, 35, 1, 24, 13, 30, 3, 5,4,7,2,5,9,0,45,332,56,7,88,2,3,4};
static int bloque;
// Prueba con el archivo de datos.txt************************************************
//static std::vector<int> vector ;
std::vector<int> read_file() {
    std::fstream fs("datos.txt", std::ios::in);
    std::string line;
    std::vector<int> ret;
    while (std::getline(fs, line)) {
        ret.push_back(std::stoi(line));
    }
    fs.close();
    return ret;
}


std::vector<int> ordenar(std::vector<int> vector) {
    std::sort(vector.begin(), vector.end());
    return vector;
}

std::vector<int> mezclar(std::vector<int> v1, std::vector<int> v2) {
    std::vector<int> mezcla(v1.size() + v2.size());

    int i = 0, j = 0, k = 0;

    while (i < v1.size() && j < v2.size()) {
        if (v1[i] <= v2[j]) {
            mezcla[k++] = v1[i++];
        } else {
            mezcla[k++] = v2[j++];
        }
    }

    // Cuando se acaben los elementos de alguno de los dos vectores
    while (i < v1.size()) {
        mezcla[k++] = v1[i++];
    }

    while (j < v2.size()) {
        mezcla[k++] = v2[j++];
    }

    return mezcla;
}

int main(int argc, char **argv) {
    MPI_Init(&argc, &argv);

    // Obtener el rank y el número de procesos
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    MPI_Comm_size(MPI_COMM_WORLD, &nprocs);

    if (rank == 0) {
     //vector = read_file();
     bloque = vector.size() / nprocs;
        for (int nRank = 1; nRank < nprocs; nRank++) {
            MPI_Send(&vector[(nRank-1) * bloque], bloque, MPI_INT, nRank, 0, MPI_COMM_WORLD);
        }

        std::vector<int> vec0(vector.begin() + bloque*(nprocs-1), vector.end());
        std::vector<int> vecFinal = ordenar(vec0);

        for (int nRank = 1; nRank < nprocs; nRank++) {
            std::vector<int> vecTemp(bloque);
            MPI_Recv(vecTemp.data(), bloque, MPI_INT, nRank, MPI_ANY_TAG, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
            vecFinal = mezclar(vecTemp, vecFinal);
        }

        for (int i = 0; i < vecFinal.size(); ++i) {
            std::printf("{%d}, ", vecFinal[i]);
        }
         std::cout << std::endl;
    } else {
        bloque = vector.size() / nprocs;
        std::vector<int> vTmp(bloque);
        MPI_Recv(vTmp.data(), bloque, MPI_INT, 0, MPI_ANY_TAG, MPI_COMM_WORLD, MPI_STATUS_IGNORE);

        std::vector<int> vectort = ordenar(vTmp);
        MPI_Send(vectort.data(), bloque, MPI_INT, 0, 0, MPI_COMM_WORLD);
    }

    MPI_Finalize();
    return 0;
}
