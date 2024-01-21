#include "chatgui.h"

#include <wx/colour.h>
#include <wx/filename.h>
#include <wx/image.h>

#include <string>

#include "chatbot.h"
#include "chatlogic.h"

namespace {
// size of chatbot window
const int width = 414;
const int height = 736;

std::string dataPath = "../";
std::string imgBasePath = dataPath + "images/";

const auto& image_from_chat_bot(panel_dialog* dialog) noexcept {
  return *dialog->chat_logic_handle().chat_bot_image();
}
} // namespace

IMPLEMENT_APP(chat_bot_app);

bool chat_bot_app::OnInit() {
  auto f = new frame(wxT("Udacity ChatBot"));
  f->Show(true);
  return true;
}

frame::frame(const wxString& title)
    : wxFrame(NULL, wxID_ANY, title, wxDefaultPosition, wxSize(width, height)) {
  frame_image_panel* ctrlPanel = new frame_image_panel(this);

  _panel_dialog = new panel_dialog(ctrlPanel, wxID_ANY);

  // create text control for user input
  int idTextXtrl = 1;
  _user_text_ctrl = new wxTextCtrl(ctrlPanel, idTextXtrl, "", wxDefaultPosition,
                                   wxSize(width, 50), wxTE_PROCESS_ENTER,
                                   wxDefaultValidator, wxTextCtrlNameStr);
  Connect(idTextXtrl, wxEVT_TEXT_ENTER, wxCommandEventHandler(frame::OnEnter));

  // create vertical sizer for panel alignment and add panels
  wxBoxSizer* vertBoxSizer = new wxBoxSizer(wxVERTICAL);
  vertBoxSizer->AddSpacer(90);
  vertBoxSizer->Add(_panel_dialog, 6, wxEXPAND | wxALL, 0);
  vertBoxSizer->Add(_user_text_ctrl, 1, wxEXPAND | wxALL, 5);
  ctrlPanel->SetSizer(vertBoxSizer);

  Centre();
}

void frame::OnEnter(wxCommandEvent& WXUNUSED(event)) {
  wxString userText = _user_text_ctrl->GetLineText(0);

  _panel_dialog->add_dialog_item(userText, true);
  _user_text_ctrl->Clear();
  _panel_dialog->chat_logic_handle().send_to_chat_bot(
      std::string(userText.mb_str()));
}

BEGIN_EVENT_TABLE(frame_image_panel, wxPanel)
EVT_PAINT(frame_image_panel::paint_event) // catch paint events
END_EVENT_TABLE()

frame_image_panel::frame_image_panel(wxFrame* parent) : wxPanel(parent) {}

void frame_image_panel::paint_event(wxPaintEvent& evt) {
  wxPaintDC dc(this);
  render(dc);
}

void frame_image_panel::paint_now() {
  wxClientDC dc(this);
  render(dc);
}

void frame_image_panel::render(wxDC& dc) {
  wxString imgFile = imgBasePath + "sf_bridge.jpg";
  wxImage image;
  image.LoadFile(imgFile);

  // rescale image to fit window dimensions
  wxSize sz = GetSize();
  wxImage imgSmall =
      image.Rescale(sz.GetWidth(), sz.GetHeight(), wxIMAGE_QUALITY_HIGH);
  _image = wxBitmap(imgSmall);

  dc.DrawBitmap(_image, 0, 0, false);
}

BEGIN_EVENT_TABLE(panel_dialog, wxPanel)
EVT_PAINT(panel_dialog::paint_event) // catch paint events
END_EVENT_TABLE()

panel_dialog::panel_dialog(wxWindow* parent, wxWindowID id)
    : wxScrolledWindow(parent, id) {
  // sizer will take care of determining the needed scroll size
  SetSizer(_dialog_sizer);

  // allow for PNG images to be handled
  wxInitAllImageHandlers();

  // pass pointer to chatbot dialog so answers can be displayed in GUI
  _chat_logic->panel_dialog_handle(this);
  _chat_logic->load_answer_graph(dataPath + "src/answergraph.txt");
}

panel_dialog::~panel_dialog() = default;

void panel_dialog::show_newest_element() noexcept {
  int dx, dy;
  GetScrollPixelsPerUnit(&dx, &dy);
  const int sy = dy * GetScrollLines(wxVERTICAL);
  DoScroll(0, sy);
}

void panel_dialog::show_scrollbar() noexcept {
  FitInside(); // ask the sizer about the needed size
  SetScrollRate(5, 5);
  Layout();
}

void panel_dialog::add_dialog_item(wxString text, bool is_from_user) {
  panel_dialog_item* item = new panel_dialog_item(this, text, is_from_user);
  _dialog_sizer->Add(
      item, 0, wxALL | (is_from_user == true ? wxALIGN_LEFT : wxALIGN_RIGHT),
      8);
  _dialog_sizer->Layout();

  show_scrollbar();
  show_newest_element();
}

void panel_dialog::print_response(std::string response) {
  wxString botText(response.c_str(), wxConvUTF8);
  add_dialog_item(botText, false);
}

void panel_dialog::paint_event(wxPaintEvent& evt) {
  wxPaintDC dc(this);
  render(dc);
}

void panel_dialog::paint_now() {
  wxClientDC dc(this);
  render(dc);
}

void panel_dialog::render(wxDC& dc) {
  wxImage image;
  image.LoadFile(imgBasePath + "sf_bridge_inner.jpg");

  wxSize sz = GetSize();
  wxImage imgSmall =
      image.Rescale(sz.GetWidth(), sz.GetHeight(), wxIMAGE_QUALITY_HIGH);

  _image = wxBitmap(imgSmall);
  dc.DrawBitmap(_image, 0, 0, false);
}

panel_dialog_item::panel_dialog_item(wxPanel* parent, wxString text,
                                     bool is_from_user)
    : wxPanel(parent, -1, wxPoint(-1, -1), wxSize(-1, -1), wxBORDER_NONE) {
  _chat_bot_img = new wxStaticBitmap(
      this, wxID_ANY,
      is_from_user ? wxBitmap(imgBasePath + "user.png", wxBITMAP_TYPE_PNG)
                   : image_from_chat_bot((panel_dialog*)parent),
      wxPoint(-1, -1), wxSize(-1, -1));
  _chat_bot_tex =
      new wxStaticText(this, wxID_ANY, text, wxPoint(-1, -1), wxSize(150, -1),
                       wxALIGN_CENTRE | wxBORDER_NONE);
  _chat_bot_tex->SetForegroundColour(is_from_user == true ? wxColor(*wxBLACK)
                                                          : wxColor(*wxWHITE));

  wxBoxSizer* horzBoxSizer = new wxBoxSizer(wxHORIZONTAL);
  horzBoxSizer->Add(_chat_bot_tex, 8, wxEXPAND | wxALL, 1);
  horzBoxSizer->Add(_chat_bot_img, 2, wxEXPAND | wxALL, 1);
  SetSizer(horzBoxSizer);

  _chat_bot_tex->Wrap(150);
  SetBackgroundColour((is_from_user == true ? wxT("YELLOW") : wxT("BLUE")));
}