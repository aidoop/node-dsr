#ifndef __NODEDSAR_H_
#define __NODEDSAR_H_

#include <napi.h>
#include "DRFLEx.h"
using namespace DRAFramework;

#include "NodeDsrConfig.h"

class NodeDsr : public Napi::ObjectWrap<NodeDsr>
{
public:
    static Napi::Object Init(Napi::Env env, Napi::Object exports);
    NodeDsr(const Napi::CallbackInfo &info);

private:
    Napi::Value Test(const Napi::CallbackInfo &info);
    Napi::Value OpenConnection(const Napi::CallbackInfo &info);
    Napi::Value CloseConnection(const Napi::CallbackInfo &info);
    Napi::Value GetLibraryVersion(const Napi::CallbackInfo &info);
    void SetOnMonitoringState(const Napi::CallbackInfo &info);
    void SetOnMonitoringAccessControl(const Napi::CallbackInfo &info);
    Napi::Value MoveJ(const Napi::CallbackInfo &info);
    Napi::Value MoveJA(const Napi::CallbackInfo &info);
    Napi::Value MoveL(const Napi::CallbackInfo &info);

public:
    static void SetOnMonitoringStateCB(Napi::Env env, Napi::Function jsCallback, uint32_t *peState);

public:
    uint32_t GetIndex();
    CDRFLEx *GetCDRFLex() { return m_pDrfl; }

public:
    Napi::FunctionReference m_cbOnMoitoringState;
    Napi::ThreadSafeFunction m_cbOnMoitoringStateTsfn;
    Napi::FunctionReference m_cbOnMonitoringAccessControl;

public:
    bool m_TpInitailizingComplted;
    bool m_bHasControlAuthority;

private:
    CDRFLEx *m_pDrfl;
    uint32_t m_nIndex;
    string m_strUrl;
    uint32_t m_nPort;
};

#endif // __NODEDSAR_H_