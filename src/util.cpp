#include <cstdint>
#include <vector>
#include <random>
#include <tuple>
#include <set>
#include <unordered_set>
#include "util.h"

using namespace std;

vector<int> first_primes = {   2, 3, 5, 7, 11, 13, 17, 19, 23, 29,
                                    31, 37, 41, 43, 47, 53, 59, 61, 67,
                                    71, 73, 79, 83, 89, 97, 101, 103,
                                    107, 109, 113, 127, 131, 137, 139,
                                    149, 151, 157, 163, 167, 173, 179,
                                    181, 191, 193, 197, 199, 211, 223,
                                    227, 229, 233, 239, 241, 251, 257,
                                    263, 269, 271, 277, 281, 283, 293,
                                    307, 311, 313, 317, 331, 337, 347, 349 };

int mulMod(int a, int b, int m) {
    int res = 0;
    while (a != 0) {
        if (a & 1) res = (res + b) % m;
        a >>= 1;
        b = (b << 1) % m;
    }
    return res;
}

int powMod(int a, int b, int n) {
    int x = 1;
    a %= n;
    while (b > 0) {
        if (b & 1) x = mulMod(x, a, n);
        a = mulMod(a, a, n);
        b >>= 1;
    }
    return x % n;
}

template <typename T>
tuple<mt19937, uniform_int_distribution<T>> makeDistribution(T lower, T upper)
{
    random_device rd;
    mt19937 gen(rd());
    uniform_int_distribution<T> distr(lower, upper);
    return { gen, distr };
}

int getRandomInDist(int lower, int upper)
{
    auto [gen, distr] = makeDistribution(lower, upper);
    return distr(gen);
}

int getRandomOfSize(int bits = 64) {
    if (bits < 2) return 0;
    auto [gen, distr] = makeDistribution<int>(0, (1 << (bits - 1)) - 1);
    return distr(gen) | (1 << (bits - 1));
}

int getRandomInGroup(int g, int q, int p)
{
    auto [gen, distr] = makeDistribution(0, q - 1);
    return powMod(g, distr(gen), p);
}

int getRandomInGroup(group G) { return getRandomInGroup(G.g, G.q, G.p); }

int getLowLevelPrime(int bits = 64) {
    while (true) {
        int candidate = getRandomOfSize(bits);
        bool is_prime = true;
        for (int first_prime : first_primes) {
            if (candidate == first_prime) return candidate;
            if (candidate % first_prime == 0) {
                is_prime = false;
                break;
            }
        }
        if (is_prime) return candidate;
    }
}

bool trialComposite(int a, int even_c, int to_test, int max_div_2) {
    if (powMod(a, even_c, to_test) == 1) return false;
    for (int i = 0; i < max_div_2; ++i) {
        int temp = static_cast<int>(1) << i;
        if (powMod(a, temp * even_c, to_test) == to_test - 1) return false;
    }
    return true;
}

bool millerRabinTest(int to_test, int accuracy = 20) {
    int max_div_2 = 0;
    int even_c = to_test - 1;
    while (even_c % 2 == 0) {
        even_c >>= 1;
        max_div_2++;
    }

    random_device rd;
    mt19937 gen(rd());
    uniform_int_distribution<int> distr(2, to_test);

    for (int i = 0; i < accuracy; ++i) {
        int a = distr(gen);
        if (trialComposite(a, even_c, to_test, max_div_2)) {
            return false;
        }
    }
    return true;
}

int getBigPrime(int bits = 64) {
    while (true) {
        int candidate = getLowLevelPrime(bits);
        if (millerRabinTest(candidate)) return candidate;
    }
}

group getGroupParameters(int q_size, int r_size) {
    int q = getBigPrime(q_size);
    while (true) {
        int r = getRandomInDist(2, powMod(2, r_size, UINT16_MAX));
        int p = q * r + 1;

        if (!millerRabinTest(p)) continue;

        int h = getRandomInDist(2, p - 1);
        int g = powMod(h, r, p);

        if (g != 1) return group{p, q, r, h, g};
    }
}

double average_degree(const vector<vector<int>>& graph) {
    std::unordered_set<int> unique_nodes;
    size_t edge_count = 0;
    for (const auto& edge : graph) {
        if (edge.size() != 2) continue;  // skip malformed edges
        unique_nodes.insert(edge[0]);
        unique_nodes.insert(edge[1]);
        edge_count++;
    }
    if (unique_nodes.empty()) return 0.0;
    return (double) edge_count / (double) unique_nodes.size();
}

tuple<int, double, vector<vector<int>>> generate_graph(int size, int min_degree, int max_degree)
{
    random_device dev;
    mt19937 rng(dev());
    uniform_int_distribution<int> dist(0, size - 1);
    uniform_int_distribution<int> d_dist(min_degree, min(size - 2, max_degree));

    double const mean = ((double) min_degree + (double) max_degree) / 2.F;
    double const average_bound_width = ((mean-(double) min_degree ) + (double) max_degree) / 2;
    double const standard_deviation  = average_bound_width / 3;
    normal_distribution<double> distribution(mean, standard_deviation);

    vector<vector<int>> graph;
    int d_sum = 0;
    for(int i = 0; i < size; ++i)
    {
        graph.emplace_back();
        int d = d_dist(rng);
        d_sum += d;
        set<int> seen;
        for(int j = 0; j < d; ++j)
        {
            int k;
            do {
                k = (int) dist(rng);
            } while (seen.contains(k) || k == i);
            seen.insert(k);
            graph.at(i).emplace_back(k);
        }
    }
    return { d_sum, (double) d_sum / (double) size, graph };
}

// Generates graphs using the Barabási–Albert model
tuple<int, double, vector<vector<int>>> generate_scale_free_graph(int m0, int m, int target_size)
{
    random_device dev;
    mt19937 rng(dev());
    vector<vector<int>> graph;
    uniform_int_distribution<int> edge_direction(0, 1);

    for(int i = 0; i < m0; ++i)
    {
        vector<int> neighbours;
        for(int j = 0; j < m0; ++j) if(i != j) neighbours.emplace_back(j);
        graph.emplace_back(neighbours);
    }

    int edges_in_m = m0*(m0-1);         // edges at initialisation
    edges_in_m += m*(target_size-m0);   // edges in full graph

    for(int i = m0; i < target_size; ++i)
    {
        uniform_int_distribution<int> dist(0, i - 1);
        graph.emplace_back();
        set<int> seen;
        for(int j = 0; j < m; ++j)
        {
            int k;
            do {
                k = dist(rng);
            } while (seen.contains(k));
            if(edge_direction(rng) == 0) graph.at(i).emplace_back(k);
            else graph.at(k).emplace_back(i);
            seen.insert(k);
        }
    }
    return { edges_in_m, (double) edges_in_m / (double) target_size, graph };
}

void print_graph(vector<vector<int>> graph)
{
    int idx = 0;
    for(const auto& node: graph)
    {
        cout << "node " << idx << ": ";
        for(auto edge: node) cout << edge << " ";
        cout << endl;
        ++idx;
    }
}
