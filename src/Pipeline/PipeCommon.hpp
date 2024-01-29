#ifndef _PIPE_COMMON_H
#define _PIPE_COMMON_H

#include "graph.hpp"
#include "../Modules/COMMON/PipelineModules.hpp" // TODO: just want an integral of all modules. can be anoher file or file name.

typedef struct ModuleDelay_tag{
    MODULE_NAME module;
    int delay = 0;
} ModuleDelay_t;

typedef struct{
    MODULE_NAME module;
    std::vector<ModuleDelay_t> inputInOrder;
} InputOrder_t; // is needed only when module is with two or more inputs

typedef std::vector<InputOrder_t> Orders_t; // only stores the info of modules with two or more inputs; TOML;


typedef struct{
    MODULE_NAME module;
    std::vector<ModuleDelay_t> succWthDelay;
} DelayAdjacency_t;

typedef std::vector<DelayAdjacency_t> DelayGraph_t;


#endif