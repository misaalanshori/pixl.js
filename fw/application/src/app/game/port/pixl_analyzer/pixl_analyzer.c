// ===================================================================================
// Project:   pixlAnalyzer
// Version:   6416f76
// Year:      2025
// ===================================================================================

// pixlAnalyzer by ATC1441, ported to pixl.js game
// https://github.com/atc1441/pixlAnalyzer

// This program is free software; you can redistribute it and/or
// modify it under the terms of the GNU General Public License
// as published by the Free Software Foundation; either version 2
// of the License, or (at your option) any later version.

// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.

#include "../common/driver.h"
#include "pixl_analyzer.h"

#include "nrf_delay.h"
#include "app_timer.h"
#include "nrf.h"
#include "bat.h"

#include <stdint.h>
#include <string.h>
#include <stdio.h>

// Display constants
#define DISP_W 128
#define DISP_H 64
#define SPECTRUM_H 32
#define WATERFALL_START 32

// Scanning constants
#define SCAN_START_FREQ 0
#define SCAN_END_FREQ 87
#define BANDWIDTH (SCAN_END_FREQ - SCAN_START_FREQ + 1)

static uint8_t m_frame_buffer[DISP_W * (DISP_H / 8)];
static uint8_t m_rssi_current[BANDWIDTH];
static uint8_t m_rssi_peak[BANDWIDTH];
static float m_rssi_floating[BANDWIDTH];
static uint8_t m_waterfall_data[DISP_W][4];

static uint32_t m_frame_count = 0;
static uint32_t m_fps = 0;
static uint32_t m_last_time = 0;

// Battery
typedef struct {
    uint8_t level;
    float voltage;
    bool is_charging;
    int16_t raw_adc;
} bat_status_t;

// Font from pixlAnalyzer
static const uint8_t font5x7[][5] = {
    {0, 0, 0, 0, 0}, {0, 0, 95, 0, 0}, {0, 7, 0, 7, 0}, {20, 127, 20, 127, 20}, {36, 42, 127, 42, 18}, {35, 19, 8, 100, 98}, {54, 73, 85, 34, 80}, {0, 5, 3, 0, 0}, {0, 28, 34, 65, 0}, {0, 65, 34, 28, 0}, {20, 8, 62, 8, 20}, {8, 8, 62, 8, 8}, {0, 80, 48, 0, 0}, {8, 8, 8, 8, 8}, {0, 96, 96, 0, 0}, {32, 16, 8, 4, 2}, {62, 81, 73, 69, 62}, {0, 66, 127, 64, 0}, {66, 97, 81, 73, 70}, {33, 65, 69, 75, 49}, {24, 20, 18, 127, 16}, {39, 69, 69, 69, 57}, {60, 74, 73, 73, 48}, {1, 1, 113, 9, 7}, {54, 73, 73, 73, 54}, {6, 73, 73, 41, 30}, {0, 54, 54, 0, 0}, {0, 86, 54, 0, 0}, {8, 20, 34, 65, 0}, {20, 20, 20, 20, 20}, {0, 65, 34, 20, 8}, {2, 1, 81, 9, 6}, {50, 73, 121, 65, 62}, {126, 17, 17, 17, 126}, {127, 73, 73, 73, 54}, {62, 65, 65, 65, 34}, {127, 65, 65, 65, 62}, {127, 73, 73, 73, 65}, {127, 9, 9, 9, 1}, {62, 65, 73, 73, 58}, {127, 8, 8, 8, 127}, {0, 65, 127, 65, 0}, {32, 64, 65, 63, 1}, {127, 8, 20, 34, 65}, {127, 64, 64, 64, 64}, {127, 2, 12, 2, 127}, {127, 4, 8, 16, 127}, {62, 65, 65, 65, 62}, {127, 9, 9, 9, 6}, {62, 65, 81, 33, 94}, {127, 9, 25, 41, 70}, {70, 73, 73, 73, 49}, {1, 1, 127, 1, 1}, {63, 64, 64, 64, 63}, {31, 32, 64, 32, 31}, {63, 64, 56, 64, 63}, {99, 20, 8, 20, 99}, {7, 8, 112, 8, 7}, {97, 81, 73, 69, 67}, {0, 127, 65, 65, 0}, {2, 4, 8, 16, 32}, {0, 65, 65, 127, 0}, {4, 2, 1, 2, 4}, {64, 64, 64, 64, 64}, {0, 1, 2, 4, 0}, {32, 84, 84, 84, 120}, {127, 72, 68, 68, 56}, {56, 68, 68, 68, 32}, {56, 68, 68, 72, 127}, {56, 84, 84, 84, 24}, {8, 126, 9, 1, 2}, {12, 82, 82, 82, 62}, {127, 8, 4, 4, 120}, {0, 68, 125, 64, 0}, {32, 64, 68, 61, 0}, {127, 16, 40, 68, 0}, {0, 65, 127, 64, 0}, {124, 4, 24, 4, 120}, {124, 8, 4, 4, 120}, {56, 68, 68, 68, 56}, {124, 20, 20, 20, 8}, {8, 20, 20, 24, 124}, {124, 8, 4, 4, 8}, {72, 84, 84, 84, 32}, {4, 63, 68, 64, 32}, {60, 64, 64, 32, 124}, {28, 32, 64, 32, 28}, {60, 64, 48, 64, 60}, {68, 40, 16, 40, 68}, {12, 80, 80, 80, 60}, {68, 100, 84, 76, 68}};

// Helpers
static inline void lcd_flush(void) {
    for (uint8_t page = 0; page < 8; page++) {
        JOY_OLED_data_start(page);
        for (int x = 0; x < DISP_W; x++) {
            JOY_OLED_send(m_frame_buffer[page * DISP_W + x]);
        }
        JOY_OLED_end();
    }
}

static inline void draw_pixel(int x, int y, bool on) {
    if (x >= 0 && x < DISP_W && y >= 0 && y < DISP_H) {
        if (on)
            m_frame_buffer[x + (y / 8) * DISP_W] |= (1 << (y % 8));
        else
            m_frame_buffer[x + (y / 8) * DISP_W] &= ~(1 << (y % 8));
    }
}

static inline void draw_vline(int x, int y1, int y2) {
    if (y1 > y2) {
        int t = y1;
        y1 = y2;
        y2 = t;
    }
    for (int y = y1; y <= y2; y++)
        draw_pixel(x, y, true);
}

static inline void draw_box(int x, int y, int w, int h, bool fill, bool color) {
    for (int i = x; i < x + w; i++) {
        if (fill) {
            for (int j = y; j < y + h; j++)
                draw_pixel(i, j, color);
        } else {
            draw_pixel(i, y, color);
            draw_pixel(i, y + h - 1, color);
        }
    }
    if (!fill) {
        for (int j = y; j < y + h; j++) {
            draw_pixel(x, j, color);
            draw_pixel(x + w - 1, j, color);
        }
    }
}

static inline void draw_char_buf(int x, int y, char c) {
    if (c < 32 || c > 122)
        c = 32;
    c -= 32;
    for (int i = 0; i < 5; i++) {
        uint8_t col = font5x7[(int)c][i];
        for (int j = 0; j < 8; j++) {
            if (col & (1 << j))
                draw_pixel(x + i, y + j, true);
            else
                draw_pixel(x + i, y + j, false);
        }
    }
}

static inline void draw_text_buf(int x, int y, const char *str) {
    while (*str) {
        draw_char_buf(x, y, *str);
        x += 6;
        str++;
    }
}

static uint32_t get_time_ms(void) {
    uint32_t ticks = app_timer_cnt_get();
    uint32_t ms = (uint32_t)(((uint64_t)ticks * 1000ULL * (APP_TIMER_CONFIG_RTC_FREQUENCY + 1)) / APP_TIMER_CLOCK_FREQ);
    return ms;
}

// Radio scanner
static void radio_init_scanner(void) {
    NRF_RADIO->TASKS_DISABLE = 1;
    while (NRF_RADIO->EVENTS_DISABLED == 0)
        ;
    NRF_RADIO->POWER = 1;
    NRF_RADIO->MODE = (RADIO_MODE_MODE_Ble_1Mbit << RADIO_MODE_MODE_Pos);
}

static void scan_band(void) {
    if ((NRF_CLOCK->HFCLKSTAT & (CLOCK_HFCLKSTAT_SRC_Msk | CLOCK_HFCLKSTAT_STATE_Msk)) !=
        (CLOCK_HFCLKSTAT_SRC_Xtal << CLOCK_HFCLKSTAT_SRC_Pos | CLOCK_HFCLKSTAT_STATE_Running << CLOCK_HFCLKSTAT_STATE_Pos)) {
        NRF_CLOCK->TASKS_HFCLKSTART = 1;
        while ((NRF_CLOCK->HFCLKSTAT & CLOCK_HFCLKSTAT_STATE_Msk) == 0)
            ;
    }
    for (uint32_t freq = SCAN_START_FREQ; freq <= SCAN_END_FREQ; freq++) {
        NRF_RADIO->FREQUENCY = freq;
        NRF_RADIO->DATAWHITEIV = 0x40;
        NRF_RADIO->EVENTS_READY = 0;
        NRF_RADIO->TASKS_RXEN = 1;
        while (NRF_RADIO->EVENTS_READY == 0)
            ;
        NRF_RADIO->EVENTS_RSSIEND = 0;
        NRF_RADIO->TASKS_RSSISTART = 1;
        while (NRF_RADIO->EVENTS_RSSIEND == 0)
            ;
        uint8_t val = NRF_RADIO->RSSISAMPLE;
        if ((freq - SCAN_START_FREQ) < BANDWIDTH)
            m_rssi_current[freq - SCAN_START_FREQ] = val;
        NRF_RADIO->EVENTS_DISABLED = 0;
        NRF_RADIO->TASKS_DISABLE = 1;
        while (NRF_RADIO->EVENTS_DISABLED == 0)
            ;
    }
}

static void process_waterfall(void) {
    for (int x = 0; x < DISP_W; x++) {
        int freq_idx = (x * BANDWIDTH) / DISP_W;
        uint8_t val = m_rssi_current[freq_idx];
        bool pixel_on = (val < 88);
        uint32_t col = (m_waterfall_data[x][3] << 24) | (m_waterfall_data[x][2] << 16) | (m_waterfall_data[x][1] << 8) | m_waterfall_data[x][0];
        col <<= 1;
        if (pixel_on)
            col |= 1;
        m_waterfall_data[x][0] = (col & 0xFF);
        m_waterfall_data[x][1] = (col >> 8) & 0xFF;
        m_waterfall_data[x][2] = (col >> 16) & 0xFF;
        m_waterfall_data[x][3] = (col >> 24) & 0xFF;
    }
}

static void show_boot_screen(void) {
    memset(m_frame_buffer, 0, sizeof(m_frame_buffer));
    draw_box(2, 2, 124, 60, false, true);
    draw_box(4, 4, 120, 56, false, true);
    draw_text_buf(46, 15, "2.4GHz");
    draw_text_buf(40, 25, "SPECTRUM");
    draw_text_buf(40, 35, "ANALYZER");
    draw_text_buf(44, 45, "ATC1441");
    lcd_flush();
    nrf_delay_ms(800);
}

static void render_scanner(void) {
    memset(m_frame_buffer, 0, sizeof(m_frame_buffer));
    for (int x = 0; x < DISP_W; x++) {
        int freq_idx = (x * BANDWIDTH) / DISP_W;
        if (freq_idx >= BANDWIDTH)
            freq_idx = BANDWIDTH - 1;
        uint8_t val = m_rssi_current[freq_idx];
        int height = 0;
        if (val < 95)
            height = 95 - val;
        if (height > SPECTRUM_H)
            height = SPECTRUM_H;
        if (height < 0)
            height = 0;

        if (height >= m_rssi_peak[freq_idx])
            m_rssi_peak[freq_idx] = height;
        else if (m_rssi_peak[freq_idx] > 0)
            m_rssi_peak[freq_idx]--;

        int bar_h = m_rssi_peak[freq_idx];
        if (bar_h > 0)
            draw_vline(x, SPECTRUM_H - bar_h, SPECTRUM_H - 1);

        float fh = (float)height;
        if (fh >= m_rssi_floating[freq_idx])
            m_rssi_floating[freq_idx] = fh;
        else {
            m_rssi_floating[freq_idx] -= 0.5f;
            if (m_rssi_floating[freq_idx] < 0)
                m_rssi_floating[freq_idx] = 0;
        }

        int dot_y = SPECTRUM_H - (int)m_rssi_floating[freq_idx] - 2;
        if (dot_y >= 0 && dot_y < SPECTRUM_H)
            draw_pixel(x, dot_y, true);
    }

    process_waterfall();

    int start_page = WATERFALL_START / 8;
    for (int x = 0; x < DISP_W; x++) {
        for (int p = 0; p < 4; p++) {
            if (start_page + p < 8)
                m_frame_buffer[x + (start_page + p) * DISP_W] = m_waterfall_data[x][p];
        }
    }

    char buf[20];
    sprintf(buf, "%luhz", m_fps);
    draw_text_buf(2, 0, buf);
    sprintf(buf, "%d", 2400 + SCAN_START_FREQ);
    draw_text_buf(4, 56, buf);
    sprintf(buf, "%d", 2400 + SCAN_END_FREQ);
    draw_text_buf(102, 56, buf);

    static int batt_ctr = 0;
    if (batt_ctr++ > 60) {
        batt_ctr = 0;
    }

    if (get_stats())
        draw_text_buf(102, 0, "CHRG");
    else {
        int pct = (bat_get_level() * 100) / 8;
        sprintf(buf, "%d%%", pct);
        draw_text_buf(102, 0, buf);
    }

    lcd_flush();
}

int pixl_analyzer_run(void) {
    JOY_init();

    memset(m_frame_buffer, 0, sizeof(m_frame_buffer));
    lcd_flush();
    show_boot_screen();

    // SAADC and chrg timer are handled by the main firmware; no initialization needed here.
    radio_init_scanner();

    memset(m_rssi_peak, 0, sizeof(m_rssi_peak));
    memset(m_rssi_floating, 0, sizeof(m_rssi_floating));
    memset(m_waterfall_data, 0, sizeof(m_waterfall_data));
    m_last_time = get_time_ms();

    while (!JOY_exit()) {
        scan_band();
        render_scanner();
        JOY_idle();

        m_frame_count++;
        uint32_t now = get_time_ms();
        if (now - m_last_time >= 1000) {
            m_fps = m_frame_count;
            m_frame_count = 0;
            m_last_time = now;
        }
    }

    // Cleanup: disable radio receive tasks
    NRF_RADIO->TASKS_DISABLE = 1;
    while (!NRF_RADIO->EVENTS_DISABLED);

    // Clear display
    memset(m_frame_buffer, 0, sizeof(m_frame_buffer));
    lcd_flush();

    return 0;
}
