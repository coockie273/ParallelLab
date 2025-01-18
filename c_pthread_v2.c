#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <math.h>
#include <cstdio>

#define SIMPLE_SPRNG
#include "sprng_cpp.h"          /* SPRNG header file                       */

#define NUM_THREADS 2
#define NUM_POINTS 1000000
#define SEED 985456376

using namespace std;

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

typedef struct {
    int points_per_thread;
    double a;
    double b;
    double result;
    FunctionPointer function;
    int stream_id;
} threadData;

void* monte_carlo(void* arg) {

    threadData* data = (threadData*)arg;

    double result = 0.0;
    init_sprng(SEED, SPRNG_DEFAULT, data->stream_id);

    for (int i = 0; i < data->points_per_thread; i++) {
        double x = data->a + (data->b - data->a) * sprng();
        result += data->function(x);
    }

    data->result = result;

    pthread_exit(NULL);
}

int main(int argc, char* argv[]) {
    if (argc != 4) {
        printf("Usage: %s <a> <b> <function_choice>\n", argv[0]);
        return 1;
    }

    double a = atof(argv[1]);
    double b = atof(argv[2]);
    int function_choice = atoi(argv[3]);

    pthread_t threads[NUM_THREADS];
    threadData thread_data[NUM_THREADS];

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

    for (int i = 0; i < NUM_THREADS; i++) {
        thread_data[i].points_per_thread = NUM_POINTS / NUM_THREADS;
        thread_data[i].a = a;
        thread_data[i].b = b;
        thread_data[i].result = 0.0;
        thread_data[i].function = func;
        thread_data[i].stream_id = i; // Идентификатор потока для генератора SPRNG

        pthread_create(&threads[i], NULL, monte_carlo, (void*)&thread_data[i]);
    }

    double total_result = 0.0;
    for (int i = 0; i < NUM_THREADS; i++) {
        pthread_join(threads[i], NULL);
        total_result += thread_data[i].result;
    }

    double result = (b - a) * total_result / NUM_POINTS;

    printf("Оценка интеграла методом Монте-Карло: %f\n", result);
    return 0;
}
