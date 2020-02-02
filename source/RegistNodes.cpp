#include "terrview/RegistNodes.h"

#include <ee0/ReflectPropTypes.h>

#include <js/RTTR.h>

RTTR_REGISTRATION
{

// base

rttr::registration::class_<terrv::Node>("terrv::node")
.property("name", &terrv::Node::GetName, &terrv::Node::SetName)
(
	rttr::metadata(ee0::UIMetaInfoTag(), ee0::UIMetaInfo("Name"))
)
.property("display", &terrv::Node::GetDisplay, &terrv::Node::SetDisplay)
(
	rttr::metadata(ee0::UIMetaInfoTag(), ee0::UIMetaInfo(terrv::Node::STR_PROP_DISPLAY))
)
;

// generator

rttr::registration::class_<terrv::node::BasicNoise>("terrv::basic_noise")
.constructor<>()
#define PARM_FILEPATH "terr/device/BasicNoise.parm.h"
#define PARM_NODE_CLASS BasicNoise
#include "terrview/node_rttr_gen.h"
#undef PARM_NODE_CLASS
#undef PARM_FILEPATH
;

rttr::registration::class_<terrv::node::FaultFractal>("terrv::fault_fractal")
.constructor<>()
#define PARM_FILEPATH "terr/device/FaultFractal.parm.h"
#define PARM_NODE_CLASS FaultFractal
#include "terrview/node_rttr_gen.h"
#undef PARM_NODE_CLASS
#undef PARM_FILEPATH
;

//rttr::registration::class_<terrv::node::FileInput>("terrv::file_input")
//.constructor<>()
//#define PARM_FILEPATH "terr/device/FileInput.parm.h"
//#define PARM_NODE_CLASS FileInput
//#include "terrview/node_rttr_gen.h"
//#undef PARM_NODE_CLASS
//#undef PARM_FILEPATH
//;
rttr::registration::class_<terrv::node::FileInput>("terrv::file_input")
    .constructor<>()
    .property("filepath", &terrv::node::FileInput::m_filepath)
    (
	    rttr::metadata(ee0::UIMetaInfoTag(), ee0::UIMetaInfo("Filepath")),
        rttr::metadata(js::RTTR::FilePathTag(), true),
        rttr::metadata(ee0::PropOpenFileTag(), ee0::PropOpenFile("*.*"))
    )
;

rttr::registration::class_<terrv::node::PerlinNoise>("terrv::perlin_noise")
.constructor<>()
#define PARM_FILEPATH "terr/device/PerlinNoise.parm.h"
#define PARM_NODE_CLASS PerlinNoise
#include "terrview/node_rttr_gen.h"
#undef PARM_NODE_CLASS
#undef PARM_FILEPATH
;

rttr::registration::class_<terrv::node::PlasmaFractal>("terrv::plasma_fractal")
.constructor<>()
#define PARM_FILEPATH "terr/device/PlasmaFractal.parm.h"
#define PARM_NODE_CLASS PlasmaFractal
#include "terrview/node_rttr_gen.h"
#undef PARM_NODE_CLASS
#undef PARM_FILEPATH
;

// output

rttr::registration::class_<terrv::node::OverlayView>("terrv::overlay_view")
.constructor<>()
#define PARM_FILEPATH "terr/device/OverlayView.parm.h"
#define PARM_NODE_CLASS OverlayView
#include "terrview/node_rttr_gen.h"
#undef PARM_NODE_CLASS
#undef PARM_FILEPATH
;

// combiner

rttr::registration::class_<terrv::node::Combiner>("terrv::combiner")
.constructor<>()
#define PARM_FILEPATH "terr/device/Combiner.parm.h"
#define PARM_NODE_CLASS Combiner
#include "terrview/node_rttr_gen.h"
#undef PARM_NODE_CLASS
#undef PARM_FILEPATH
;

// filter

rttr::registration::class_<terrv::node::Curves>("terrv::curves")
.constructor<>()
#define PARM_FILEPATH "terr/device/Curves.parm.h"
#define PARM_NODE_CLASS Curves
#include "terrview/node_rttr_gen.h"
#undef PARM_NODE_CLASS
#undef PARM_FILEPATH
;

// natural

rttr::registration::class_<terrv::node::Erosion>("terrv::erosion")
.constructor<>()
#define PARM_FILEPATH "terr/device/Erosion.parm.h"
#define PARM_NODE_CLASS Erosion
#include "terrview/node_rttr_gen.h"
#undef PARM_NODE_CLASS
#undef PARM_FILEPATH
;

// converter

rttr::registration::class_<terrv::node::Colorizer>("terrv::colorizer")
.constructor<>()
#define PARM_FILEPATH "terr/device/Colorizer.parm.h"
#define PARM_NODE_CLASS Colorizer
#include "terrview/node_rttr_gen.h"
#undef PARM_NODE_CLASS
#undef PARM_FILEPATH
;

}

namespace terrv
{

void nodes_regist_rttr()
{
}

}