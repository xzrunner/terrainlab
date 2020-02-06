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

#define EXE_FILEPATH "terrview/node_rttr_gen.h"
#define SKIP_FILE_INPUT
#include "terrview/node_regist_cfg.h"
#undef SKIP_FILE_INPUT
#undef EXE_FILEPATH

rttr::registration::class_<terrv::node::FileInput>("terrv::file_input")
    .constructor<>()
    .property("filepath", &terrv::node::FileInput::m_filepath)
    (
	    rttr::metadata(ee0::UIMetaInfoTag(), ee0::UIMetaInfo("Filepath")),
        rttr::metadata(js::RTTR::FilePathTag(), true),
        rttr::metadata(ee0::PropOpenFileTag(), ee0::PropOpenFile("*.*"))
    )
;

}

namespace terrv
{

void nodes_regist_rttr()
{
}

}