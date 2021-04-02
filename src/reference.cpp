//
// NAPI 헤더입니다.
// npm을 통해 node-addon-api를 로컬로 설치하면 ./node_modules 폴더에 같이 딸려옵니다.
// 이것을 설치하지 않고, 먼저 작업하면 include에 빨간줄이 그어집니다.
#include <assert.h>
#include <napi.h>
#include <unistd.h>

#include <iostream>

#include "DRFLEx.h"
using namespace DRAFramework;

// class NodeDsr
// {
//   static

//       CDRFLEx *m_pDrfl;

//   bool g_bHasControlAuthority = FALSE;
//   bool g_TpInitailizingComplted = FALSE;
//   bool g_mStat = FALSE;
//   bool g_Stop = FALSE;
// };

CDRFLEx Drfl;
bool g_bHasControlAuthority = FALSE;
bool g_TpInitailizingComplted = FALSE;
bool g_mStat = FALSE;
bool g_Stop = FALSE;

void OnTpInitializingCompleted() {
  g_TpInitailizingComplted = TRUE;
  Drfl.ManageAccessControl(MANAGE_ACCESS_CONTROL_FORCE_REQUEST);
}

void OnHommingCompleted() {
  cout << "homming completed" << endl;
}

void OnProgramStopped(const PROGRAM_STOP_CAUSE) {
  assert(Drfl.PlayDrlStop(STOP_TYPE_SLOW));
  cout << "program stopped" << endl;
}
void OnMonitoringDataCB(const LPMONITORING_DATA pData)

{
  // cout << "# monitoring 0 data "
  //      << pData->_tCtrl._tTask._fActualPos[0][0]
  //      << pData->_tCtrl._tTask._fActualPos[0][1]
  //      << pData->_tCtrl._tTask._fActualPos[0][2]
  //      << pData->_tCtrl._tTask._fActualPos[0][3]
  //      << pData->_tCtrl._tTask._fActualPos[0][4]
  //      << pData->_tCtrl._tTask._fActualPos[0][5] << endl;
}

void OnMonitoringDataExCB(const LPMONITORING_DATA_EX pData) {
  // cout << "# monitoring 1 data "
  //      << pData->_tCtrl._tWorld._fTargetPos[0]
  //      << pData->_tCtrl._tWorld._fTargetPos[1]
  //      << pData->_tCtrl._tWorld._fTargetPos[2]
  //      << pData->_tCtrl._tWorld._fTargetPos[3]
  //      << pData->_tCtrl._tWorld._fTargetPos[4]
  //      << pData->_tCtrl._tWorld._fTargetPos[5] << endl;
}

void OnMonitoringCtrlIOCB(const LPMONITORING_CTRLIO pData) {
  // cout << "# monitoring ctrl 0 data" << endl;
  // for (int i = 0; i < 16; i++)
  // {
  //   cout << (int)pData->_tInput._iActualDI[i] << endl;
  // }
}

void OnMonitoringCtrlIOExCB(const LPMONITORING_CTRLIO_EX pData) {
  // cout << "# monitoring ctrl 1 data" << endl;
  // for (int i = 0; i < 16; i++)
  // {
  //   cout << (int)pData->_tInput._iActualDI[i] << endl;
  // }
  // for (int i = 0; i < 16; i++)
  // {
  //   cout << (int)pData->_tOutput._iTargetDO[i] << endl;
  // }
}

void OnMonitoringStateCB(const ROBOT_STATE eState) {
  // 50msec �̳� �۾��� ������ ��.
  switch ((unsigned char)eState) {
    case STATE_EMERGENCY_STOP:
      // popup
      break;
    case STATE_STANDBY:
    case STATE_MOVING:
    case STATE_TEACHING:
      break;
    case STATE_SAFE_STOP:
      if (g_bHasControlAuthority) {
        Drfl.SetSafeStopResetType(SAFE_STOP_RESET_TYPE_DEFAULT);
        Drfl.SetRobotControl(CONTROL_RESET_SAFET_STOP);
      }
      break;
    case STATE_SAFE_OFF:
      if (g_bHasControlAuthority)
        assert(Drfl.SetRobotControl(CONTROL_SERVO_ON));
      break;
    case STATE_SAFE_STOP2:
      if (g_bHasControlAuthority)
        Drfl.SetRobotControl(CONTROL_RECOVERY_SAFE_STOP);
      break;
    case STATE_SAFE_OFF2:
      if (g_bHasControlAuthority) {
        Drfl.SetRobotControl(CONTROL_RECOVERY_SAFE_OFF);
      }
      break;
    case STATE_RECOVERY:
      //Drfl.SetRobotControl(CONTROL_RESET_RECOVERY);
      break;
    default:
      break;
  }
  return;
  cout << "current state: " << (int)eState << endl;
}

void OnMonitroingAccessControlCB(const MONITORING_ACCESS_CONTROL eTrasnsitControl) {
  switch (eTrasnsitControl) {
    case MONITORING_ACCESS_CONTROL_REQUEST:
      assert(Drfl.ManageAccessControl(MANAGE_ACCESS_CONTROL_RESPONSE_NO));
      //Drfl.ManageAccessControl(MANAGE_ACCESS_CONTROL_RESPONSE_YES);
      break;
    case MONITORING_ACCESS_CONTROL_GRANT:
      g_bHasControlAuthority = TRUE;
      OnMonitoringStateCB(Drfl.GetRobotState());
      break;
    case MONITORING_ACCESS_CONTROL_DENY:
    case MONITORING_ACCESS_CONTROL_LOSS:
      g_bHasControlAuthority = FALSE;
      if (g_TpInitailizingComplted) {
        assert(Drfl.ManageAccessControl(MANAGE_ACCESS_CONTROL_REQUEST));
      }
      break;
    default:
      break;
  }
}

void OnLogAlarm(LPLOG_ALARM tLog) {
  g_mStat = true;
  cout << "Alarm Info: "
       << "group(" << (unsigned int)tLog->_iGroup << "), index(" << tLog->_iIndex
       << "), param(" << tLog->_szParam[0] << "), param(" << tLog->_szParam[1] << "), param(" << tLog->_szParam[2] << ")" << endl;
}

void OnTpPopup(LPMESSAGE_POPUP tPopup) {
  cout << "Popup Message: " << tPopup->_szText << endl;
  cout << "Message Level: " << tPopup->_iLevel << endl;
  cout << "Button Type: " << tPopup->_iBtnType << endl;
}

void OnTpLog(const char *strLog) {
  cout << "Log Message: " << strLog << endl;
}

void OnTpProgress(LPMESSAGE_PROGRESS tProgress) {
  cout << "Progress cnt : " << (int)tProgress->_iTotalCount << endl;
  cout << "Current cnt : " << (int)tProgress->_iCurrentCount << endl;
}

void OnTpGetuserInput(LPMESSAGE_INPUT tInput) {
  cout << "User Input : " << tInput->_szText << endl;
  cout << "Data Type : " << (int)tInput->_iType << endl;
}

Napi::Boolean InitializeDsr(const Napi::CallbackInfo &info) {
  cout << "called InitializeDsr" << endl;

  Napi::Env env = info.Env();

  // Drfl.set_on_homming_completed(OnHommingCompleted);
  // Drfl.set_on_monitoring_data(OnMonitoringDataCB);
  // Drfl.set_on_monitoring_data_ex(OnMonitoringDataExCB);
  // Drfl.set_on_monitoring_ctrl_io(OnMonitoringCtrlIOCB);
  // Drfl.set_on_monitoring_ctrl_io_ex(OnMonitoringCtrlIOExCB);
  // Drfl.set_on_monitoring_state(OnMonitoringStateCB);
  Drfl.set_on_monitoring_access_control(OnMonitroingAccessControlCB);
  Drfl.set_on_tp_initializing_completed(OnTpInitializingCompleted);
  // Drfl.set_on_log_alarm(OnLogAlarm);
  // Drfl.set_on_tp_popup(OnTpPopup);
  // Drfl.set_on_tp_log(OnTpLog);
  // Drfl.set_on_tp_progress(OnTpProgress);
  // Drfl.set_on_tp_get_user_input(OnTpGetuserInput);
  // Drfl.set_on_program_stopped(OnProgramStopped);

  return Napi::Boolean::New(env, true);
}

Napi::Boolean OpenConnection(const Napi::CallbackInfo &info) {
  cout << "called OpenConnection" << endl;

  Napi::Env env = info.Env();

  std::cout << "info.Length: " << info.Length() << endl;

  if (info.Length() < 2) {
    Napi::TypeError::New(env, "wrong number of arguments")
        .ThrowAsJavaScriptException();

    return Napi::Boolean::New(env, false);
  }

  string arg0 = info[0].As<Napi::String>().Utf8Value();
  unsigned int arg1 = info[1].As<Napi::Number>().Uint32Value();
  cout << "arg0: " << arg0 << endl;
  cout << "arg1: " << arg1 << endl;

  cout << Drfl.get_library_version() << endl;

  bool bConnected = Drfl.open_connection(arg0, arg1);
  cout << "Connected?: " << bConnected << endl;

  if (bConnected) {
    SYSTEM_VERSION tSysVerion;
    Drfl.get_system_version(&tSysVerion);
    cout << "setup_monitoring: " << Drfl.setup_monitoring_version(1) << endl;
    Drfl.set_digital_output(GPIO_CTRLBOX_DIGITAL_INDEX_10, TRUE);

    cout << "System version: " << tSysVerion._szController << endl;
    cout << "Library version: " << Drfl.GetLibraryVersion() << endl;

    while ((Drfl.get_robot_state() != STATE_STANDBY) || !g_bHasControlAuthority) {
      // cout << "ControlAuthority: " << g_bHasControlAuthority << endl;
      // cout << Drfl.get_robot_state() << endl;
      usleep(1000);

      // TODO: break this infinite loop here
    }

    Drfl.set_robot_mode(ROBOT_MODE_MANUAL);
    Drfl.set_robot_system(ROBOT_SYSTEM_REAL);
  }

  return Napi::Boolean::New(env, bConnected);
}

Napi::Boolean CloseConnection(const Napi::CallbackInfo &info) {
  cout << "called CloseConnection" << endl;

  Napi::Env env = info.Env();

  Drfl.close_connection();

  return Napi::Boolean::New(env, true);
}

Napi::String GetLibraryVersion(const Napi::CallbackInfo &info) {
  cout << "called GetLibraryVersion" << endl;

  Napi::Env env = info.Env();

  const char *libVersion = Drfl.get_library_version();

  return Napi::String::New(env, libVersion);
}

// Ref: https://github.com/nodejs/node-addon-examples/blob/4c3b7816662e3a4ab26d41f007bbd1784205bda6/array_buffer_to_native/node-addon-api/array_buffer_to_native.cc
Napi::Boolean MoveJ(const Napi::CallbackInfo &info) {
  cout << "called MoveJ" << endl;

  Napi::Env env = info.Env();

  cout << info.Length() << endl;
  cout << info[0].IsArray() << endl;
  Napi::Array inputs = info[0].As<Napi::Array>();
  cout << inputs.Length() << endl;
  cout << inputs.Get(static_cast<uint32_t>(0)).As<Napi::Number>().FloatValue() << endl;

  // cout << inputs.Get("0").IsNumber() << endl;

  // cout << "Using ArrayBuffer.." << endl;
  // Napi::TypedArray buf = info[0].As<Napi::TypedArray>();
  // cout << buf.ElementSize() << endl;

  //float *fInputs = reinterpret_cast<float *>(buf.Data());
  // for (size_t index = 0; index < 1; index++)
  // {
  //     cout << fInputs[index] << endl;
  // }

  // //const auto fInputs = static_cast<const float *>(buffer.Data());
  // //cout << fJointArray[0] << ", " << fJointArray[1] << endl;

  return Napi::Boolean::New(env, true);
}

// addon initializer
Napi::Object init(Napi::Env env, Napi::Object exports) {
  exports.Set(Napi::String::New(env, "initialize"), Napi::Function::New(env, InitializeDsr));
  exports.Set(Napi::String::New(env, "getLibraryVersion"), Napi::Function::New(env, GetLibraryVersion));
  exports.Set(Napi::String::New(env, "openConnection"), Napi::Function::New(env, OpenConnection));
  exports.Set(Napi::String::New(env, "movej"), Napi::Function::New(env, MoveJ));
  exports.Set(Napi::String::New(env, "closeConnection"), Napi::Function::New(env, CloseConnection));

  return exports;
};

NODE_API_MODULE(node_dsr, init);