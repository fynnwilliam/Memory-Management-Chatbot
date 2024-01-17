#ifndef CHATBOT_H_
#define CHATBOT_H_

#include <wx/bitmap.h>

#include <string>
class graph_node;
class chat_logic;

class chat_bot {
private:
  wxBitmap* _image = nullptr;
  graph_node* _current_node = nullptr;
  graph_node* _root_node = nullptr;
  chat_logic* _chat_logic = nullptr;

  int levenshtein_distance(std::string s1, std::string s2);

public:
  chat_bot();
  chat_bot(std::string filename);
  ~chat_bot();

  chat_bot(const chat_bot& other);
  chat_bot(chat_bot&& other) noexcept;
  chat_bot& operator=(const chat_bot& rhs);
  chat_bot& operator=(chat_bot&& rhs);

  void current_node(graph_node* node);
  void root_node(graph_node* root_node) { _root_node = root_node; }
  void chat_logic_handle(chat_logic* chat_logic) { _chat_logic = chat_logic; }
  chat_logic* chat_logic_handle() { return _chat_logic; }
  wxBitmap* image_handle() { return _image; }
  void receive_from_user(std::string message);
};

#endif // CHATBOT_H_