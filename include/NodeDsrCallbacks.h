#pragma once

#include "DRFLEx.h"
#include "NodeDsr.h"
#include "NodeDsrArray.h"

////////////////////////////////////////////////////////
// Default Callback functions
#define DECLARE_TpInitializingCompleted(__INDEX__)                                      \
  void OnTpInitializingCompleted##__INDEX__() {                                         \
    uint32_t nIndex = static_cast<uint32_t>(atoi(#__INDEX__));                          \
    NodeDsr *pclNodeDsr = NodeDsrArray::Get(nIndex);                                    \
    pclNodeDsr->m_TpInitailizingComplted = TRUE;                                        \
    pclNodeDsr->GetCDRFLex()->ManageAccessControl(MANAGE_ACCESS_CONTROL_FORCE_REQUEST); \
  }
DECLARE_TpInitializingCompleted(0);
DECLARE_TpInitializingCompleted(1);
DECLARE_TpInitializingCompleted(2);
DECLARE_TpInitializingCompleted(3);
DECLARE_TpInitializingCompleted(4);
DECLARE_TpInitializingCompleted(5);
DECLARE_TpInitializingCompleted(6);
DECLARE_TpInitializingCompleted(7);
DECLARE_TpInitializingCompleted(8);
DECLARE_TpInitializingCompleted(9);
TOnTpInitializingCompletedCB g_pfnTpInitializingCompleted[NODEDSR_INSTANCE_LIMIT] = {OnTpInitializingCompleted0, OnTpInitializingCompleted1, OnTpInitializingCompleted2, OnTpInitializingCompleted3, OnTpInitializingCompleted4, OnTpInitializingCompleted5, OnTpInitializingCompleted6, OnTpInitializingCompleted7, OnTpInitializingCompleted8, OnTpInitializingCompleted9};

#define DECLARE_MonitoringState(__INDEX__)                                              \
  void OnMonitoringState##__INDEX__(const ROBOT_STATE eState) {                         \
    uint32_t nIndex = static_cast<uint32_t>(atoi(#__INDEX__));                          \
    NodeDsr *pclNodeDsr = NodeDsrArray::Get(nIndex);                                    \
    switch ((unsigned char)eState) {                                                    \
      case STATE_EMERGENCY_STOP:                                                        \
        break;                                                                          \
      case STATE_STANDBY:                                                               \
      case STATE_MOVING:                                                                \
      case STATE_TEACHING:                                                              \
        break;                                                                          \
      case STATE_SAFE_STOP:                                                             \
        if (pclNodeDsr->m_bHasControlAuthority) {                                       \
          pclNodeDsr->GetCDRFLex()->SetSafeStopResetType(SAFE_STOP_RESET_TYPE_DEFAULT); \
          pclNodeDsr->GetCDRFLex()->SetRobotControl(CONTROL_RESET_SAFET_STOP);          \
        }                                                                               \
        break;                                                                          \
      case STATE_SAFE_OFF:                                                              \
        if (pclNodeDsr->m_bHasControlAuthority)                                         \
          assert(pclNodeDsr->GetCDRFLex()->SetRobotControl(CONTROL_SERVO_ON));          \
        break;                                                                          \
      case STATE_SAFE_STOP2:                                                            \
        if (pclNodeDsr->m_bHasControlAuthority)                                         \
          pclNodeDsr->GetCDRFLex()->SetRobotControl(CONTROL_RECOVERY_SAFE_STOP);        \
        break;                                                                          \
      case STATE_SAFE_OFF2:                                                             \
        if (pclNodeDsr->m_bHasControlAuthority) {                                       \
          pclNodeDsr->GetCDRFLex()->SetRobotControl(CONTROL_RECOVERY_SAFE_OFF);         \
        }                                                                               \
        break;                                                                          \
      case STATE_RECOVERY:                                                              \
        break;                                                                          \
      default:                                                                          \
        break;                                                                          \
    }                                                                                   \
  }
DECLARE_MonitoringState(0);
DECLARE_MonitoringState(1);
DECLARE_MonitoringState(2);
DECLARE_MonitoringState(3);
DECLARE_MonitoringState(4);
DECLARE_MonitoringState(5);
DECLARE_MonitoringState(6);
DECLARE_MonitoringState(7);
DECLARE_MonitoringState(8);
DECLARE_MonitoringState(9);
TOnMonitoringStateCB g_pfnMonitoringState[NODEDSR_INSTANCE_LIMIT] = {OnMonitoringState0, OnMonitoringState1, OnMonitoringState2, OnMonitoringState3, OnMonitoringState4, OnMonitoringState5, OnMonitoringState6, OnMonitoringState7, OnMonitoringState8, OnMonitoringState9};

#define DECLARE_MonitroingAccessControl(__INDEX__)                                                \
  void OnMonitroingAccessControl##__INDEX__(const MONITORING_ACCESS_CONTROL eTrasnsitControl) {   \
    uint32_t nIndex = static_cast<uint32_t>(atoi(#__INDEX__));                                    \
    NodeDsr *pclNodeDsr = NodeDsrArray::Get(nIndex);                                              \
    switch (eTrasnsitControl) {                                                                   \
      case MONITORING_ACCESS_CONTROL_REQUEST:                                                     \
        assert(pclNodeDsr->GetCDRFLex()->ManageAccessControl(MANAGE_ACCESS_CONTROL_RESPONSE_NO)); \
        break;                                                                                    \
      case MONITORING_ACCESS_CONTROL_GRANT:                                                       \
        pclNodeDsr->m_bHasControlAuthority = TRUE;                                                \
        OnMonitoringState##__INDEX__(pclNodeDsr->GetCDRFLex()->GetRobotState());                  \
        break;                                                                                    \
      case MONITORING_ACCESS_CONTROL_DENY:                                                        \
      case MONITORING_ACCESS_CONTROL_LOSS:                                                        \
        pclNodeDsr->m_bHasControlAuthority = FALSE;                                               \
        if (pclNodeDsr->m_TpInitailizingComplted) {                                               \
          assert(pclNodeDsr->GetCDRFLex()->ManageAccessControl(MANAGE_ACCESS_CONTROL_REQUEST));   \
        }                                                                                         \
        break;                                                                                    \
      default:                                                                                    \
        break;                                                                                    \
    }                                                                                             \
  }
DECLARE_MonitroingAccessControl(0);
DECLARE_MonitroingAccessControl(1);
DECLARE_MonitroingAccessControl(2);
DECLARE_MonitroingAccessControl(3);
DECLARE_MonitroingAccessControl(4);
DECLARE_MonitroingAccessControl(5);
DECLARE_MonitroingAccessControl(6);
DECLARE_MonitroingAccessControl(7);
DECLARE_MonitroingAccessControl(8);
DECLARE_MonitroingAccessControl(9);
TOnMonitoringAccessControlCB g_pfnMonitroingAccessControl[NODEDSR_INSTANCE_LIMIT] = {OnMonitroingAccessControl0, OnMonitroingAccessControl1, OnMonitroingAccessControl2, OnMonitroingAccessControl3, OnMonitroingAccessControl4, OnMonitroingAccessControl5, OnMonitroingAccessControl6, OnMonitroingAccessControl7, OnMonitroingAccessControl8, OnMonitroingAccessControl9};

///////////////////////////////////////////////////////////////////////////////
// NAPI ThreadSafe callbacks
///////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////
// TOnMonitoringStateCB callbacks
#define DECLARE_MonitoringStateCB(__INDEX__)                                                          \
  void OnMonitoringStateCB##__INDEX__(const ROBOT_STATE eState) {                                     \
    uint32_t nIndex = static_cast<uint32_t>(atoi(#__INDEX__));                                        \
    NodeDsr *pclNodeDsr = NodeDsrArray::Get(nIndex);                                                  \
    DBGPRINT("OnMonitoringStateCB, %d, %p\n", nIndex, pclNodeDsr);                                    \
    if ((pclNodeDsr != NULL) || (pclNodeDsr->m_cbOnMoitoringState != NULL)) {                         \
      uint32_t *inputValue = new uint32_t(static_cast<uint32_t>(eState));                             \
      pclNodeDsr->m_cbOnMoitoringStateTsfn.BlockingCall(inputValue, NodeDsr::SetOnMonitoringStateCB); \
    }                                                                                                 \
  }
DECLARE_MonitoringStateCB(0);
DECLARE_MonitoringStateCB(1);
DECLARE_MonitoringStateCB(2);
DECLARE_MonitoringStateCB(3);
DECLARE_MonitoringStateCB(4);
DECLARE_MonitoringStateCB(5);
DECLARE_MonitoringStateCB(6);
DECLARE_MonitoringStateCB(7);
DECLARE_MonitoringStateCB(8);
DECLARE_MonitoringStateCB(9);
TOnMonitoringStateCB g_pfnMonitoringStateCB[NODEDSR_INSTANCE_LIMIT] = {OnMonitoringStateCB0, OnMonitoringStateCB1, OnMonitoringStateCB2, OnMonitoringStateCB3, OnMonitoringStateCB4, OnMonitoringStateCB5, OnMonitoringStateCB6, OnMonitoringStateCB7, OnMonitoringStateCB8, OnMonitoringStateCB9};

///////////////////////////////////////////////////////////////////////////////
// TOnMonitoringAccessControlCB callbacks
#define DECLARE_MonitoringAccessControlCB(__INDEX__)                                          \
  void OnMonitoringAccessControlCB##__INDEX__(const MONITORING_ACCESS_CONTROL eState) {       \
    uint32_t nIndex = static_cast<uint32_t>(atoi(#__INDEX__));                                \
    NodeDsr *pclNodeDsr = NodeDsrArray::Get(nIndex);                                          \
    if ((pclNodeDsr != NULL) || (pclNodeDsr->m_cbOnMoitoringState != NULL)) {                 \
      Napi::Env env = pclNodeDsr->m_cbOnMoitoringState.Env();                                 \
      Napi::Number accessControlNapi = Napi::Number::New(env, static_cast<uint32_t>(eState)); \
      std::vector<napi_value> args = {accessControlNapi};                                     \
      pclNodeDsr->m_cbOnMoitoringState.Call(args);                                            \
    }                                                                                         \
  }
DECLARE_MonitoringAccessControlCB(0);
DECLARE_MonitoringAccessControlCB(1);
DECLARE_MonitoringAccessControlCB(2);
DECLARE_MonitoringAccessControlCB(3);
DECLARE_MonitoringAccessControlCB(4);
DECLARE_MonitoringAccessControlCB(5);
DECLARE_MonitoringAccessControlCB(6);
DECLARE_MonitoringAccessControlCB(7);
DECLARE_MonitoringAccessControlCB(8);
DECLARE_MonitoringAccessControlCB(9);
TOnMonitoringAccessControlCB g_pfnMonitoringAccessControlCB[NODEDSR_INSTANCE_LIMIT] = {OnMonitoringAccessControlCB0, OnMonitoringAccessControlCB1, OnMonitoringAccessControlCB2, OnMonitoringAccessControlCB3, OnMonitoringAccessControlCB4, OnMonitoringAccessControlCB5, OnMonitoringAccessControlCB6, OnMonitoringAccessControlCB7, OnMonitoringAccessControlCB8, OnMonitoringAccessControlCB9};


///////////////////////////////////////////////////////////////////////////////
// TOnDisconnectedCB callbacks
#define DECLARE_Disconnected(__INDEX__)                                      \
  void OnDisconnected##__INDEX__() {                                         \
    uint32_t nIndex = static_cast<uint32_t>(atoi(#__INDEX__));                          \
    NodeDsr *pclNodeDsr = NodeDsrArray::Get(nIndex);                                    \
    DBGPRINT("OnDisconnectedCB, %d, %p\n", nIndex, pclNodeDsr);                                    \
    if ((pclNodeDsr != NULL) || (pclNodeDsr->m_cbOnMoitoringState != NULL)) {                         \
      pclNodeDsr->m_cbOnDisconnectedTsfn.BlockingCall(NodeDsr::SetOnDisconnectedCB); \
    }    \
  }
DECLARE_Disconnected(0);
DECLARE_Disconnected(1);
DECLARE_Disconnected(2);
DECLARE_Disconnected(3);
DECLARE_Disconnected(4);
DECLARE_Disconnected(5);
DECLARE_Disconnected(6);
DECLARE_Disconnected(7);
DECLARE_Disconnected(8);
DECLARE_Disconnected(9);
TOnDisconnectedCB g_pfnDisconnected[NODEDSR_INSTANCE_LIMIT] = {OnDisconnected0, OnDisconnected1, OnDisconnected2, OnDisconnected3, OnDisconnected4, OnDisconnected5, OnDisconnected6, OnDisconnected7, OnDisconnected8, OnDisconnected9};
