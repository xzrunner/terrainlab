#pragma once

#include "terrview/Node.h"

namespace terrv
{

void nodes_regist_rttr();

namespace node
{

// generator

#define PARM_FILEPATH "terr/device/BasicNoise.parm.h"
#define PARM_NODE_CLASS BasicNoise
#define PARM_NODE_CLASS_STR "BasicNoise"
#define PARM_NODE_NAME "basic_noise"
#include "terrview/node_def_gen.h"
#undef PARM_NODE_NAME
#undef PARM_NODE_CLASS
#undef PARM_NODE_CLASS_STR
#undef PARM_FILEPATH
;

#define PARM_FILEPATH "terr/device/FaultFractal.parm.h"
#define PARM_NODE_CLASS FaultFractal
#define PARM_NODE_CLASS_STR "FaultFractal"
#define PARM_NODE_NAME "fault_fractal"
#include "terrview/node_def_gen.h"
#undef PARM_NODE_NAME
#undef PARM_NODE_CLASS
#undef PARM_NODE_CLASS_STR
#undef PARM_FILEPATH
;

#define PARM_FILEPATH "terr/device/FileInput.parm.h"
#define PARM_NODE_CLASS FileInput
#define PARM_NODE_CLASS_STR "FileInput"
#define PARM_NODE_NAME "file_input"
#include "terrview/node_def_gen.h"
#undef PARM_NODE_NAME
#undef PARM_NODE_CLASS
#undef PARM_NODE_CLASS_STR
#undef PARM_FILEPATH
;

#define PARM_FILEPATH "terr/device/PerlinNoise.parm.h"
#define PARM_NODE_CLASS PerlinNoise
#define PARM_NODE_CLASS_STR "PerlinNoise"
#define PARM_NODE_NAME "perlin_noise"
#include "terrview/node_def_gen.h"
#undef PARM_NODE_NAME
#undef PARM_NODE_CLASS
#undef PARM_NODE_CLASS_STR
#undef PARM_FILEPATH
;

#define PARM_FILEPATH "terr/device/PlasmaFractal.parm.h"
#define PARM_NODE_CLASS PlasmaFractal
#define PARM_NODE_CLASS_STR "PlasmaFractal"
#define PARM_NODE_NAME "plasma_fractal"
#include "terrview/node_def_gen.h"
#undef PARM_NODE_NAME
#undef PARM_NODE_CLASS
#undef PARM_NODE_CLASS_STR
#undef PARM_FILEPATH
;

// natural

#define PARM_FILEPATH "terr/device/Erosion.parm.h"
#define PARM_NODE_CLASS Erosion
#define PARM_NODE_CLASS_STR "Erosion"
#define PARM_NODE_NAME "erosion"
#include "terrview/node_def_gen.h"
#undef PARM_NODE_NAME
#undef PARM_NODE_CLASS
#undef PARM_NODE_CLASS_STR
#undef PARM_FILEPATH
;


}

}