#include <iostream>
#include <assert.h>
#include <unistd.h>

#include "NodeDsr.h"
#include "NapiHelper.h"

Napi::Object NodeDsr::Init(Napi::Env env, Napi::Object exports)
{
    Napi::Function func =
        DefineClass(env,
                    "NodeDsr",
                    {
                        InstanceMethod("test", &NodeDsr::Test),
                        InstanceMethod("openConnection", &NodeDsr::OpenConnection),
                        InstanceMethod("closeConnection", &NodeDsr::CloseConnection),
                        InstanceMethod("getLibraryVersion", &NodeDsr::GetLibraryVersion),
                        InstanceMethod("setOnMonitoringState", &NodeDsr::SetOnMonitoringState),
                        InstanceMethod("movej", &NodeDsr::MoveJ),
                        InstanceMethod("moveja", &NodeDsr::MoveJA),
                        InstanceMethod("movel", &NodeDsr::MoveL),

                    });

    Napi::FunctionReference *constructor = new Napi::FunctionReference();
    *constructor = Napi::Persistent(func);
    env.SetInstanceData(constructor);

    exports.Set("NodeDsr", func);
    return exports;
}

NodeDsr::NodeDsr(const Napi::CallbackInfo &info)
    : Napi::ObjectWrap<NodeDsr>(info), m_pDrfl(NULL)
{
    Napi::Env env = info.Env();

    m_pDrfl = new CDRFLEx();
    if (!m_pDrfl)
    {
        Napi::TypeError::New(env, "failed to createCDRFLEx").ThrowAsJavaScriptException();
    }
}

Napi::Value NodeDsr::OpenConnection(const Napi::CallbackInfo &info)
{
    DBGPRINT("called OpenConnection\n");

    Napi::Env env = info.Env();

    DBGPRINT("info.Length: %u\n", info.Length());

    if (info.Length() < 2)
    {
        Napi::TypeError::New(env, "wrong number of arguments").ThrowAsJavaScriptException();
        return Napi::Boolean::New(env, false);
    }

    string arg0 = info[0].As<Napi::String>().Utf8Value();
    unsigned int arg1 = info[1].As<Napi::Number>().Uint32Value();
    DBGPRINT("arg0: %s\n", arg0.c_str());
    DBGPRINT("arg1: %d\n", arg1);

    bool bConnected = m_pDrfl->open_connection(arg0, arg1);
    DBGPRINT("Result: %d\n", bConnected);

    return Napi::Boolean::New(env, bConnected);
}

Napi::Value NodeDsr::CloseConnection(const Napi::CallbackInfo &info)
{
    DBGPRINT("called CloseConnection\n");

    Napi::Env env = info.Env();

    m_pDrfl->close_connection();

    return Napi::Boolean::New(env, true);
}

Napi::Value NodeDsr::Test(const Napi::CallbackInfo &info)
{
    DBGPRINT("called Test\n");

    Napi::Env env = info.Env();

    if (jsfuncPrivate != NULL)
    {
        Napi::Env env = jsfuncPrivate.Env();
        Napi::Number robotStateNapi = Napi::Number::New(env, static_cast<uint32_t>(3));
        std::vector<napi_value> args = {robotStateNapi};
        jsfuncPrivate.Call(args);
    }

    return Napi::Boolean::New(env, true);
}

Napi::Value NodeDsr::GetLibraryVersion(const Napi::CallbackInfo &info)
{
    DBGPRINT("called GetLibraryVersion\n");

    Napi::Env env = info.Env();

    const char *libVersion = m_pDrfl->get_library_version();

    return Napi::String::New(env, libVersion);
}

// Function Prototype
// void set_on_monitoring_state(TOnMonitoringStateCB pCallbackFunc)
//static Napi::FunctionReference r_log;
NodeDsr *gpThis = NULL;
extern "C" void emitLogInJS(ROBOT_STATE robotState)
{
    DBGPRINT("called emitLogInJS\n");

    if (gpThis->jsfuncPrivate != NULL)
    {
        Napi::Env env = gpThis->jsfuncPrivate.Env();
        Napi::Number robotStateNapi = Napi::Number::New(env, static_cast<uint32_t>(3));
        std::vector<napi_value> args = {robotStateNapi};
        gpThis->jsfuncPrivate.Call(args);
    }
}
void NodeDsr::SetOnMonitoringState(const Napi::CallbackInfo &info)
{
    DBGPRINT("called SetOnMonitoringState\n");

    DBGPRINT("param length: %d\n", info.Length());
    DBGPRINT("Napi::Persistent(info[0].As<Napi::Function>()): %p\n", Napi::Persistent(info[0].As<Napi::Function>()));

    jsfuncPrivate = Napi::Persistent(info[0].As<Napi::Function>());

    //r_log = Napi::Persistent(info[0].As<Napi::Function>());

    m_pDrfl->set_on_monitoring_state(emitLogInJS);
    gpThis = this;
}

// Function Prototype
// bool movej(float fTargetPos[NUM_JOINT], float fTargetVel, float fTargetAcc, float fTargetTime = 0.f,
// MOVE_MODE eMoveMode = MOVE_MODE_ABSOLUTE, float fBlendingRadius = 0.f, BLENDING_SPEED_TYPE eBlendingType = BLENDING_SPEED_TYPE_DUPLICATE)
Napi::Value NodeDsr::MoveJ(const Napi::CallbackInfo &info)
{
    DBGPRINT("called MoveJ\n");

    Napi::Env env = info.Env();

    ///////////////////////////////////////////////////////////////////
    // input parameter: fTartPos[NUM_JOINT]
    // Ref: https://github.com/nodejs/node-addon-examples/blob/4c3b7816662e3a4ab26d41f007bbd1784205bda6/array_buffer_to_native/node-addon-api/array_buffer_to_native.cc
    Napi::Array inputs = info[0].As<Napi::Array>();
    uint32_t nInputArrayLen = inputs.Length();
    printf("nInputArrayLen: %d\n", nInputArrayLen);
    if (nInputArrayLen != NUM_JOINT)
    {
        Napi::TypeError::New(env, "input pos array length wrong").ThrowAsJavaScriptException();
        return Napi::Boolean::New(env, false);
    }
    float fTargetPos[6] = {
        0.0,
    };
    for (uint32_t nIter = 0; nIter < nInputArrayLen; nIter++)
    {
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
    // input parameter: bAbsoluteMoveparam
    bool bAbsoluteMove = info[4].As<Napi::Boolean>().Value();
    DBGPRINT("bAbsoluteMove: %d\n", bAbsoluteMove);

    ///////////////////////////////////////////////////////////////////
    // input parameter: fBlendingRadius
    float fBlendingRadius = info[5].As<Napi::Number>().FloatValue();
    DBGPRINT("fBlendingRadius: %f\n", fBlendingRadius);

    ///////////////////////////////////////////////////////////////////
    // input parameter: bBlendOverride
    bool bBlendOverride = info[6].As<Napi::Boolean>().Value();
    DBGPRINT("bBlendOverride: %d\n", bBlendOverride);

    bool bFuncRet = m_pDrfl->movej(fTargetPos, fTargetVel, fTargetAcc, fTargetTime, static_cast<MOVE_MODE>(bAbsoluteMove), fBlendingRadius, static_cast<BLENDING_SPEED_TYPE>(bBlendOverride));

    return Napi::Boolean::New(env, bFuncRet);
}

// Function Prototype
// bool amovej(float fTargetPos[NUM_JOINT], float fTargetVel, float fTargetAcc, float fTargetTime = 0.f,
//            MOVE_MODE eMoveMode = MOVE_MODE_ABSOLUTE, BLENDING_SPEED_TYPE eBlendingType = BLENDING_SPEED_TYPE_DUPLICATE)
Napi::Value NodeDsr::MoveJA(const Napi::CallbackInfo &info)
{
    DBGPRINT("called MoveJA\n");

    Napi::Env env = info.Env();

    ///////////////////////////////////////////////////////////////////
    // input parameter: fTartPos[NUM_JOINT]
    // Ref: https://github.com/nodejs/node-addon-examples/blob/4c3b7816662e3a4ab26d41f007bbd1784205bda6/array_buffer_to_native/node-addon-api/array_buffer_to_native.cc
    Napi::Array inputs = info[0].As<Napi::Array>();
    uint32_t nInputArrayLen = inputs.Length();
    printf("nInputArrayLen: %d\n", nInputArrayLen);
    if (nInputArrayLen != NUM_JOINT)
    {
        Napi::TypeError::New(env, "input pos array length wrong").ThrowAsJavaScriptException();
        return Napi::Boolean::New(env, false);
    }
    float fTargetPos[6] = {
        0.0,
    };
    for (uint32_t nIter = 0; nIter < nInputArrayLen; nIter++)
    {
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
    // input parameter: bAbsoluteMove
    bool bAbsoluteMove = info[4].As<Napi::Boolean>().Value();
    DBGPRINT("bAbsoluteMove: %d\n", bAbsoluteMove);

    ///////////////////////////////////////////////////////////////////
    // input parameter: bBlendOverride
    bool bBlendOverride = info[5].As<Napi::Boolean>().Value();
    DBGPRINT("bBlendOverride: %d\n", bBlendOverride);

    bool bFuncRet = m_pDrfl->amovej(fTargetPos, fTargetVel, fTargetAcc, fTargetTime, static_cast<MOVE_MODE>(bAbsoluteMove), static_cast<BLENDING_SPEED_TYPE>(bBlendOverride));

    return Napi::Boolean::New(env, bFuncRet);
}

///////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////
// Function Prototype
// bool movel(float fTargetPos[NUM_TASK], float fTargetVel[2], float fTargetAcc[2], float fTargetTime = 0.f,
//            MOVE_MODE eMoveMode = MOVE_MODE_ABSOLUTE, MOVE_REFERENCE eMoveReference = MOVE_REFERENCE_BASE,
//            float fBlendingRadius = 0.f, BLENDING_SPEED_TYPE eBlendingType = BLENDING_SPEED_TYPE_DUPLICATE)
Napi::Value NodeDsr::MoveL(const Napi::CallbackInfo &info)
{
    DBGPRINT("called MoveL\n");

    Napi::Env env = info.Env();

    ///////////////////////////////////////////////////////////////////
    // input parameter: fTartPos[NUM_TASK]
    // Ref: https://github.com/nodejs/node-addon-examples/blob/4c3b7816662e3a4ab26d41f007bbd1784205bda6/array_buffer_to_native/node-addon-api/array_buffer_to_native.cc
    Napi::Array targetPoses = info[0].As<Napi::Array>();
    float fTargetPos[NUM_TASK];
    if (NapiHelper::covertToFloatArray(targetPoses, NUM_TASK, fTargetPos) == false)
    {
        Napi::TypeError::New(env, "pos array length wrong").ThrowAsJavaScriptException();
        return Napi::Boolean::New(env, false);
    }

    ///////////////////////////////////////////////////////////////////
    // input parameter: fTargetVel[2]
    Napi::Array targetVels = info[1].As<Napi::Array>();
    float fTargetVel[2];
    if (NapiHelper::covertToFloatArray(targetVels, 2, fTargetVel) == false)
    {
        Napi::TypeError::New(env, "vel array length wrong").ThrowAsJavaScriptException();
        return Napi::Boolean::New(env, false);
    }

    ///////////////////////////////////////////////////////////////////
    // input parameter: fTargetAcc[2]
    Napi::Array targetAccs = info[2].As<Napi::Array>();
    float fTargetAcc[2];
    if (NapiHelper::covertToFloatArray(targetAccs, 2, fTargetAcc) == false)
    {
        Napi::TypeError::New(env, "acc array length wrong").ThrowAsJavaScriptException();
        return Napi::Boolean::New(env, false);
    }

    ///////////////////////////////////////////////////////////////////
    // input parameter: fTargetTime
    float fTargetTime = info[3].As<Napi::Number>().FloatValue();
    DBGPRINT("fTargetTime: %f\n", fTargetTime);

    ///////////////////////////////////////////////////////////////////
    // input parameter: bAbsoluteMove
    bool bAbsoluteMove = info[4].As<Napi::Boolean>().Value();
    DBGPRINT("bAbsoluteMove: %d\n", bAbsoluteMove);

    ///////////////////////////////////////////////////////////////////
    // input parameter: eMoveReference
    MOVE_REFERENCE eMoveReference = static_cast<MOVE_REFERENCE>(info[5].As<Napi::Number>().Uint32Value());
    DBGPRINT("eMoveReference: %d\n", eMoveReference);

    ///////////////////////////////////////////////////////////////////
    // input parameter: fBlendingRadius
    float fBlendingRadius = info[6].As<Napi::Number>().FloatValue();
    DBGPRINT("fBlendingRadius: %f\n", fBlendingRadius);

    ///////////////////////////////////////////////////////////////////
    // input parameter: bBlendOverride
    bool bBlendOverride = info[7].As<Napi::Boolean>().Value();
    DBGPRINT("bBlendOverride: %d\n", bBlendOverride);

    bool bFuncRet = m_pDrfl->movel(fTargetPos, fTargetVel, fTargetAcc, fTargetTime, static_cast<MOVE_MODE>(bAbsoluteMove), eMoveReference, fBlendingRadius, static_cast<BLENDING_SPEED_TYPE>(bBlendOverride));

    return Napi::Boolean::New(env, bFuncRet);
}