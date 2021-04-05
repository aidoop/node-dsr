#include "NodeDsr.h"

Napi::Object InitAll(Napi::Env env, Napi::Object exports) {
  return NodeDsr::Init(env, exports);
}

NODE_API_MODULE(nodedsr, InitAll)