
#pragma once
#include "KeyMap.h"
#include "UILayout.h"

struct Window {
  // UI layout of window
  List<UIItem> areas;
  UserInputs user_inputs;

  // Creates empty window
  Window(std::string* configfolder, List<Operator>* operators);

  // Closes the window
  ~Window();

  // Save UI of window
  void OnWrite();

  // Restore saved UI
  void OnRead();

  // Maps user inputs into an executing commands
  void ProcessEvents(List<struct OpThread>* exec_queue);

  // Draws the UI only!! (Buttons, menus...)
  void Draw();

  // Just shows the buff to the user
  void SendBuffToSystem();

  // window processing methods
 public:
  bool IsActive();

  // terminates the window
  void Destroy();

  // toggles window conole
  void ToggleConsole();

  void getWinRect(Rect<SCR_UINT>& rect);

  void setWinRect(Rect<SCR_UINT>& rect);


 private:
  // This is where interactin with the system happends
  // Only things we need from the system are Fbuffer & user inputs
  class SystemHandler* SysH;
  CompiledKeyMap compiled_key_map;
};