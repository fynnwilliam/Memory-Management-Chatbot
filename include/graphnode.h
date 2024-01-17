#ifndef GRAPHNODE_H
#define GRAPHNODE_H

#include <memory>
#include <string>
#include <vector>

#include "chatbot.h"

class graph_edge;

class graph_node {
private:
  // edges to subsequent nodes
  std::vector<std::unique_ptr<graph_edge>> _child_edges;
  std::vector<graph_edge*> _parent_edges; // edges to preceding nodes
  std::vector<std::string> _answers;
  chat_bot _chat_bot;
  int _id;

public:
  graph_node(int id);
  ~graph_node();

  int id() { return _id; }
  int child_count() { return _child_edges.size(); }
  graph_edge& child_edge_at(int index);
  const auto& answers() const noexcept { return _answers; }
  int parent_count() { return _parent_edges.size(); }
  void add_token(std::string token); // add answers to list
  void add_to_parent(graph_edge* edge);
  void add_to_child(std::unique_ptr<graph_edge> edge);
  void move_chat_bot_here(chat_bot bot);
  void move_chat_bot_here(graph_node* node);
};

#endif // GRAPHNODE_H