#pragma once

#include "Core/Seance.h"
#include "Core/Operator.h"
#include "Object.h"
#include "Parser.h"
#include "Platform/SysHandler.h"
#include "UI/UInputs.h"
#include "UI/UInputsMap.h"
#include "UI/UInterface.h"

Seance::Seance(Str* path) {

  // load project
  if (/*file specified*/ false) {  
    OnRead(/*file path*/);
    return;
  }  

  // Load defaults
  initOps(this);

  path->trim(Range(0, path->rfind('\\', Range(0, path->length))));
  *path += Str("Configuration\\");

  Str ui_path;
  ui_path = *path;
  ui_path += Str("UInterface\\Default.yaml");
  ui.UIroot = UICompile(&operators, Read_Yaml(&ui_path));

  Str km_path;
  km_path = *path;
  km_path += Str("UInputsMap\\Default.yaml");

  (ui.kmap = NEW_DBG(KeyMap) KeyMap())->Compile(Read_Yaml(&km_path), &operators, ui.UIroot);

  ui.sysh = NEW_DBG(SysHandler) SysHandler();

  // Set icon
  Str icon_path;
  icon_path = *path;
  icon_path += Str("icon.ico");
  ui.sysh->SetIcon(icon_path);

}

Seance::~Seance() {
  objects.del();
  operators.del();
  threads.del();
}

void Seance::OnWrite(/*file path*/) {}

void Seance::OnRead(/*file path*/) {}

void UserInterface::Input(Seance& C) {

  if (!sysh->Active()) {
    return;
  }

  sysh->Inputs(*kmap->uinputs);
  
  if (kmap->uinputs->IsEvent) {
    kmap->evaluate(&C.threads);
    UIroot->ProcEvent(&C, kmap->uinputs->Cursor);
  }

}

void UserInterface::Output() {

  if (kmap->uinputs->IsEvent) {
    UIroot->Draw(nullptr);
    sysh->Output(UIroot);
  }
}