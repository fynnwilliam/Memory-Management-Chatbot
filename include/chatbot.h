#ifndef CHATBOT_H_
#define CHATBOT_H_

#include <wx/bitmap.h>

#include <string>
class GraphNode;
class ChatLogic;

class ChatBot {
private:
  wxBitmap* _image = nullptr;

  GraphNode* _currentNode = nullptr;
  GraphNode* _rootNode = nullptr;
  ChatLogic* _chatLogic = nullptr;

  int ComputeLevenshteinDistance(std::string s1, std::string s2);

public:
  ChatBot();
  ChatBot(std::string filename);
  ~ChatBot();

  ChatBot(const ChatBot& other);
  ChatBot(ChatBot&& other) noexcept;
  ChatBot& operator=(const ChatBot& rhs);
  ChatBot& operator=(ChatBot&& rhs);

  void SetCurrentNode(GraphNode* node);
  void SetRootNode(GraphNode* rootNode) { _rootNode = rootNode; }
  void SetChatLogicHandle(ChatLogic* chatLogic) { _chatLogic = chatLogic; }
  ChatLogic* GetChatLogicHandle() { return _chatLogic; }
  wxBitmap* GetImageHandle() { return _image; }

  void ReceiveMessageFromUser(std::string message);
};

#endif // CHATBOT_H_