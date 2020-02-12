#include "wmv/RegistNodes.h"

#include <ee0/ReflectPropTypes.h>

#include <js/RTTR.h>

RTTR_REGISTRATION
{

// base

rttr::registration::class_<wmv::Node>("wmv::node")
.property("name", &wmv::Node::GetName, &wmv::Node::SetName)
(
	rttr::metadata(ee0::UIMetaInfoTag(), ee0::UIMetaInfo("Name"))
)
.property("display", &wmv::Node::GetDisplay, &wmv::Node::SetDisplay)
(
	rttr::metadata(ee0::UIMetaInfoTag(), ee0::UIMetaInfo(wmv::Node::STR_PROP_DISPLAY))
)
;

#define EXE_FILEPATH "wmv/node_rttr_gen.h"
#define SKIP_FILE_NODE
#include "wmv/node_regist_cfg.h"
#undef SKIP_FILE_NODE
#undef EXE_FILEPATH

rttr::registration::class_<wmv::node::FileInput>("wmv::file_input")
    .constructor<>()
    .property("filepath", &wmv::node::FileInput::m_filepath)
    (
	    rttr::metadata(ee0::UIMetaInfoTag(), ee0::UIMetaInfo("Filepath")),
        rttr::metadata(js::RTTR::FilePathTag(), true),
        rttr::metadata(ee0::PropOpenFileTag(), ee0::PropOpenFile("*.*"))
    )
;

rttr::registration::class_<wmv::node::HeightOutput>("wmv::height_output")
    .constructor<>()
    .property("filepath", &wmv::node::HeightOutput::m_filepath)
    (
	    rttr::metadata(ee0::UIMetaInfoTag(), ee0::UIMetaInfo("Filepath")),
        rttr::metadata(js::RTTR::FilePathTag(), true),
        rttr::metadata(ee0::PropOpenFileTag(), ee0::PropOpenFile("*.*"))
    )
;

}

namespace wmv
{

void nodes_regist_rttr()
{
}

}