
#pragma once

#if (defined (_WIN32) || defined (_WIN64))
#pragma warning( push )
#pragma warning( disable : 4068)
#endif

#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wundef"
#pragma clang diagnostic ignored "-Wcast-qual"
#pragma clang diagnostic ignored "-Wold-style-cast"

#include "JuceHeader.h"

#pragma clang diagnostic pop

#if (defined (_WIN32) || defined (_WIN64))
#pragma warning( pop )
#endif
