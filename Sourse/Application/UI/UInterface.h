#pragma once

#include "FrameBuff.h"
#include "Core/Operator.h"

enum struct UIIstate {
  NONE = 0,
  ENTERED,
  INSIDE,
  LEAVED,
};

struct Wrap {
  struct UIItem* rig = nullptr;
  struct UIItem* top = nullptr;
  struct UIItem* lef = nullptr;
  struct UIItem* bot = nullptr;
};

struct UIItem {

  UIItem();
  ~UIItem();

  Hrchy<UIItem> hrchy;

  // Save UI of window
  void OnWrite();

  // Restore saved UI
  void OnRead();

  // User difined callbacks wrapers
  void Resize(Rect<float>& newrect);
  void ProcEvent(Seance* C, vec2<SCR_UINT>& loc_cursor);
  void Draw(UIItem* project_to);

  // User defined callbacks
  void (*ProcBody)(UIItem* This, Seance* C, vec2<SCR_UINT>& loc_cursor);
  void (*DrawBody)(UIItem* This, UIItem* project_to);

  Rect<float> rect;
  vec2<bool> rigid;
  vec2<bool> inv_pos;
  vec2<float> minsize;
  bool ownbuff = true;

  // Event info
  UIIstate state;
  bool redraw = true;

  // Edit info
  char flag = 0;
  Wrap wrap;
  bool hide = false;
  Rect<float> prev_rect;
  
  // Draw info
  FBuff<RGBA_32>* buff = nullptr;
  void* CustomData = nullptr;

  UIItem* find(struct Str* string);

 private:
  void resize_discard(bool dir);
  bool resize_dir(float rescale, bool dir);
  void update_neighbors(bool recursive);
  void update_buff(bool recursive);
  void ResizeBody(Rect<float>& out, bool dir);
  void clear_flags();
  void save_config();
  bool valid_resize(Rect<float>& newrec, bool dir);
};

UIItem* UICompile(List<Operator>* ops, struct DataBlock* db);