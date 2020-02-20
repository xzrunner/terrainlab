#pragma once

#include "wmv/Node.h"

#include <wm/NoiseQuality.h>
#include <wm/device/TurbulenceNoise.h>
#include <wm/device/Combiner.h>
#include <wm/device/Curves.h>

namespace wmv
{

void nodes_regist_rttr();

namespace node
{

#define EXE_FILEPATH "wmv/node_def_gen.h"
#include "wmv/node_regist_cfg.h"
#undef EXE_FILEPATH

}

}