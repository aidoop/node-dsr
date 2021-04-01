#ifndef __NODEDSRCALLBACKS_H_
#define __NODEDSRCALLBACKS_H_

#include "DRFLEx.h"
#include "NodeDsr.h"
#include "NodeDsrArray.h"

///////////////////////////////////////////////////////////////////////////////
// TOnMonitoringStateCB callbacks
#define DECLARE_MonitoringStateCB(__INDEX__)                                                                \
    void OnMonitoringStateCB##__INDEX__(const ROBOT_STATE eState)                                           \
    {                                                                                                       \
        uint32_t nIndex = static_cast<uint32_t>(atoi(#__INDEX__));                                          \
        NodeDsr *pclNodeDsr = NodeDsrArray::Get(nIndex);                                                    \
        DBGPRINT("OnMonitoringStateCB, %d, %p\n", nIndex, pclNodeDsr);                                      \
        if ((pclNodeDsr != NULL) || (pclNodeDsr->m_cbOnMoitoringState != NULL))                             \
        {                                                                                                   \
            uint32_t *inputValue = new uint32_t(static_cast<uint32_t>(eState));                             \
            pclNodeDsr->m_cbOnMoitoringStateTsfn.BlockingCall(inputValue, NodeDsr::SetOnMonitoringStateCB); \
        }                                                                                                   \
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
TOnMonitoringStateCB g_pfnMonitoringStateCB[NODEDSR_NUMBER_LIMIT] = {OnMonitoringStateCB0, OnMonitoringStateCB1, OnMonitoringStateCB2, OnMonitoringStateCB3, OnMonitoringStateCB4, OnMonitoringStateCB5, OnMonitoringStateCB6, OnMonitoringStateCB7, OnMonitoringStateCB8, OnMonitoringStateCB9};

///////////////////////////////////////////////////////////////////////////////
// TOnMonitoringAccessControlCB callbacks
#define DECLARE_MonitoringAccessControlCB(__INDEX__)                                                \
    void OnMonitoringAccessControlCB##__INDEX__(const MONITORING_ACCESS_CONTROL eState)             \
    {                                                                                               \
        uint32_t nIndex = static_cast<uint32_t>(atoi(#__INDEX__));                                  \
        NodeDsr *pclNodeDsr = NodeDsrArray::Get(nIndex);                                            \
        if ((pclNodeDsr != NULL) || (pclNodeDsr->m_cbOnMoitoringState != NULL))                     \
        {                                                                                           \
            Napi::Env env = pclNodeDsr->m_cbOnMoitoringState.Env();                                 \
            Napi::Number accessControlNapi = Napi::Number::New(env, static_cast<uint32_t>(eState)); \
            std::vector<napi_value> args = {accessControlNapi};                                     \
            pclNodeDsr->m_cbOnMoitoringState.Call(args);                                            \
        }                                                                                           \
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
TOnMonitoringAccessControlCB g_pfnMonitoringAccessControlCB[NODEDSR_NUMBER_LIMIT] = {OnMonitoringAccessControlCB0, OnMonitoringAccessControlCB1, OnMonitoringAccessControlCB2, OnMonitoringAccessControlCB3, OnMonitoringAccessControlCB4, OnMonitoringAccessControlCB5, OnMonitoringAccessControlCB6, OnMonitoringAccessControlCB7, OnMonitoringAccessControlCB8, OnMonitoringAccessControlCB9};

#endif // __NODEDSRCALLBACKS_H_