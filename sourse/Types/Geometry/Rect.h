
#pragma once

#include "Vec2.h"


template <typename Type>
struct Rect {

  vec2<Type> pos, size;

  Rect() {}

  Rect(Rect<Type>& rec) {
    this->pos = rec.pos;
    this->size = rec.size;
  }

  Rect(const vec2<Type>& pos, const vec2<Type>& size) {
    this->pos = pos;
    this->size = size;
  }

  Rect(Type posx, Type posy, Type sizex, Type sizey) {
    this->pos.assign(posx, posy);
    this->size.assign(sizex, sizey);
  }

  // convertion
  template <typename ConversionType>
  Rect<Type>& operator=(Rect<ConversionType>& rect) {
    pos = rect.pos;
    size = rect.size;
    return *this;
  }

  bool operator==(Rect<Type>& rect) {
    return (pos == rect.pos && size == rect.size);
  }

  bool enclosed_in(Rect<Type>& rect, bool prnt = false) { 

    if (prnt) {
      return (pos.x + size.x <= rect.size.x && pos.y + size.y <= rect.size.y && pos.x >= 0 && pos.y >= 0);
    }

    pos -= rect.pos;
    bool ret = this->enclosed_in(rect, true); 
    pos += rect.pos;
    return ret;
  }

  void intersection(Rect<Type>& rect, Rect<Type>& out) {
    if (overlap(rect)) {
      out = *this;
      for (char i = 0; i < 2; i++) {
        CLAMP(out.pos[i], rect.pos[i], rect.pos[i] + rect.size[i]);
        Type p2 = pos[i] + size[i]; 
        CLAMP(p2, rect.pos[i], rect.pos[i] + rect.size[i]);
        out.size[i] = p2 - out.pos[i];
      }
    } else {
      out.size.assign(0, 0);
      out.pos.assign(0, 0);
    }
  }

  bool inside(vec2<Type>& p) { return (p.operator>(pos) && (pos + size).operator>(p)); }

  bool inside(Type x, Type y) { return (pos.x < x && pos.y < y && pos.x + size.x > x && pos.y + size.y > y); }

  inline vec2<Type> size_vec() { return vec2<Type>(size.x, size.y); }

  inline vec2<Type> size_vec_w() { return vec2<Type>(size.x + pos.x, size.y + pos.y); }

  void inv_y(Type scr_y) { pos.y = scr_y - pos.y - size.y; }

  void move(Type dx, Type dy) {
    pos.x += dx;
    pos.y += dy;
  }

  inline bool above(Rect<Type>& rect) { return (pos.y + size.y < rect.pos.y); }
  inline bool bellow(Rect<Type>& rect) { return (rect.pos.y + rect.size.y < pos.y); }
  inline bool right(Rect<Type>& rect) { return (pos.x + size.x < rect.pos.x); }
  inline bool left(Rect<Type>& rect) { return (rect.pos.x + rect.size.x < pos.x); }

  inline bool intersect_y(Rect<Type>& rect) { 
    IF(INRANGE(rect.pos.x, pos.x, pos.x + size.x), return true)
    IF(INRANGE(pos.x, rect.pos.x, rect.pos.x + rect.size.x), return true)
    return false;
  }

  inline bool intersect_x(Rect<Type>& rect) {
    IF(INRANGE(rect.pos.y, pos.y, pos.y + size.y), return true)
    IF(INRANGE(pos.y, rect.pos.y, rect.pos.y + rect.size.y), return true)
    return false;
  }

  bool overlap(Rect<Type>& rect) {
    return (intersect_x(rect) && intersect_y(rect));
  } 
};

template <typename Type>
struct Edge {
  vec2<Type> v1, v2;
};