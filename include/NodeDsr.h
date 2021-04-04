#pragma once

#include <napi.h>

#include "DRFLEx.h"
using namespace DRAFramework;

#include "NodeDsrConfig.h"

class NodeDsr : public Napi::ObjectWrap<NodeDsr> {
public:
  static Napi::Object Init(Napi::Env env, Napi::Object exports);
  NodeDsr(const Napi::CallbackInfo &info);

private:
  Napi::Value TestCallback(const Napi::CallbackInfo &info);
  Napi::Value TestReturnArray(const Napi::CallbackInfo &info);
  Napi::Value OpenConnection(const Napi::CallbackInfo &info);
  Napi::Value CloseConnection(const Napi::CallbackInfo &info);
  Napi::Value GetLibraryVersion(const Napi::CallbackInfo &info);
  Napi::Value GetSystemVersion(const Napi::CallbackInfo &info);
  void SetOnMonitoringState(const Napi::CallbackInfo &info);
  void SetOnMonitoringAccessControl(const Napi::CallbackInfo &info);
  void SetOnDisconnected(const Napi::CallbackInfo &info);
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
  Napi::Value GetAnalogOutput(const Napi::CallbackInfo &info);
  Napi::Value SetModeAnalogInput(const Napi::CallbackInfo &info);
  Napi::Value GetModeAnalogOutput(const Napi::CallbackInfo &info);
  Napi::Value DrlStart(const Napi::CallbackInfo &info);
  Napi::Value DrlStop(const Napi::CallbackInfo &info);
  Napi::Value DrlPause(const Napi::CallbackInfo &info);
  Napi::Value DrlResume(const Napi::CallbackInfo &info);
  Napi::Value ChangeOperationSpeed(const Napi::CallbackInfo &info);
  Napi::Value Trans(const Napi::CallbackInfo &info);

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
};
