#include "app_slideshow.h"
#include "slideshow_scene.h"
#include "slideshow_view.h"
#include "amiibo_helper.h"

#include "nrf_log.h"

#include "settings.h"
// #include "hal_nfc_t2t.h"
// #include "nfc_mf1.h"
// #include "tag_emulation.h"

static void slideshow_scene_main_event_cb(slideshow_view_event_t event, slideshow_view_t *p_view) {
    app_slideshow_t *app = p_view->user_data;
    NRF_LOG_DEBUG("slideshow_scene_main_event_cb!");
    if (event == SLIDESHOW_VIEW_EVENT_MENU) {
        mui_scene_dispatcher_next_scene(app->p_scene_dispatcher, SLIDESHOW_SCENE_MENU);
    } else if (event == SLIDESHOW_VIEW_EVENT_PREV) {

    } else if (event == SLIDESHOW_VIEW_EVENT_NEXT) {

    }
}

void slideshow_scene_main_on_enter(void *user_data) {
    app_slideshow_t *app = user_data;

    // hal_nfc_set_nrfx_irq_enable(true);

    // tag_emulation_sense_run();

    slideshow_view_set_event_cb(app->p_slideshow_view, slideshow_scene_main_event_cb);
    mui_view_dispatcher_switch_to_view(app->p_view_dispatcher, SLIDESHOW_VIEW_ID_MAIN);
}

void slideshow_scene_main_on_exit(void *user_data) {
    app_slideshow_t *app = user_data;
    // tag_emulation_sense_end();
    // hal_nfc_set_nrfx_irq_enable(false);
}
