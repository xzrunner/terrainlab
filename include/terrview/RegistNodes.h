#pragma once

#include "terrview/Node.h"

#include <terr/device/Combiner.h>
#include <terr/device/Curves.h>

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

#define PARM_FILEPATH "terr/device/Color.parm.h"
#define PARM_NODE_CLASS Color
#define PARM_NODE_CLASS_STR "Color"
#define PARM_NODE_NAME "color"
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

// output

#define PARM_FILEPATH "terr/device/OverlayView.parm.h"
#define PARM_NODE_CLASS OverlayView
#define PARM_NODE_CLASS_STR "OverlayView"
#define PARM_NODE_NAME "overlay_view"
#include "terrview/node_def_gen.h"
#undef PARM_NODE_NAME
#undef PARM_NODE_CLASS
#undef PARM_NODE_CLASS_STR
#undef PARM_FILEPATH
;

// combiner

#define PARM_FILEPATH "terr/device/Chooser.parm.h"
#define PARM_NODE_CLASS Chooser
#define PARM_NODE_CLASS_STR "Chooser"
#define PARM_NODE_NAME "chooser"
#include "terrview/node_def_gen.h"
#undef PARM_NODE_NAME
#undef PARM_NODE_CLASS
#undef PARM_NODE_CLASS_STR
#undef PARM_FILEPATH
;

#define Method terr::device::Combiner::Method
#define PARM_FILEPATH "terr/device/Combiner.parm.h"
#define PARM_NODE_CLASS Combiner
#define PARM_NODE_CLASS_STR "Combiner"
#define PARM_NODE_NAME "combiner"
#include "terrview/node_def_gen.h"
#undef PARM_NODE_NAME
#undef PARM_NODE_CLASS
#undef PARM_NODE_CLASS_STR
#undef PARM_FILEPATH
#undef Method
;

// filter

#define Type terr::device::Curves::Type
#define PARM_FILEPATH "terr/device/Curves.parm.h"
#define PARM_NODE_CLASS Curves
#define PARM_NODE_CLASS_STR "Curves"
#define PARM_NODE_NAME "curves"
#include "terrview/node_def_gen.h"
#undef PARM_NODE_NAME
#undef PARM_NODE_CLASS
#undef PARM_NODE_CLASS_STR
#undef PARM_FILEPATH
#undef Type
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

// selector

#define PARM_FILEPATH "terr/device/SelectHeight.parm.h"
#define PARM_NODE_CLASS SelectHeight
#define PARM_NODE_CLASS_STR "SelectHeight"
#define PARM_NODE_NAME "select_height"
#include "terrview/node_def_gen.h"
#undef PARM_NODE_NAME
#undef PARM_NODE_CLASS
#undef PARM_NODE_CLASS_STR
#undef PARM_FILEPATH
;

#define PARM_FILEPATH "terr/device/SelectSlope.parm.h"
#define PARM_NODE_CLASS SelectSlope
#define PARM_NODE_CLASS_STR "SelectSlope"
#define PARM_NODE_NAME "select_slope"
#include "terrview/node_def_gen.h"
#undef PARM_NODE_NAME
#undef PARM_NODE_CLASS
#undef PARM_NODE_CLASS_STR
#undef PARM_FILEPATH
;

// converter

#define PARM_FILEPATH "terr/device/Colorizer.parm.h"
#define PARM_NODE_CLASS Colorizer
#define PARM_NODE_CLASS_STR "Colorizer"
#define PARM_NODE_NAME "colorizer"
#include "terrview/node_def_gen.h"
#undef PARM_NODE_NAME
#undef PARM_NODE_CLASS
#undef PARM_NODE_CLASS_STR
#undef PARM_FILEPATH
;

}

}