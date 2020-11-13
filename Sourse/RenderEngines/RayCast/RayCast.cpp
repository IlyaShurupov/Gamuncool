#include "public/RayCast.h"

#include "Ray.h"

using namespace RayCast;


struct CycleData;
void RenderPass(RenderSettings* settings, Ray ray, FBFF_COLOR& color);
void WritePassToBuff(FBuff* Buff, SCR_UINT i, SCR_UINT j, CycleData& CData, FBFF_COLOR& color);
void InitCycle(CycleData& CData, RenderSettings* settings);
inline void UpdYPass(CycleData& CData, SCR_UINT& i);
inline void UpdXPass(CycleData& CData, SCR_UINT& i);

struct CycleData {
  SCR_UINT x_passes = 0, y_passes = 0;
  Vec3f step_x, step_y;
  Ray CamRay;
  SCR_UINT pxl_size = 0;
  Camera* CamAtrb = nullptr;
};

void RayCast::RenderToBuff(RenderSettings* Settings, FBuff* Buff) {
  CycleData CData;
  InitCycle(CData, Settings);

  for (SCR_UINT j = 0; j < CData.y_passes; UpdYPass(CData, j)) {
    for (SCR_UINT i = 0; i < CData.x_passes; UpdXPass(CData, i)) {
      FBFF_COLOR Color;
      RenderPass(Settings, CData.CamRay, Color);
      WritePassToBuff(Buff, i, j, CData, Color);
    }
  }
}

void RenderPass(RenderSettings* settings, Ray ray, FBFF_COLOR& color) {
  ray.Dir.Normalize();

  ray.Cast(settings->getObjList(), FLT_MAX);
  if (ray.HitData.Hit) {
    color = 0x000000ff;
  } else {
    color = 0x00ff0000;
  }
}

void WritePassToBuff(FBuff* Buff, SCR_UINT i, SCR_UINT j, CycleData& CData, FBFF_COLOR& color) {
  if (CData.pxl_size == 1) {
    Buff->set(i, j, &color);
    return;
  }

  SCR_UINT pos_x = i * CData.pxl_size - (CData.pxl_size / 2);
  SCR_UINT pos_y = j * CData.pxl_size - (CData.pxl_size / 2);
  CLAMP(pos_x, (SCR_UINT)0, SCR_UINT_MAX);
  CLAMP(pos_y, (SCR_UINT)0, SCR_UINT_MAX);
  Buff->DrawRect(pos_x, pos_y, color, CData.pxl_size, CData.pxl_size);
}

void InitCycle(CycleData& CData, RenderSettings* settings) {
  Object* cam = settings->getCamera();
  Camera* CamAtrb = cam->GetCameraComponent();
  Mat3f* Rotation = &cam->TransformMat;

  float cam_y =
      (float)sqrt(float(CamAtrb->Height.get()) / CamAtrb->Width.get());
  float cam_x = 1 / cam_y;

  SCR_UINT pxl_size = (SCR_UINT)floor(1.f / settings->Resolution.get());

  if (pxl_size != 1 && pxl_size & 1) {
    pxl_size++;
  }

  CData.y_passes = (SCR_UINT)floor(CamAtrb->Height.get() / pxl_size) - 1;
  CData.x_passes = (SCR_UINT)floor(CamAtrb->Width.get() / pxl_size) - 1;

  CData.step_x = Rotation->I * (cam_x / CData.x_passes);
  CData.step_y = Rotation->J * (-cam_y / CData.y_passes);

  cam->getWorldLocation(CData.CamRay.Pos);

  CData.CamRay.Dir -= Rotation->I * (1.f / 2 * cam_x);
  CData.CamRay.Dir += Rotation->J * (1.f / 2 * cam_y);
  CData.CamRay.Dir -= Rotation->K * CamAtrb->Lens.get();
  CData.CamRay.Dir += (CData.step_x / 2) + (CData.step_y / 2);
  CData.CamRay.Dir.Normalize();

  CData.CamAtrb = CamAtrb;
  CData.pxl_size = pxl_size;
}

inline void UpdYPass(CycleData& CData, SCR_UINT& i) {
  i++;
  CData.CamRay.Dir -= CData.step_x * (float)(CData.x_passes + 1);
  CData.CamRay.Dir += CData.step_y;
}

inline void UpdXPass(CycleData& CData, SCR_UINT& i) {
  i++;
  CData.CamRay.Dir += CData.step_x;
}

RenderSettings::RenderSettings() {
  ObjList = nullptr;
  Camera = nullptr;
  Resolution.assign(0.5f, 0.1f, 1.f);
}

RenderSettings::~RenderSettings() {}

void RenderSettings::setObjList(List<Object>* list) { ObjList = list; }

void RenderSettings::setCamera(Object* camera) { Camera = camera; }

Object* RayCast::RenderSettings::getCamera() { return Camera; }

List<Object>* RayCast::RenderSettings::getObjList() { return ObjList; }