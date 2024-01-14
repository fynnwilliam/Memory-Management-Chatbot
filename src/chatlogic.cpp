#include "chatlogic.h"

#include <algorithm>
#include <fstream>
#include <iostream>
#include <iterator>
#include <sstream>
#include <tuple>
#include <vector>

#include "chatbot.h"
#include "graphedge.h"
#include "graphnode.h"

ChatLogic::ChatLogic() = default;

ChatLogic::~ChatLogic() = default;

template <typename T>
void ChatLogic::AddAllTokensToElement(std::string tokenID, tokenlist& tokens,
                                      T& element) {
  auto token = tokens.begin();
  while (true) {
    token = std::find_if(token, tokens.end(), [&tokenID](const auto& pair) {
      return pair.first == tokenID;
      ;
    });
    if (token != tokens.end()) {
      element.AddToken(token->second);
      token++;
    } else {
      break;
    }
  }
}

void ChatLogic::LoadAnswerGraphFromFile(std::string filename) {
  std::ifstream file(filename);

  if (file) {
    std::string lineStr;
    while (getline(file, lineStr)) {
      tokenlist tokens;
      while (lineStr.size() > 0) {
        int posTokenFront = lineStr.find("<");
        int posTokenBack = lineStr.find(">");
        if (posTokenFront < 0 || posTokenBack < 0)
          break;
        std::string tokenStr =
            lineStr.substr(posTokenFront + 1, posTokenBack - 1);

        int posTokenInfo = tokenStr.find(":");
        if (posTokenInfo != std::string::npos) {
          std::string tokenType = tokenStr.substr(0, posTokenInfo);
          std::string tokenInfo =
              tokenStr.substr(posTokenInfo + 1, tokenStr.size() - 1);

          tokens.emplace_back(std::move(tokenType), std::move(tokenInfo));
        }

        lineStr = lineStr.substr(posTokenBack + 1, lineStr.size());
      }

      auto type =
          std::find_if(tokens.begin(), tokens.end(),
                       [](const std::pair<std::string, std::string>& pair) {
                         return pair.first == "TYPE";
                       });
      if (type != tokens.end()) {
        auto idToken =
            std::find_if(tokens.begin(), tokens.end(),
                         [](const std::pair<std::string, std::string>& pair) {
                           return pair.first == "ID";
                         });
        if (idToken != tokens.end()) {
          int id = std::stoi(idToken->second);

          if (type->second == "NODE") {
            auto newNode = std::find_if(
                _nodes.begin(), _nodes.end(),
                [&id](const auto& node) { return node->GetID() == id; });

            if (newNode == _nodes.end()) {
              _nodes.emplace_back(std::make_unique<GraphNode>(id));
              newNode = _nodes.end() - 1;
              AddAllTokensToElement("ANSWER", tokens, **newNode);
            }
          }

          if (type->second == "EDGE") {
            auto parentToken = std::find_if(
                tokens.begin(), tokens.end(),
                [](const std::pair<std::string, std::string>& pair) {
                  return pair.first == "PARENT";
                });
            auto childToken = std::find_if(
                tokens.begin(), tokens.end(),
                [](const std::pair<std::string, std::string>& pair) {
                  return pair.first == "CHILD";
                });

            if (parentToken != tokens.end() && childToken != tokens.end()) {
              const auto& parentNode = *std::find_if(
                  _nodes.begin(), _nodes.end(),
                  [&parentToken](const auto& node) {
                    return node->GetID() == std::stoi(parentToken->second);
                  });
              const auto& childNode = *std::find_if(
                  _nodes.begin(), _nodes.end(),
                  [&childToken](const auto& node) {
                    return node->GetID() == std::stoi(childToken->second);
                  });

              auto edge = std::make_unique<GraphEdge>(id);
              edge->SetChildNode(childNode.get());
              edge->SetParentNode(parentNode.get());
              _edges.emplace_back(edge.get());

              AddAllTokensToElement("KEYWORD", tokens, *edge);
              childNode->AddEdgeToParentNode(edge.get());
              parentNode->AddEdgeToChildNode(std::move(edge));
            }
          }
        } else {
          std::cout << "Error: ID missing. Line is ignored!" << std::endl;
        }
      }
    } // eof loop over all lines in the file

    file.close();

  } // eof check for file availability
  else {
    std::cout << "File could not be opened!" << std::endl;
    return;
  }

  // identify root node
  GraphNode* rootNode = nullptr;
  for (auto& node : _nodes) {
    if (node->GetNumberOfParents() == 0) {
      if (rootNode == nullptr) {
        rootNode = node.get();
      } else {
        std::cout << "ERROR : Multiple root nodes detected" << std::endl;
      }
    }
  }

  auto chatBot = ChatBot{"../images/chatbot.png"};

  // add pointer to chatlogic so that chatbot answers can be passed on to the
  // GUI
  chatBot.SetChatLogicHandle(this);

  chatBot.SetRootNode(rootNode);
  rootNode->MoveChatbotHere(std::move(chatBot));
}

void ChatLogic::SetPanelDialogHandle(ChatBotPanelDialog* panelDialog) {
  _panelDialog = panelDialog;
}

void ChatLogic::SetChatbotHandle(ChatBot* chatbot) { _chatBot = chatbot; }

void ChatLogic::SendMessageToChatbot(std::string message) {
  _chatBot->ReceiveMessageFromUser(message);
}

void ChatLogic::SendMessageToUser(std::string message) {
  _panelDialog->PrintChatbotResponse(message);
}

wxBitmap* ChatLogic::GetImageFromChatbot() {
  return _chatBot->GetImageHandle();
}