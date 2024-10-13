from mpi4py import MPI
import math
import random
import sys

def f1(x):
    return math.sin(x) * math.exp(-x)

def f2(x):
    return math.log(x + 1) / (x ** 2 + 1)

def f3(x):
    return math.exp(math.sin(x)) * math.cosh(x)

def f4(x):
    return x ** 3 * math.cos(x) / math.sqrt(x + 1)

def f5(x):
    return math.tanh(x) * math.log(x + 2)

functions = {
    0: f1,
    1: f2,
    2: f3,
    3: f4,
    4: f5
}

comm = MPI.COMM_WORLD
rank = comm.Get_rank()
size = comm.Get_size()

if len(sys.argv) != 4:
    print("Использование: python mpi_program.py <a> <b> <function_choice>")
    sys.exit(1)

a = float(sys.argv[1])
b = float(sys.argv[2])
function_choice = int(sys.argv[3])

if function_choice not in functions:
    if rank == 0:
        print("Ошибка: Неправильный выбор функции. Допустимые значения: 0-4")
    sys.exit(1)

function = functions[function_choice]

num_points = 1000000
points_per_process = num_points // size

local_result = 0.0
for _ in range(points_per_process):
    x = random.uniform(a, b)
    local_result += function(x)

total_result = comm.reduce(local_result, op=MPI.SUM, root=0)

if rank == 0:
    integral = (b - a) * total_result / (points_per_process * size)
    print(f"Оценка интеграла методом Монте-Карло: {integral}")
