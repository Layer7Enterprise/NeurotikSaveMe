#include "Helpers.h"

bool ShouldPotentiate(int type) {
  if (type == GLU && type != NO_LRN)
    return true;
  return false;
}
