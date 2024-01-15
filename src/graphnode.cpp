#include "graphnode.h"

#include <iostream>

#include "graphedge.h"

GraphNode::GraphNode(int id) : _id{id} {}

GraphNode::~GraphNode() = default;

void GraphNode::AddToken(std::string token) {
  _answers.emplace_back(std::move(token));
}

void GraphNode::AddEdgeToParentNode(GraphEdge* edge) {
  _parentEdges.emplace_back(edge);
}

void GraphNode::AddEdgeToChildNode(std::unique_ptr<GraphEdge> edge) {
  _childEdges.emplace_back(std::move(edge));
}

void GraphNode::MoveChatbotHere(chat_bot chatbot) {
  _chatBot = std::move(chatbot);
  _chatBot.current_node(this);
}

void GraphNode::MoveChatbotToNewNode(GraphNode* node) {
  node->MoveChatbotHere(std::move(_chatBot));
}

GraphEdge& GraphNode::GetChildEdgeAtIndex(int index) {
  return *_childEdges[index];
}