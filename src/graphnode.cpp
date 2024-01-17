#include "graphnode.h"

#include <iostream>

#include "graphedge.h"

graph_node::graph_node(int id) : _id{id} {}

graph_node::~graph_node() = default;

void graph_node::add_token(std::string token) {
  _answers.emplace_back(std::move(token));
}

void graph_node::add_to_parent(graph_edge* edge) {
  _parent_edges.emplace_back(edge);
}

void graph_node::add_to_child(std::unique_ptr<graph_edge> edge) {
  _child_edges.emplace_back(std::move(edge));
}

void graph_node::move_chat_bot_here(chat_bot bot) {
  _chat_bot = std::move(bot);
  _chat_bot.current_node(this);
}

void graph_node::move_chat_bot_here(graph_node* node) {
  node->move_chat_bot_here(std::move(_chat_bot));
}

graph_edge& graph_node::child_edge_at(int index) {
  return *_child_edges[index];
}