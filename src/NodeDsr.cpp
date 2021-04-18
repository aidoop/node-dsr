#include "NodeDsr.h"

#include <assert.h>
#include <unistd.h>

#include <iostream>

#include "NapiHelper.h"
#include "NodeDsrArray.h"
#include "NodeDsrCallbacks.h"

Napi::Object NodeDsr::Init(Napi::Env env, Napi::Object exports) {
  Napi::Function func =
      DefineClass(env,
                  "NodeDsr",
                  {
#ifdef DRADEBUG
                      InstanceMethod("testCallback", &NodeDsr::TestCallback),
                      InstanceMethod("testReturnArray", &NodeDsr::TestReturnArray),
#endif
                      InstanceMethod("openConnection", &NodeDsr::OpenConnection),
                      InstanceMethod("closeConnection", &NodeDsr::CloseConnection),
                      InstanceMethod("getLibraryVersion", &NodeDsr::GetLibraryVersion),
                      InstanceMethod("getSystemVersion", &NodeDsr::GetSystemVersion),
                      InstanceMethod("setOnMonitoringState", &NodeDsr::SetOnMonitoringState),
                      InstanceMethod("setOnMonitoringAccessControl", &NodeDsr::SetOnMonitoringAccessControl),
                      InstanceMethod("setOnDisconnected", &NodeDsr::SetOnDisconnected),
                      InstanceMethod("home", &NodeDsr::MoveHome),
                      InstanceMethod("movej", &NodeDsr::MoveJ),
                      InstanceMethod("amovej", &NodeDsr::MoveJA),
                      InstanceMethod("movel", &NodeDsr::MoveL),
                      InstanceMethod("amovel", &NodeDsr::MoveLA),
                      InstanceMethod("setToolDigitalOutput", &NodeDsr::SetToolDigitalOutput),
                      InstanceMethod("getToolDigitalOutput", &NodeDsr::GetToolDigitalOutput),
                      InstanceMethod("getToolDigitalInput", &NodeDsr::GetToolDigitalInput),
                      InstanceMethod("setDigitalOutput", &NodeDsr::SetDigitalOutput),
                      InstanceMethod("getDigitalOutput", &NodeDsr::GetDigitalOutput),
                      InstanceMethod("getDigitalInput", &NodeDsr::GetDigitalInput),
                      InstanceMethod("setAnalogOutput", &NodeDsr::SetAnalogOutput),
                      InstanceMethod("getAnalogInput", &NodeDsr::GetAnalogInput),
                      InstanceMethod("setModeAnalogInput", &NodeDsr::SetModeAnalogInput),
                      InstanceMethod("getModeAnalogOutput", &NodeDsr::GetModeAnalogOutput),
                      InstanceMethod("startDRL", &NodeDsr::DrlStart),
                      InstanceMethod("stopDRL", &NodeDsr::DrlStop),
                      InstanceMethod("pauseDRL", &NodeDsr::DrlPause),
                      InstanceMethod("resumeDRL", &NodeDsr::DrlResume),
                      InstanceMethod("changeOperationSpeed", &NodeDsr::ChangeOperationSpeed),
                      InstanceMethod("trans", &NodeDsr::Trans),
                      InstanceMethod("getCurrentPos", &NodeDsr::GetCurrentPos),
                      InstanceMethod("setSingularityHandling", &NodeDsr::SetSingularityHandling),
                      InstanceMethod("getRobotStatus", &NodeDsr::GetRobotState),
                      InstanceMethod("setTaskSpeedLevel", &NodeDsr::SetTaskSpeedLevel),
                      InstanceMethod("setJointSpeedLevel", &NodeDsr::SetJointSpeedLevel),
                      InstanceMethod("getTaskSpeedData", &NodeDsr::GetTaskSpeedData),
                      InstanceMethod("getJointSpeedData", &NodeDsr::GetJointSpeedData),
                      InstanceMethod("setTaskSpeedCustom", &NodeDsr::SetTaskSpeedCustom),
                      InstanceMethod("setJointSpeedCustom", &NodeDsr::SetJointSpeedCustom),
                  });

  Napi::FunctionReference *constructor = new Napi::FunctionReference();
  *constructor = Napi::Persistent(func);
  env.SetInstanceData(constructor);

  exports.Set("NodeDsr", func);
  return exports;
}

NodeDsr::NodeDsr(const Napi::CallbackInfo &info)
    : Napi::ObjectWrap<NodeDsr>(info), m_pDrfl(NULL), m_strUrl(""), m_nPort(12345), m_nIndex(0), m_TpInitailizingComplted(false), m_bHasControlAuthority(false), m_nTaskSpeedLevel(0), m_fTaskSpeedVel{60, 90, 120}, m_fTaskSpeedAcc{80, 100, 130}, m_nJointSpeedLevel(0), m_fJointSpeedVel{10, 20, 30}, m_fJointSpeedAcc{20, 30, 40}, m_fTaskSpeedVelCustom(-1.0f), m_fTaskSpeedAccCustom(-1.0f), m_fJointSpeedVelCustom(-1.0f), m_fJointSpeedAccCustom(-1.0f), m_bTaskSpeedCustom(false), m_bJointSpeedCustom(false) {
  Napi::Env env = info.Env();

  uint32_t nInfoLen = info.Length();

  if (nInfoLen < 1) {
    Napi::TypeError::New(env, "constructor must have an index number").ThrowAsJavaScriptException();
    return;
  }

  m_strUrl = info[0].As<Napi::String>().Utf8Value();
  if (nInfoLen >= 2) {
    m_nPort = info[1].As<Napi::Number>().Uint32Value();
  }
  DBGPRINT("url: %s\n", m_strUrl.c_str());
  DBGPRINT("port: %d\n", m_nPort);

  m_pDrfl = new CDRFLEx();
  if (!m_pDrfl) {
    Napi::TypeError::New(env, "failed to createCDRFLEx").ThrowAsJavaScriptException();
  }

  // set this pointer to NodeDsrArray
  m_nIndex = NodeDsrArray::Set(this);
  if (m_nIndex < 0) {
    Napi::TypeError::New(env, "cannot assign new nodedsr by insufficient index capacity.").ThrowAsJavaScriptException();
    return;
  }
  DBGPRINT("NodeDsr Index : %d\n", m_nIndex);

  m_pDrfl->set_on_monitoring_state(g_pfnMonitoringState[m_nIndex]);
  m_pDrfl->set_on_monitoring_access_control(g_pfnMonitroingAccessControl[m_nIndex]);
  m_pDrfl->set_on_tp_initializing_completed(g_pfnTpInitializingCompleted[m_nIndex]);
}

Napi::Value NodeDsr::OpenConnection(const Napi::CallbackInfo &info) {
  DBGPRINT("called OpenConnection\n");

  Napi::Env env = info.Env();

  DBGPRINT("info.Length: %u\n", static_cast<uint32_t>(info.Length()));

  bool bConnected = false;
  try {
    bConnected = m_pDrfl->open_connection(m_strUrl, m_nPort);
    DBGPRINT("connection result: %d\n", bConnected);
    if (bConnected == false) {
      throw std::runtime_error("Network connection failed\n");
    }

    // TODO: must check this gpio output
    // m_pDrfl->set_digital_output(GPIO_CTRLBOX_DIGITAL_INDEX_10, TRUE);
    uint32_t nWaitCount = 0;
    while ((m_pDrfl->get_robot_state() != STATE_STANDBY) || !m_bHasControlAuthority) {
      usleep(1000);
      // wait for 2 seconds
      if (nWaitCount > 2000) {
        throw std::runtime_error("Robot State Error\n");
        break;
      }
      nWaitCount++;
    }

    m_pDrfl->set_robot_mode(ROBOT_MODE_MANUAL);
    m_pDrfl->set_robot_system(ROBOT_SYSTEM_REAL);
  } catch (const std::exception &e) {
    std::cerr << e.what() << '\n';
  }

  return Napi::Boolean::New(env, bConnected);
}

Napi::Value NodeDsr::CloseConnection(const Napi::CallbackInfo &info) {
  DBGPRINT("called CloseConnection\n");

  Napi::Env env = info.Env();

  m_pDrfl->close_connection();

  // m_cbOnMoitoringStateTsfn.Release();

  return Napi::Boolean::New(env, true);
}

#ifdef DRADEBUG
Napi::Value NodeDsr::TestCallback(const Napi::CallbackInfo &info) {
  DBGPRINT("called TestCallback\n");

  Napi::Env env = info.Env();

  if (m_cbOnMoitoringState != NULL) {
    Napi::Env env = m_cbOnMoitoringState.Env();
    Napi::Number robotStateNapi = Napi::Number::New(env, static_cast<uint32_t>(7777));
    std::vector<napi_value> args = {robotStateNapi};
    m_cbOnMoitoringState.Call(args);
  }

  if (m_cbOnMonitoringAccessControl != NULL) {
    Napi::Env env = m_cbOnMonitoringAccessControl.Env();
    Napi::Number robotStateNapi = Napi::Number::New(env, static_cast<uint32_t>(8888));
    std::vector<napi_value> args = {robotStateNapi};
    m_cbOnMonitoringAccessControl.Call(args);
  }

  return Napi::Boolean::New(env, true);
}

Napi::Value NodeDsr::TestReturnArray(const Napi::CallbackInfo &info) {
  DBGPRINT("called TestReturnArray\n");

  Napi::Env env = info.Env();

  float fTestValues[6] = {111.111, 222.222, -333.333, 444.444, -555.555, 666.666};
  Napi::Array resultArray = Napi::Array::New(info.Env(), NUM_TASK);
  for (uint32_t nIter = 0; nIter < 6; nIter++) {
    resultArray[nIter] = Napi::Number::New(env, fTestValues[nIter]);
  }

  return resultArray;
}
#endif  // DRADEBUG

// dsr api function protocol
// const char* get_library_version()
Napi::Value NodeDsr::GetLibraryVersion(const Napi::CallbackInfo &info) {
  DBGPRINT("called GetLibraryVersion\n");

  Napi::Env env = info.Env();

  const char *libVersion = m_pDrfl->get_library_version();

  return Napi::String::New(env, libVersion);
}

// dsr api function protocol
// bool get_system_version(LPSYSTEM_VERSION pVersion)
Napi::Value NodeDsr::GetSystemVersion(const Napi::CallbackInfo &info) {
  DBGPRINT("called GetSystemVersion\n");

  Napi::Env env = info.Env();

  uint32_t nInfoLen = info.Length();
  if (nInfoLen < 1) {
    Napi::TypeError::New(env, "invalid function parameter").ThrowAsJavaScriptException();
    return Napi::String::New(env, "");
  }

  uint32_t nReqType = info[0].As<Napi::Number>().Uint32Value();

  SYSTEM_VERSION tSysVersion;
  memset(reinterpret_cast<void *>(&tSysVersion), 0, sizeof(SYSTEM_VERSION));
  m_pDrfl->get_system_version(&tSysVersion);

  char szReqVersion[MAX_SYMBOL_SIZE];
  switch (nReqType) {
    case 0:
      strcpy(szReqVersion, tSysVersion._szSmartTp);
      break;
    case 1:
      strcpy(szReqVersion, tSysVersion._szController);
      break;
    case 2:
      strcpy(szReqVersion, tSysVersion._szInterpreter);
      break;
    case 3:
      strcpy(szReqVersion, tSysVersion._szInverter);
      break;
    case 4:
      strcpy(szReqVersion, tSysVersion._szSafetyBoard);
      break;
    case 5:
      strcpy(szReqVersion, tSysVersion._szRobotSerial);
      break;
    case 6:
      strcpy(szReqVersion, tSysVersion._szRobotModel);
      break;
    case 7:
      strcpy(szReqVersion, tSysVersion._szJTSBoard);
      break;
    case 8:
      strcpy(szReqVersion, tSysVersion._szController);
      break;
    default:
      break;
  }

  return Napi::String::New(env, szReqVersion);
}

// dsr api function protocol
// void set_on_monitoring_state(TOnMonitoringStateCB pCallbackFunc)
// typedef void (*TOnMonitoringStateCB)(const ROBOT_STATE);
void NodeDsr::SetOnMonitoringStateCB(Napi::Env env, Napi::Function jsCallback, uint32_t *peState) {
  DBGPRINT("called SetOnMonitoringStateCB - start\n");
  Napi::Number robotStateNapi = Napi::Number::New(env, static_cast<uint32_t>(*peState));
  std::vector<napi_value> args = {robotStateNapi};
  jsCallback.Call(args);
  DBGPRINT("called SetOnMonitoringStateCB - end\n");

  delete peState;
}

void NodeDsr::SetOnMonitoringState(const Napi::CallbackInfo &info) {
  DBGPRINT("called SetOnMonitoringState\n");
  DBGPRINT("param length: %d\n", static_cast<uint32_t>(info.Length()));

  Napi::Env env = info.Env();

  m_cbOnMoitoringState = Napi::Persistent(info[0].As<Napi::Function>());

  // Create a ThreadSafeFunction
  m_cbOnMoitoringStateTsfn = Napi::ThreadSafeFunction::New(
      env,
      info[0].As<Napi::Function>(),  // JavaScript function called asynchronously
      "Resource Name",               // Name
      0,                             // Unlimited queue
      1                              // Only one thread will use this initially
  );

  m_pDrfl->set_on_monitoring_state(g_pfnMonitoringStateCB[m_nIndex]);
}

// dsr api function protocol
// void set_on_monitoring_access_control(TOnMonitoringAccessControlCB pCallbackFunc)
// typedef void (*TOnMonitoringAccessControlCB)(const MONITORING_ACCESS_CONTROL);
void NodeDsr::SetOnMonitoringAccessControl(const Napi::CallbackInfo &info) {
  DBGPRINT("called SetOnMonitoringAccessControl\n");
  DBGPRINT("param length: %d\n", static_cast<uint32_t>(info.Length()));

  m_cbOnMonitoringAccessControl = Napi::Persistent(info[0].As<Napi::Function>());

  m_pDrfl->set_on_monitoring_access_control(g_pfnMonitoringAccessControlCB[m_nIndex]);
}

// dsr api function protocol
// void void set_on_disconnected(TOnDisconnectedCB pCallbackFunc)
// typedef void (*TOnDisconnectedCB)();
void NodeDsr::SetOnDisconnected(const Napi::CallbackInfo &info) {
  DBGPRINT("called SetOnDisconnected\n");
  DBGPRINT("param length: %d\n", static_cast<uint32_t>(info.Length()));

  Napi::Env env = info.Env();

  m_cbOnDisconnected = Napi::Persistent(info[0].As<Napi::Function>());

  // Create a ThreadSafeFunction
  m_cbOnDisconnectedTsfn = Napi::ThreadSafeFunction::New(
      env,
      info[0].As<Napi::Function>(),  // JavaScript function called asynchronously
      "TOnDisconnectedCB",           // Name
      0,                             // Unlimited queue
      1                              // Only one thread will use this initially
  );

  m_pDrfl->set_on_disconnected(g_pfnDisconnected[m_nIndex]);
}

void NodeDsr::SetOnDisconnectedCB(Napi::Env env, Napi::Function jsCallback) {
  DBGPRINT("called SetOnDisconnectedCB - start\n");
  std::vector<napi_value> args = {};
  jsCallback.Call(args);
  DBGPRINT("called SetOnDisconnectedCB - end\n");
}

// dsr api function protocol
// bool move_home(MOVE_HOME eMode = MOVE_HOME_MECHANIC, unsigned char bRun = (unsigned)1)
Napi::Value NodeDsr::MoveHome(const Napi::CallbackInfo &info) {
  DBGPRINT("called MoveHome\n");
  uint32_t nInfoIndex = 0;
  Napi::Env env = info.Env();
  uint32_t nInfoLen = info.Length();

  if (nInfoLen < 2) {
    Napi::TypeError::New(env, "invalid function parameter").ThrowAsJavaScriptException();
    return Napi::Boolean::New(env, false);
  }

  bool bHomeUser = info[nInfoIndex++].As<Napi::Boolean>().Value();
  bool bRun = info[nInfoIndex++].As<Napi::Boolean>().Value();
  DBGPRINT("bHomeUser: %d\n", bHomeUser);
  DBGPRINT("bRun: %d\n", bRun);

  bool bFuncRet = m_pDrfl->move_home(static_cast<MOVE_HOME>(bHomeUser), static_cast<unsigned char>(bRun));
  return Napi::Boolean::New(env, bFuncRet);
}
// dsr api function protocol
// bool movej(float fTargetPos[NUM_JOINT], float fTargetVel, float fTargetAcc, float fTargetTime = 0.f,
// MOVE_MODE eMoveMode = MOVE_MODE_ABSOLUTE, float fBlendingRadius = 0.f, BLENDING_SPEED_TYPE eBlendingType = BLENDING_SPEED_TYPE_DUPLICATE)
Napi::Value NodeDsr::MoveJ(const Napi::CallbackInfo &info) {
  DBGPRINT("called MoveJ\n");

  uint32_t nInfoIndex = 0;
  Napi::Env env = info.Env();
  uint32_t nInfoLen = info.Length();

  if (nInfoLen < 3) {
    Napi::TypeError::New(env, "invalid function parameter").ThrowAsJavaScriptException();
    return Napi::Boolean::New(env, false);
  }

  ///////////////////////////////////////////////////////////////////
  // input parameter: fTartPos[NUM_JOINT]
  // Ref: https://github.com/nodejs/node-addon-examples/blob/4c3b7816662e3a4ab26d41f007bbd1784205bda6/array_buffer_to_native/node-addon-api/array_buffer_to_native.cc
  Napi::Array inputs = info[nInfoIndex++].As<Napi::Array>();
  uint32_t nInputArrayLen = inputs.Length();
  printf("nInputArrayLen: %d\n", nInputArrayLen);
  if (nInputArrayLen != NUM_JOINT) {
    Napi::TypeError::New(env, "input pos array length wrong").ThrowAsJavaScriptException();
    return Napi::Boolean::New(env, false);
  }
  float fTargetPos[6] = {
      0.0,
  };
  for (uint32_t nIter = 0; nIter < nInputArrayLen; nIter++) {
    fTargetPos[nIter] = inputs.Get(static_cast<uint32_t>(nIter)).As<Napi::Number>().FloatValue();
    DBGPRINT("fTargetPos[%d]: %f\n", nIter, fTargetPos[nIter]);
  }

  ///////////////////////////////////////////////////////////////////
  // input parameter: fTargetVel
  float fTargetVel = info[nInfoIndex++].As<Napi::Number>().FloatValue();
  DBGPRINT("fTargetVel: %f\n", fTargetVel);

  ///////////////////////////////////////////////////////////////////
  // input parameter: fTargetAcc
  float fTargetAcc = info[nInfoIndex++].As<Napi::Number>().FloatValue();
  DBGPRINT("fTargetAcc: %f\n", fTargetAcc);

  float fTargetTime = 0.0f;
  bool bRelativeMove = false;
  float fBlendingRadius = 0.0f;
  bool bBlendOverride = false;

  ///////////////////////////////////////////////////////////////////
  // input parameter: fTargetTime
  if (nInfoLen > nInfoIndex) {
    fTargetTime = info[nInfoIndex++].As<Napi::Number>().FloatValue();
    DBGPRINT("fTargetTime: %f\n", fTargetTime);
  }

  ///////////////////////////////////////////////////////////////////
  // input parameter: bRelativeMoveparam
  if (nInfoLen > nInfoIndex) {
    bRelativeMove = info[nInfoIndex++].As<Napi::Boolean>().Value();
    DBGPRINT("bRelativeMove: %d\n", bRelativeMove);
  }

  ///////////////////////////////////////////////////////////////////
  // input parameter: fBlendingRadius
  if (nInfoLen > nInfoIndex) {
    fBlendingRadius = info[nInfoIndex++].As<Napi::Number>().FloatValue();
    DBGPRINT("fBlendingRadius: %f\n", fBlendingRadius);
  }

  ///////////////////////////////////////////////////////////////////
  // input parameter: bBlendOverride
  if (nInfoLen > nInfoIndex) {
    bBlendOverride = info[nInfoIndex++].As<Napi::Boolean>().Value();
    DBGPRINT("bBlendOverride: %d\n", bBlendOverride);
  }

  bool bFuncRet = m_pDrfl->movej(fTargetPos, fTargetVel, fTargetAcc, fTargetTime,
                                 static_cast<MOVE_MODE>(bRelativeMove), fBlendingRadius, static_cast<BLENDING_SPEED_TYPE>(bBlendOverride));

  return Napi::Boolean::New(env, bFuncRet);
}

// dsr api function protocol
// bool amovej(float fTargetPos[NUM_JOINT], float fTargetVel, float fTargetAcc, float fTargetTime = 0.f,
//            MOVE_MODE eMoveMode = MOVE_MODE_ABSOLUTE, BLENDING_SPEED_TYPE eBlendingType = BLENDING_SPEED_TYPE_DUPLICATE)
Napi::Value NodeDsr::MoveJA(const Napi::CallbackInfo &info) {
  DBGPRINT("called MoveJA\n");

  Napi::Env env = info.Env();

  ///////////////////////////////////////////////////////////////////
  // input parameter: fTartPos[NUM_JOINT]
  Napi::Array inputs = info[0].As<Napi::Array>();
  uint32_t nInputArrayLen = inputs.Length();
  printf("nInputArrayLen: %d\n", nInputArrayLen);
  if (nInputArrayLen != NUM_JOINT) {
    Napi::TypeError::New(env, "input pos array length wrong").ThrowAsJavaScriptException();
    return Napi::Boolean::New(env, false);
  }
  float fTargetPos[6] = {
      0.0,
  };
  for (uint32_t nIter = 0; nIter < nInputArrayLen; nIter++) {
    fTargetPos[nIter] = inputs.Get(static_cast<uint32_t>(nIter)).As<Napi::Number>().FloatValue();
    DBGPRINT("fTargetPos[%d]: %f\n", nIter, fTargetPos[nIter]);
  }

  ///////////////////////////////////////////////////////////////////
  // input parameter: fTargetVel
  float fTargetVel = info[1].As<Napi::Number>().FloatValue();
  DBGPRINT("fTargetVel: %f\n", fTargetVel);

  ///////////////////////////////////////////////////////////////////
  // input parameter: fTargetAcc
  float fTargetAcc = info[2].As<Napi::Number>().FloatValue();
  DBGPRINT("fTargetAcc: %f\n", fTargetAcc);

  ///////////////////////////////////////////////////////////////////
  // input parameter: fTargetTime
  float fTargetTime = info[3].As<Napi::Number>().FloatValue();
  DBGPRINT("fTargetTime: %f\n", fTargetTime);

  ///////////////////////////////////////////////////////////////////
  // input parameter: bRelativeMove
  bool bRelativeMove = info[4].As<Napi::Boolean>().Value();
  DBGPRINT("bRelativeMove: %d\n", bRelativeMove);

  ///////////////////////////////////////////////////////////////////
  // input parameter: bBlendOverride
  bool bBlendOverride = info[5].As<Napi::Boolean>().Value();
  DBGPRINT("bBlendOverride: %d\n", bBlendOverride);

  bool bFuncRet = m_pDrfl->amovej(fTargetPos, fTargetVel, fTargetAcc, fTargetTime,
                                  static_cast<MOVE_MODE>(bRelativeMove), static_cast<BLENDING_SPEED_TYPE>(bBlendOverride));

  return Napi::Boolean::New(env, bFuncRet);
}

///////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////
// dsr api function protocol
// bool movel(float fTargetPos[NUM_TASK], float fTargetVel[2], float fTargetAcc[2], float fTargetTime = 0.f,
//            MOVE_MODE eMoveMode = MOVE_MODE_ABSOLUTE, MOVE_REFERENCE eMoveReference = MOVE_REFERENCE_BASE,
//            float fBlendingRadius = 0.f, BLENDING_SPEED_TYPE eBlendingType = BLENDING_SPEED_TYPE_DUPLICATE)
Napi::Value NodeDsr::MoveL(const Napi::CallbackInfo &info) {
  DBGPRINT("called MoveL\n");
  uint32_t nInfoIndex = 0;
  Napi::Env env = info.Env();
  uint32_t nInfoLen = info.Length();

  if (nInfoLen < 3) {
    Napi::TypeError::New(env, "invalid function parameter").ThrowAsJavaScriptException();
    return Napi::Boolean::New(env, false);
  }

  ///////////////////////////////////////////////////////////////////
  // input parameter: fTartPos[NUM_TASK]
  // Ref: https://github.com/nodejs/node-addon-examples/blob/4c3b7816662e3a4ab26d41f007bbd1784205bda6/array_buffer_to_native/node-addon-api/array_buffer_to_native.cc
  Napi::Array targetPoses = info[nInfoIndex++].As<Napi::Array>();
  float fTargetPos[NUM_TASK];
  if (NapiHelper::covertToFloatArray(targetPoses, NUM_TASK, fTargetPos) == false) {
    Napi::TypeError::New(env, "pos array length wrong").ThrowAsJavaScriptException();
    return Napi::Boolean::New(env, false);
  }

  ///////////////////////////////////////////////////////////////////
  // input parameter: fTargetVel[2]
  Napi::Array targetVels = info[nInfoIndex++].As<Napi::Array>();
  float fTargetVel[2];
  if (NapiHelper::covertToFloatArray(targetVels, 2, fTargetVel) == false) {
    Napi::TypeError::New(env, "vel array length wrong").ThrowAsJavaScriptException();
    return Napi::Boolean::New(env, false);
  }

  ///////////////////////////////////////////////////////////////////
  // input parameter: fTargetAcc[2]
  Napi::Array targetAccs = info[nInfoIndex++].As<Napi::Array>();
  float fTargetAcc[2];
  if (NapiHelper::covertToFloatArray(targetAccs, 2, fTargetAcc) == false) {
    Napi::TypeError::New(env, "acc array length wrong").ThrowAsJavaScriptException();
    return Napi::Boolean::New(env, false);
  }

  float fTargetTime = 0.0f;
  bool bRelativeMove = false;
  MOVE_REFERENCE eMoveReference = MOVE_REFERENCE_BASE;
  float fBlendingRadius = 0.0f;
  bool bBlendOverride = false;

  ///////////////////////////////////////////////////////////////////
  // input parameter: fTargetTime
  if (nInfoLen > nInfoIndex) {
    fTargetTime = info[nInfoIndex++].As<Napi::Number>().FloatValue();
    DBGPRINT("fTargetTime: %f\n", fTargetTime);
  }

  ///////////////////////////////////////////////////////////////////
  // input parameter: bRelativeMove
  if (nInfoLen > nInfoIndex) {
    bRelativeMove = info[nInfoIndex++].As<Napi::Boolean>().Value();
    DBGPRINT("bRelativeMove: %d\n", bRelativeMove);
  }

  ///////////////////////////////////////////////////////////////////
  // input parameter: eMoveReference
  if (nInfoLen > nInfoIndex) {
    eMoveReference = static_cast<MOVE_REFERENCE>(info[nInfoIndex++].As<Napi::Number>().Uint32Value());
    DBGPRINT("eMoveReference: %d\n", eMoveReference);
  }

  ///////////////////////////////////////////////////////////////////
  // input parameter: fBlendingRadius
  if (nInfoLen > nInfoIndex) {
    fBlendingRadius = info[nInfoIndex++].As<Napi::Number>().FloatValue();
    DBGPRINT("fBlendingRadius: %f\n", fBlendingRadius);
  }

  ///////////////////////////////////////////////////////////////////
  // input parameter: bBlendOverride
  if (nInfoLen > nInfoIndex) {
    bBlendOverride = info[nInfoIndex++].As<Napi::Boolean>().Value();
    DBGPRINT("bBlendOverride: %d\n", bBlendOverride);
  }

  bool bFuncRet = m_pDrfl->movel(fTargetPos, fTargetVel, fTargetAcc, fTargetTime, static_cast<MOVE_MODE>(bRelativeMove),
                                 eMoveReference, fBlendingRadius, static_cast<BLENDING_SPEED_TYPE>(bBlendOverride));

  return Napi::Boolean::New(env, bFuncRet);
}

///////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////
// dsr api function protocol
// bool amovel(float fTargetPos[NUM_TASK], float fTargetVel[2], float fTargetAcc[2], float fTargetTime = 0.f,
//      MOVE_MODE eMoveMode = MOVE_MODE_ABSOLUTE, MOVE_REFERENCE eMoveReference = MOVE_REFERENCE_BASE,
//       BLENDING_SPEED_TYPE eBlendingType = BLENDING_SPEED_TYPE_DUPLICATE)
Napi::Value NodeDsr::MoveLA(const Napi::CallbackInfo &info) {
  DBGPRINT("called MoveLA\n");
  uint32_t nInfoIndex = 0;
  Napi::Env env = info.Env();

  ///////////////////////////////////////////////////////////////////
  // input parameter: fTartPos[NUM_TASK]
  // Ref: https://github.com/nodejs/node-addon-examples/blob/4c3b7816662e3a4ab26d41f007bbd1784205bda6/array_buffer_to_native/node-addon-api/array_buffer_to_native.cc
  Napi::Array targetPoses = info[nInfoIndex++].As<Napi::Array>();
  float fTargetPos[NUM_TASK];
  if (NapiHelper::covertToFloatArray(targetPoses, NUM_TASK, fTargetPos) == false) {
    Napi::TypeError::New(env, "pos array length wrong").ThrowAsJavaScriptException();
    return Napi::Boolean::New(env, false);
  }

  ///////////////////////////////////////////////////////////////////
  // input parameter: fTargetVel[2]
  Napi::Array targetVels = info[nInfoIndex++].As<Napi::Array>();
  float fTargetVel[2];
  if (NapiHelper::covertToFloatArray(targetVels, 2, fTargetVel) == false) {
    Napi::TypeError::New(env, "vel array length wrong").ThrowAsJavaScriptException();
    return Napi::Boolean::New(env, false);
  }

  ///////////////////////////////////////////////////////////////////
  // input parameter: fTargetAcc[2]
  Napi::Array targetAccs = info[nInfoIndex++].As<Napi::Array>();
  float fTargetAcc[2];
  if (NapiHelper::covertToFloatArray(targetAccs, 2, fTargetAcc) == false) {
    Napi::TypeError::New(env, "acc array length wrong").ThrowAsJavaScriptException();
    return Napi::Boolean::New(env, false);
  }

  ///////////////////////////////////////////////////////////////////
  // input parameter: fTargetTime
  float fTargetTime = info[nInfoIndex++].As<Napi::Number>().FloatValue();
  DBGPRINT("fTargetTime: %f\n", fTargetTime);

  ///////////////////////////////////////////////////////////////////
  // input parameter: bRelativeMove
  bool bRelativeMove = info[nInfoIndex++].As<Napi::Boolean>().Value();
  DBGPRINT("bRelativeMove: %d\n", bRelativeMove);

  ///////////////////////////////////////////////////////////////////
  // input parameter: eMoveReference
  MOVE_REFERENCE eMoveReference = static_cast<MOVE_REFERENCE>(info[nInfoIndex++].As<Napi::Number>().Uint32Value());
  DBGPRINT("eMoveReference: %d\n", eMoveReference);

  ///////////////////////////////////////////////////////////////////
  // input parameter: bBlendOverride
  bool bBlendOverride = info[nInfoIndex++].As<Napi::Boolean>().Value();
  DBGPRINT("bBlendOverride: %d\n", bBlendOverride);

  bool bFuncRet = m_pDrfl->amovel(fTargetPos, fTargetVel, fTargetAcc, fTargetTime, static_cast<MOVE_MODE>(bRelativeMove),
                                  eMoveReference, static_cast<BLENDING_SPEED_TYPE>(bBlendOverride));

  return Napi::Boolean::New(env, bFuncRet);
}

///////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////
// dsr api function protocol
// bool stop(STOP_TYPE eStopType = STOP_TYPE_QUICK)
Napi::Value NodeDsr::Stop(const Napi::CallbackInfo &info) {
  DBGPRINT("called Stop\n");
  uint32_t nInfoIndex = 0;
  Napi::Env env = info.Env();
  uint32_t nInfoLen = info.Length();

  if (nInfoLen < 1) {
    Napi::TypeError::New(env, "invalid function parameter").ThrowAsJavaScriptException();
    return Napi::Boolean::New(env, false);
  }

  uint32_t nStopType = info[nInfoIndex++].As<Napi::Number>().Uint32Value();
  DBGPRINT("nStopType: %d\n", nStopType);

  bool bFuncRet = m_pDrfl->stop(static_cast<STOP_TYPE>(nStopType));
  return Napi::Boolean::New(env, bFuncRet);
}

///////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////
// dsr api function protocol
// bool move_pause()
Napi::Value NodeDsr::MovePause(const Napi::CallbackInfo &info) {
  DBGPRINT("called MovePause\n");
  Napi::Env env = info.Env();

  bool bFuncRet = m_pDrfl->move_pause();
  return Napi::Boolean::New(env, bFuncRet);
}

///////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////
// dsr api function protocol
// bool move_pause()
Napi::Value NodeDsr::MoveResume(const Napi::CallbackInfo &info) {
  DBGPRINT("called MoveResume\n");
  Napi::Env env = info.Env();

  bool bFuncRet = m_pDrfl->move_resume();
  return Napi::Boolean::New(env, bFuncRet);
}

///////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////
// dsr api function protocol
// bool mwait()
Napi::Value NodeDsr::MWait(const Napi::CallbackInfo &info) {
  DBGPRINT("called MoveResume\n");
  Napi::Env env = info.Env();

  bool bFuncRet = m_pDrfl->mwait();
  return Napi::Boolean::New(env, bFuncRet);
}

///////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////
// dsr api function protocol
// bool set_tool_digital_output(GPIO_TOOL_DIGITAL_INDEX eGpioIndex, bool bOnOff)
Napi::Value NodeDsr::SetToolDigitalOutput(const Napi::CallbackInfo &info) {
  DBGPRINT("called SetToolDigitalOutput\n");
  uint32_t nInfoIndex = 0;

  Napi::Env env = info.Env();
  uint32_t nInfoLen = info.Length();

  if (nInfoLen != 2) {
    Napi::TypeError::New(env, "invalid function parameter").ThrowAsJavaScriptException();
    return Napi::Boolean::New(env, false);
  }

  uint32_t nGpioIndex = info[nInfoIndex++].As<Napi::Number>().Uint32Value();
  DBGPRINT("nGpioIndex: %d\n", nGpioIndex);
  bool bOnOff = info[nInfoIndex++].As<Napi::Boolean>().Value();
  DBGPRINT("bOnOff: %d\n", bOnOff);

  bool bFuncRet = m_pDrfl->set_tool_digital_output(static_cast<GPIO_TOOL_DIGITAL_INDEX>(nGpioIndex), bOnOff);
  return Napi::Boolean::New(env, bFuncRet);
}

///////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////
// dsr api function protocol
// bool set_tool_digital_output(GPIO_TOOL_DIGITAL_INDEX eGpioIndex, bool bOnOff)
Napi::Value NodeDsr::GetToolDigitalOutput(const Napi::CallbackInfo &info) {
  DBGPRINT("called GetToolDigitalOutput\n");
  uint32_t nInfoIndex = 0;

  Napi::Env env = info.Env();
  uint32_t nInfoLen = info.Length();

  if (nInfoLen < 1) {
    Napi::TypeError::New(env, "invalid function parameter").ThrowAsJavaScriptException();
    return Napi::Boolean::New(env, false);
  }

  uint32_t nGpioIndex = info[nInfoIndex++].As<Napi::Number>().Uint32Value();
  DBGPRINT("nGpioIndex: %d\n", nGpioIndex);

  bool bFuncRet = m_pDrfl->get_tool_digital_output(static_cast<GPIO_TOOL_DIGITAL_INDEX>(nGpioIndex));
  return Napi::Boolean::New(env, bFuncRet);
}

///////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////
// dsr api function protocol
// bool get_tool_digital_input(GPIO_TOOL_DIGITAL_INDEX eGpioIndex)
Napi::Value NodeDsr::GetToolDigitalInput(const Napi::CallbackInfo &info) {
  DBGPRINT("called GetToolDigitalInput\n");
  uint32_t nInfoIndex = 0;

  Napi::Env env = info.Env();
  uint32_t nInfoLen = info.Length();

  if (nInfoLen < 1) {
    Napi::TypeError::New(env, "invalid function parameter").ThrowAsJavaScriptException();
    return Napi::Boolean::New(env, false);
  }

  uint32_t nGpioIndex = info[nInfoIndex++].As<Napi::Number>().Uint32Value();
  DBGPRINT("nGpioIndex: %d\n", nGpioIndex);

  bool bFuncRet = m_pDrfl->get_tool_digital_input(static_cast<GPIO_TOOL_DIGITAL_INDEX>(nGpioIndex));
  return Napi::Boolean::New(env, bFuncRet);
}

///////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////
// dsr api function protocol
// bool set_digital_output(GPIO_CTRLBOX_DIGITAL_INDEX eGpioIndex, bool bOnOff)
Napi::Value NodeDsr::SetDigitalOutput(const Napi::CallbackInfo &info) {
  DBGPRINT("called SetDigitalOutput\n");
  uint32_t nInfoIndex = 0;

  Napi::Env env = info.Env();
  uint32_t nInfoLen = info.Length();

  if (nInfoLen < 2) {
    Napi::TypeError::New(env, "invalid function parameter").ThrowAsJavaScriptException();
    return Napi::Boolean::New(env, false);
  }

  uint32_t nGpioIndex = info[nInfoIndex++].As<Napi::Number>().Uint32Value();
  DBGPRINT("nGpioIndex: %d\n", nGpioIndex);
  bool bOnOff = info[nInfoIndex++].As<Napi::Boolean>().Value();
  DBGPRINT("bOnOff: %d\n", bOnOff);

  bool bFuncRet = m_pDrfl->set_digital_output(static_cast<GPIO_CTRLBOX_DIGITAL_INDEX>(nGpioIndex), bOnOff);
  return Napi::Boolean::New(env, bFuncRet);
}

///////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////
// dsr api function protocol
// bool get_digital_output(GPIO_CTRLBOX_DIGITAL_INDEX eGpioIndex)
Napi::Value NodeDsr::GetDigitalOutput(const Napi::CallbackInfo &info) {
  DBGPRINT("called GetDigitalOutput\n");
  uint32_t nInfoIndex = 0;

  Napi::Env env = info.Env();
  uint32_t nInfoLen = info.Length();

  if (nInfoLen < 1) {
    Napi::TypeError::New(env, "invalid function parameter").ThrowAsJavaScriptException();
    return Napi::Boolean::New(env, false);
  }

  uint32_t nGpioIndex = info[nInfoIndex++].As<Napi::Number>().Uint32Value();
  DBGPRINT("nGpioIndex: %d\n", nGpioIndex);

  bool bFuncRet = m_pDrfl->get_digital_output(static_cast<GPIO_CTRLBOX_DIGITAL_INDEX>(nGpioIndex));
  return Napi::Boolean::New(env, bFuncRet);
}

///////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////
// dsr api function protocol
// bool get_digital_input(GPIO_CTRLBOX_DIGITAL_INDEX eGpioIndex)
Napi::Value NodeDsr::GetDigitalInput(const Napi::CallbackInfo &info) {
  DBGPRINT("called GetDigitalInput\n");
  uint32_t nInfoIndex = 0;

  Napi::Env env = info.Env();
  uint32_t nInfoLen = info.Length();

  if (nInfoLen < 1) {
    Napi::TypeError::New(env, "invalid function parameter").ThrowAsJavaScriptException();
    return Napi::Boolean::New(env, false);
  }

  uint32_t nGpioIndex = info[nInfoIndex++].As<Napi::Number>().Uint32Value();
  DBGPRINT("nGpioIndex: %d\n", nGpioIndex);

  bool bFuncRet = m_pDrfl->get_digital_input(static_cast<GPIO_CTRLBOX_DIGITAL_INDEX>(nGpioIndex));
  return Napi::Boolean::New(env, bFuncRet);
}

///////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////
// dsr api function protocol
// bool set_analog_output(GPIO_CTRLBOX_ANALOG_INDEX eGpioIndex, float fValue)
Napi::Value NodeDsr::SetAnalogOutput(const Napi::CallbackInfo &info) {
  DBGPRINT("called SetAnalogOutput\n");
  uint32_t nInfoIndex = 0;

  Napi::Env env = info.Env();
  uint32_t nInfoLen = info.Length();

  if (nInfoLen < 2) {
    Napi::TypeError::New(env, "invalid function parameter").ThrowAsJavaScriptException();
    return Napi::Boolean::New(env, false);
  }

  uint32_t nGpioIndex = info[nInfoIndex++].As<Napi::Number>().Uint32Value();
  DBGPRINT("nGpioIndex: %d\n", nGpioIndex);
  float fValue = info[nInfoIndex++].As<Napi::Number>().FloatValue();
  DBGPRINT("fValue: %f\n", fValue);

  bool bFuncRet = m_pDrfl->set_analog_output(static_cast<GPIO_CTRLBOX_ANALOG_INDEX>(nGpioIndex), fValue);
  return Napi::Boolean::New(env, bFuncRet);
}

///////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////
// dsr api function protocol
// float get_analog_input(GPIO_CTRLBOX_ANALOG_INDEX eGpioIndex)
Napi::Value NodeDsr::GetAnalogInput(const Napi::CallbackInfo &info) {
  DBGPRINT("called GetAnalogInput\n");
  uint32_t nInfoIndex = 0;

  Napi::Env env = info.Env();
  uint32_t nInfoLen = info.Length();

  if (nInfoLen < 1) {
    Napi::TypeError::New(env, "invalid function parameter").ThrowAsJavaScriptException();
    return Napi::Boolean::New(env, false);
  }

  uint32_t nGpioIndex = info[nInfoIndex++].As<Napi::Number>().Uint32Value();
  DBGPRINT("nGpioIndex: %d\n", nGpioIndex);

  float fFuncRet = m_pDrfl->get_analog_input(static_cast<GPIO_CTRLBOX_ANALOG_INDEX>(nGpioIndex));
  DBGPRINT("fFuncRet: %f\n", fFuncRet);
  return Napi::Number::New(env, static_cast<double>(fFuncRet));
}

///////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////
// dsr api function protocol
// bool set_mode_analog_input(GPIO_CTRLBOX_ANALOG_INDEX eGpioIndex, GPIO_ANALOG_TYPE eAnalogType = GPIO_ANALOG_TYPE_CURRENT)
Napi::Value NodeDsr::SetModeAnalogInput(const Napi::CallbackInfo &info) {
  DBGPRINT("called SetModeAnalogInput\n");
  uint32_t nInfoIndex = 0;

  Napi::Env env = info.Env();
  uint32_t nInfoLen = info.Length();

  if (nInfoLen < 2) {
    Napi::TypeError::New(env, "invalid function parameter").ThrowAsJavaScriptException();
    return Napi::Boolean::New(env, false);
  }

  uint32_t nGpioIndex = info[nInfoIndex++].As<Napi::Number>().Uint32Value();
  DBGPRINT("nGpioIndex: %d\n", nGpioIndex);
  uint32_t nGpioAnalogType = info[nInfoIndex++].As<Napi::Number>().Uint32Value();
  DBGPRINT("nGpioAnalogType: %d\n", nGpioAnalogType);

  bool bFuncRet = m_pDrfl->set_mode_analog_input(static_cast<GPIO_CTRLBOX_ANALOG_INDEX>(nGpioIndex),
                                                 static_cast<GPIO_ANALOG_TYPE>(nGpioAnalogType));
  return Napi::Boolean::New(env, bFuncRet);
}

///////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////
// dsr api function protocol
// bool set_mode_analog_output(GPIO_CTRLBOX_ANALOG_INDEX eGpioIndex, GPIO_ANALOG_TYPE eAnalogType = GPIO_ANALOG_TYPE_CURRENT)
Napi::Value NodeDsr::GetModeAnalogOutput(const Napi::CallbackInfo &info) {
  DBGPRINT("called GetModeAnalogOutput\n");
  uint32_t nInfoIndex = 0;

  Napi::Env env = info.Env();
  uint32_t nInfoLen = info.Length();

  if (nInfoLen < 1) {
    Napi::TypeError::New(env, "invalid function parameter").ThrowAsJavaScriptException();
    return Napi::Boolean::New(env, false);
  }

  uint32_t nGpioIndex = info[nInfoIndex++].As<Napi::Number>().Uint32Value();
  DBGPRINT("nGpioIndex: %d\n", nGpioIndex);
  uint32_t nGpioAnalogType = info[nInfoIndex++].As<Napi::Number>().Uint32Value();
  DBGPRINT("nGpioAnalogType: %d\n", nGpioAnalogType);

  bool bFuncRet = m_pDrfl->set_mode_analog_output(static_cast<GPIO_CTRLBOX_ANALOG_INDEX>(nGpioIndex),
                                                  static_cast<GPIO_ANALOG_TYPE>(nGpioAnalogType));
  return Napi::Boolean::New(env, bFuncRet);
}

///////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////
// dsr api function protocol
// bool drl_start(ROBOT_SYSTEM eRobotSystem, string strDrlProgram)
Napi::Value NodeDsr::DrlStart(const Napi::CallbackInfo &info) {
  DBGPRINT("called DrlStart\n");
  uint32_t nInfoIndex = 0;

  Napi::Env env = info.Env();
  uint32_t nInfoLen = info.Length();

  if (nInfoLen < 2) {
    Napi::TypeError::New(env, "invalid function parameter").ThrowAsJavaScriptException();
    return Napi::Boolean::New(env, false);
  }

  uint32_t nRobotSystem = info[nInfoIndex++].As<Napi::Number>().Uint32Value();
  DBGPRINT("nRobotSystem: %d\n", nRobotSystem);
  std::string strDrlProgram = info[nInfoIndex++].As<Napi::String>().Utf8Value();
  DBGPRINT("strDrlProgram: %s\n", strDrlProgram.c_str());

  bool bFuncRet = m_pDrfl->drl_start(static_cast<ROBOT_SYSTEM>(nRobotSystem), strDrlProgram.c_str());
  return Napi::Boolean::New(env, bFuncRet);
}

///////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////
// dsr api function protocol
// bool drl_stop(STOP_TYPE eStopType = STOP_TYPE_QUICK)
Napi::Value NodeDsr::DrlStop(const Napi::CallbackInfo &info) {
  DBGPRINT("called DrlStop\n");
  uint32_t nInfoIndex = 0;
  Napi::Env env = info.Env();
  uint32_t nInfoLen = info.Length();

  if (nInfoLen < 1) {
    Napi::TypeError::New(env, "invalid function parameter").ThrowAsJavaScriptException();
    return Napi::Boolean::New(env, false);
  }

  uint32_t nStopType = info[nInfoIndex++].As<Napi::Number>().Uint32Value();
  DBGPRINT("nStopType: %d\n", nStopType);

  bool bFuncRet = m_pDrfl->drl_stop(static_cast<STOP_TYPE>(nStopType));
  return Napi::Boolean::New(env, bFuncRet);
}

///////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////
// dsr api function protocol
// bool drl_pause()
Napi::Value NodeDsr::DrlPause(const Napi::CallbackInfo &info) {
  DBGPRINT("called DrlPause\n");
  Napi::Env env = info.Env();

  bool bFuncRet = m_pDrfl->drl_pause();
  return Napi::Boolean::New(env, bFuncRet);
}

///////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////
// dsr api function protocol
// bool drl_resume()
Napi::Value NodeDsr::DrlResume(const Napi::CallbackInfo &info) {
  DBGPRINT("called DrlResume\n");
  Napi::Env env = info.Env();

  bool bFuncRet = m_pDrfl->drl_resume();
  return Napi::Boolean::New(env, bFuncRet);
}

///////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////
// dsr api function protocol
// bool change_operation_speed(float fSpeed)
Napi::Value NodeDsr::ChangeOperationSpeed(const Napi::CallbackInfo &info) {
  DBGPRINT("called ChangeOperationSpeed\n");
  uint32_t nInfoIndex = 0;
  Napi::Env env = info.Env();
  uint32_t nInfoLen = info.Length();

  if (nInfoLen < 1) {
    Napi::TypeError::New(env, "invalid function parameter").ThrowAsJavaScriptException();
    return Napi::Boolean::New(env, false);
  }

  float fSpeed = info[nInfoIndex++].As<Napi::Number>().FloatValue();
  DBGPRINT("fSpeed: %f\n", fSpeed);

  bool bFuncRet = m_pDrfl->change_operation_speed(fSpeed);
  return Napi::Boolean::New(env, bFuncRet);
}

///////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////
// dsr api function protocol
// LPROBOT_POSE trans(float fSourcePos[NUM_TASK], float fOffset[NUM_TASK],
//                    COORDINATE_SYSTEM eSourceRef = COORDINATE_SYSTEM_BASE, COORDINATE_SYSTEM eTargetRef = COORDINATE_SYSTEM_BASE)
Napi::Value NodeDsr::Trans(const Napi::CallbackInfo &info) {
  DBGPRINT("called Trans\n");
  uint32_t nInfoIndex = 0;
  Napi::Env env = info.Env();
  uint32_t nInfoLen = info.Length();

  if (nInfoLen < 2) {
    Napi::TypeError::New(env, "invalid function parameter").ThrowAsJavaScriptException();
    return Napi::Boolean::New(env, false);
  }

  ///////////////////////////////////////////////////////////////////
  // input parameter: fSourcePos[NUM_TASK]
  Napi::Array sourcePoses = info[nInfoIndex++].As<Napi::Array>();
  float fSourcePos[NUM_TASK];
  if (NapiHelper::covertToFloatArray(sourcePoses, NUM_TASK, fSourcePos) == false) {
    Napi::TypeError::New(env, "sourcePoses array length wrong").ThrowAsJavaScriptException();
    return Napi::Boolean::New(env, false);
  }

  ///////////////////////////////////////////////////////////////////
  // input parameter: fOffset[NUM_TASK]
  Napi::Array offsetPoses = info[nInfoIndex++].As<Napi::Array>();
  float fOffset[NUM_TASK];
  if (NapiHelper::covertToFloatArray(offsetPoses, NUM_TASK, fOffset) == false) {
    Napi::TypeError::New(env, "offsetPoses array length wrong").ThrowAsJavaScriptException();
    return Napi::Boolean::New(env, false);
  }

  uint32_t nSourceRef = static_cast<uint32_t>(COORDINATE_SYSTEM_BASE);
  uint32_t nTargetRef = static_cast<uint32_t>(COORDINATE_SYSTEM_BASE);
  if (nInfoLen > 2) {
    nSourceRef = info[nInfoIndex++].As<Napi::Number>().Uint32Value();
    DBGPRINT("nSourceRef: %d\n", nSourceRef);
    nTargetRef = info[nInfoIndex++].As<Napi::Number>().Uint32Value();
    DBGPRINT("nTargetRef: %d\n", nTargetRef);
  }

  ROBOT_POSE *pstResultPose = m_pDrfl->trans(fSourcePos, fOffset,
                                             static_cast<COORDINATE_SYSTEM>(nSourceRef), static_cast<COORDINATE_SYSTEM>(nTargetRef));

  if (pstResultPose == nullptr) {
    return Napi::Boolean::New(env, false);
  }

  Napi::Array resultArray = Napi::Array::New(info.Env(), NUM_TASK);
  for (uint32_t nIter = 0; nIter < NUM_TASK; nIter++) {
    resultArray[nIter] = Napi::Number::New(env, pstResultPose->_fPosition[nIter]);
  }

  return resultArray;
}

///////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////
// dsr api function protocol
// LPROBOT_POSE get_current_pose(ROBOT_SPACE eSpaceType = ROBOT_SPACE_JOINT)
Napi::Value NodeDsr::GetCurrentPos(const Napi::CallbackInfo &info) {
  DBGPRINT("called GetCurrentPos\n");
  uint32_t nInfoIndex = 0;
  Napi::Env env = info.Env();
  uint32_t nInfoLen = info.Length();

  if (nInfoLen < 1) {
    Napi::TypeError::New(env, "invalid function parameter").ThrowAsJavaScriptException();
    return Napi::Boolean::New(env, false);
  }
  ///////////////////////////////////////////////////////////////////
  // input parameter: fSourcePos[NUM_TASK]
  bool bTaskSpace = info[nInfoIndex++].As<Napi::Boolean>().Value();
  ROBOT_POSE *pstResultPose = m_pDrfl->get_current_pose(static_cast<ROBOT_SPACE>(bTaskSpace));
  if (pstResultPose == nullptr) {
    return Napi::Boolean::New(env, false);
  }

  size_t arrayLength = (bTaskSpace == true) ? NUM_TASK : NUM_JOINT;
  Napi::Array resultArray = Napi::Array::New(info.Env(), arrayLength);
  for (uint32_t nIter = 0; nIter < NUM_TASK; nIter++) {
    resultArray[nIter] = Napi::Number::New(env, pstResultPose->_fPosition[nIter]);
  }

  return resultArray;
}

///////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////
// dsr api function protocol
// bool set_singularity_handling(SINGULARITY_AVOIDANCE eMode)
Napi::Value NodeDsr::SetSingularityHandling(const Napi::CallbackInfo &info) {
  uint32_t nInfoIndex = 0;
  Napi::Env env = info.Env();
  uint32_t nInfoLen = info.Length();

  if (nInfoLen < 1) {
    Napi::TypeError::New(env, "invalid function parameter").ThrowAsJavaScriptException();
    return Napi::Boolean::New(env, false);
  }

  uint32_t nMode = info[nInfoIndex++].As<Napi::Number>().Uint32Value();
  DBGPRINT("nMode: %d\n", nMode);

  bool bFuncRet = m_pDrfl->set_singularity_handling(static_cast<SINGULARITY_AVOIDANCE>(nMode));
  return Napi::Boolean::New(env, bFuncRet);
}

///////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////
// dsr api function protocol
// ROBOT_STATE get_robot_state()
Napi::Value NodeDsr::GetRobotState(const Napi::CallbackInfo &info) {
  Napi::Env env = info.Env();

  uint32_t nFuncRet = static_cast<uint32_t>(m_pDrfl->get_robot_state());
  return Napi::Number::New(env, static_cast<double>(nFuncRet));
}

///////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////
// utility funcitons
Napi::Value NodeDsr::SetTaskSpeedLevel(const Napi::CallbackInfo &info) {
  uint32_t nInfoIndex = 0;
  Napi::Env env = info.Env();
  uint32_t nInfoLen = info.Length();

  if (nInfoLen < 1) {
    Napi::TypeError::New(env, "invalid function parameter").ThrowAsJavaScriptException();
    return Napi::Boolean::New(env, false);
  }

  uint32_t nLevel = info[nInfoIndex++].As<Napi::Number>().Uint32Value();
  DBGPRINT("nLevel: %d\n", nLevel);

  if (nLevel > 2)
    nLevel = 0;

  m_nTaskSpeedLevel = nLevel;

  m_fTaskSpeedVelCustom = -1.0f;
  m_fTaskSpeedAccCustom = -1.0f;
  m_bTaskSpeedCustom = false;

  return Napi::Boolean::New(env, true);
}

Napi::Value NodeDsr::SetJointSpeedLevel(const Napi::CallbackInfo &info) {
  uint32_t nInfoIndex = 0;
  Napi::Env env = info.Env();
  uint32_t nInfoLen = info.Length();

  if (nInfoLen < 1) {
    Napi::TypeError::New(env, "invalid function parameter").ThrowAsJavaScriptException();
    return Napi::Boolean::New(env, false);
  }

  uint32_t nLevel = info[nInfoIndex++].As<Napi::Number>().Uint32Value();
  DBGPRINT("nLevel: %d\n", nLevel);

  m_nJointSpeedLevel = nLevel;

  m_fJointSpeedVelCustom = -1.0f;
  m_fJointSpeedAccCustom = -1.0f;
  m_bJointSpeedCustom = false;

  return Napi::Boolean::New(env, true);
}

Napi::Value NodeDsr::GetTaskSpeedData(const Napi::CallbackInfo &info) {
  Napi::Env env = info.Env();

  float fTaskVel = (m_bTaskSpeedCustom == true) ? m_fTaskSpeedVelCustom : m_fTaskSpeedVel[m_nTaskSpeedLevel];
  float fTaskAcc = (m_bTaskSpeedCustom == true) ? m_fTaskSpeedAccCustom : m_fTaskSpeedAcc[m_nTaskSpeedLevel];

  Napi::Array resultArray = Napi::Array::New(info.Env(), 2);
  resultArray[static_cast<uint32_t>(0)] = Napi::Number::New(env, fTaskVel);
  resultArray[static_cast<uint32_t>(1)] = Napi::Number::New(env, fTaskAcc);

  return resultArray;
}

Napi::Value NodeDsr::GetJointSpeedData(const Napi::CallbackInfo &info) {
  Napi::Env env = info.Env();

  float fJointVel = (m_bJointSpeedCustom == true) ? m_fJointSpeedVelCustom : m_fJointSpeedVel[m_nJointSpeedLevel];
  float fJointAcc = (m_bJointSpeedCustom == true) ? m_fJointSpeedAccCustom : m_fJointSpeedAcc[m_nJointSpeedLevel];

  Napi::Array resultArray = Napi::Array::New(info.Env(), 2);
  resultArray[static_cast<uint32_t>(0)] = Napi::Number::New(env, fJointVel);
  resultArray[static_cast<uint32_t>(1)] = Napi::Number::New(env, fJointAcc);

  return resultArray;
}

Napi::Value NodeDsr::SetTaskSpeedCustom(const Napi::CallbackInfo &info) {
  uint32_t nInfoIndex = 0;
  Napi::Env env = info.Env();
  uint32_t nInfoLen = info.Length();

  if (nInfoLen < 2) {
    Napi::TypeError::New(env, "invalid function parameter").ThrowAsJavaScriptException();
    return Napi::Boolean::New(env, false);
  }

  float fVec = info[nInfoIndex++].As<Napi::Number>().FloatValue();
  DBGPRINT("fVec: %f\n", fVec);
  float fAcc = info[nInfoIndex++].As<Napi::Number>().FloatValue();
  DBGPRINT("fAcc: %f\n", fAcc);

  m_fTaskSpeedVelCustom = fVec;
  m_fTaskSpeedAccCustom = fAcc;

  m_bTaskSpeedCustom = true;

  return Napi::Boolean::New(env, true);
}

Napi::Value NodeDsr::SetJointSpeedCustom(const Napi::CallbackInfo &info) {
  uint32_t nInfoIndex = 0;
  Napi::Env env = info.Env();
  uint32_t nInfoLen = info.Length();

  if (nInfoLen < 2) {
    Napi::TypeError::New(env, "invalid function parameter").ThrowAsJavaScriptException();
    return Napi::Boolean::New(env, false);
  }

  float fVec = info[nInfoIndex++].As<Napi::Number>().FloatValue();
  DBGPRINT("fVec: %f\n", fVec);
  float fAcc = info[nInfoIndex++].As<Napi::Number>().FloatValue();
  DBGPRINT("fAcc: %f\n", fAcc);

  m_fJointSpeedVelCustom = fVec;
  m_fJointSpeedAccCustom = fAcc;

  m_bJointSpeedCustom = true;

  return Napi::Boolean::New(env, true);
}