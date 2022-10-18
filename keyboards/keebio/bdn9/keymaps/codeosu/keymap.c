/* Copyright 2019 Danny Nguyen <danny@keeb.io>
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */
#include QMK_KEYBOARD_H

// Necessary for Window movement with encoder
bool     is_alt_tab_active = false;
uint16_t alt_tab_timer     = 0;

enum bdn9_layers { 
    GENERAL = 0, 
    CODE, 
    OSU, 
    ADJUST 
};

enum encoder_names {
    _LEFT,
    _RIGHT,
    _MIDDLE,
};

enum custom_keycodes {
    M_COPYA = SAFE_RANGE,
};

bool process_record_user(uint16_t keycode, keyrecord_t *record) {
    switch (keycode) {
        case M_COPYA:
            if (record->event.pressed) {
                SEND_STRING(SS_LCTL("ac")); // Select all and copy
            }
            break;
    }
    return true;
};

const uint16_t PROGMEM keymaps[][MATRIX_ROWS][MATRIX_COLS] = {
    /*  Layer: General
        ┌───────────┬───────────┬───────────┐
        │  Window   │  Tabbing  │  Volume   │
        │ Prv   Nxt │ Prv   Nxt │ Down  Up  │
        ├───────────┼───────────┼───────────┤
        │ Layer 3   │ Layer 1   │ Mute      │  // Top Row
        ├───────────┼───────────┼───────────┤
        │ Previous  │ Play/Pause│ Next      │  // Middle Row
        ├───────────┼───────────┼───────────┤
        │ Cut       │ Copy      │ Paste     │  // Bottom Row
        └───────────┴───────────┴───────────┘
     */
    [GENERAL] = LAYOUT(
        TO(ADJUST), TO(CODE), KC_MUTE, 
        KC_MPRV, KC_MPLY, KC_MNXT, 
        C(KC_X), C(KC_C), C(KC_V)),
    /*  Layer: Code
        ┌───────────┬───────────┬───────────┐
        │ Undo Redo │Word Scroll│  Search   │
        │           │ Prv   Nxt │ Prv   Nxt │
        ├───────────┼───────────┼───────────┤
        │ Layer 2   │ Layer 0   │ New       │  // Top Row
        ├───────────┼───────────┼───────────┤
        │ SelectAll │ CopyAll   | Save      │  // Middle Row
        ├───────────┼───────────┼───────────┤
        │ Cut       │ Copy      │ Paste     │  // Bottom Row
        └───────────┴───────────┴───────────┘
     */
    [CODE] = LAYOUT(
        TO(GENERAL), TO(OSU), C(KC_N), 
        C(KC_A), M_COPYA, C(KC_S), 
        C(KC_X), C(KC_C), C(KC_V)),
    /*  Layer: osu!
        ┌───────────┬───────────┬───────────┐
        │osu! Volume│    N/A    │    N/A    │
        │   -   +   │           │           │
        ├───────────┼───────────┼───────────┤
        │ Layer 0   │ Layer 3   │ N/A       │  // Top Row
        ├───────────┼───────────┼───────────┤
        │ Escape    │ Restart   │ Settings  │  // Middle Row
        ├───────────┼───────────┼───────────┤
        │ Z         │ X         │ Space     │  // Bottom Row
        └───────────┴───────────┴───────────┘
     */
    [OSU] = LAYOUT(
        TO(CODE), TO(ADJUST), KC_NO, 
        KC_ESC, KC_GRAVE, C(KC_O), 
        KC_Z, KC_X, KC_SPC),
    /*  Layer: Adjustment
        ┌───────────┬───────────┬───────────┐
        │ RGB Mode  │  RGB Hue  │  RGB Sat  │
        │ Prv   Nxt │   -   +   │   -   +   │
        ├───────────┼───────────┼───────────┤
        │ Layer 2   │ Layer 0   │ RGB Tog   │  // Top Row
        ├───────────┼───────────┼───────────┤
        │ RGB Val+  │ RGB SPD+  │ N/A       │  // Middle Row
        ├───────────┼───────────┼───────────┤
        │ RGB Val-  │ RGB SPD-  │ N/A       │  // Bottom Row
        └───────────┴───────────┴───────────┘
     */
    [ADJUST] = LAYOUT(
        TO(OSU), TO(GENERAL), RGB_TOG, 
        RGB_VAI, RGB_SPI, KC_NO, 
        RGB_VAD, RGB_SPD, KC_NO),
};

bool encoder_update_user(uint8_t index, bool clockwise) {
    if (index == _LEFT) {
        if (get_highest_layer(layer_state) == GENERAL) { // Window movement
            if (clockwise) {
                if (!is_alt_tab_active) {
                    is_alt_tab_active = true;
                    register_code(KC_LALT);
                }
                alt_tab_timer = timer_read();
                tap_code16(KC_TAB);
            } else {
                if (!is_alt_tab_active) {
                    is_alt_tab_active = true;
                    register_code(KC_LALT);
                }
                alt_tab_timer = timer_read();
                tap_code16(S(KC_TAB));
            }
        }
        if (get_highest_layer(layer_state) == CODE) { // History scrubbing
            if (clockwise) {
                tap_code16(C(KC_Y));
            } else {
                tap_code16(C(KC_Z));
            }
        }
        if (get_highest_layer(layer_state) == OSU) { // osu! volume
            if (clockwise) {
                tap_code(KC_UP);
            } else {
                tap_code(KC_DOWN);
            }
        }
        if (get_highest_layer(layer_state) == ADJUST) { // RGB Mode +/-
            if (clockwise) {
                rgb_matrix_step_noeeprom();
            } else {
                rgb_matrix_step_reverse_noeeprom();
            }
        }
    } else if (index == _MIDDLE) {
        if (get_highest_layer(layer_state) == GENERAL) { // Tab scrolling
            if (clockwise) {
                tap_code16(C(KC_TAB));
            } else {
                tap_code16(S(C(KC_TAB)));
            }
        }
        if (get_highest_layer(layer_state) == CODE) { // Scroll horizontally by word
            if (clockwise) {
                tap_code16(C(KC_RGHT));
            } else {
                tap_code16(C(KC_LEFT));
            }
        }
        if (get_highest_layer(layer_state) == OSU) { // Scroll wheel
            if (clockwise) {
                tap_code(KC_WH_U);
            } else {
                tap_code(KC_WH_D);
            }
        }
        if (get_highest_layer(layer_state) == ADJUST) { // RGB Hue +/-
            if (clockwise) {
                rgb_matrix_increase_hue_noeeprom();
            } else {
                rgb_matrix_decrease_hue_noeeprom();
            }
        }
    } else if (index == _RIGHT) {
        if (get_highest_layer(layer_state) == GENERAL) { // Volume Control
            if (clockwise) {
                tap_code16(C(KC_VOLU));
            } else {
                tap_code16(C(KC_VOLD));
            }
        }
        if (get_highest_layer(layer_state) == CODE) { // Scroll through search results
            if (clockwise) {
                tap_code(KC_F3);
            } else {
                tap_code16(S(KC_F3));
            }
        }
        if (get_highest_layer(layer_state) == OSU) { // N/A
            if (clockwise) {
                tap_code(KC_NO);
            } else {
                tap_code(KC_NO);
            }
        }
        if (get_highest_layer(layer_state) == ADJUST) { // RGB Saturation
            if (clockwise) {
                rgb_matrix_increase_sat_noeeprom();
            } else {
                rgb_matrix_decrease_sat_noeeprom();
            }
        }
    }
    return false;
}

// Use Alt-Tabbing with encoder
void matrix_scan_user(void) {
    if (is_alt_tab_active) {
        if (timer_elapsed(alt_tab_timer) > 1250) {
            unregister_code(KC_LALT);
            is_alt_tab_active = false;
        }
    }
}

// Allow the desired RGB Mode to persist, but set one LED as the layer indicator
void rgb_matrix_indicators_user(void) {
    switch (get_highest_layer(layer_state)) {
        case GENERAL:
            break;
        case CODE:
            rgb_matrix_set_color(4, RGB_WHITE);
            break;
        case OSU:
            rgb_matrix_set_color(4, RGB_PINK);
            break;
        case ADJUST:
            rgb_matrix_set_color(4, RGB_TEAL);
            break;
    }
}
