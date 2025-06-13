#include "node.h"

vector<message> node::initiate(int l)
{
    vector<message> messages;
	for (int target : n_out)
 	{
		auto x = getRandomInGroup(G);
		auto r = getRandomInGroup(G);
		init.emplace_back(x, r);

        message msg_f = { id, target, r, powMod(G.g, x, G.p), l - 1 };
        messages.emplace_back(msg_f);
	}
    return messages;
}

vector<message> node::forward(const message& msg)
{
    auto y = getRandomInGroup(G);
    keys.push_back(powMod(msg.gx, y, G.p));

    message msg_b = { id, msg.source, msg.r, powMod(G.g, y, G.p) };
    vector<message> messages;
    messages.reserve(n_out.size() + 1);
    messages.emplace_back(msg_b);
    if(msg.l == 0UL) return messages;

    for (int target : n_out)
    {
        route route = {
                .s_id       = msg.source,
                .s_nonce    = msg.r,
                .t_id       = target,
                .t_nonce    = getRandomInGroup(G),
                .f_gx       = msg.gx,
                .b_gx       = 0,
                .key        = getRandomInGroup(G),
        };
        routes.emplace_back(route);
        message msg_f = { id,route.t_id,route.t_nonce,powMod(msg.gx, route.key, G.p), msg.l - 1 };
        messages.push_back(msg_f);
    }
    return messages;
}

vector<message> node::backward(const message& msg)
{
    vector<message> messages;
    messages.reserve(1);
    auto xr = std::ranges::find(init, msg.r, &pair<int, int>::second);
    if (xr != init.end())
    {
        int key = powMod(msg.gx, xr->first, G.p);
        if (std::find(keys.begin(), keys.end(), key) != keys.end()) {
            messages.emplace_back(id, msg.source, msg.r, msg.gx, vector<int>{id});
        }
        keys.push_back(key);
        return messages;
    }
    auto route = std::ranges::find(routes, msg.r, &route::t_nonce);
    if (route == routes.end()) return messages;
    route->b_gx = msg.gx;
    messages.emplace_back(id, (*route).s_id, (*route).s_nonce, powMod(msg.gx, (*route).key, G.p));
    return messages;
}

vector<message> node::publish(const message& msg)
{
    vector<message> messages;
    messages.reserve(routes.size());
    vector<int> ext_path = msg.path;
    ext_path.push_back(id);

    if (id == msg.path.front())
    {
        messages.emplace_back(ext_path);
        return messages;
    }

    for (auto& route : routes)
    {
        if (powMod(route.b_gx, route.key, G.p) == msg.gx && route.s_nonce == msg.r)
            messages.emplace_back(id, route.t_id, route.t_nonce, route.b_gx, ext_path);
    }
    return messages;
}

void node::broadcast(const message& msg) {
    return;
    // auto path = msg.path;
    // for(size_t i = 0; i < path.size() - 1; ++i)
        // topology.emplace_back(path[i], path[(i + 1) % path.size()]);
}
