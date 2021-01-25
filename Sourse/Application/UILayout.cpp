
#include "public/UILayout.h"
#include "public/Seance.h"

// --------- Button ---------------- //

void button_proc(UIItem* This, List<OpThread>* op_threads, struct UserInputs* user_inputs, vec2<SCR_UINT> & cursor) {
  This->upd_ev_state(cursor, user_inputs);

  if (This->redraw) {
    if (user_inputs->LMB.state == InputState::RELEASED) {
      op_threads->add(DBG_NEW OpThread((Operator *)This->CustomData, OpEventState::INVOKE, NULL));
    }
  }
}

void button_draw(UIItem* This, UIItem* project_to) {
  RGBA_32 color = 0xff090909;
  project_to->buff->DrawRect(This->rect, color);
  This->redraw = false;
}

UIItem* ui_add_button(UIItem* parent, vec2<SCR_UINT> pos, List<Operator>* operators, std::string* op_idname) {

  Operator* op_ptr = nullptr;
  Bounds bnds = Bounds(0, op_idname->length());
  FOREACH_NODE(Operator, operators, op_node) {
    if (strs_match(&op_node->Data->idname, op_idname, bnds, bnds)) {
      op_ptr = op_node->Data;
      break;
    }
  }

  if (!op_ptr) {
    return NULL;
  }

  UIItem* button = DBG_NEW UIItem(NULL);
  button->Draw = button_draw;
  button->ProcEvent = button_proc;

  button->CustomData = (void*)op_ptr;

  button->hierarchy.join(parent);


  button->rect = Rect<SCR_UINT>(pos.x, pos.y, 40, 20);

  return button;
}

// --------- Region ---------------- //

void region_proc(UIItem* This, List<OpThread>* op_threads, struct UserInputs* user_inputs, vec2<SCR_UINT> & cursor) {

  This->upd_ev_state(cursor, user_inputs);

  if (This->redraw) {
    FOREACH_NODE(UIItem, (&This->hierarchy.childs), child_node) {
      child_node->Data->ProcEvent(child_node->Data, op_threads, user_inputs, (cursor - This->rect.pos));
    }
  }
}

void region_draw(UIItem* This, UIItem* project_to) {

  RGBA_32 color = 0xffffffff;
  This->buff->clear(&color);

  FOREACH_NODE(UIItem, (&This->hierarchy.childs), child_node) {
    child_node->Data->Draw(child_node->Data, This);
  }

  This->buff->project_to(project_to->buff, This->rect.pos);

  This->redraw = false;
}

UIItem* ui_add_region(UIItem* parent, Rect<SCR_UINT> rect) {
  UIItem* region = DBG_NEW UIItem(&rect.size);

  region->Draw = region_draw;
  region->ProcEvent = region_proc;

  region->rect = rect;

  region->hierarchy.join(parent);
  return region;
}


// ---------  Area ---------------- //

typedef struct AreaData {
  std::string name;
} AreaData;

void area_proc(UIItem* This, List<OpThread>* op_threads, struct UserInputs* user_inputs, vec2<SCR_UINT> & cursor) {

  This->upd_ev_state(cursor, user_inputs);

  if (This->redraw) {
    FOREACH_NODE(UIItem, (&This->hierarchy.childs), child_node) {
      child_node->Data->ProcEvent(child_node->Data, op_threads, user_inputs, (cursor - This->rect.pos));
    }
  }
}

void area_draw(UIItem* This, UIItem* project_to) {

  FOREACH_NODE(UIItem, (&This->hierarchy.childs), child_node) {
    child_node->Data->rect.pos += This->rect.pos;
    child_node->Data->Draw(child_node->Data, project_to);
    child_node->Data->rect.pos -= This->rect.pos;
  }

  This->redraw = false;
}

UIItem* ui_add_area(UIItem* parent, Rect<SCR_UINT> rect, std::string name) {
  UIItem* Area = DBG_NEW UIItem(NULL);

  Area->Draw = area_draw;
  Area->ProcEvent = area_proc;

  AreaData* ad = DBG_NEW AreaData;
  Area->CustomData = (void*)ad;

  ad->name = name;
  Area->rect = rect;

  Area->hierarchy.join(parent);
  return Area;
}

// ------------------ UI Root --------------------------------- //

void Uiproc(UIItem* This, List<OpThread>* op_threads, struct UserInputs* user_inputs, vec2<SCR_UINT> & loc_cursor) {

  FOREACH_NODE(UIItem, (&This->hierarchy.childs), child_node) {
    child_node->Data->ProcEvent(child_node->Data, op_threads, user_inputs, loc_cursor);
  }

  This->redraw = true;
}

void UIdraw(UIItem* This, UIItem* project_to) {

  RGBA_32 color = 0xff1d1d21;
  This->buff->clear(&color);

  FOREACH_NODE(UIItem, (&This->hierarchy.childs), child_node) {
    child_node->Data->Draw(child_node->Data, This);
  }

  This->redraw = false;
}

UIItem* ui_add_root(Rect<SCR_UINT> rect) {

  UIItem* UIroot = DBG_NEW UIItem(&rect.size);
  UIroot->ProcEvent = Uiproc;
  UIroot->Draw = UIdraw;

  UIroot->rect = rect;
  UIroot->minsize.y = 60;
  UIroot->minsize.x = 100;

  return UIroot;
}

// ---------------------- UI compiling -------------------------  //

UIItem* UI_compile(List<Operator>* operators, std::string* ui_path, Window* parent) {

  UIItem* UIroot = ui_add_root(Rect<SCR_UINT>(550, 200, 900, 600));

  UIItem* Area = ui_add_area(UIroot, Rect<SCR_UINT>(20, 20, 300, 300), "View3d");

  UIItem* Region = ui_add_region(Area, Rect<SCR_UINT>(20, 20, 200, 200));

  UIItem* Button = ui_add_button(Region, vec2<SCR_UINT>(20, 20), operators, &std::string("Toggle Console"));

  return UIroot;
}

void UIItem::upd_ev_state(vec2<SCR_UINT>& cursor, struct UserInputs* user_inputs) {
  UICursorState newState;

  if (rect.inside(cursor)) {

    if (ev_state == UICursorState::NONE) {
      newState = UICursorState::ENTERED;
    } else {
      newState = UICursorState::INSIDE;
      redraw = true;
    }

  } else {
    if (ev_state == UICursorState::INSIDE) {
      newState = UICursorState::LEAVED;
    } else {
      newState = UICursorState::NONE;
    }
  }

  if (ev_state != newState) {
    redraw = true;
    ev_state = newState;
  }
}
