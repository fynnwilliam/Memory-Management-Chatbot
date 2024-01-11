#include "chatbot.h"

#include <algorithm>
#include <ctime>
#include <functional>
#include <iostream>
#include <random>

#include "chatlogic.h"
#include "graphedge.h"
#include "graphnode.h"

namespace {
template <typename T>
auto deep_copy(const T* item) {
  return item ? new T{*item} : nullptr;
}
}  // namespace

ChatBot::ChatBot() = default;

// constructor WITH memory allocation
ChatBot::ChatBot(std::string filename) {
  std::cout << "ChatBot Constructor\n";

  // load image into heap memory
  _image = new wxBitmap(filename, wxBITMAP_TYPE_PNG);
}

ChatBot::~ChatBot() {
  std::cout << "ChatBot Destructor\n";
std::cout << _currentNode << '\n'
          << _rootNode << '\n'
          << _chatLogic << '\n';
  if (_image != nullptr) {
    delete _image;
    _image = nullptr;
  }
}

ChatBot::ChatBot(const ChatBot& other) {
  std::cout << "copy ctor...\n";
  _image = deep_copy(other._image);
  _currentNode = other._currentNode;
  _rootNode = other._rootNode;
  _chatLogic = other._chatLogic;
}

ChatBot::ChatBot(ChatBot&& other) noexcept {
  std::cout << "move ctor...\n";
  _image = other._image;
  _currentNode = other._currentNode;
  _rootNode = other._rootNode;
  _chatLogic = other._chatLogic;

  other._image = nullptr;
}

ChatBot& ChatBot::operator=(const ChatBot& rhs) {
  std::cout << "copy assignment operator\n";
  _image = deep_copy(rhs._image);
  _currentNode = rhs._currentNode;
  _rootNode = rhs._rootNode;
  _chatLogic = rhs._chatLogic;
  return *this;
}

ChatBot& ChatBot::operator=(ChatBot&& rhs) {
  std::cout << "move assignment operator\n";
  _image = rhs._image;
  _currentNode = rhs._currentNode;
  _rootNode = rhs._rootNode;
  _chatLogic = rhs._chatLogic;

  rhs._image = nullptr;
  return *this;
}

void ChatBot::ReceiveMessageFromUser(std::string message) {
  // loop over all edges and keywords and compute Levenshtein distance to query
  typedef std::pair<GraphEdge&, int> EdgeDist;
  std::vector<EdgeDist> levDists;  // format is <ptr,levDist>

  for (size_t index = 0, size = _currentNode->GetNumberOfChildEdges();
       index < size; ++index) {
    auto& edge = _currentNode->GetChildEdgeAtIndex(index);
    for (const auto& keyword : edge.GetKeywords()) {
      levDists.emplace_back(std::ref(edge),
                            ComputeLevenshteinDistance(keyword, message));
    }
  }

  // select best fitting edge to proceed along
  GraphNode* newNode;
  if (levDists.size() > 0) {
    // sort in ascending order of Levenshtein distance (best fit is at the top)
    std::sort(levDists.begin(), levDists.end(),
              [](const EdgeDist& a, const EdgeDist& b) {
                return a.second < b.second;
              });
    newNode = levDists.at(0).first.GetChildNode();  // after sorting the best
                                                    // edge is at first position
  } else {
    // go back to root node
    newNode = _rootNode;
  }

  // tell current node to move chatbot to new node
  _currentNode->MoveChatbotToNewNode(newNode);
}

void ChatBot::SetCurrentNode(GraphNode* node) {
  // update pointer to current node
  _currentNode = node;

  // select a random node answer (if several answers should exist)
  const auto& answers = _currentNode->GetAnswers();

  std::mt19937 generator(int(std::time(0)));
  std::uniform_int_distribution<int> dis(0, answers.size() - 1);
  std::string answer = answers.at(dis(generator));

  _chatLogic->SetChatbotHandle(this);
  // send selected node answer to user
  _chatLogic->SendMessageToUser(answer);
}

int ChatBot::ComputeLevenshteinDistance(std::string s1, std::string s2) {
  // convert both strings to upper-case before comparing
  std::transform(s1.begin(), s1.end(), s1.begin(), ::toupper);
  std::transform(s2.begin(), s2.end(), s2.begin(), ::toupper);

  // compute Levenshtein distance measure between both strings
  const size_t m(s1.size());
  const size_t n(s2.size());

  if (m == 0) return n;
  if (n == 0) return m;

  size_t* costs = new size_t[n + 1];

  for (size_t k = 0; k <= n; k++) costs[k] = k;

  size_t i = 0;
  for (std::string::const_iterator it1 = s1.begin(); it1 != s1.end();
       ++it1, ++i) {
    costs[0] = i + 1;
    size_t corner = i;

    size_t j = 0;
    for (std::string::const_iterator it2 = s2.begin(); it2 != s2.end();
         ++it2, ++j) {
      size_t upper = costs[j + 1];
      if (*it1 == *it2) {
        costs[j + 1] = corner;
      } else {
        size_t t(upper < corner ? upper : corner);
        costs[j + 1] = (costs[j] < t ? costs[j] : t) + 1;
      }

      corner = upper;
    }
  }

  int result = costs[n];
  delete[] costs;

  return result;
}