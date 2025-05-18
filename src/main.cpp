#include <iostream>
#include <chrono>
#include <deque>
#include <vector>
#include <fstream>
#include <unordered_set>
#include <iomanip>
#include "util.h"
#include "node.h"
#include "message.h"

using namespace std;


// Sends the appropriate message based on the message type
vector<message> send(vector<node*> &nodes, const message& msg) {
    switch (msg.t) {
        case f: return nodes[msg.target]->forward(msg);
        case b: return nodes[msg.target]->backward(msg);
        case p: return nodes[msg.target]->publish(msg);
        case broadcast:
            for(auto v : nodes) v->broadcast(msg);
            return {};
        default: return {};
    }
}

// Reads out a file that has lines with two comma-separated integers (u,v)
vector<vector<int>> read_file(int max_neighbours, int max_size)
{
    ifstream file("transaction_data.csv");
    string line;
    vector<vector<int>> graph;

    while (getline(file, line))
    {
        uint64_t slash_index = line.find(',');
        int source = stoi(line.substr(0, slash_index));
        int target = stoi(line.substr(slash_index + 1, line.length()));
        if(source > max_size || target > max_size) continue;
        if(source == (int) graph.size()) graph.emplace_back();
        if((int) graph.at(source).size() < max_neighbours) graph.at(source).emplace_back(target);
        if(target == (int) graph.size()) graph.emplace_back();
    }
    file.close();
    return graph;
}

// Output writer
void write_file(const string& s, long hash, group G)
{
    string h_path = "/output/-" + to_string(hash);
    string params = "group [p=" + to_string(G.p) + ",q=" + to_string(G.q) + ",r=" + to_string(G.r) + ",h=" + to_string(G.h) + ",g=" + to_string(G.g) + "].log";
    ofstream file(h_path + params, ofstream::app);
    file << s;
    file.close();
}

// Utility for generating a vector of node objects
void generate_nodes(vector<vector<int>> &graph, vector<node*> &nodes, group G)
{
    int idx = 0;
    for (const vector<int> &neighbours : graph)
    {
        node *n = new node(idx, {}, neighbours, G);
        nodes.push_back(n);
        ++idx;
    }
}


// The main function that runs a single test case and returns (#cycles, #edges, #totalmsg, #forwardmsg, #backwardmsg, #publishmsg, #broadcastmsg, runtime)
tuple<int, int, int, int, int, int, int, int> run(int n, vector<node*> &nodes, int l, int m, double d_avg)
{
    auto t1 = chrono::high_resolution_clock::now();
    unordered_set<string> cycles;
    unordered_set<string> global_cycles;
    int global_msg_count = 0, old_msg_count = 0, cycle_edge_count = 0;
    vector<int> msg_count = { 0, 0, 0, 0 };

    for (int i = 0; i < n; ++i)
    {
        auto msg_init = nodes[i]->initiate(l);
        deque msg_queue(deque<message>(msg_init.begin(), msg_init.end()));
        while (!msg_queue.empty())
        {
            message msg = msg_queue.front();
            ++old_msg_count;
            msg_count.at(msg.t)++;
            if (msg.t == broadcast)
            {
                auto [_, res] = cycles.insert(msg.getpath());
                if(res) cycle_edge_count += (int) msg.getpath().size() - 1;
            }
            auto mailbox = send(nodes, msg);
            for (const auto &mm : mailbox) msg_queue.emplace_back(mm);
            msg_queue.pop_front();
        }
        global_cycles.merge(cycles);
        global_msg_count += old_msg_count;
        old_msg_count = 0UL, cycles.clear();
    }
    auto t2 = chrono::high_resolution_clock::now();
    auto t_diff = chrono::duration_cast<chrono::milliseconds>(t2 - t1).count();

    cout << fixed << setprecision(2)
         << "n=" << n << ", "
         << "m=" << m << ", "
         << "d_avg="  << d_avg << ", "
         << "l="      << l << ", "
         << "n_cyc="  << global_cycles.size() << ", "
         << "c_edge=" << cycle_edge_count << ", "
         << "n_msg="  << global_msg_count << ", "
         << "n_for="  << msg_count.at(f) << ", "
         << "n_echo=" << msg_count.at(b) << ", "
         << "n_pub="  << msg_count.at(p) << ", "
         << "n_brd="  << msg_count.at(broadcast) << ", "
         << "t="      << t_diff << "ms (" << t_diff / 1000 << "s)" << endl;
    return {global_cycles.size(), cycle_edge_count, global_msg_count, msg_count.at(f), msg_count.at(b), msg_count.at(p), msg_count.at(broadcast), t_diff};
}

// The main function loops over the parameters graph size, edge density, search depth, and calls run for each combination of parameters
int main(){

    int l_lower    =   2;   // cycle length min 2
    int l_upper    =   4;   // cycle length max 4
    int n_lower    =   50;  // size min
    int n_upper    =   50;  // size max
    int d_lower    =   3;   // m min (lower m is lower average degree)
    int d_upper    =   9;   // m max (higher m is higher average degree)
    int iterations =   1;   // amount of times to repeat experiment

    group G = getGroupParameters(20, 40);
    cout << "=============================================================PARAM=============================================================\n";
    cout << "group [p=" << G.p << ", q=" << G.q << ", r=" << G.r << ", h=" << G.h << ", g=" << G.g << "]\ngraph size [" << n_lower;
    cout << ',' << n_upper << "] with l [" << l_lower << ',' << l_upper << "] and degree [" << d_lower << ',' << d_upper << "]\n\n";
    cout << "=============================================================STATS=============================================================\n";

    // To use the data in transaction_data.csv
    // {
    //     vector<vector<int>> graph = read_file(1, 1);
    //     vector<node*> nodes;
    //     generate_nodes(graph, nodes, G);
    //     for(int k = l_lower; k <= l_upper; ++k)
    //         run((int) graph.size(), nodes, k, -1, average_degree(graph));
    // }

    auto clock = chrono::high_resolution_clock::now();
    auto hash = duration_cast<chrono::milliseconds>(clock.time_since_epoch()).count();
    write_file("n,m,d_avg,l,n_cyc,c_edge,n_msg,n_for,n_echo,n_pub,n_brd,t\n", (long) hash, G);

    // Determines the number of iterations for each set of parameters
    for(int iteration = 0; iteration < iterations; ++iteration)
    {
        // Determines the size of the graph being generated
        for(int i = n_lower; i <= n_upper; ++i)
        {
            // Determines the density of the graph
            for(int j = d_lower; j <= d_upper; ++j)
            {
                // Generate a random graph with specified parameters (n=i, m=j, m0=j)
                vector<node*> nodes;
                nodes.reserve(i);
                auto [m, d_avg, graph] = generate_scale_free_graph(j, j, i);
                generate_nodes(graph, nodes, G);
                string s = to_string(i) + ',' + to_string(m) + ',' + to_string(d_avg) + ',';

                // Determines the upper cycle length (search depth)
                for(int k = l_lower; k <= l_upper; ++k)
                {
                    auto [n_cyc, c_edge, n_msg, n_for, n_echo, n_pub, n_brd, t] = run(i, nodes, k, m, d_avg);
                    string ss = s + to_string(k) + ',' + to_string(n_cyc) + ',' + to_string(c_edge) + ',' + to_string(n_msg) + ',' + to_string(n_for) + 
                                ',' + to_string(n_echo)+ ',' + to_string(n_pub)+ ',' + to_string(n_brd)+ ',' + to_string(t) + '\n';
                    write_file(ss, (long) hash, G);
                }
                for(auto node : nodes) delete node;
            }
        }
    }
    return 0;
}
