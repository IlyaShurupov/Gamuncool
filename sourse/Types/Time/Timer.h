#pragma once

typedef long long int TIME_MS__;

struct Timer {

  TIME_MS__ start;
  TIME_MS__ duration;

  Timer();
  Timer(TIME_MS__ time);
  bool timeout();
  void reset();
  TIME_MS__ past();
  TIME_MS__ remain();

  float ease_in(TIME_MS__ duration = 0);
  float ease_out(TIME_MS__ duration = 0);
};

void TreadSleep(TIME_MS__ duration);
