#ifndef NODE_H
#define NODE_H

#include <algorithm>
#include <utility>
#include <vector>
#include <cstdint>
#include "util.h"
#include "message.h"

using namespace std;

struct route
{
    int s_id, s_nonce;       // source id, nonce and key
    int t_id, t_nonce;	     // same but gx can be found from key
    int f_gx, b_gx;          // back and forward keys
    int key; 	             // instance-specific
};

struct edge
{
    int source;
    int target;
};

class node
{
    private:
		int id;					        // unique node id
		vector<int> n_in; 		        // incoming neighbours
		vector<int> n_out; 		        // outgoing neighbours
		vector<int> keys;               // all keys
        vector<pair<int, int>> init;    // all nonces
		vector<route> routes;           // for all instances
        vector<edge> topology;          // known non-neighbouring edges
        group G;				        // ddh-safe subgroup

    public:
		node(int id, vector<int> n_in, vector<int> n_out, group G)
		{
			this->id 	= id;
			this->n_in 	= std::move(n_in);
			this->n_out = std::move(n_out);
            this->G     = G;
		}

        vector<message> initiate(int);
        vector<message> forward(const message& msg);
        vector<message> backward(const message& msg);
        vector<message> publish(const message& msg);
        void broadcast(const message& msg);

        void topology_print()
        {
            cout << "[TOP~" << id << "] | ";
            for(edge e : topology) cout << e.source << "->" << e.target << " | ";
            cout << endl;
        }

        void keys_print()
        {
            string s = "[KEY~" + to_string(id) + "] (";
            for (int e: keys) s += to_string(e) + ", ";
            s.erase(s.end() - 1, s.end());
            cout << s << endl;
        }

        uint64_t neighbour_size()
        {
            return n_out.size();
        }

		// virtual ~node();
};

#endif