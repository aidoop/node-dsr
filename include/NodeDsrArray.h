#pragma once

#include <array>

#include "NodeDsrConfig.h"

class NodeDsr;

class NodeDsrArray {
 public:
  static int32_t Set(NodeDsr *pclNodeDsr) {
    // find empty array buffer
    uint32_t nIndex;
    for (nIndex = 0; nIndex < NODEDSR_INSTANCE_LIMIT; nIndex++) {
      if (NodeDsrArray::sm_NodeDsrArray[nIndex] == NULL) {
        break;
      }
    }

    if (nIndex >= NODEDSR_INSTANCE_LIMIT)
      return -1;

    NodeDsrArray::sm_NodeDsrArray[nIndex] = pclNodeDsr;
    return nIndex;
  }

  static void Reset(uint32_t nIndex) {
    NodeDsrArray::sm_NodeDsrArray[nIndex] = NULL;
  }

  static NodeDsr *Get(uint32_t nIndex) {
    return NodeDsrArray::sm_NodeDsrArray.at(nIndex);
  }

 public:
  static std::array<NodeDsr *, NODEDSR_INSTANCE_LIMIT> sm_NodeDsrArray;
};

std::array<NodeDsr *, NODEDSR_INSTANCE_LIMIT> NodeDsrArray::sm_NodeDsrArray;
