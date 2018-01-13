/**
 * Copyright (c) 2016-present, Facebook, Inc.
 * All rights reserved.
 *
 * This source code is licensed under the BSD-style license found in the
 * LICENSE file in the root directory of this source tree. An additional grant
 * of patent rights can be found in the PATENTS file in the same directory.
 */

#pragma once

#include <fstream>

#if defined(__clang__) || defined(__GNUC__)
# define HORNVECS_DEPRECATED(msg) __attribute__((__deprecated__(msg)))
#elif defined(_MSC_VER)
# define HORNVECS_DEPRECATED(msg) __declspec(deprecated(msg))
#else
# define HORNVECS_DEPRECATED(msg)
#endif

namespace hornvecs {

namespace utils {

  int64_t size(std::ifstream&);
  void seek(std::ifstream&, int64_t);
}

}
