#include "scheduler.h"

#include <iostream>
#include <map>
#include <queue>
#include <vector>

#include <common/strutil.h>
#include <compiler/graph.h>
#include <compiler/node.h>
#include <compiler/value.h>

namespace oniku {

namespace {

std::map<Node*, int> FindNecessaryNodes(const Graph& graph) {
    // Find necessary nodes.
    std::queue<const Value*> q;
    for (const Value* value : graph.output_values()) {
        q.push(value);
    }

    std::map<Node*, int> input_counts;
    while (!q.empty()) {
        const Value* value = q.front();
        q.pop();
        if (Node* node = value->producer()) {
            if (!input_counts.emplace(node, node->GetNumActualInputs()).second) continue;
            for (const Value* input : node->inputs()) {
                q.push(input);
            }
        }
    }
    return input_counts;
}

// A simple topological sort.
std::vector<Node*> ScheduleNaively(const Graph& graph, std::map<Node*, int> input_counts) {
    std::queue<const Value*> q;
    // Sort them topologically.
    for (const Value* value : graph.input_values()) {
        q.push(value);
    }

    std::vector<Node*> nodes;

    auto schedule_node = [&nodes, &q](Node* node) {
        nodes.push_back(node);
        for (const Value* output : node->outputs()) {
            q.push(output);
        }
    };

    // Schedule nodes which are already schedulable (e.g., Constant).
    for (const auto& p : input_counts) {
        if (p.second == 0) {
            schedule_node(p.first);
        }
    }

    while (!q.empty()) {
        const Value* value = q.front();
        q.pop();
        for (Node* node : value->users()) {
            auto found = input_counts.find(node);
            if (found == input_counts.end()) continue;
            int cnt = --found->second;
            if (cnt > 0) continue;
            schedule_node(node);
        }
    }
    return nodes;
}

}  // namespace

void ScheduleComputation(const Graph& graph) {
    std::map<Node*, int> input_counts = FindNecessaryNodes(graph);
    std::vector<Node*> nodes = ScheduleNaively(graph, input_counts);

    // Sanity check.
    std::set<const Value*> output_set;
    for (const Value* value : graph.output_values()) {
        output_set.insert(value);
    }
    for (const Node* node : nodes) {
        for (const Value* output : node->outputs()) output_set.erase(output);
    }
    CHECK(output_set.empty()) << "Cannot output: " << Join(MapToString(output_set, [](const Value* value) { return value->name(); }));

    for (size_t i = 0; i < nodes.size(); ++i) {
        nodes[i]->set_onikux_order(i + 1);
    }
}

}  // namespace oniku
