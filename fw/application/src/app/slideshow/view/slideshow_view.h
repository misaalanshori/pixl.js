#ifndef SLIDESHOW_VIEW_H
#define SLIDESHOW_VIEW_H

#include "mui_include.h"
#include "vfs.h"
#include "app_timer.h"

struct slideshow_view_s;
typedef struct slideshow_view_s slideshow_view_t;

typedef enum {
    SLIDESHOW_VIEW_EVENT_MENU,
    SLIDESHOW_VIEW_EVENT_PREV,
    SLIDESHOW_VIEW_EVENT_NEXT,
} slideshow_view_event_t;

typedef void (*slideshow_view_event_cb)(slideshow_view_event_t event, slideshow_view_t *p_view);

 struct slideshow_view_s{
    mui_view_t* p_view;
    slideshow_view_event_cb event_cb;
    app_timer_t frame_tick_timer_data;
    app_timer_id_t frame_tick_timer_id;

    void* user_data;
} ;

slideshow_view_t* slideshow_view_create();
void slideshow_view_free(slideshow_view_t* p_view);
mui_view_t* slideshow_view_get_view(slideshow_view_t* p_view);


static inline void slideshow_view_set_event_cb(slideshow_view_t* p_view, slideshow_view_event_cb event_cb){
    p_view->event_cb = event_cb;
}

static inline void slideshow_view_set_user_data(slideshow_view_t* p_view, void* user_data){
    p_view->user_data = user_data;
}



#endif
