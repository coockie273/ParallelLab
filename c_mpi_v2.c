#include <stdio.h>
#include <stdlib.h>
#include <mpi.h>
#include <math.h>
#include <cstdio>

#define SIMPLE_SPRNG
#include "sprng_cpp.h"          /* SPRNG header file */

#define SEED 985456376
#define NUM_POINTS 1000000

// Функция 1: f(x) = sin(x) * exp(-x)
double f1(double x) {
    return sin(x) * exp(-x);
}

// Функция 2: f(x) = log(x + 1) / (x^2 + 1)
double f2(double x) {
    return log(x + 1) / (x * x + 1);
}

// Функция 3: f(x) = exp(sin(x)) * cosh(x)
double f3(double x) {
    return exp(sin(x)) * cosh(x);
}

// Функция 4: f(x) = x^3 * cos(x) / sqrt(x + 1)
double f4(double x) {
    return x * x * x * cos(x) / sqrt(x + 1);
}

// Функция 5: f(x) = tanh(x) * log(x + 2)
double f5(double x) {
    return tanh(x) * log(x + 2);
}

typedef double (*FunctionPointer)(double);

int main(int argc, char* argv[]) {
    int rank, size;
    MPI_Init(&argc, &argv);
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    MPI_Comm_size(MPI_COMM_WORLD, &size);

    // Проверка на количество аргументов
    if (argc != 4) {
        if (rank == 0) {
            printf("Usage: %s <a> <b> <function_choice>\n", argv[0]);
        }
        MPI_Finalize();
        return 1;
    }

    // Чтение аргументов
    double a = atof(argv[1]);
    double b = atof(argv[2]);
    int function_choice = atoi(argv[3]);

    // Выбор функции для интегрирования
    FunctionPointer func;
    switch (function_choice) {
        case 0:
            func = f1;
            break;
        case 1:
            func = f2;
            break;
        case 2:
            func = f3;
            break;
        case 3:
            func = f4;
            break;
        case 4:
            func = f5;
            break;
        default:
            if (rank == 0) {
                printf("Неправильный выбор функции. Допустимые значения: 0-4\n");
            }
            MPI_Finalize();
            return 1;
    }

    // Определяем количество точек для каждого процесса
    int points_per_process = NUM_POINTS / size;
    double local_sum = 0.0;

    // Инициализация генератора случайных чисел SPRNG
    int stream_id = rank;  // Уникальный идентификатор потока для каждого процесса
    init_sprng(SEED, SPRNG_DEFAULT, stream_id); // Инициализация с уникальным SEED для каждого потока

    // Генерация случайных точек и вычисление интеграла методом Монте-Карло
    for (int i = 0; i < points_per_process; i++) {
        double x = a + (b - a) * sprng(); // Генерация случайного числа в диапазоне [a, b]
        local_sum += func(x);
    }

    // Сбор глобальной суммы с помощью MPI_Reduce
    double global_sum = 0.0;
    MPI_Reduce(&local_sum, &global_sum, 1, MPI_DOUBLE, MPI_SUM, 0, MPI_COMM_WORLD);

    // Вычисление и вывод результата на процессе с рангом 0
    if (rank == 0) {
        double result = (b - a) * global_sum / NUM_POINTS;
        printf("Оценка интеграла методом Монте-Карло: %f\n", result);
    }

    MPI_Finalize();
    return 0;
}
