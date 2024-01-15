#ifndef GRAPHNODE_H_
#define GRAPHNODE_H_

#include <memory>
#include <string>
#include <vector>

#include "chatbot.h"

class GraphEdge;

class GraphNode {
private:
  std::vector<std::unique_ptr<GraphEdge>>
      _childEdges; // edges to subsequent nodes

  std::vector<GraphEdge*> _parentEdges; // edges to preceding nodes
  chat_bot _chatBot;

  int _id;
  std::vector<std::string> _answers;

public:
  GraphNode(int id);
  ~GraphNode();

  int GetID() { return _id; }
  int GetNumberOfChildEdges() { return _childEdges.size(); }
  GraphEdge& GetChildEdgeAtIndex(int index);
  const std::vector<std::string>& GetAnswers() const noexcept {
    return _answers;
  }
  int GetNumberOfParents() { return _parentEdges.size(); }

  void AddToken(std::string token); // add answers to list
  void AddEdgeToParentNode(GraphEdge* edge);
  void AddEdgeToChildNode(std::unique_ptr<GraphEdge> edge);
  void MoveChatbotHere(chat_bot chatbot);
  void MoveChatbotToNewNode(GraphNode* node);
};

#endif // GRAPHNODE_H_