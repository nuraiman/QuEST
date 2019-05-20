import math

n_nodes = [4, 7, 8, 13, 16, 25, 27, 32, 37, 61, 64, 81, 93, 128, 201, 216, 256, 333, 473, 512]
# n_nodes = [4, 7, 8, 13, 16, 25, 27, 32, 37, 61, 64]
n_tasks = [36 * x for x in n_nodes]

p_range = n_tasks
p_rows=[12, 21, 24, 36, 24, 30, 36, 36, 37, 61, 48, 54, 62, 72, 108, 96, 96, 111, 132, 144]
p_cols=[12, 12, 12, 13, 24, 30, 27, 32, 36, 36, 48, 54, 54, 64, 67, 81, 96, 108, 129, 128]

# rang = [7, 8, 9, 10, 11]

# n_nodes = [int(math.ceil(2**i/36.0)) for i in rang]
# n_tasks = [int(2**i) for i in rang]
# 
# p_range = [int(math.floor(2**i)) for i in rang]
# p_rows = [int(rang[i]/2) for i in range(len(rang))]
# p_cols = [rang[i] - p_rows[i] for i in range(len(rang))]
# p_rows = [int(2**x) for x in p_rows]
# p_cols = [int(2**x) for x in p_cols]

# #p_range=[16, 28, 32, 52, 64, 100, 108, 128, 148, 244, 256, 324, 372, 512, 804, 864, 1024, 1332, 1892, 2048]
# p_range = n_tasks

base_memory = 1250000000 # per node, in #doubles, corresponding to 10GB
mem_limit = 1.0 * base_memory

strong_scaling_mn=2**17
strong_scaling_k=2**9

def cubic_root(x):
    return x**(1.0/3.0)

def square_root(x):
    return math.sqrt(x)

def get_weak_scaling_p0_mn():
    return [int(math.floor(136.0*cubic_root(p*mem_limit/(3.0 * 228.0 * 136.0)))) for p in n_nodes]

def get_weak_scaling_p0_k():
    return [int(math.floor(228.0/(136.0**2.0) * (p**2.0))) for p in p0_mn]

def get_weak_scaling_p1_mn():
    return [int(math.floor(136.0*cubic_root((p**(2.0/3.0))*mem_limit/(3.0 * 228.0 * 136.0)))) for p in n_nodes]

def get_weak_scaling_p1_k():
    return [int(math.floor(228.0/(136.0**2.0) * (p**2))) for p in p1_mn]

def apply_correctness_factor(l, factor):
    #poly = np.polyfit([4, n_nodes[len(n_nodes)-1]], [factor, 0.1], deg=1)
    #factors = [np.polyval(poly, x) for x in n_nodes]
    #return [int(factors[i] * l[i]) for i in range(len(n_nodes))]
    return [int(factor * l[i]) for i in range(len(n_nodes))]

def two_large(mn, k):
    for i, m_val in enumerate(mn):
        k_1d = k[i]
        n_1d = m_val

        total_domain = n_1d**2 + 2*n_1d*k_1d
        n_2d = int(math.floor((square_root(512**2 + 4*total_domain) - 512)/2))
        k_2d = 256

        mn[i] = n_2d
        k[i] = k_2d

    return (mn, k)

p0_mn = get_weak_scaling_p0_mn()
p0_k = get_weak_scaling_p0_k()

p1_mn = get_weak_scaling_p1_mn()
p1_k = get_weak_scaling_p1_k()

p0_mn = apply_correctness_factor(p0_mn, 0.4)
p0_k = apply_correctness_factor(p0_k, 0.4)

p1_mn = apply_correctness_factor(p1_mn, 0.4)
p1_k = apply_correctness_factor(p1_k, 0.4)

(weak_scaling_p0_mn, weak_scaling_p0_k) = two_large(p0_mn, p0_k)
(weak_scaling_p1_mn, weak_scaling_p1_k) = two_large(p1_mn, p1_k)

print("Strong scaling m = n: ", strong_scaling_mn)
print("Strong scaling k: ", strong_scaling_k)
print("Limited memory, m = n: ", weak_scaling_p0_mn)
print("Limited memory, k: ", weak_scaling_p0_k)
print("Extra memory, m = n: ", weak_scaling_p1_mn)
print("Extra memory, k: ", weak_scaling_p1_k)

mem_limit = 5 * base_memory

n_rep = 3

template_file = "generate_scripts.sh"
output_file = "generate_scripts_filled.sh"

def get_string(num_list):
    result = '('
    for x in num_list:
        result += str(x) + " "
    result += ')'
    return result

time = "00:20:00"

with open(template_file) as in_file:
    with open(output_file, 'w') as out_file:
        for line in in_file:
            line = line.replace("experiment_time=()", "experiment_time=" + time)
            line = line.replace("n_nodes=()", "n_nodes=" + get_string(n_nodes))
            line = line.replace("n_tasks=()", "n_tasks=" + get_string(n_tasks))

            line = line.replace("p_range=()", "p_range=" + get_string(p_range))
            line = line.replace("p_rows=()", "p_rows=" + get_string(p_rows))
            line = line.replace("p_cols=()", "p_cols=" + get_string(p_cols))

            line = line.replace("weak_scaling_p0_mn=()", "weak_scaling_p0_mn=" + get_string(weak_scaling_p0_mn))
            line = line.replace("weak_scaling_p0_k=()", "weak_scaling_p0_k=" + get_string(weak_scaling_p0_k))

            line = line.replace("weak_scaling_p1_mn=()", "weak_scaling_p1_mn=" + get_string(weak_scaling_p1_mn))
            line = line.replace("weak_scaling_p1_k=()", "weak_scaling_p1_k=" + get_string(weak_scaling_p1_k))

            line = line.replace("strong_scaling_mn=()", "strong_scaling_mn=" + str(strong_scaling_mn))
            line = line.replace("strong_scaling_k=()", "strong_scaling_k=" + str(strong_scaling_k))

            line = line.replace("mem_limit=()", "mem_limit=" + str(mem_limit))
            line = line.replace("n_rep=()", "n_rep=" + str(n_rep))

            out_file.write(line)
