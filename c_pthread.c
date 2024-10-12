#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <math.h>

#define NUM_THREADS 4
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

typedef struct {
    int points_per_thread;
    double a;
    double b;
    double result;
    FunctionPointer function;
} threadData;


void* monte_carlo(threadData* data) {

    double result = 0;

    for (int i = 0; i < data->points_per_thread; i++) {

        double x = data->a + (data->b - data->a) * (double)rand() / RAND_MAX;
	result += data->function(x);
    }

    data->result += result;


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

    threadData data;

    data.points_per_thread = NUM_POINTS / NUM_THREADS;
    data.a = a;
    data.b = b;
    data.result = 0.0;

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

    data.function = func;

    for (int i = 0; i < NUM_THREADS; i++) {

        pthread_create(&threads[i], NULL, monte_carlo, (void*)&data);

    }

    for (int i = 0; i < NUM_THREADS; i++) {

        pthread_join(threads[i], NULL);

    }

    double result = (data.b - data.a) * data.result / (data.points_per_thread * NUM_THREADS);

    printf("Оценка интеграла методом Монте-Карло: %f\n", result);
    return 0;
}
