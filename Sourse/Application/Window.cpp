
#include "public/Window.h"

#include "public/Operator.h"
#include "public/Win32API.h"

void Window::Draw() {
  FOREACH_NODE(UIItem, (&areas), area_node) {
    /*
    Editor* edt = area_node->Data->editor;

    FBuff* fbuff = SysH->getFBuff();

    area_node->Data->rect.v0->assign(0, 0);
    area_node->Data->rect.v1->assign(0, fbuff->height);
    area_node->Data->rect.v2->assign(fbuff->width, fbuff->height);
    area_node->Data->rect.v3->assign(fbuff->width, 0);

    // edt->Draw(scene, &area_node->Data->rect, fbuff);
    */
  }
}

Window::Window(std::string* configfolder, List<Operator>* operators) {
  HeapSetInformation(NULL, HeapEnableTerminationOnCorruption, NULL, 0);
  SystemHandler* SysHdl = new SystemHandler(800, 800);

  if (!SUCCEEDED(CoInitialize(NULL))) {
    printf("ERROR: imma about to crash\n");
  }

  if (!SUCCEEDED(SysHdl->Initialize())){
    printf("ERROR: system handler is out of his mind\n");
  }
  
  this->SysH = SysHdl;

  std::string keymap_path = *configfolder + "KeyMaps\\Default.txt";
  std::string icon_path = *configfolder + "icon.ico";
  SysH->SetIcon(icon_path);
  compiled_key_map.Compile(operators, &user_inputs, &keymap_path);
}

Window::~Window() { CoUninitialize(); }

void Window::OnWrite() {}

void Window::OnRead() {}

void Window::ProcessEvents(List<OpThread>* op_threads) {
  SysH->getUserInputs(&user_inputs);
  if (this->IsActive()) {
    compiled_key_map.ProcEvents(op_threads);
  }
}

void Window::SendBuffToSystem() {
  SysH->SysOutput();
}

bool Window::IsActive() {
  return SysH->active();
}

void Window::Destroy() {
  SysH->destroy();
}

void Window::ToggleConsole() {

  SysH->consoletoggle();
}

void Window::getWinRect(Rect<SCR_UINT>& rect) {
  SysH->getRect(rect);
}

void Window::setWinRect(Rect<SCR_UINT>& rect) {
  SysH->setRect(rect);
}
