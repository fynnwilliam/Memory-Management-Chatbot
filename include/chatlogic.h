#ifndef CHATLOGIC_H
#define CHATLOGIC_H

#include <string>
#include <vector>

#include "chatgui.h"

class chat_bot;
class graph_edge;
class graph_node;

class chat_logic {
private:
  std::vector<std::unique_ptr<graph_node>> _nodes;
  std::vector<graph_edge*> _edges;
  graph_node* _current_node = nullptr;
  chat_bot* _chat_bot = nullptr;
  panel_dialog* _panel_dialog = nullptr;

  using tokenlist = std::vector<std::pair<std::string, std::string>>;

  template <typename T>
  void add_tokens(std::string token_id, tokenlist& tokens, T& element);

public:
  chat_logic();
  ~chat_logic();

  void panel_dialog_handle(panel_dialog* dialog);
  void chat_bot_handle(chat_bot* bot);
  void load_answer_graph(std::string filename);
  void send_to_chat_bot(std::string message);
  void send_to_user(std::string message);
  wxBitmap* chat_bot_image();
};

#endif // CHATLOGIC_H