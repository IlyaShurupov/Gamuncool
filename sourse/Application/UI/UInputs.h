#pragma once 

#include "Geometry/Vec2.h"
#include "UInputsMap.h"

#define USRINPUT_DECL(name) Input name = Input(#name)

struct UInputs {

  USRINPUT_DECL(K0);
  USRINPUT_DECL(K1);
  USRINPUT_DECL(K2);
  USRINPUT_DECL(K3);
  USRINPUT_DECL(K4);
  USRINPUT_DECL(K5);
  USRINPUT_DECL(K6);
  USRINPUT_DECL(K7);
  USRINPUT_DECL(K8);
  USRINPUT_DECL(K9);

  USRINPUT_DECL(A);
  USRINPUT_DECL(B);
  USRINPUT_DECL(C);
  USRINPUT_DECL(D);
  USRINPUT_DECL(E);
  USRINPUT_DECL(F);
  USRINPUT_DECL(G);
  USRINPUT_DECL(H);
  USRINPUT_DECL(I);
  USRINPUT_DECL(J);
  USRINPUT_DECL(K);
  USRINPUT_DECL(L);
  USRINPUT_DECL(M);
  USRINPUT_DECL(N);
  USRINPUT_DECL(O);
  USRINPUT_DECL(P);
  USRINPUT_DECL(Q);
  USRINPUT_DECL(R);
  USRINPUT_DECL(S);
  USRINPUT_DECL(T);
  USRINPUT_DECL(U);
  USRINPUT_DECL(V);
  USRINPUT_DECL(W);
  USRINPUT_DECL(X);
  USRINPUT_DECL(Y);
  USRINPUT_DECL(Z);

  USRINPUT_DECL(WIN_KEY);
  USRINPUT_DECL(SPACE);
  USRINPUT_DECL(ENTER);
  USRINPUT_DECL(DEL);
  USRINPUT_DECL(EREASE);
  USRINPUT_DECL(ESCAPE);
  USRINPUT_DECL(TAB);

  USRINPUT_DECL(SHIFT_L);
  USRINPUT_DECL(SHIFT_R);
  USRINPUT_DECL(CTR_L);
  USRINPUT_DECL(CTR_R);
  USRINPUT_DECL(ALT_L);
  USRINPUT_DECL(ALT_R);

  USRINPUT_DECL(ARROW_UP);
  USRINPUT_DECL(ARROW_DOWN);
  USRINPUT_DECL(ARROW_LEFT);
  USRINPUT_DECL(ARROW_RIGHT);

  USRINPUT_DECL(RMB);
  USRINPUT_DECL(LMB);
  USRINPUT_DECL(MMB);

  // ...
  USRINPUT_DECL(_UIEND_);

  bool IsEvent = false;
  vec2<SCR_INT> Cdelta;
  vec2<SCR_INT> Cursor;
  vec2<SCR_INT> PrevCursor;
  struct Window* act_window;
};