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

chat_logic::chat_logic() = default;

chat_logic::~chat_logic() = default;

template <typename T>
void chat_logic::add_tokens(std::string token_id, tokenlist& tokens,
                            T& element) {
  auto token = tokens.begin();
  while (true) {
    token = std::find_if(token, tokens.end(), [&token_id](const auto& pair) {
      return pair.first == token_id;
      ;
    });
    if (token != tokens.end()) {
      element.add_token(token->second);
      token++;
    } else {
      break;
    }
  }
}

void chat_logic::load_answer_graph(std::string filename) {
  std::ifstream file(filename);

  if (file) {
    std::string line;
    while (getline(file, line)) {
      tokenlist tokens;
      while (line.size() > 0) {
        int first = line.find('<');
        int last = line.find('>');
        if (first < 0 || last < 0)
          break;
        auto token_str = line.substr(first + 1, last - 1);

        int pos_token_info = token_str.find(':');
        if (pos_token_info != std::string::npos) {
          auto token_type = token_str.substr(0, pos_token_info);
          auto token_info =
              token_str.substr(pos_token_info + 1, token_str.size() - 1);

          tokens.emplace_back(std::move(token_type), std::move(token_info));
        }

        line = line.substr(last + 1, line.size());
      }

      auto type =
          std::find_if(tokens.begin(), tokens.end(),
                       [](const std::pair<std::string, std::string>& pair) {
                         return pair.first == "TYPE";
                       });
      if (type != tokens.end()) {
        auto id_token =
            std::find_if(tokens.begin(), tokens.end(),
                         [](const std::pair<std::string, std::string>& pair) {
                           return pair.first == "ID";
                         });
        if (id_token != tokens.end()) {
          int id = std::stoi(id_token->second);

          if (type->second == "NODE") {
            auto node = std::find_if(
                _nodes.begin(), _nodes.end(),
                [&id](const auto& node) { return node->id() == id; });

            if (node == _nodes.end()) {
              _nodes.emplace_back(std::make_unique<graph_node>(id));
              node = _nodes.end() - 1;
              add_tokens("ANSWER", tokens, **node);
            }
          }

          if (type->second == "EDGE") {
            auto parent_token = std::find_if(
                tokens.begin(), tokens.end(),
                [](const std::pair<std::string, std::string>& pair) {
                  return pair.first == "PARENT";
                });
            auto child_token = std::find_if(
                tokens.begin(), tokens.end(),
                [](const std::pair<std::string, std::string>& pair) {
                  return pair.first == "CHILD";
                });

            if (parent_token != tokens.end() && child_token != tokens.end()) {
              const auto& parent = *std::find_if(
                  _nodes.begin(), _nodes.end(),
                  [&parent_token](const auto& node) {
                    return node->id() == std::stoi(parent_token->second);
                  });
              const auto& child = *std::find_if(
                  _nodes.begin(), _nodes.end(),
                  [&child_token](const auto& node) {
                    return node->id() == std::stoi(child_token->second);
                  });

              auto edge = std::make_unique<graph_edge>(id);
              edge->child(child.get());
              edge->parent(parent.get());
              _edges.emplace_back(edge.get());

              add_tokens("KEYWORD", tokens, *edge);
              child->add_to_parent(edge.get());
              parent->add_to_child(std::move(edge));
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
  graph_node* root_node = nullptr;
  for (auto& node : _nodes) {
    if (node->parent_count() == 0) {
      if (root_node == nullptr) {
        root_node = node.get();
      } else {
        std::cout << "ERROR : Multiple root nodes detected" << std::endl;
      }
    }
  }

  auto bot = chat_bot{"../images/chatbot.png"};

  // add pointer to chatlogic so that chatbot answers can be passed on to the
  // GUI
  bot.chat_logic_handle(this);

  bot.root_node(root_node);
  root_node->move_chat_bot_here(std::move(bot));
}

void chat_logic::panel_dialog_handle(ChatBotPanelDialog* dialog) {
  _panel_dialog = dialog;
}

void chat_logic::chat_bot_handle(chat_bot* bot) { _chat_bot = bot; }

void chat_logic::send_to_chat_bot(std::string message) {
  _chat_bot->receive_from_user(std::move(message));
}

void chat_logic::send_to_user(std::string message) {
  _panel_dialog->PrintChatbotResponse(std::move(message));
}

wxBitmap* chat_logic::chat_bot_image() { return _chat_bot->image_handle(); }