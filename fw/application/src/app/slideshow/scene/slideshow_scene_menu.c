#include "app_slideshow.h"
#include "slideshow_scene.h"

#include "mini_app_launcher.h"
#include "mini_app_registry.h"

#include "amiibo_helper.h"
#include "nrf_log.h"

#include "i18n/language.h"
#include "settings.h"

#include "mui_icons.h"

#include "tag_helper.h"

#include "settings.h"

#include "nrf_log.h"

typedef enum {
    SLIDESHOW_MENU_HOME,
} slideshow_menu_item_t;

void slideshow_scene_menu_on_event(mui_list_view_event_t event, mui_list_view_t *p_list_view, mui_list_item_t *p_item) {
    slideshow_menu_item_t item = (slideshow_menu_item_t)p_item->user_data;
    switch (item) {
        case SLIDESHOW_MENU_HOME:
            mini_app_launcher_exit(mini_app_launcher());
            NRF_LOG_DEBUG("Home menu item selected");
            break;
    }
}

void slideshow_scene_menu_on_enter(void *user_data) {
    app_slideshow_t *app = user_data;

    mui_list_view_add_item(app->p_list_view, ICON_HOME, "Main Menu", (void *)SLIDESHOW_MENU_HOME);

    mui_list_view_set_selected_cb(app->p_list_view, slideshow_scene_menu_on_event);
    mui_view_dispatcher_switch_to_view(app->p_view_dispatcher, SLIDESHOW_VIEW_ID_LIST);
}

void slideshow_scene_menu_on_exit(void *user_data) {
    app_slideshow_t *app = user_data;
    tag_emulation_save();
    mui_list_view_clear_items(app->p_list_view);
}
