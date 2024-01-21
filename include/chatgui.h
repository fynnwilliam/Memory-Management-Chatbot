#ifndef CHATGUI_H_
#define CHATGUI_H_

#include <wx/wx.h>

#include <memory>

class chat_logic;

// middle part of the window containing the dialog between user and chatbot
class panel_dialog : public wxScrolledWindow {
private:
  wxBoxSizer* _dialog_sizer = new wxBoxSizer(wxVERTICAL);
  wxBitmap _image;
  std::unique_ptr<chat_logic> _chat_logic = std::make_unique<chat_logic>();

  void show_newest_element() noexcept;
  void show_scrollbar() noexcept;

public:
  panel_dialog(wxWindow* parent, wxWindowID id);
  ~panel_dialog();

  chat_logic& chat_logic_handle() { return *_chat_logic; }

  void paint_event(wxPaintEvent& evt);
  void paint_now();
  void render(wxDC& dc);

  void add_dialog_item(wxString text, bool is_from_user = true);
  void print_response(std::string response);

  DECLARE_EVENT_TABLE()
};

class panel_dialog_item : public wxPanel {
private:
  wxStaticBitmap* _chat_bot_img;
  wxStaticText* _chat_bot_tex;

public:
  panel_dialog_item(wxPanel* parent, wxString text, bool is_from_user);
};

class frame : public wxFrame {
private:
  panel_dialog* _panel_dialog;
  wxTextCtrl* _user_text_ctrl;

  void OnEnter(wxCommandEvent& WXUNUSED(event));

public:
  frame(const wxString& title);
};

// control panel for background image display
class frame_image_panel : public wxPanel {
  wxBitmap _image;

public:
  frame_image_panel(wxFrame* parent);

  void paint_event(wxPaintEvent& evt);
  void paint_now();
  void render(wxDC& dc);

  DECLARE_EVENT_TABLE()
};

// wxWidgets app that hides main()
class chat_bot_app : public wxApp {
public:
  virtual bool OnInit() override;
};

#endif // CHATGUI_H_
