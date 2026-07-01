/*
Copyright 2022 @Yowkees
Copyright 2022 MURAOKA Taro (aka KoRoN, @kaoriya)

This program is free software: you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation, either version 2 of the License, or
(at your option) any later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/

#include "keycodes.h"
#include "modifiers.h"
#include QMK_KEYBOARD_H

#include "quantum.h"
#include "keymap_japanese.h"

#ifdef COMBO_ENABLE
enum {
    CM_MSST,
    CM_EEPR,
    CM_LPRN,
    CM_RPRN,
    CM_LBRC,
    CM_RBRC,
    CM_LCBR,
    CM_RCBR,
    CM_MB3,
    CM_F11,
    CM_F12,
    CM_GRAV,
    CM_CAPS,
    CM_INSE,
    CM_NUML,
    CM_SYSR,
    CM_PUBR,
    CM_SLCK,
    CM_KANA,
};

const uint16_t PROGMEM P_CM_MSST[] = {CW_TOGG, KC_LGUI, COMBO_END};
const uint16_t PROGMEM P_CM_EEPR[] = {CW_TOGG, KC_LALT, COMBO_END};
const uint16_t PROGMEM P_CM_LPRN[] = {JP_7, JP_8, COMBO_END};
const uint16_t PROGMEM P_CM_RPRN[] = {JP_8, JP_9, COMBO_END};
const uint16_t PROGMEM P_CM_LBRC[] = {JP_4, JP_5, COMBO_END};
const uint16_t PROGMEM P_CM_RBRC[] = {JP_5, JP_6, COMBO_END};
const uint16_t PROGMEM P_CM_LCBR[] = {JP_1, JP_2, COMBO_END};
const uint16_t PROGMEM P_CM_RCBR[] = {JP_2, JP_3, COMBO_END};
const uint16_t PROGMEM P_CM_MB3[]  = {KC_BTN2, KC_WH_D, COMBO_END};
const uint16_t PROGMEM P_CM_F11[]  = {KC_F8, KC_F9, COMBO_END};
const uint16_t PROGMEM P_CM_F12[]  = {KC_F9, KC_F10, COMBO_END};
const uint16_t PROGMEM P_CM_GRAV[] = {KC_F1, KC_F2, COMBO_END};
const uint16_t PROGMEM P_CM_CAPS[] = {KC_F2, KC_F3, COMBO_END};
const uint16_t PROGMEM P_CM_INSE[] = {KC_F3, KC_F4, COMBO_END};
const uint16_t PROGMEM P_CM_NUML[] = {KC_F4, KC_F5, COMBO_END};
const uint16_t PROGMEM P_CM_SYSR[] = {KC_F1, KC_F2, KC_F3, COMBO_END};
const uint16_t PROGMEM P_CM_PUBR[] = {KC_F2, KC_F3, KC_F4, COMBO_END};
const uint16_t PROGMEM P_CM_SLCK[] = {KC_F3, KC_F4, KC_F5, COMBO_END};
const uint16_t PROGMEM P_CM_KANA[] = {KC_F6, KC_F7, KC_F8, COMBO_END};

combo_t key_combos[] = {
    [CM_MSST] = COMBO(P_CM_MSST, TG(LY_MOUSE_ST)),
    [CM_EEPR] = COMBO(P_CM_EEPR, TG(LY_EEPROM)),
    [CM_LPRN] = COMBO(P_CM_LPRN, JP_LPRN),
    [CM_RPRN] = COMBO(P_CM_RPRN, JP_RPRN),
    [CM_LBRC] = COMBO(P_CM_LBRC, JP_LBRC),
    [CM_RBRC] = COMBO(P_CM_RBRC, JP_RBRC),
    [CM_LCBR] = COMBO(P_CM_LCBR, JP_LCBR),
    [CM_RCBR] = COMBO(P_CM_RCBR, JP_RCBR),
    [CM_MB3]  = COMBO(P_CM_MB3 , KC_BTN3),
    [CM_F11]  = COMBO(P_CM_F11 , KC_F11),
    [CM_F12]  = COMBO(P_CM_F12 , KC_F12),
    [CM_GRAV] = COMBO(P_CM_GRAV, JP_GRV),
    [CM_CAPS] = COMBO(P_CM_CAPS, KC_CAPS),
    [CM_INSE] = COMBO(P_CM_INSE, KC_INS),
    [CM_NUML] = COMBO(P_CM_NUML, KC_NUM_LOCK),
    [CM_SYSR] = COMBO(P_CM_SYSR, KC_SYSTEM_REQUEST),
    [CM_PUBR] = COMBO(P_CM_PUBR, KC_PAUSE),
    [CM_SLCK] = COMBO(P_CM_SLCK, KC_SCROLL_LOCK),
    [CM_KANA] = COMBO(P_CM_KANA, JP_KANA),
};
#endif

#ifdef KEY_OVERRIDE_ENABLE
const key_override_t sclm_coln_ko  = ko_make_basic(MOD_MASK_SHIFT, JP_SCLN, JP_COLN);
const key_override_t minus_unds_ko = ko_make_basic(MOD_MASK_SHIFT, JP_MINS, JP_UNDS);
const key_override_t prtsc_winp_ko = ko_make_basic(MOD_MASK_SHIFT, KC_PSCR, S(G(KC_S)));

const key_override_t **key_overrides = (const key_override_t *[]){
    &sclm_coln_ko,
    &minus_unds_ko,
    &prtsc_winp_ko,
    NULL
};
#endif

#ifdef TAP_DANCE_ENABLE
enum {
    TD_L1_BTN1 = 0,
};
void l1btn1_finished(tap_dance_state_t *state, void *user_data);
void l1btn1_reset(tap_dance_state_t *state, void *user_data);
tap_dance_action_t tap_dance_actions[] = {
    [TD_L1_BTN1] = ACTION_TAP_DANCE_FN_ADVANCED(NULL, l1btn1_finished, l1btn1_reset),
};
#endif

#define L1_BTN1 TD(TD_L1_BTN1)
#define L1_BTN2 LT(LY_FN,KC_SPC)
#define L1_BTN3 LCTL_T(KC_TAB)

#define R1_BTN1 RCTL_T(KC_BSPC)
#define R1_BTN2 LSFT_T(KC_ENT)
#define R1_BTN6 RSFT_T(KC_DEL)

// clang-format off
const uint16_t PROGMEM keymaps[][MATRIX_ROWS][MATRIX_COLS] = {
  [LY_MAIN] = LAYOUT_universal(
    JP_Q     , JP_W     , JP_E     , JP_R     , JP_T     ,                            JP_Y     , JP_U     , JP_I     , JP_O     , JP_P     ,
    JP_A     , JP_S     , JP_D     , JP_F     , JP_G     ,                            JP_H     , JP_J     , JP_K     , JP_L     , JP_MINS  ,
    JP_Z     , JP_X     , JP_C     , JP_V     , JP_B     ,                            JP_N     , JP_M     , JP_COMM  , JP_DOT   , JP_SCLN  ,
    KC_ESC   , KC_LGUI  , KC_LALT  , L1_BTN3  , L1_BTN2  , L1_BTN1  ,      R1_BTN1  , R1_BTN2  , _______  , _______  , _______  , R1_BTN6
  ),

  [LY_SYMBOL] = LAYOUT_universal(
    JP_EXLM  , JP_QUES  , JP_EQL   , JP_QUOT  , JP_TILD  ,                            KC_PSCR  , JP_7     , JP_8     , JP_9     , JP_ASTR  ,
    JP_AMPR  , JP_HASH  , JP_DLR   , JP_DQUO  , JP_GRV   ,                            JP_YEN   , JP_4     , JP_5     , JP_6     , JP_PLUS  ,
    JP_AT    , JP_PERC  , JP_CIRC  , JP_PIPE  , JP_BSLS  ,                            XXXXXXX  , JP_1     , JP_2     , JP_3     , JP_SLSH  ,
    CW_TOGG  , _______  , _______  , _______  , _______  , _______  ,      _______  ,  JP_0    , _______  , _______  , _______  , _______
  ),

  [LY_MOUSE] = LAYOUT_universal(
    _______  , _______  , _______  , _______  , _______  ,                            _______  , _______  , KC_WH_U  , _______  , _______  ,
    _______  , _______  , _______  , _______  , _______  ,                            _______  , KC_BTN1  , KC_WH_D , KC_BTN2  , _______  ,
    _______  , _______  , _______  , _______  , _______  ,                            _______  , _______  , _______  , _______  , _______  ,
    _______  , _______  , _______  , _______  , _______  , _______  ,      _______  , _______  , _______  , _______  , _______  , _______
  ),

  [LY_MOUSE_ST] = LAYOUT_universal(
    KC_F1    , KC_F2    , KC_F3    , KC_F4    , KC_F5    ,                            KC_PSCR  , KC_PGUP  , KC_WH_U  , KC_PGDN  , KC_RALT  ,
    C(JP_A)  , C(JP_S)  , C(JP_D)  , C(JP_F)  , C(JP_G)  ,                            KC_HOME  , KC_BTN1  , KC_WH_D  , KC_BTN2  , KC_END   ,
    C(JP_Z)  , C(JP_X)  , C(JP_C)  , C(JP_V)  , KC_APP   ,                            KC_F6    , KC_F7    , KC_F8    , KC_F9    , KC_F10   ,
    _______  , _______  , _______  , _______  , MO(LY_FN), TG(LY_MOUSE_ST),_______ ,  _______  , _______  , _______  , _______  , _______
  ),

  [LY_FN] = LAYOUT_universal(
    KC_F1    , KC_F2    , KC_F3    , KC_F4    , KC_F5    ,                            KC_PSCR  , KC_PGUP  , KC_UP    , KC_PGDN  , KC_RALT  ,
    C(JP_A)  , C(JP_S)  , C(JP_D)  , C(JP_F)  , C(JP_G)  ,                            KC_HOME  , KC_LEFT  , KC_DOWN  , KC_RGHT  , KC_END   ,
    C(JP_Z)  , C(JP_X)  , C(JP_C)  , C(JP_V)  , KC_APP   ,                            KC_F6    , KC_F7    , KC_F8    , KC_F9    , KC_F10   ,
    _______  , _______  , _______  , _______  , _______  , XXXXXXX  ,     _______  ,  _______  , _______  , _______  , _______  , _______
  ),

  [LY_EEPROM] = LAYOUT_universal(
    XXXXXXX  , XXXXXXX  , XXXXXXX  , XXXXXXX  , QK_BOOT  ,                            QK_BOOT  , AML_TO   , XXXXXXX  , XXXXXXX  , OLED_TO  ,
    XXXXXXX  , XXXXXXX  , XXXXXXX  , XXXXXXX  , XXXXXXX  ,                            CPI_I100 , AML_I1S  , AML_KI50 , XXXXXXX  , SCRL_DVD ,
    XXXXXXX  , XXXXXXX  , XXXXXXX  , XXXXXXX  , XXXXXXX  ,                            CPI_D100 , AML_D1S  , AML_KI50 , XXXXXXX  , SCRL_DVI ,
    TO(0)    , XXXXXXX  , XXXXXXX  , XXXXXXX  , KBC_RST  , KBC_SAVE ,      KBC_SAVE , KBC_RST  , _______  , _______  , _______  , TO(0)
  ),
};
// clang-format on

layer_state_t layer_state_set_user(layer_state_t state) {
    uint8_t layer = get_highest_layer(state);
    // Auto enable scroll mode when the highest layer is 3
    keyball_set_scroll_mode(layer == LY_FN);
    return state;
}

#ifdef COMBO_SHOULD_TRIGGER
bool combo_should_trigger(uint16_t combo_index, combo_t *combo, uint16_t keycode, keyrecord_t *record) {
    bool bret = true;
    uint8_t layer = get_highest_layer(layer_state);
    switch (combo_index) {
        case CM_MSST ... CM_EEPR:
            bret = layer != LY_MAIN;
            break;
        case CM_LPRN ... CM_RCBR:
            bret = layer != LY_SYMBOL;
            break;
        case CM_MB3:
            bret = layer != LY_MOUSE && layer !=  LY_MOUSE_ST;
            break;
        case CM_F11 ... CM_KANA:
            bret = layer != LY_FN;
            break;
    }
    return bret;
}
#endif

#ifdef OLED_ENABLE
bool keyball_oledkit_active_user() {
    uint8_t layer = get_highest_layer(layer_state);
    if (layer == LY_EEPROM) {
        return true;
    }
    return false;
}

void keyball_oledkit_render(bool master, bool left) {
    keyball_oledkit_logo();
    keyball_oledkit_layer(master);
    keyball_oledkit_keypress(master, left);
}
#endif

#ifdef TAP_DANCE_ENABLE
typedef struct {
    bool is_press_action;
    int state;
} tap;

enum {
    SINGLE_TAP = 1,
    SINGLE_HOLD = 2,
    DOUBLE_TAP = 3,
};

int cur_dance(tap_dance_state_t *state) {
    if (state->count == 1) {
        if (state->interrupted || state->pressed) return SINGLE_HOLD;
        else return SINGLE_TAP;
    } else if (state->count == 2) {
        return DOUBLE_TAP;
    } else return 8;
}

static tap l1btn1_state;

void l1btn1_finished(tap_dance_state_t *state, void *user_data) {
    l1btn1_state.state = cur_dance(state);
    switch (l1btn1_state.state) {
        case SINGLE_TAP: register_code(JP_MHEN); break;
        case SINGLE_HOLD: layer_on(LY_SYMBOL); break;
        case DOUBLE_TAP: register_code(JP_HENK); break;
    }
}

void l1btn1_reset(tap_dance_state_t *state, void *user_data) {
    switch (l1btn1_state.state) {
        case SINGLE_TAP: unregister_code(JP_MHEN); break;
        case SINGLE_HOLD: layer_off(LY_FN); break;
        case DOUBLE_TAP: unregister_code(JP_HENK); break;
    }
    l1btn1_state.state = 0;
}
#endif

#ifdef CAPS_WORD_ENABLE
bool caps_word_press_user(uint16_t keycode) {
    switch (keycode) {
        case JP_A ... JP_Z:
            add_weak_mods(MOD_BIT(KC_LSFT));
        case JP_1 ... JP_0:
        case JP_MINS:
        case JP_UNDS:
        case KC_BSPC:
        case KC_DEL:
        case KC_LSFT:
        case KC_RSFT:
            return true;
        case JP_SCLN:
            return (get_mods() | get_oneshot_mods()) & MOD_MASK_SHIFT;
    }
    return false;
}
#endif
