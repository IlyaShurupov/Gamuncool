#pragma once
#include "Property.h"
enum class OpEventState {
  EXECUTE,
  INVOKE,
  MODAL_EVENT,
};

enum class OpState {
  NONE = 0,
  RUNNING_MODAL,
  FINISHED,
  CANCELED,
};

enum class ThreadState {
  RUNNING,
  CLOSED,
  DENIED,
};

struct ModalEvent {
  Str idname;

  ModalEvent(){};
  ModalEvent(Str idname) { this->idname = idname; }
  ModalEvent(char* idname) { this->idname = idname; }
};

struct Operator {
  // OP idname
  struct Str idname;

  // Current state of op
  OpState state;

  // Modal Map event idnames
  class List<ModalEvent> modal_events;

  // Arguments
  Properties Props;

  // Callbacks
  void (*Invoke)(struct Seance* C, Operator* op) = nullptr;
  void (*Execute)(struct Seance* C, Operator* op) = nullptr;
  void (*Modal)(struct Seance* C, Operator* op, ModalEvent* modal_ev) = nullptr;
  bool (*Poll)(struct Seance* C, Operator* op) = nullptr;

  // Modal cache data
  void* CustomData = nullptr;

  ~Operator();
};

struct OpThread {
  OpThread(Operator* op, OpEventState op_event, ModalEvent* modal_event);

  ThreadState state;
  Operator* op;
  OpEventState op_event;
  ModalEvent* modal_event;
};

void initOps(struct Seance* C);
Operator* find_op(List<Operator>* operators, Str* op_idname);