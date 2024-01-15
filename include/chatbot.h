#ifndef CHATBOT_H_
#define CHATBOT_H_

#include <wx/bitmap.h>

#include <string>
class GraphNode;
class ChatLogic;

class chat_bot {
private:
  wxBitmap* _image = nullptr;
  GraphNode* _current_node = nullptr;
  GraphNode* _root_node = nullptr;
  ChatLogic* _chat_logic = nullptr;

  int levenshtein_distance(std::string s1, std::string s2);

public:
  chat_bot();
  chat_bot(std::string filename);
  ~chat_bot();

  chat_bot(const chat_bot& other);
  chat_bot(chat_bot&& other) noexcept;
  chat_bot& operator=(const chat_bot& rhs);
  chat_bot& operator=(chat_bot&& rhs);

  void current_node(GraphNode* node);
  void root_node(GraphNode* root_node) { _root_node = root_node; }
  void chat_logic_handle(ChatLogic* chat_logic) { _chat_logic = chat_logic; }
  ChatLogic* chat_logic_handle() { return _chat_logic; }
  wxBitmap* image_handle() { return _image; }

  void receive_message_from_user(std::string message);
};

#endif // CHATBOT_H_