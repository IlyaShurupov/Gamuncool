
#include "public/UILayout.h"
#include "Object.h"
#include "public/KeyMap.h"
#include "public/Seance.h"



UIItem::UIItem(vec2<SCR_UINT>* size) {

  rs_type = UIResizeType::NONE;
  state = UIstate::NONE;
  crnr = UIAttachCorner::BOTTOMLEFT;
  ProcBody = nullptr;
  DrawBody = nullptr;

  if (this->ownbuff = (bool)size) {
    buff = NEW_DBG(FBuff<RGBA_32>) FBuff<RGBA_32>();
    buff->resize(size->x, size->y);
  }
}

UIItem::~UIItem() {
  hierarchy.childs.del();
  if (buff) {
    DELETE_DBG(FBuff<RGBA_32>, buff);
  }
  // if (CustomData)
  // DELETE_DBG() CustomData;
}

void UIItem::ProcEvent(List<OpThread>* op_threads, struct UserInputs* user_inputs, vec2<SCR_UINT>& cursor, Seance* C) {

  if (hide)
    return;

  UIstate newState;

  if (rect.inside((float)cursor.x, (float)cursor.y)) {

    if (state == UIstate::NONE) {
      newState = UIstate::ENTERED;
    } else {
      newState = UIstate::INSIDE;
      redraw = true;
    }

  } else {
    if (state == UIstate::INSIDE) {
      newState = UIstate::LEAVED;
    } else {
      newState = UIstate::NONE;
    }
  }

  if (state != newState) {
    redraw = true;
    state = newState;
  }

  if (state == UIstate::INSIDE) {
    if (ProcBody)
      ProcBody(this, op_threads, user_inputs, cursor, C);
  }

  if (redraw) {
    FOREACH_NODE(UIItem, (&hierarchy.childs), child_node) {
      vec2<SCR_UINT> pos = vec2<SCR_UINT>((SCR_UINT)rect.pos.x, (SCR_UINT)rect.pos.y);
      child_node->Data->ProcEvent(op_threads, user_inputs, (cursor - pos), C);
    }
  }
}

void UIItem::Draw(UIItem* project_to) {

  if (hide)
    return;

  if (DrawBody)
    DrawBody(this, project_to);

  FOREACH_NODE(UIItem, (&hierarchy.childs), child_node) {

    if (ownbuff) {
      child_node->Data->Draw(this);
      continue;
    }

    child_node->Data->rect.pos += rect.pos;
    child_node->Data->Draw(project_to);
    child_node->Data->rect.pos -= rect.pos;
  }

  if (ownbuff && project_to) {
    buff->project_to(project_to->buff, vec2<SCR_UINT>((SCR_UINT)rect.pos.x, (SCR_UINT)rect.pos.y));
  }

  redraw = false;
}

void UIItem::Resize(vec2<float>& rescale) {

  if (!hierarchy.parent) {
    update_neighbors(true);
  }

  prev_rect = rect;
  Rect<float>& prnt_rec = hierarchy.parent->rect;

  if (rs_type == UIResizeType::FOLLOW || rs_type == UIResizeType::FOLLOW_X) {

    /*
    UIItem* right = wrap.rig;
    UIItem* left = wrap.lef;

    while (right) {
      if (right->rs_type != UIResizeType::FOLLOW && right->rs_type != UIResizeType::FOLLOW_X) {
        right = right->wrap.rig;
        break;
      }
      right = right->wrap.rig;
    }

    while (left) {
      if (left->rs_type != UIResizeType::FOLLOW && left->rs_type != UIResizeType::FOLLOW_X) {
        left = left->wrap.lef;
        break;
      }
      left = left->wrap.lef;
    }

    if (right) {

    }
    if (left) {

    }
    */

    float width = (rect.size.x + rect.pos.x) * rescale.x;
    rect.pos.x *= rescale.x;
    rect.size.x = width - rect.pos.x;
  }

  if (rs_type == UIResizeType::FOLLOW || rs_type == UIResizeType::FOLLOW_Y) {
    float height = (rect.size.y + rect.pos.y) * rescale.y;
    rect.pos.y *= rescale.y;
    rect.size.y = height - rect.pos.y;
  }

  if (rs_type != UIResizeType::FOLLOW) {
    if (crnr == UIAttachCorner::BOTTOMRIGGHT || crnr == UIAttachCorner::TOPRIGHT) {
      float dx = prnt_rec.size.x * (1 - (1 / rescale.x));
      rect.pos.x += dx;
    }

    if (crnr == UIAttachCorner::TOPLEFT || crnr == UIAttachCorner::TOPRIGHT) {
      float dy = prnt_rec.size.y * (1 - (1 / rescale.y));
      rect.pos.y += dy;
    }
  }

  if (hierarchy.parent) {
    if (rs_type != UIResizeType::FOLLOW) {
      hide = false;
      if (rect.pos.x + rect.size.x > prnt_rec.size.x || rect.pos.y + rect.size.y > prnt_rec.size.y || rect.pos.x < 0 || rect.pos.y < 0) {
        hide = true;
        if (buff) {
          buff->free();
        }
      }
    }
  }

  if (ownbuff && !hide) {
    buff->resize((SCR_UINT)rect.size.x, (SCR_UINT)rect.size.y);
  }

  vec2<float> chld_rscl(rect.size.x / prev_rect.size.x, rect.size.y / prev_rect.size.y);

  /*
  FOREACH_NODE(UIItem, (&hierarchy.childs), child_node) {
    if (child_node->Data->rs_type != UIResizeType::FOLLOW) {
      child_node->Data->Resize(chld_rscl);
    }
  }
  */

  FOREACH_NODE(UIItem, (&hierarchy.childs), child_node) {
      child_node->Data->Resize(chld_rscl);
  }

  redraw = true;
}

void UIItem::update_neighbors(bool recursive) {

  FOREACH(&hierarchy.childs, UIItem, ui_node) {
    ui_node->Data->wrap.bot = nullptr;
    ui_node->Data->wrap.rig = nullptr;
    ui_node->Data->wrap.lef = nullptr;
    ui_node->Data->wrap.top = nullptr;
  }

  float dist_t = FLT_MAX;
  float dist_b = FLT_MAX;
  float dist_l = FLT_MAX;
  float dist_r = FLT_MAX;

  FOREACH(&hierarchy.childs, UIItem, cld_node) {
    UIItem& cld = *cld_node->Data;

    if (cld.wrap.top && cld.wrap.bot && cld.wrap.lef && cld.wrap.rig) {
      continue;
    }

    FOREACH(&hierarchy.childs, UIItem, ui_node) {
      Rect<float>& i_rec = ui_node->Data->rect;

      bool intr_y = cld.rect.intersect_y(i_rec);
      bool intr_x = cld.rect.intersect_x(i_rec);

      if (intr_y) {

        if (!cld.wrap.top && cld.rect.above(i_rec) && dist_t > cld.rect.pos.y + cld.rect.size.y - i_rec.pos.y) {
          cld.wrap.top = ui_node->Data;
          ui_node->Data->wrap.bot = &cld;

        } else if (!cld.wrap.bot && cld.rect.bellow(i_rec) && dist_b > cld.rect.pos.y - i_rec.pos.y + i_rec.size.y) {
          cld.wrap.bot = ui_node->Data;
          ui_node->Data->wrap.top = &cld;
        }

      } else if (intr_x) {

        if (!cld.wrap.rig && cld.rect.right(i_rec) && dist_r > cld.rect.pos.x + cld.rect.size.x - i_rec.pos.x) {
          cld.wrap.rig = ui_node->Data;
          ui_node->Data->wrap.lef = &cld;

        } else if (!cld.wrap.lef && cld.rect.left(i_rec) && dist_l > cld.rect.pos.x - i_rec.pos.x + i_rec.size.x) {
          cld.wrap.lef = ui_node->Data;
          ui_node->Data->wrap.rig = &cld;
        }
      }

      if (cld.wrap.top && cld.wrap.bot && cld.wrap.lef && cld.wrap.rig) {
        break;
      }
    }
  }

  if (recursive) {
    FOREACH(&hierarchy.childs, UIItem, ui_node) { ui_node->Data->update_neighbors(recursive); }
  }
}

// --------- Button ---------------- //

void button_proc(UIItem* This, List<OpThread>* op_threads, struct UserInputs* user_inputs, vec2<SCR_UINT>& cursor, Seance* C) {
  if (user_inputs->LMB.state == InputState::RELEASED) {
    op_threads->add(NEW_DBG(OpThread) OpThread((Operator*)This->CustomData, OpEventState::EXECUTE, nullptr));
  }
}

void button_draw(UIItem* This, UIItem* project_to) {

  RGBA_32 color1 = 0xffffffff;
  RGBA_32 color2 = 0xff090909;
  if (This->state == UIstate::LEAVED || This->state == UIstate::NONE) {
    color1 = 0xffaaaaaa;
  }

  Rect<SCR_UINT> rect(This->rect);

  project_to->buff->DrawRect(rect, color1);
  project_to->buff->DrawBounds(rect, color2, 1);
}

UIItem* ui_add_button(UIItem* parent, vec2<SCR_UINT> pos, List<Operator>* operators, Str* op_idname, UIResizeType rs_type, UIAttachCorner crnr) {

  UIItem* button = NEW_DBG(UIItem) UIItem(nullptr);

  button->hierarchy.join(parent);

  button->ownbuff = false;
  button->DrawBody = button_draw;
  button->ProcBody = button_proc;
  button->crnr = crnr;
  button->rect.size.assign(40, 20);
  button->rect.pos.assign((float)pos.x, (float)pos.y);

  // own
  Operator* op_ptr = find_op(operators, op_idname);
  if (!op_ptr) {
    return nullptr;
  }
  button->CustomData = (void*)op_ptr;
  // own

  return button;
}


// --------- Region ---------------- //

typedef struct UIRegionData {
  Operator* op = nullptr;
  Object* RS_ptr = nullptr;
} UIRegionData;

void region_proc(UIItem* This, List<OpThread>* op_threads, struct UserInputs* user_inputs, vec2<SCR_UINT>& cursor, Seance* C) {

  UIRegionData* rd = (UIRegionData*)This->CustomData;

  if (rd->RS_ptr) {

    op_threads->add(NEW_DBG(OpThread) OpThread(rd->op, OpEventState::EXECUTE, nullptr));

  } else {

    FOREACH_NODE(Object, (&C->project.collection), obj_node) {
      if (obj_node->Data->GetRenderComponent()) {
        rd->RS_ptr = obj_node->Data;
        rd->op->Props.Pointers_Buff[0]->assign((void*)This->buff);
        rd->op->Props.Pointers_Obj[0]->assign(rd->RS_ptr);
      }
    }
  }
}

void region_draw(UIItem* This, UIItem* project_to) {}

UIItem* ui_add_region(UIItem* parent, Rect<SCR_UINT> rect, List<Operator>* operators, UIResizeType rs_type, UIAttachCorner crnr) {


  UIItem* region = NEW_DBG(UIItem) UIItem(&rect.size);

  region->hierarchy.join(parent);

  region->ownbuff = true;
  region->DrawBody = region_draw;
  region->ProcBody = region_proc;
  region->crnr = crnr;
  region->rect.size.assign((float)rect.size.x, (float)rect.size.y);
  region->rect.pos.assign((float)rect.pos.x, (float)rect.pos.y);

  region->rs_type = rs_type;

  // own
  Operator* op_ptr = find_op(operators, &Str("Render To Buff"));
  if (!op_ptr) {
    return nullptr;
  }

  UIRegionData* rd = NEW_DBG(UIRegionData) UIRegionData();
  region->CustomData = (void*)rd;
  rd->op = op_ptr;
  // own

  return region;
}


// ---------  Area ---------------- //

void area_proc(UIItem* This, List<OpThread>* op_threads, struct UserInputs* user_inputs, vec2<SCR_UINT>& cursor, Seance* C) {}

void area_draw(UIItem* This, UIItem* project_to) {

  RGBA_32 color2 = 0xff050505;
  short thick = 3;

  if (This->state == UIstate::LEAVED || This->state == UIstate::NONE) {
    color2 = 0xff101010;
    thick = 2;
  }

  Rect<SCR_UINT> rect(This->rect);
  project_to->buff->DrawBounds(rect, color2, thick);
}

UIItem* ui_add_area(UIItem* parent, Rect<SCR_UINT> rect, Str name, UIResizeType rs_type, UIAttachCorner crnr) {

  UIItem* Area = NEW_DBG(UIItem) UIItem(nullptr);

  Area->hierarchy.join(parent);

  Area->ownbuff = false;
  Area->DrawBody = area_draw;
  Area->ProcBody = area_proc;
  Area->idname = name;
  Area->rs_type = rs_type;
  Area->crnr = crnr;
  Area->rect.size.assign((float)rect.size.x, (float)rect.size.y);
  Area->rect.pos.assign((float)rect.pos.x, (float)rect.pos.y);

  // own

  return Area;
}

// ------------------ UI Root --------------------------------- //

void Uiproc(UIItem* This, List<OpThread>* op_threads, struct UserInputs* user_inputs, vec2<SCR_UINT>& loc_cursor, Seance* C) {}

void UIdraw(UIItem* This, UIItem* project_to) {
  RGBA_32 color = 0xff1d1d21;
  This->buff->clear(&color);
}

UIItem* ui_add_root(Rect<SCR_UINT> rect) {

  UIItem* UIroot = NEW_DBG(UIItem) UIItem(&rect.size);

  UIroot->ProcBody = Uiproc;
  UIroot->DrawBody = UIdraw;
  UIroot->rs_type = UIResizeType::FOLLOW;
  UIroot->crnr = UIAttachCorner::BOTTOMLEFT;
  UIroot->rect.size.assign((float)rect.size.x, (float)rect.size.y);
  UIroot->rect.pos.assign((float)rect.pos.x, (float)rect.pos.y);

  UIroot->minsize.y = 60;
  UIroot->minsize.x = 100;
  UIroot->ownbuff = true;
  return UIroot;
}

// ---------------------- UI compiling -------------------------  //

UIItem* UI_compile(List<Operator>* operators, Str* ui_path, Window* parent) {

  UIItem* UIroot = ui_add_root(Rect<SCR_UINT>(550, 200, 900, 600));

  UIItem* Area = ui_add_area(UIroot, Rect<SCR_UINT>(100, 100, 300, 300), "View3d", UIResizeType::FOLLOW, UIAttachCorner::TOPRIGHT);

  UIItem* Region = ui_add_region(Area, Rect<SCR_UINT>(5, 5, 290, 290), operators, UIResizeType::FOLLOW, UIAttachCorner::BOTTOMLEFT);

  UIItem* Button = ui_add_button(Area, vec2<SCR_UINT>(242, 262), operators, &Str("Add Plane"), UIResizeType::NONE, UIAttachCorner::TOPRIGHT);

  short width = 25;
  short border = 10;
  Rect<SCR_UINT> rect = Rect<SCR_UINT>(border, (SCR_UINT)UIroot->rect.size.y - width - border, (SCR_UINT)UIroot->rect.size.x - border * 2, width);
  UIItem* Area2 = ui_add_area(UIroot, rect, "topbar", UIResizeType::FOLLOW_X, UIAttachCorner::TOPLEFT);

  ui_add_button(Area2, vec2<SCR_UINT>(3, 3), operators, &Str("Toggle Console"), UIResizeType::NONE, UIAttachCorner::BOTTOMLEFT);
  ui_add_button(Area2, vec2<SCR_UINT>(3 + 40 * 1, 3), operators, &Str("End Seance"), UIResizeType::NONE, UIAttachCorner::BOTTOMLEFT);
  ui_add_button(Area2, vec2<SCR_UINT>(3 + 40 * 2, 3), operators, &Str("Log Heap"), UIResizeType::NONE, UIAttachCorner::BOTTOMLEFT);
  return UIroot;
}
