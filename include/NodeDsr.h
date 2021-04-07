#pragma once

#include <napi.h>

#include "DRFLEx.h"
using namespace DRAFramework;

#include "NodeDsrConfig.h"

#define NODEDSR_SPEED_LEVEL (3)

class NodeDsr : public Napi::ObjectWrap<NodeDsr> {
 public:
  static Napi::Object Init(Napi::Env env, Napi::Object exports);
  NodeDsr(const Napi::CallbackInfo &info);

 private:
#ifdef DRADEBUG
  Napi::Value TestCallback(const Napi::CallbackInfo &info);
  Napi::Value TestReturnArray(const Napi::CallbackInfo &info);
#endif
  Napi::Value OpenConnection(const Napi::CallbackInfo &info);
  Napi::Value CloseConnection(const Napi::CallbackInfo &info);
  Napi::Value GetLibraryVersion(const Napi::CallbackInfo &info);
  Napi::Value GetSystemVersion(const Napi::CallbackInfo &info);
  void SetOnMonitoringState(const Napi::CallbackInfo &info);
  void SetOnMonitoringAccessControl(const Napi::CallbackInfo &info);
  void SetOnDisconnected(const Napi::CallbackInfo &info);
  Napi::Value MoveHome(const Napi::CallbackInfo &info);
  Napi::Value MoveJ(const Napi::CallbackInfo &info);
  Napi::Value MoveJA(const Napi::CallbackInfo &info);
  Napi::Value MoveL(const Napi::CallbackInfo &info);
  Napi::Value MoveLA(const Napi::CallbackInfo &info);
  Napi::Value Stop(const Napi::CallbackInfo &info);
  Napi::Value MovePause(const Napi::CallbackInfo &info);
  Napi::Value MoveResume(const Napi::CallbackInfo &info);
  Napi::Value MWait(const Napi::CallbackInfo &info);
  Napi::Value SetToolDigitalOutput(const Napi::CallbackInfo &info);
  Napi::Value GetToolDigitalOutput(const Napi::CallbackInfo &info);
  Napi::Value GetToolDigitalInput(const Napi::CallbackInfo &info);
  Napi::Value SetDigitalOutput(const Napi::CallbackInfo &info);
  Napi::Value GetDigitalOutput(const Napi::CallbackInfo &info);
  Napi::Value GetDigitalInput(const Napi::CallbackInfo &info);
  Napi::Value SetAnalogOutput(const Napi::CallbackInfo &info);
  Napi::Value GetAnalogInput(const Napi::CallbackInfo &info);
  Napi::Value SetModeAnalogInput(const Napi::CallbackInfo &info);
  Napi::Value GetModeAnalogOutput(const Napi::CallbackInfo &info);
  Napi::Value DrlStart(const Napi::CallbackInfo &info);
  Napi::Value DrlStop(const Napi::CallbackInfo &info);
  Napi::Value DrlPause(const Napi::CallbackInfo &info);
  Napi::Value DrlResume(const Napi::CallbackInfo &info);
  Napi::Value ChangeOperationSpeed(const Napi::CallbackInfo &info);
  Napi::Value Trans(const Napi::CallbackInfo &info);
  Napi::Value GetCurrentPos(const Napi::CallbackInfo &info);
  Napi::Value SetSingularityHandling(const Napi::CallbackInfo &info);
  Napi::Value GetRobotState(const Napi::CallbackInfo &info);
  Napi::Value SetTaskSpeedLevel(const Napi::CallbackInfo &info);
  Napi::Value SetJointSpeedLevel(const Napi::CallbackInfo &info);
  Napi::Value GetTaskSpeedData(const Napi::CallbackInfo &info);
  Napi::Value GetJointSpeedData(const Napi::CallbackInfo &info);
  Napi::Value SetTaskSpeedCustom(const Napi::CallbackInfo &info);
  Napi::Value SetJointSpeedCustom(const Napi::CallbackInfo &info);

 public:
  static void SetOnMonitoringStateCB(Napi::Env env, Napi::Function jsCallback, uint32_t *peState);
  static void SetOnDisconnectedCB(Napi::Env env, Napi::Function jsCallback);

 public:
  uint32_t GetIndex() { return m_nIndex; }
  CDRFLEx *GetCDRFLex() { return m_pDrfl; }

 public:
  Napi::FunctionReference m_cbOnMoitoringState;
  Napi::ThreadSafeFunction m_cbOnMoitoringStateTsfn;
  Napi::FunctionReference m_cbOnMonitoringAccessControl;

  Napi::FunctionReference m_cbOnDisconnected;
  Napi::ThreadSafeFunction m_cbOnDisconnectedTsfn;

 private:
  CDRFLEx *m_pDrfl;
  string m_strUrl;
  uint32_t m_nPort;
  int32_t m_nIndex;

 public:
  bool m_TpInitailizingComplted;
  bool m_bHasControlAuthority;

  // speed levels
 private:
  uint32_t m_nTaskSpeedLevel;
  float m_fTaskSpeedVel[NODEDSR_SPEED_LEVEL];
  float m_fTaskSpeedAcc[NODEDSR_SPEED_LEVEL];
  uint32_t m_nJointSpeedLevel;
  float m_fJointSpeedVel[NODEDSR_SPEED_LEVEL];
  float m_fJointSpeedAcc[NODEDSR_SPEED_LEVEL];

  float m_fTaskSpeedVelCustom;
  float m_fTaskSpeedAccCustom;
  float m_fJointSpeedVelCustom;
  float m_fJointSpeedAccCustom;
  bool m_bTaskSpeedCustom;
  bool m_bJointSpeedCustom;
};
