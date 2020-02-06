#pragma once

#include "terrview/Node.h"

#include <terr/device/Combiner.h>
#include <terr/device/Curves.h>

namespace terrv
{

void nodes_regist_rttr();

namespace node
{

#define EXE_FILEPATH "terrview/node_def_gen.h"
#include "terrview/node_regist_cfg.h"
#undef EXE_FILEPATH

}

}