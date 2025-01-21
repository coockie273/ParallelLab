#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <omp.h>
#include <cstdio>

#define SIMPLE_SPRNG
#include <sprng_cpp.h>

#define NUM_POINTS 1000000
#define SEED 985456376

#define NUM_THREADS 4

// Функция 1: f(x) = sin(x) * exp(-x)
double f1(double x) {
    return sin(x) * exp(-x);
}

// Функция 2: f(x) = log(x + 1) / (x * x + 1)
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

    if (argc != 4) {
        printf("Usage: %s <a> <b> <function_choice>\n", argv[0]);
        return 1;
    }

    omp_set_num_threads(NUM_THREADS);

    double a = atof(argv[1]);
    double b = atof(argv[2]);
    int function_choice = atoi(argv[3]);

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
            printf("Неправильный выбор функции. Допустимые значения: 0-4\n");
            return 1;
    }

    double local_sum = 0.0;

    #pragma omp parallel
    {
        init_sprng(SEED, SPRNG_DEFAULT);

        #pragma omp for reduction(+:local_sum)
        for (int i = 0; i < NUM_POINTS; i++) {
            double x = a + (b - a) * sprng();
            local_sum += func(x);
        }
    }
    double result = (b - a) * local_sum / NUM_POINTS;

    printf("Оценка интеграла методом Монте-Карло: %f\n", result);

    return 0;
}
