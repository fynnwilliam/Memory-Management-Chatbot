#ifndef GRAPHEDGE_H
#define GRAPHEDGE_H

#include <string>
#include <vector>

class graph_node;

class graph_edge {
private:
  int _id;
  graph_node* _child = nullptr;
  graph_node* _parent = nullptr;

  // list of topics associated with this edge
  std::vector<std::string> _keywords;

public:
  graph_edge(int id);

  int id() { return _id; }
  void child(graph_node* node);
  void parent(graph_node* node);
  graph_node* child() const { return _child; }
  const auto& keywords() const noexcept { return _keywords; }
  void add_token(std::string token);
};

#endif // GRAPHEDGE_H