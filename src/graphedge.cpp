#include "graphedge.h"

#include "graphnode.h"

graph_edge::graph_edge(int id) { _id = id; }

void graph_edge::child(graph_node* node) { _child = node; }

void graph_edge::parent(graph_node* node) { _parent = node; }

void graph_edge::add_token(std::string token) {
  _keywords.emplace_back(std::move(token));
}