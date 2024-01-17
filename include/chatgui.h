#ifndef CHATGUI_H_
#define CHATGUI_H_

#include <wx/wx.h>

#include <memory>

class chat_logic;

// middle part of the window containing the dialog between user and chatbot
class ChatBotPanelDialog : public wxScrolledWindow {
private:
  wxBoxSizer* _dialogSizer = new wxBoxSizer(wxVERTICAL);
  wxBitmap _image;
  std::unique_ptr<chat_logic> _chat_logic = std::make_unique<chat_logic>();

  void show_newest_element() noexcept;
  void show_scrollbar() noexcept;

public:
  ChatBotPanelDialog(wxWindow* parent, wxWindowID id);
  ~ChatBotPanelDialog();

  chat_logic& chat_logic_handle() { return *_chat_logic; }

  void paintEvent(wxPaintEvent& evt);
  void paintNow();
  void render(wxDC& dc);

  void AddDialogItem(wxString text, bool isFromUser = true);
  void PrintChatbotResponse(std::string response);

  DECLARE_EVENT_TABLE()
};

class ChatBotPanelDialogItem : public wxPanel {
private:
  wxStaticBitmap* _chatBotImg;
  wxStaticText* _chatBotTxt;

public:
  ChatBotPanelDialogItem(wxPanel* parent, wxString text, bool isFromUser);
};

class ChatBotFrame : public wxFrame {
private:
  ChatBotPanelDialog* _panel_dialog;
  wxTextCtrl* _userTextCtrl;

  void OnEnter(wxCommandEvent& WXUNUSED(event));

public:
  ChatBotFrame(const wxString& title);
};

// control panel for background image display
class ChatBotFrameImagePanel : public wxPanel {
  wxBitmap _image;

public:
  ChatBotFrameImagePanel(wxFrame* parent);

  void paintEvent(wxPaintEvent& evt);
  void paintNow();
  void render(wxDC& dc);

  DECLARE_EVENT_TABLE()
};

// wxWidgets app that hides main()
class ChatBotApp : public wxApp {
public:
  virtual bool OnInit();
};

#endif // CHATGUI_H_
