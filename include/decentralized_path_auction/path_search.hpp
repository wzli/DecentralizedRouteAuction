#pragma once

#include <decentralized_path_auction/path.hpp>
#include <functional>

namespace decentralized_path_auction {

class PathSearch {
public:
    enum Error {
        SUCCESS = 0,

        PATH_EXTENDED,
        PATH_CONTRACTED,

        PATH_EMPTY,
        PATH_NODE_INVALID,
        PATH_NODE_DISABLED,
        PATH_BID_NOT_FOUND,
        PATH_BID_OVER_SELF,
        PATH_PARKING_VIOLATION,

        DESTINATION_NODE_INVALID,
        DESTINATION_NODE_NO_PARKING,
        DESTINATION_NODE_DUPLICATED,

        SOURCE_NODE_NOT_PROVIDED,
        SOURCE_NODE_INVALID,
        SOURCE_NODE_DISABLED,

        CONFIG_AGENT_ID_EMPTY,
        CONFIG_TIME_EXCHANGE_RATE_NON_POSITIVE,
        CONFIG_TRAVEL_TIME_MISSING,
    };

    using TravelTime =
            std::function<float(const Graph::NodePtr& prev, const Graph::NodePtr& cur, const Graph::NodePtr& next)>;

    static float travelDistance(const Graph::NodePtr&, const Graph::NodePtr& cur, const Graph::NodePtr& next) {
        return bg::distance(cur->position, next->position);
    }

    struct Config {
        std::string agent_id;
        float time_exchange_rate = 1;
        TravelTime travel_time = travelDistance;

        Error validate() const;
    };

    PathSearch(Config config)
            : _config(std::move(config)) {}

    Error setDestination(Graph::Nodes nodes);
    Error iterateSearch(Path& path);
    Error finalizePrice(Path& path) const;

    Config& getConfig() { return _config; }

private:
    bool detectCycle(const Auction::Bid& bid, const Path& path);
    float getCostEstimate(const Graph::NodePtr& node, const Auction::Bid& bid) const;

    Config _config;
    Graph _dst_nodes;
    size_t _cycle_nonce = 0;
    size_t _cost_nonce = 0;
};

}  // namespace decentralized_path_auction
