
#pragma once

#include "Rect.h"
#include "LinkedList.h"
#include <cassert>

#define RGBA_32 int32_t
#define FBFF_COLOR RGBA_32

#define COL32_A 0xff000000
#define COL32_R 0x00ff0000
#define COL32_G 0x0000ff00
#define COL32_B 0x000000ff

namespace COLOR_RGBA_32 {
  inline void set_A(RGBA_32& color, unsigned char val) {
    color &= 0x00ffffff;
    color |= (RGBA_32(val) << 24);
  }

  inline void set_R(RGBA_32& color, unsigned char val) {
    color &= 0xff00ffff;
    color |= (RGBA_32(val) << 16);
  }

  inline void set_G(RGBA_32& color, unsigned char val) {
    color &= 0xffff00ff;
    color |= (RGBA_32(val) << 8);
  }

  inline void set_B(RGBA_32& color, unsigned char val) {
    color &= 0xffffff00;
    color |= (RGBA_32(val));
  }

  inline unsigned char get_A(RGBA_32& color) {
    unsigned char out = color;
    out = (color & 0xff000000) >> 24;
    return out;
  }

  inline unsigned char get_R(RGBA_32& color) {
    unsigned char out = color;
    out = (color & 0x00ff0000) >> 16;
    return out;
  }

  inline unsigned char get_G(RGBA_32& color) {
    unsigned char out = color;
    out = (color & 0x0000ff00) >> 8;
    return out;
  }

  inline unsigned char get_B(RGBA_32& color) {
    unsigned char out = color;
    out = (color & 0x000000ff);
    return out;
  }

  inline void premultiply(RGBA_32& color) {
    unsigned char A = get_A(color);
    set_R(color, unsigned char((get_R(color) * A) / 255.f));
    set_G(color, unsigned char((get_G(color) * A) / 255.f));
    set_B(color, unsigned char((get_B(color) * A) / 255.f));
  }
};

template <typename Color_t>
struct FBuff {

  // world buffers have their own buffers while local share one 
  Hierarchy<FBuff, List<FBuff>, 0> wrld_hrchy;
  Hierarchy<FBuff, List<FBuff>, 1> local_hrchy;

  // dimentions
  vec2<SCR_UINT> size;

  // pixel array
  Color_t* pxls = nullptr;

  // z order val
  int z_depth;

  // ------  cast properties ---------  //
  
  // position in parent fbuffer
  vec2<SCR_UINT> pos;

  // dimentions of the root fbuffer
  SCR_UINT* root_width = nullptr; 
  SCR_UINT* root_height = nullptr;

  FBuff(SCR_UINT width, SCR_UINT height);
  FBuff();
  ~FBuff();

  Color_t* get(SCR_UINT x, SCR_UINT y);
  void set(SCR_UINT x, SCR_UINT y, Color_t* color);
  
  void resize(SCR_UINT width, SCR_UINT height);
  
  void cast(FBuff& out, Rect<SCR_UINT>& bounds);
  void move(SCR_UINT dx, SCR_UINT dy);

  void premultiply();

  // simple draw methods
  void DrawRect(Rect<SCR_UINT>& rect, Color_t& color);
  void clear(Color_t* color);
};

template <typename Color_t>
FBuff<Color_t>::FBuff() {
  size.assign(0, 0);
  z_depth = 0;
}

template <typename Color_t>
FBuff<Color_t>::FBuff(SCR_UINT width, SCR_UINT height) {
  size.assign(width, height);
  pxls = DBG_NEW Color_t[(__int64)height * width];
}

template <typename Color_t>
void FBuff<Color_t>::set(SCR_UINT x, SCR_UINT y, Color_t* color) {
  *get(x, y) = *color;
}

template <typename Color_t>
void FBuff<Color_t>::clear(Color_t* color) {
  long int len = size.x * size.y;
  for (int i = 0; i < len; i++) {
    pxls[i] = *color;
  }
}

template <typename Color_t>
void FBuff<Color_t>::resize(SCR_UINT width, SCR_UINT height) {
  delete pxls;
  pxls = DBG_NEW Color_t[height * (__int64)width];
  size.assign(width, height);
}

template<typename Color_t>
void FBuff<Color_t>::move(SCR_UINT dx, SCR_UINT dy) {
  pos.x += dx;
  pos.y += dy;

  FOREACH_NODE(FBuff, (&local_hrchy.childs), cld_node) {
    cld_node->Data->move(dx, dy);
  }

  FOREACH_NODE(FBuff, (&wrld_hrchy.childs), cld_node) {
    cld_node->Data->move(dx, dy);
  }
}

// NO CLAMPING
template <typename Color_t>
void FBuff<Color_t>::DrawRect(Rect<SCR_UINT>& rect, Color_t& color) {
  
  SCR_UINT lastpxlx = rect.pos.x + rect.size.x;
  SCR_UINT lastpxly = rect.pos.y + rect.size.y;

  for (SCR_UINT i = rect.pos.x; i < lastpxlx; i++) {
    for (SCR_UINT j = rect.pos.y; j < lastpxly; j++) {
      set(i, j, &color);
    }
  }
}

template <typename Color_t>
Color_t* FBuff<Color_t>::get(SCR_UINT x, SCR_UINT y) {

  if (!local_hrchy.parent) {
    return pxls + (__int64)size.x * y + x;
  }

  return local_hrchy.parent->pxls + *root_width * ((__int64)y + pos.y) + (x + pos.x);
}

template <typename Color_t>
void FBuff<Color_t>::cast(FBuff& out, Rect<SCR_UINT>& rect) {
  
  assert((rect.size.y <= size.y) && (rect.size.x <= size.y));

  local_hrchy.childs.add(&out);

  out.local_hrchy.parent = this;

  FBuff* root = local_hrchy.root();

  out.root_height = &root->size.y;
  out.root_width = &root->size.x;
  out.pxls = get(rect.pos.x, rect.pos.y);
}

template<typename Color_t>
void FBuff<Color_t>::premultiply() {
  for (SCR_UINT i = 0; i < size.x; i++) {
    for (SCR_UINT j = 0; j < size.y; j++) {
      COLOR_RGBA_32::premultiply(*get(i, j));
    }
  }
}

template <typename Color_t>
FBuff<Color_t>::~FBuff() {
  delete pxls;
  local_hrchy.leave();
}