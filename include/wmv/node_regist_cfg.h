#ifndef EXE_FILEPATH
#error "You must define EXE_FILEPATH macro before include this file"
#endif

// generator

#define PARM_NODE_CLASS Color
#define PARM_NODE_NAME color
#include EXE_FILEPATH

#define PARM_NODE_CLASS Constant
#define PARM_NODE_NAME constant
#include EXE_FILEPATH

#define PARM_NODE_CLASS Gradient
#define PARM_NODE_NAME gradient
#include EXE_FILEPATH

#ifndef SKIP_FILE_NODE
#define PARM_NODE_CLASS FileInput
#define PARM_NODE_NAME file_input
#include EXE_FILEPATH
#endif // SKIP_FILE_NODE

#define PARM_NODE_CLASS LayoutGen
#define PARM_NODE_NAME layout_gen
#include EXE_FILEPATH

#define PARM_NODE_CLASS FaultFractal
#define PARM_NODE_NAME fault_fractal
#include EXE_FILEPATH

#define PARM_NODE_CLASS PlasmaFractal
#define PARM_NODE_NAME plasma_fractal
#include EXE_FILEPATH

#define PARM_NODE_CLASS AdvanceNoise
#define PARM_NODE_NAME advance_noise
#include EXE_FILEPATH

#define PARM_NODE_CLASS BasicNoise
#define PARM_NODE_NAME basic_noise
#include EXE_FILEPATH

#define PARM_NODE_CLASS CellularNoise
#define PARM_NODE_NAME cellular_noise
#include EXE_FILEPATH

#define PARM_NODE_CLASS PerlinNoise
#define PARM_NODE_NAME perlin_noise
#include EXE_FILEPATH

// output

#ifndef SKIP_FILE_NODE
#define PARM_NODE_CLASS HeightOutput
#define PARM_NODE_NAME height_output
#include EXE_FILEPATH
#endif // SKIP_FILE_NODE

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

#define PARM_NODE_CLASS Gaussian
#define PARM_NODE_NAME gaussian
#include EXE_FILEPATH

#define PARM_NODE_CLASS Incline
#define PARM_NODE_NAME incline
#include EXE_FILEPATH

#define PARM_NODE_CLASS Step
#define PARM_NODE_NAME step
#include EXE_FILEPATH

// natural

#define PARM_NODE_CLASS Erosion
#define PARM_NODE_NAME erosion
#include EXE_FILEPATH

#define PARM_NODE_CLASS ErosionGPU
#define PARM_NODE_NAME erosion_gpu
#include EXE_FILEPATH

#define PARM_NODE_CLASS ThermalWeathering
#define PARM_NODE_NAME thermal_weathering
#include EXE_FILEPATH

#define PARM_NODE_CLASS Wind
#define PARM_NODE_NAME wind
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