#pragma once

#include <napi.h>

#include "NodeDsrConfig.h"

class NapiHelper {
 public:
  static bool covertToFloatArray(Napi::Array inputs, uint32_t nWantedSize, float *fArray) {
    uint32_t nInputArrayLen = inputs.Length();
    if (nInputArrayLen != nWantedSize) {
      return false;
    }

    float fTargetPos[nInputArrayLen] = {
        0.0,
    };
    for (uint32_t nIter = 0; nIter < nInputArrayLen; nIter++) {
      fArray[nIter] = inputs.Get(nIter).As<Napi::Number>().FloatValue();
      DBGPRINT("Conveted [nIter]: %f\n", fArray[nIter]);
    }
    return true;
  }
};
