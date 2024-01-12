#ifndef GRAPHEDGE_H_
#define GRAPHEDGE_H_

#include <string>
#include <vector>

class GraphNode;  

class GraphEdge {
 private:
  GraphNode *_childNode = nullptr;
  GraphNode *_parentNode = nullptr;

  int _id;
  std::vector<std::string>
      _keywords;  // list of topics associated with this edge

 public:
  GraphEdge(int id);

  int GetID() { return _id; }
  void SetChildNode(GraphNode *childNode);
  void SetParentNode(GraphNode *parentNode);
  GraphNode *GetChildNode() const { return _childNode; }
  const std::vector<std::string> &GetKeywords() const noexcept {
    return _keywords;
  }

  void AddToken(std::string token);
};

#endif // GRAPHEDGE_H_