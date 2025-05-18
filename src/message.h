#ifndef MESSAGE_H
#define MESSAGE_H

#include <cstdint>
#include <utility>
#include <sstream>

using namespace std;

enum type { f, b, p, broadcast };

class message
{
    public:
        type t;             // message type
        int source{};       // sender
        int target{};       // send to
        int r{};            // nonce
        int gx{};           // ddh-safe x in g^x
        int l{};            // time-to-live
        vector<int> path{}; // source for publish

        message(int source, int target, int r, int gx, int l)
        { // forward
            this->t       = f;
            this->source  = source;
            this->target  = target;
            this->r       = r;
            this->gx      = gx;
            this->l       = l;
        }

        message(int source, int target, int r, int gx)
        { // backward
            this->t       = b;
            this->source  = source;
            this->target  = target;
            this->r       = r;
            this->gx      = gx;
        }

        message(int source, int target, int r, int gx, vector<int> path)
        { // publish
            this->t       = p;
            this->source  = source;
            this->target  = target;
            this->r       = r;
            this->gx      = gx;
            this->path    = std::move(path);
        }

        explicit message(vector<int> path)
        { // broadcast
            this->t     = broadcast;
            this->path  = std::move(path);
        }

        [[nodiscard]] string getpath() const
        {
            if (path.empty()) return "";
            std::ostringstream oss;
            oss << path[0];
            for (size_t i = 1; i < path.size(); ++i) oss << ' ' << path[i];
            return oss.str();
        }

        void print() const
        {
            if(t == broadcast)
            {
                cout << "[BRDC] (path={ ";
                for(auto e : path) cout << e << " ";
                cout << "})" << endl;
                return;
            }

            string s = "[";
            if(source < 10) s += "0";
            s += to_string(source) + "->";
            if(target < 10) s += "0";
            cout << s << target << "] (";
            switch (t) {
                case f:
                    cout << "t=f, r=" << r << ", gx=" << gx << ", l=" << l << ")";
                    break;
                case b:
                    cout << "t=b, r=" << r << ", gx=" << gx << ")";
                    break;
                case p:
                    cout << "t=p, r=" << r << ", path={ ";
                    for(auto e : path) cout << e << " ";
                    cout << "})";
                    break;
                default: break;
            }
        }

        [[nodiscard]] string typestring() const
        {
            if(t == f) return "forward";
            if(t == b) return "backward";
            if(t == p) return "publish";
            if(t == broadcast) return "broadcast";
            return "unknown";
        }
};


#endif