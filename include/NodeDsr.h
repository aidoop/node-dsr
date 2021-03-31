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
    Napi::Value MoveJ(const Napi::CallbackInfo &info);
    Napi::Value MoveJA(const Napi::CallbackInfo &info);
    Napi::Value MoveL(const Napi::CallbackInfo &info);

public:
    Napi::FunctionReference jsfuncPrivate;

private:
    CDRFLEx *m_pDrfl;
};

#endif // __NODEDSAR_H_