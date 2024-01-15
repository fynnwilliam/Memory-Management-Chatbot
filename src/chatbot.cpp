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
template <typename T> auto deep_copy(const T* item) {
  return item ? new T{*item} : nullptr;
}
} // namespace

chat_bot::chat_bot() = default;

chat_bot::chat_bot(std::string filename) {
  std::cout << "chat_bot Constructor\n";
  _image = new wxBitmap(filename, wxBITMAP_TYPE_PNG);
}

chat_bot::~chat_bot() {
  std::cout << "chat_bot Destructor\n";
  std::cout << _current_node << '\n' << _root_node << '\n' << _chat_logic << '\n';
  if (_image != nullptr) {
    delete _image;
    _image = nullptr;
  }
}

chat_bot::chat_bot(const chat_bot& other) {
  std::cout << "copy ctor...\n";
  _image = deep_copy(other._image);
  _current_node = other._current_node;
  _root_node = other._root_node;
  _chat_logic = other._chat_logic;
}

chat_bot::chat_bot(chat_bot&& other) noexcept {
  std::cout << "move ctor...\n";
  _image = other._image;
  _current_node = other._current_node;
  _root_node = other._root_node;
  _chat_logic = other._chat_logic;

  other._image = nullptr;
}

chat_bot& chat_bot::operator=(const chat_bot& rhs) {
  std::cout << "copy assignment operator\n";
  _image = deep_copy(rhs._image);
  _current_node = rhs._current_node;
  _root_node = rhs._root_node;
  _chat_logic = rhs._chat_logic;
  return *this;
}

chat_bot& chat_bot::operator=(chat_bot&& rhs) {
  std::cout << "move assignment operator\n";
  _image = rhs._image;
  _current_node = rhs._current_node;
  _root_node = rhs._root_node;
  _chat_logic = rhs._chat_logic;

  rhs._image = nullptr;
  return *this;
}

void chat_bot::receive_message_from_user(std::string message) {
  // loop over all edges and keywords and compute Levenshtein distance to query
  using edge_dist = std::pair<GraphEdge&, int>;
  std::vector<edge_dist> lev_dists;

  for (size_t index = 0, size = _current_node->GetNumberOfChildEdges();
       index < size; ++index) {
    auto& edge = _current_node->GetChildEdgeAtIndex(index);
    for (const auto& keyword : edge.GetKeywords()) {
      lev_dists.emplace_back(std::ref(edge),
                            levenshtein_distance(keyword, message));
    }
  }

  // select best fitting edge to proceed along
  GraphNode* node;
  if (lev_dists.size() > 0) {
    // sort in ascending order of Levenshtein distance (best fit is at the top)
    std::sort(lev_dists.begin(), lev_dists.end(),
              [](const edge_dist& a, const edge_dist& b) {
                return a.second < b.second;
              });
    node = lev_dists.at(0).first.GetChildNode(); // after sorting the best
                                                   // edge is at first position
  } else {
    // go back to root node
    node = _root_node;
  }

  _current_node->MoveChatbotToNewNode(node);
}

void chat_bot::current_node(GraphNode* node) {
  _current_node = node;

  // select a random node answer (if several answers should exist)
  const auto& answers = _current_node->GetAnswers();

  std::mt19937 generator(int(std::time(0)));
  std::uniform_int_distribution<int> dis(0, answers.size() - 1);
  std::string answer = answers.at(dis(generator));

  _chat_logic->SetChatbotHandle(this);
  _chat_logic->SendMessageToUser(answer);
}

int chat_bot::levenshtein_distance(std::string s1, std::string s2) {
  std::transform(s1.begin(), s1.end(), s1.begin(), ::toupper);
  std::transform(s2.begin(), s2.end(), s2.begin(), ::toupper);

  // compute Levenshtein distance measure between both strings
  const size_t m(s1.size());
  const size_t n(s2.size());

  if (m == 0)
    return n;
  if (n == 0)
    return m;

  size_t* costs = new size_t[n + 1];

  for (size_t k = 0; k <= n; k++)
    costs[k] = k;

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