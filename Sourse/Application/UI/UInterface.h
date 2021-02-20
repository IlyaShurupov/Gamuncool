#pragma once

#include "BitMap/BitMap.h"
#include "Operator/Operator.h"
#include "Hierarchy.h"

enum struct UIIstate {
  NONE = 0,
  ENTERED,
  INSIDE,
  LEAVED,
};

struct Wrap {
  class UIItem* rig = nullptr;
  class UIItem* top = nullptr;
  class UIItem* lef = nullptr;
  class UIItem* bot = nullptr;
};

class UIItem {
 public:
  UIItem(struct DataBlock* UIdb);
  ~UIItem();

  Hrchy<UIItem> hrchy;

  // Save UI of window
  void OnWrite();

  // Restore saved UI
  void OnRead();

  // User difined callbacks wrapers
  void Resize(Rect<float>& newrect);
  void ProcEvent(struct Seance* C, vec2<SCR_INT>& loc_cursor);
  void Draw(UIItem* project_to);

  // User defined callbacks
  virtual void ProcBody(UIItem* This, Seance* C, vec2<SCR_INT>& loc_cursor) {}
  virtual void DrawBody(UIItem* This, UIItem* draw_to) {}

  Rect<float> rect;
  vec2<bool> rigid;
  vec2<bool> inv_pos;
  vec2<float> minsize;
  bool ownbuff = true;
  bool infinite_ = true;

  // Event info
  UIIstate state;
  bool redraw = true;

  // Edit info
  char flag = 0;
  Wrap wrap;
  bool hide = false;
  Rect<float> prev_rect;
  
  // Draw info
  BitMap<RGBA_32>* buff = nullptr;

  UIItem* find(struct Str* string);

  UIItem* active_lower();
  void move(vec2<float> pos);
  void PosInParent(UIItem* inframe, vec2<float>& out);
  bool valid(bool dir);

 private:
  UIIstate State(vec2<SCR_INT>& cursor);
  void resize_discard(bool dir);
  bool resize_dir(float rescale, bool dir, bool& root);
  void update_neighbors(bool recursive);
  void update_buff(bool recursive);
  void ResizeBody(Rect<float>& out, bool dir);
  void clear_flags();
  void save_config();
};

UIItem* UICompile(struct Operators* ops, struct DataBlock* db);