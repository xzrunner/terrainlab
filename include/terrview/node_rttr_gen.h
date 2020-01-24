#ifndef PARM_FILEPATH
#error "You must define PARM_FILEPATH macro before include this file"
#endif

#ifndef PARM_NODE_CLASS
#error "You must define PARM_NODE_CLASS macro before include this file"
#endif

#define PARAM_INFO(id, type, name, member, default_val)        \
.property(#name, &terrv::node::##PARM_NODE_CLASS::member)      \
(                                                              \
	rttr::metadata(ee0::UIMetaInfoTag(), ee0::UIMetaInfo(#id)) \
)
#include PARM_FILEPATH
#undef  PARAM_INFO