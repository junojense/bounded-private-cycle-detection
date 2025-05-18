#ifndef UTIL_H
#define UTIL_H

#include <iostream>
#include <cstdint>
#include <vector>
#include <random>
#include <bitset>
#include <tuple>

using namespace std;

struct group { int p, q, r, h, g; };

int mulMod(int, int, int);
int powMod(int, int, int) ;

template <typename T>
tuple<mt19937, uniform_int_distribution<T>> makeDistribution(T lower, T upper);

int getRandomInDist(int lower, int upper);
int getRandomOfSize(int) ;
int getRandomInGroup(group);
int getLowLevelPrime(int);
bool trialComposite(int, int, int, int);
bool millerRabinTest(int, int);
int getBigPrime(int);
group getGroupParameters(int, int);
double average_degree(const vector<vector<int>>& graph);
tuple<int, double, vector<vector<int>>> generate_graph(int, int, int);
tuple<int, double, vector<vector<int>>> generate_scale_free_graph(int, int, int);
void print_graph(vector<vector<int>>);

#endif