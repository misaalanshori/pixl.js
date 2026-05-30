#ifndef APP_SLIDESHOW_H
#define APP_SLIDESHOW_H

#include "mini_app_defines.h"
#include "mui_list_view.h"
#include "slideshow_view.h"

typedef struct {
    slideshow_view_t *p_slideshow_view;
    mui_list_view_t *p_list_view;
    mui_view_dispatcher_t *p_view_dispatcher;
    mui_scene_dispatcher_t *p_scene_dispatcher;
} app_slideshow_t;

typedef enum { SLIDESHOW_VIEW_ID_LIST, SLIDESHOW_VIEW_ID_MAIN } slideshow_view_id_t;

extern mini_app_t app_slideshow_info;

#endif
