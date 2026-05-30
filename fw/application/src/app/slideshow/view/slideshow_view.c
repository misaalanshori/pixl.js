#include "slideshow_view.h"
#include "nrf_log.h"
#include "nrf_pwr_mgmt.h"

static void slideshow_view_on_draw(mui_view_t *p_view, mui_canvas_t *p_canvas) {
    mui_canvas_draw_utf8(p_canvas, 0, 10, "Hello World!");
}

static void slideshow_view_on_input(mui_view_t *p_view, mui_input_event_t *event) {
    slideshow_view_t *p_slideshow_view = p_view->user_data;
    switch (event->type) {
    case INPUT_TYPE_LONG: {
        if (p_slideshow_view->event_cb) {
            p_slideshow_view->event_cb(SLIDESHOW_VIEW_EVENT_MENU, p_slideshow_view);
        }
        break;
    }
    case INPUT_TYPE_SHORT: {

        if (event->key == INPUT_KEY_CENTER) {
            if (p_slideshow_view->event_cb) {
                p_slideshow_view->event_cb(SLIDESHOW_VIEW_EVENT_MENU, p_slideshow_view);
            }
        } else if (event->key == INPUT_KEY_LEFT) {
            if (p_slideshow_view->event_cb) {
                p_slideshow_view->event_cb(SLIDESHOW_VIEW_EVENT_PREV, p_slideshow_view);
            }
        } else if (event->key == INPUT_KEY_RIGHT) {
            if (p_slideshow_view->event_cb) {
                p_slideshow_view->event_cb(SLIDESHOW_VIEW_EVENT_NEXT, p_slideshow_view);
            }
        }
    }
    }
}

static void slideshow_view_on_enter(mui_view_t *p_view) {
    slideshow_view_t *p_slideshow_view = p_view->user_data;
    int32_t err_code =
        app_timer_start(p_slideshow_view->frame_tick_timer_id, APP_TIMER_TICKS(100), (void *)p_slideshow_view);
    APP_ERROR_CHECK(err_code);
}

static void slideshow_view_on_exit(mui_view_t *p_view) {
    slideshow_view_t *p_slideshow_view = p_view->user_data;
    app_timer_stop(p_slideshow_view->frame_tick_timer_id);
}

static void slideshow_view_on_frame_tick(void *p_context) {
    slideshow_view_t *p_view = p_context;
    nrf_pwr_mgmt_feed();
}

slideshow_view_t *slideshow_view_create() {
    slideshow_view_t *p_slideshow_view = mui_mem_malloc(sizeof(slideshow_view_t));
    memset(p_slideshow_view, 0, sizeof(slideshow_view_t));

    mui_view_t *p_view = mui_view_create();
    p_view->user_data = p_slideshow_view;
    p_view->draw_cb = slideshow_view_on_draw;
    p_view->input_cb = slideshow_view_on_input;
    p_view->enter_cb = slideshow_view_on_enter;
    p_view->exit_cb = slideshow_view_on_exit;

    p_slideshow_view->p_view = p_view;

    p_slideshow_view->frame_tick_timer_id = &p_slideshow_view->frame_tick_timer_data;
    int32_t err_code =
        app_timer_create(&p_slideshow_view->frame_tick_timer_id, APP_TIMER_MODE_REPEATED, slideshow_view_on_frame_tick);
    APP_ERROR_CHECK(err_code);

    return p_slideshow_view;
}
void slideshow_view_free(slideshow_view_t *p_view) {
    mui_view_free(p_view->p_view);
    mui_mem_free(p_view);
}
mui_view_t *slideshow_view_get_view(slideshow_view_t *p_view) { return p_view->p_view; }
