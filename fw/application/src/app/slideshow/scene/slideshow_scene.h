#ifndef SLIDESHOW_SCENE_H
#define SLIDESHOW_SCENE_H

#include "mui_scene_dispatcher.h"

// Generate scene id and total number
#define ADD_SCENE(prefix, name, id) SLIDESHOW_SCENE_##id,
typedef enum {
#include "slideshow_scene_config.h"
    SLIDESHOW_SCENE_MAX,
} ble_scene_id_t;
#undef ADD_SCENE

extern const mui_scene_t slideshow_scene_defines[];




#endif
