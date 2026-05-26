#include "app_slideshow.h"
#include "mini_app_launcher.h"
#include "mini_app_registry.h"

#include "mui_icons.h"
#include "mui_include.h"

#include "i18n/language.h"
#include "mui_list_view.h"
#include "mui_view_dispatcher.h"

#include "nrf_log.h"

static void app_slideshow_on_run(mini_app_inst_t *p_app_inst);
static void app_slideshow_on_kill(mini_app_inst_t *p_app_inst);
static void app_slideshow_on_event(mini_app_inst_t *p_app_inst, mini_app_event_t *p_event);

typedef enum {
    SLIDESHOW_MENU_HOME,
} slideshow_menu_item_t ;
void slideshow_menu_on_event(mui_list_view_event_t event, mui_list_view_t *p_list_view, mui_list_view_t *p_item) {
    slideshow_menu_item_t item = (slideshow_menu_item_t)p_item->user_data;
    switch (item) {
        case SLIDESHOW_MENU_HOME:
            mini_app_launcher_exit(mini_app_launcher());
            NRF_LOG_DEBUG("Home menu item selected");
            break;
    }
}

void app_slideshow_on_run(mini_app_inst_t *p_app_inst) {

    app_slideshow_t *p_app_handle = mui_mem_malloc(sizeof(app_slideshow_t));
    p_app_inst->p_handle = p_app_handle;

    p_app_handle->p_list_view = mui_list_view_create();
    p_app_handle->p_view_dispatcher = mui_view_dispatcher_create();

    mui_list_view_set_user_data(p_app_handle->p_list_view, p_app_handle);

    mui_view_dispatcher_add_view(p_app_handle->p_view_dispatcher, SLIDESHOW_VIEW_ID_LIST, mui_list_view_get_view(p_app_handle->p_list_view));

    mui_view_dispatcher_attach(p_app_handle->p_view_dispatcher, MUI_LAYER_FULLSCREEN);

    mui_list_view_add_item(p_app_handle->p_list_view, ICON_HOME, "Main Menu", (void *)SLIDESHOW_MENU_HOME);

    mui_list_view_set_selected_cb(p_app_handle->p_list_view, slideshow_menu_on_event);

    mui_view_dispatcher_switch_to_view(p_app_handle->p_view_dispatcher, SLIDESHOW_VIEW_ID_LIST);
}

void app_slideshow_on_kill(mini_app_inst_t *p_app_inst) {
    app_slideshow_t *p_app_handle = p_app_inst->p_handle;

    mui_view_dispatcher_detach(p_app_handle->p_view_dispatcher, MUI_LAYER_FULLSCREEN);
    mui_view_dispatcher_free(p_app_handle->p_view_dispatcher);

    mui_mem_free(p_app_handle);

    p_app_inst->p_handle = NULL;
}

void app_slideshow_on_event(mini_app_inst_t *p_app_inst, mini_app_event_t *p_event) {}

mini_app_t app_slideshow_info = {.id = MINI_APP_ID_SLIDESHOW,
                                 .name = "图片幻灯片",
                                 .name_i18n_key = _L_APP_SLIDESHOW,
                                 .icon = 0xe020,
                                 .deamon = false,
                                 .sys = false,
                                 .hibernate_enabled = false,
                                 .run_cb = app_slideshow_on_run,
                                 .kill_cb = app_slideshow_on_kill,
                                 .on_event_cb = app_slideshow_on_event};
