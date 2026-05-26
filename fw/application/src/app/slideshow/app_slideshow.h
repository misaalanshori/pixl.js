#ifndef APP_SLIDESHOW_H
#define APP_SLIDESHOW_H

#include "mini_app_defines.h"
#include "mui_list_view.h"

typedef struct {
    mui_list_view_t *p_list_view;
    mui_view_dispatcher_t *p_view_dispatcher;
} app_slideshow_t;

typedef enum { SLIDESHOW_VIEW_ID_LIST } slideshow_view_id_t;

extern mini_app_t app_slideshow_info;

#endif
