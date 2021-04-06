#include <assert.h>
#include <unistd.h>

#include <iostream>

#include "NapiHelper.h"
#include "NodeDsr.h"
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
                      InstanceMethod("getAnalogOutput", &NodeDsr::GetAnalogOutput),
                      InstanceMethod("setModeAnalogInput", &NodeDsr::SetModeAnalogInput),
                      InstanceMethod("getModeAnalogOutput", &NodeDsr::GetModeAnalogOutput),
                      InstanceMethod("startDRL", &NodeDsr::DrlStart),
                      InstanceMethod("stopDRL", &NodeDsr::DrlStop),
                      InstanceMethod("pauseDRL", &NodeDsr::DrlPause),
                      InstanceMethod("resumeDRL", &NodeDsr::DrlResume),
                      InstanceMethod("changeOperationSpeed", &NodeDsr::ChangeOperationSpeed),
                      InstanceMethod("trans", &NodeDsr::Trans),
                      InstanceMethod("getCurrentPos", &NodeDsr::GetCurrentPos),
                  });

  Napi::FunctionReference *constructor = new Napi::FunctionReference();
  *constructor = Napi::Persistent(func);
  env.SetInstanceData(constructor);

  exports.Set("NodeDsr", func);
  return exports;
}

NodeDsr::NodeDsr(const Napi::CallbackInfo &info)
    : Napi::ObjectWrap<NodeDsr>(info), m_pDrfl(NULL), m_strUrl(""), m_nPort(12345), m_nIndex(0), m_TpInitailizingComplted(false), m_bHasControlAuthority(false), m_nTaskSpeedLevel(0), m_fTaskSpeedVel{30, 50, 100}, m_fTaskSpeedAcc{40, 60, 100}, m_nJointSpeedLevel(0), m_fJointSpeedVel{5, 10, 20}, m_fJointSpeedAcc{20, 30, 40} {
  {
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

    bool bConnected = m_pDrfl->open_connection(m_strUrl, m_nPort);
    DBGPRINT("connection result: %d\n", bConnected);

    // TODO: must check this gpio output
    m_pDrfl->set_digital_output(GPIO_CTRLBOX_DIGITAL_INDEX_10, TRUE);
    while ((m_pDrfl->get_robot_state() != STATE_STANDBY) || !m_bHasControlAuthority) {
      usleep(1000);
    }

    m_pDrfl->set_robot_mode(ROBOT_MODE_MANUAL);
    m_pDrfl->set_robot_system(ROBOT_SYSTEM_REAL);

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

    return Napi::Boolean::New(env, false);
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
#endif

  // dsr api function protocol
  // const char* get_library_version()
  Napi::Value NodeDsr::GetLibraryVersion(const Napi::CallbackInfo &info) {
    DBGPRINT("called GetLibraryVersion\n");
    Napi::Env env = info.Env();

    return Napi::String::New(env, "");
  }

  // dsr api function protocol
  // bool get_system_version(LPSYSTEM_VERSION pVersion)
  Napi::Value NodeDsr::GetSystemVersion(const Napi::CallbackInfo &info) {
    DBGPRINT("called GetSystemVersion\n");

    Napi::Env env = info.Env();
    return Napi::String::New(env, "");
  }

  // dsr api function protocol
  // void set_on_monitoring_state(TOnMonitoringStateCB pCallbackFunc)
  // typedef void (*TOnMonitoringStateCB)(const ROBOT_STATE);
  void NodeDsr::SetOnMonitoringStateCB(Napi::Env env, Napi::Function jsCallback, uint32_t * peState) {
    DBGPRINT("called SetOnMonitoringStateCB - start\n");
  }

  void NodeDsr::SetOnMonitoringState(const Napi::CallbackInfo &info) {
    DBGPRINT("called SetOnMonitoringState\n");
  }

  // dsr api function protocol
  // void set_on_monitoring_access_control(TOnMonitoringAccessControlCB pCallbackFunc)
  // typedef void (*TOnMonitoringAccessControlCB)(const MONITORING_ACCESS_CONTROL);
  void NodeDsr::SetOnMonitoringAccessControl(const Napi::CallbackInfo &info) {
    DBGPRINT("called SetOnMonitoringAccessControl\n");
  }

  // dsr api function protocol
  // void void set_on_disconnected(TOnDisconnectedCB pCallbackFunc)
  // typedef void (*TOnDisconnectedCB)();
  void NodeDsr::SetOnDisconnected(const Napi::CallbackInfo &info) {
    DBGPRINT("called SetOnDisconnected\n");
  }

  void NodeDsr::SetOnDisconnectedCB(Napi::Env env, Napi::Function jsCallback) {
    DBGPRINT("called SetOnDisconnectedCB - start\n");
  }

  Napi::Value NodeDsr::MoveHome(const Napi::CallbackInfo &info) {
    DBGPRINT("called MoveHome\n");
    Napi::Env env = info.Env();
    return Napi::Boolean::New(env, false);
  }

  // dsr api function protocol
  // bool movej(float fTargetPos[NUM_JOINT], float fTargetVel, float fTargetAcc, float fTargetTime = 0.f,
  // MOVE_MODE eMoveMode = MOVE_MODE_ABSOLUTE, float fBlendingRadius = 0.f, BLENDING_SPEED_TYPE eBlendingType = BLENDING_SPEED_TYPE_DUPLICATE)
  Napi::Value NodeDsr::MoveJ(const Napi::CallbackInfo &info) {
    DBGPRINT("called MoveJ\n");
    Napi::Env env = info.Env();
    return Napi::Boolean::New(env, false);
  }

  // dsr api function protocol
  // bool amovej(float fTargetPos[NUM_JOINT], float fTargetVel, float fTargetAcc, float fTargetTime = 0.f,
  //            MOVE_MODE eMoveMode = MOVE_MODE_ABSOLUTE, BLENDING_SPEED_TYPE eBlendingType = BLENDING_SPEED_TYPE_DUPLICATE)
  Napi::Value NodeDsr::MoveJA(const Napi::CallbackInfo &info) {
    DBGPRINT("called MoveJA\n");
    Napi::Env env = info.Env();
    return Napi::Boolean::New(env, false);
  }

  ///////////////////////////////////////////////////////////////////
  ///////////////////////////////////////////////////////////////////
  // dsr api function protocol
  // bool movel(float fTargetPos[NUM_TASK], float fTargetVel[2], float fTargetAcc[2], float fTargetTime = 0.f,
  //            MOVE_MODE eMoveMode = MOVE_MODE_ABSOLUTE, MOVE_REFERENCE eMoveReference = MOVE_REFERENCE_BASE,
  //            float fBlendingRadius = 0.f, BLENDING_SPEED_TYPE eBlendingType = BLENDING_SPEED_TYPE_DUPLICATE)
  Napi::Value NodeDsr::MoveL(const Napi::CallbackInfo &info) {
    DBGPRINT("called MoveL\n");
    Napi::Env env = info.Env();
    return Napi::Boolean::New(env, false);
  }

  ///////////////////////////////////////////////////////////////////
  ///////////////////////////////////////////////////////////////////
  // dsr api function protocol
  // bool amovel(float fTargetPos[NUM_TASK], float fTargetVel[2], float fTargetAcc[2], float fTargetTime = 0.f,
  //      MOVE_MODE eMoveMode = MOVE_MODE_ABSOLUTE, MOVE_REFERENCE eMoveReference = MOVE_REFERENCE_BASE,
  //       BLENDING_SPEED_TYPE eBlendingType = BLENDING_SPEED_TYPE_DUPLICATE)
  Napi::Value NodeDsr::MoveLA(const Napi::CallbackInfo &info) {
    DBGPRINT("called MoveLA\n");
    Napi::Env env = info.Env();
    return Napi::Boolean::New(env, false);
  }

  ///////////////////////////////////////////////////////////////////
  ///////////////////////////////////////////////////////////////////
  // dsr api function protocol
  // bool stop(STOP_TYPE eStopType = STOP_TYPE_QUICK)
  Napi::Value NodeDsr::Stop(const Napi::CallbackInfo &info) {
    DBGPRINT("called Stop\n");
    Napi::Env env = info.Env();
    return Napi::Boolean::New(env, false);
  }

  ///////////////////////////////////////////////////////////////////
  ///////////////////////////////////////////////////////////////////
  // dsr api function protocol
  // bool move_pause()
  Napi::Value NodeDsr::MovePause(const Napi::CallbackInfo &info) {
    DBGPRINT("called MovePause\n");
    Napi::Env env = info.Env();
    return Napi::Boolean::New(env, false);
  }

  ///////////////////////////////////////////////////////////////////
  ///////////////////////////////////////////////////////////////////
  // dsr api function protocol
  // bool move_pause()
  Napi::Value NodeDsr::MoveResume(const Napi::CallbackInfo &info) {
    DBGPRINT("called MoveResume\n");
    Napi::Env env = info.Env();
    return Napi::Boolean::New(env, false);
  }

  ///////////////////////////////////////////////////////////////////
  ///////////////////////////////////////////////////////////////////
  // dsr api function protocol
  // bool mwait()
  Napi::Value NodeDsr::MWait(const Napi::CallbackInfo &info) {
    DBGPRINT("called MoveResume\n");
    Napi::Env env = info.Env();
    return Napi::Boolean::New(env, false);
  }

  ///////////////////////////////////////////////////////////////////
  ///////////////////////////////////////////////////////////////////
  // dsr api function protocol
  // bool set_tool_digital_output(GPIO_TOOL_DIGITAL_INDEX eGpioIndex, bool bOnOff)
  Napi::Value NodeDsr::SetToolDigitalOutput(const Napi::CallbackInfo &info) {
    DBGPRINT("called SetToolDigitalOutput\n");
    Napi::Env env = info.Env();
    return Napi::Boolean::New(env, false);
  }

  ///////////////////////////////////////////////////////////////////
  ///////////////////////////////////////////////////////////////////
  // dsr api function protocol
  // bool set_tool_digital_output(GPIO_TOOL_DIGITAL_INDEX eGpioIndex, bool bOnOff)
  Napi::Value NodeDsr::GetToolDigitalOutput(const Napi::CallbackInfo &info) {
    DBGPRINT("called GetToolDigitalOutput\n");
    Napi::Env env = info.Env();
    return Napi::Boolean::New(env, false);
  }

  ///////////////////////////////////////////////////////////////////
  ///////////////////////////////////////////////////////////////////
  // dsr api function protocol
  // bool get_tool_digital_input(GPIO_TOOL_DIGITAL_INDEX eGpioIndex)
  Napi::Value NodeDsr::GetToolDigitalInput(const Napi::CallbackInfo &info) {
    DBGPRINT("called GetToolDigitalInput\n");
    Napi::Env env = info.Env();
    return Napi::Boolean::New(env, false);
  }

  ///////////////////////////////////////////////////////////////////
  ///////////////////////////////////////////////////////////////////
  // dsr api function protocol
  // bool set_digital_output(GPIO_CTRLBOX_DIGITAL_INDEX eGpioIndex, bool bOnOff)
  Napi::Value NodeDsr::SetDigitalOutput(const Napi::CallbackInfo &info) {
    DBGPRINT("called SetDigitalOutput\n");
    Napi::Env env = info.Env();
    return Napi::Boolean::New(env, false);
  }

  ///////////////////////////////////////////////////////////////////
  ///////////////////////////////////////////////////////////////////
  // dsr api function protocol
  // bool get_digital_output(GPIO_CTRLBOX_DIGITAL_INDEX eGpioIndex)
  Napi::Value NodeDsr::GetDigitalOutput(const Napi::CallbackInfo &info) {
    DBGPRINT("called GetDigitalOutput\n");
    Napi::Env env = info.Env();
    return Napi::Boolean::New(env, false);
  }

  ///////////////////////////////////////////////////////////////////
  ///////////////////////////////////////////////////////////////////
  // dsr api function protocol
  // bool get_digital_input(GPIO_CTRLBOX_DIGITAL_INDEX eGpioIndex)
  Napi::Value NodeDsr::GetDigitalInput(const Napi::CallbackInfo &info) {
    DBGPRINT("called GetDigitalInput\n");
    Napi::Env env = info.Env();
    return Napi::Boolean::New(env, false);
  }

  ///////////////////////////////////////////////////////////////////
  ///////////////////////////////////////////////////////////////////
  // dsr api function protocol
  // bool set_analog_output(GPIO_CTRLBOX_ANALOG_INDEX eGpioIndex, float fValue)
  Napi::Value NodeDsr::SetAnalogOutput(const Napi::CallbackInfo &info) {
    DBGPRINT("called SetAnalogOutput\n");
    Napi::Env env = info.Env();
    return Napi::Boolean::New(env, false);
  }

  ///////////////////////////////////////////////////////////////////
  ///////////////////////////////////////////////////////////////////
  // dsr api function protocol
  // float get_analog_input(GPIO_CTRLBOX_ANALOG_INDEX eGpioIndex)
  Napi::Value NodeDsr::GetAnalogOutput(const Napi::CallbackInfo &info) {
    DBGPRINT("called GetAnalogOutput\n");
    Napi::Env env = info.Env();
    return Napi::Boolean::New(env, false);
  }

  ///////////////////////////////////////////////////////////////////
  ///////////////////////////////////////////////////////////////////
  // dsr api function protocol
  // bool set_mode_analog_input(GPIO_CTRLBOX_ANALOG_INDEX eGpioIndex, GPIO_ANALOG_TYPE eAnalogType = GPIO_ANALOG_TYPE_CURRENT)
  Napi::Value NodeDsr::SetModeAnalogInput(const Napi::CallbackInfo &info) {
    DBGPRINT("called SetModeAnalogInput\n");
    Napi::Env env = info.Env();
    return Napi::Boolean::New(env, false);
  }

  ///////////////////////////////////////////////////////////////////
  ///////////////////////////////////////////////////////////////////
  // dsr api function protocol
  // bool set_mode_analog_output(GPIO_CTRLBOX_ANALOG_INDEX eGpioIndex, GPIO_ANALOG_TYPE eAnalogType = GPIO_ANALOG_TYPE_CURRENT)
  Napi::Value NodeDsr::GetModeAnalogOutput(const Napi::CallbackInfo &info) {
    DBGPRINT("called GetModeAnalogOutput\n");
    Napi::Env env = info.Env();
    return Napi::Boolean::New(env, false);
  }

  ///////////////////////////////////////////////////////////////////
  ///////////////////////////////////////////////////////////////////
  // dsr api function protocol
  // bool drl_start(ROBOT_SYSTEM eRobotSystem, string strDrlProgram)
  Napi::Value NodeDsr::DrlStart(const Napi::CallbackInfo &info) {
    DBGPRINT("called DrlStart\n");
    Napi::Env env = info.Env();
    return Napi::Boolean::New(env, false);
  }

  ///////////////////////////////////////////////////////////////////
  ///////////////////////////////////////////////////////////////////
  // dsr api function protocol
  // bool drl_stop(STOP_TYPE eStopType = STOP_TYPE_QUICK)
  Napi::Value NodeDsr::DrlStop(const Napi::CallbackInfo &info) {
    DBGPRINT("called DrlStop\n");
    Napi::Env env = info.Env();
    return Napi::Boolean::New(env, false);
  }

  ///////////////////////////////////////////////////////////////////
  ///////////////////////////////////////////////////////////////////
  // dsr api function protocol
  // bool drl_pause()
  Napi::Value NodeDsr::DrlPause(const Napi::CallbackInfo &info) {
    DBGPRINT("called DrlPause\n");
    Napi::Env env = info.Env();
    return Napi::Boolean::New(env, false);
  }

  ///////////////////////////////////////////////////////////////////
  ///////////////////////////////////////////////////////////////////
  // dsr api function protocol
  // bool drl_resume()
  Napi::Value NodeDsr::DrlResume(const Napi::CallbackInfo &info) {
    DBGPRINT("called DrlResume\n");
    Napi::Env env = info.Env();
    return Napi::Boolean::New(env, false);
  }

  ///////////////////////////////////////////////////////////////////
  ///////////////////////////////////////////////////////////////////
  // dsr api function protocol
  // bool change_operation_speed(float fSpeed)
  Napi::Value NodeDsr::ChangeOperationSpeed(const Napi::CallbackInfo &info) {
    DBGPRINT("called ChangeOperationSpeed\n");
    Napi::Env env = info.Env();
    return Napi::Boolean::New(env, false);
  }

  ///////////////////////////////////////////////////////////////////
  ///////////////////////////////////////////////////////////////////
  // dsr api function protocol
  // LPROBOT_POSE trans(float fSourcePos[NUM_TASK], float fOffset[NUM_TASK],
  //                    COORDINATE_SYSTEM eSourceRef = COORDINATE_SYSTEM_BASE, COORDINATE_SYSTEM eTargetRef = COORDINATE_SYSTEM_BASE)
  Napi::Value NodeDsr::Trans(const Napi::CallbackInfo &info) {
    DBGPRINT("called Trans\n");
    Napi::Env env = info.Env();
    return Napi::Boolean::New(env, false);
  }

  ///////////////////////////////////////////////////////////////////
  ///////////////////////////////////////////////////////////////////
  // dsr api function protocol
  // LPROBOT_POSE get_current_pose(ROBOT_SPACE eSpaceType = ROBOT_SPACE_JOINT)
  Napi::Value NodeDsr::GetCurrentPos(const Napi::CallbackInfo &info) {
    DBGPRINT("called GetCurrentPos\n");
    Napi::Env env = info.Env();
    return Napi::Boolean::New(env, false);
  }

  ///////////////////////////////////////////////////////////////////
  ///////////////////////////////////////////////////////////////////
  // dsr api function protocol
  // bool set_singularity_handling(SINGULARITY_AVOIDANCE eMode)
  Napi::Value NodeDsr::SetSingularityHandling(const Napi::CallbackInfo &info) {
    DBGPRINT("called setSingularityHandling\n");
    Napi::Env env = info.Env();
    return Napi::Boolean::New(env, false);
  }

  ///////////////////////////////////////////////////////////////////
  ///////////////////////////////////////////////////////////////////
  // dsr api function protocol
  // ROBOT_STATE get_robot_state()
  Napi::Value NodeDsr::GetRobotState(const Napi::CallbackInfo &info) {
    Napi::Env env = info.Env();
    return Napi::Boolean::New(env, false);
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

    return Napi::Boolean::New(env, true);
  }

  Napi::Value NodeDsr::GetTaskSpeedData(const Napi::CallbackInfo &info) {
    Napi::Env env = info.Env();

    Napi::Array resultArray = Napi::Array::New(info.Env(), 2);
    resultArray[static_cast<uint32_t>(0)] = Napi::Number::New(env, m_fTaskSpeedVel[m_nTaskSpeedLevel]);
    resultArray[static_cast<uint32_t>(1)] = Napi::Number::New(env, m_fTaskSpeedAcc[m_nTaskSpeedLevel]);

    return resultArray;
  }

  Napi::Value NodeDsr::GetJointSpeedData(const Napi::CallbackInfo &info) {
    Napi::Env env = info.Env();

    Napi::Array resultArray = Napi::Array::New(info.Env(), 2);
    resultArray[static_cast<uint32_t>(0)] = Napi::Number::New(env, m_fJointSpeedVel[m_nJointSpeedLevel]);
    resultArray[static_cast<uint32_t>(1)] = Napi::Number::New(env, m_fJointSpeedAcc[m_nJointSpeedLevel]);

    return resultArray;
  }