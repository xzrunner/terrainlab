#pragma once

#include "terrainlab/Node.h"

#include <terraingraph/NoiseQuality.h>
#include <terraingraph/device/TurbulenceNoise.h>
#include <terraingraph/device/Combiner.h>
#include <terraingraph/device/Curves.h>

namespace terrainlab
{

void nodes_regist_rttr();

namespace node
{

#define EXE_FILEPATH "terrainlab/node_def_gen.h"
#include "terrainlab/node_regist_cfg.h"
#undef EXE_FILEPATH

}

}