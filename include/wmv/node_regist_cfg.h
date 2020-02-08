#ifndef EXE_FILEPATH
#error "You must define EXE_FILEPATH macro before include this file"
#endif

// generator

#define PARM_NODE_CLASS BasicNoise
#define PARM_NODE_NAME basic_noise
#include EXE_FILEPATH

#define PARM_NODE_CLASS Color
#define PARM_NODE_NAME color
#include EXE_FILEPATH

#define PARM_NODE_CLASS FaultFractal
#define PARM_NODE_NAME fault_fractal
#include EXE_FILEPATH

#ifndef SKIP_FILE_INPUT
#define PARM_NODE_CLASS FileInput
#define PARM_NODE_NAME file_input
#include EXE_FILEPATH
#endif // SKIP_FILE_INPUT

#define PARM_NODE_CLASS LayoutGen
#define PARM_NODE_NAME layout_gen
#include EXE_FILEPATH

#define PARM_NODE_CLASS PerlinNoise
#define PARM_NODE_NAME perlin_noise
#include EXE_FILEPATH

#define PARM_NODE_CLASS PlasmaFractal
#define PARM_NODE_NAME plasma_fractal
#include EXE_FILEPATH

// output

#define PARM_NODE_CLASS OverlayView
#define PARM_NODE_NAME overlay_view
#include EXE_FILEPATH

// combiner

#define PARM_NODE_CLASS Chooser
#define PARM_NODE_NAME chooser
#include EXE_FILEPATH

#define PARM_NODE_CLASS Combiner
#define PARM_NODE_NAME combiner
#include EXE_FILEPATH

// filter

#define PARM_NODE_CLASS Curves
#define PARM_NODE_NAME curves
#include EXE_FILEPATH

// natural

#define PARM_NODE_CLASS Erosion
#define PARM_NODE_NAME erosion
#include EXE_FILEPATH

// selector

#define PARM_NODE_CLASS SelectHeight
#define PARM_NODE_NAME select_height
#include EXE_FILEPATH

#define PARM_NODE_CLASS SelectSlope
#define PARM_NODE_NAME select_slope
#include EXE_FILEPATH

// converter

#define PARM_NODE_CLASS Colorizer
#define PARM_NODE_NAME colorizer
#include EXE_FILEPATH