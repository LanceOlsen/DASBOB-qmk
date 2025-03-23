#include QMK_KEYBOARD_H
#include "timer.h"

enum dasbob_layers {
  _QWERTY,
  _NUMBER,
  _SYMBOL,
  _NAV,
};

typedef struct {
    uint16_t tap_key;
    uint16_t hold_mod;
} tap_dance_data_t;

void td_mod_finished(tap_dance_state_t *state, void *user_data) {
    tap_dance_data_t *data = (tap_dance_data_t *)user_data;
    if (state->pressed) {
        register_code(data->hold_mod);
    } else if (state->count == 1) {
        tap_code16(data->tap_key);
    }
}

void td_mod_reset(tap_dance_state_t *state, void *user_data) {
    tap_dance_data_t *data = (tap_dance_data_t *)user_data;
    unregister_code(data->hold_mod);
}

enum {
  TD_SFT_PCT = 0,
  TD_CTL_DOL = 1,
  TD_ALT_CRT = 2,
  TD_GUI_PCT = 3,
  TD_SFT_LPA = 4,
  TD_CTL_RPA = 5,
  TD_CTL_CLN = 6,
};

tap_dance_data_t td_shift_percent = { S(KC_5), KC_LSFT };
tap_dance_data_t td_ctrl_dollar = { S(KC_4), KC_LCTL };
tap_dance_data_t td_alt_caret = { S(KC_6), KC_LALT };
tap_dance_data_t td_gui_percent = { S(KC_5), KC_LGUI };
tap_dance_data_t td_shift_lparen = { S(KC_9), KC_LSFT };
tap_dance_data_t td_ctrl_rparen = { S(KC_0), KC_LCTL };
tap_dance_data_t td_ctrl_colon = { S(KC_SCLN), KC_LCTL };

#define ACTION_TAP_DANCE_FN_ADVANCED_USER(user_fn_on_each_tap, user_fn_on_dance_finished, user_fn_on_dance_reset, user_user_data) \
        { .fn = {user_fn_on_each_tap, user_fn_on_dance_finished, user_fn_on_dance_reset}, .user_data = (void*)user_user_data, }

tap_dance_action_t tap_dance_actions[] = {
    [TD_SFT_PCT] = ACTION_TAP_DANCE_FN_ADVANCED_USER(NULL, td_mod_finished, td_mod_reset, &td_shift_percent),
    [TD_CTL_DOL] = ACTION_TAP_DANCE_FN_ADVANCED_USER(NULL, td_mod_finished, td_mod_reset, &td_ctrl_dollar),
    [TD_ALT_CRT] = ACTION_TAP_DANCE_FN_ADVANCED_USER(NULL, td_mod_finished, td_mod_reset, &td_alt_caret),
    [TD_GUI_PCT] = ACTION_TAP_DANCE_FN_ADVANCED_USER(NULL, td_mod_finished, td_mod_reset, &td_gui_percent),
    [TD_SFT_LPA] = ACTION_TAP_DANCE_FN_ADVANCED_USER(NULL, td_mod_finished, td_mod_reset, &td_shift_lparen),
    [TD_CTL_RPA] = ACTION_TAP_DANCE_FN_ADVANCED_USER(NULL, td_mod_finished, td_mod_reset, &td_ctrl_rparen),
    [TD_CTL_CLN] = ACTION_TAP_DANCE_FN_ADVANCED_USER(NULL, td_mod_finished, td_mod_reset, &td_ctrl_colon),
};

enum custom_keycodes {
    KC_REV_SEMI = SAFE_RANGE,
};

// Set to true any time KC_REV_SEMI is being pressed
bool is_gui_down = false;

// Set to true after KC_REV_SEMI has been held for TAPPING_TERM
bool is_gui_held = false;

uint16_t gui_timer;

bool process_record_user(uint16_t keycode, keyrecord_t *record) {
    switch (keycode) {
        case KC_REV_SEMI:
            // Send : when tapped (held less than TAPPING_TERM)
            // Send ; when tapped (held less than TAPPING_TERM) with Shift or OSM Shift

            if (record->event.pressed) {
                // Key pressed
                gui_timer = timer_read();
                is_gui_down = true;
                is_gui_held = false;
            } else {
                // Key released
                is_gui_down = false;

                if (is_gui_held) {
                    // Do nothing if the key was held longer than TAPPING_TERM, or was used as a modifier because
                    // another key was pressed and released while KC_REV_SEMI was down
                    unregister_mods(MOD_BIT(KC_LGUI));
                    is_gui_held = false;
                } else {
                    if (get_mods() & MOD_BIT(KC_LSFT)) {
                        // If Shift is held, send ;
                        unregister_code(KC_LSFT);
                        tap_code(KC_SCLN);
                        register_code(KC_LSFT);
                    } else if (get_oneshot_mods() & MOD_MASK_SHIFT) {
                        // If OSM Shift was tapped, send ;
                        del_oneshot_mods(MOD_MASK_SHIFT);
                        tap_code(KC_SCLN);
                    } else {
                        // If no Shift behavior is activated, send :
                        tap_code16(S(KC_SCLN));
                    }
                }
            }
            return false; // Skip further processing for this key
   }
    return true; // Process all other keycodes as usual
}

void matrix_scan_user(void) {
    // Set is_gui_held if the KC_REV_SEMI key is held longer than TAPPING_TERM
    if (is_gui_down && !is_gui_held && timer_elapsed(gui_timer) > TAPPING_TERM) {
        register_mods(MOD_BIT(KC_LGUI));
        is_gui_held = true;
    }
}

/* Left vs right layout for CHORDAL_HOLD */
const char chordal_hold_layout[MATRIX_ROWS][MATRIX_COLS] PROGMEM =
    LAYOUT_split_3x5_3(
        'L', 'L', 'L', 'L', 'L',  'R', 'R', 'R', 'R', 'R',
        'L', 'L', 'L', 'L', 'L',  'R', 'R', 'R', 'R', 'R',
        'L', 'L', 'L', 'L', 'L',  'R', 'R', 'R', 'R', 'R',
                  'L', 'L', 'L',  'R', 'R', 'R'
    );

const uint16_t PROGMEM keymaps[][MATRIX_ROWS][MATRIX_COLS] = {
     /* QWERTY (0)
      * ┌───┬───┬───┬───┬───┐       ┌───┬───┬───┬───┬───┐
      * │ Q │ W │ E │ R │ T │       │ Y │ U │ I │ O │ P │
      * ├───┼───┼───┼───┼───┤       ├───┼───┼───┼───┼───┤
      * │A g│S o│D c│F s│G h│       │H h│J s│K c│L o│:;g│
      * ├───┼───┼───┼───┼───┤       ├───┼───┼───┼───┼───┤
      * │ Z │ X │ C │ V │ B │       │ N │ M │ , │ . │ / │
      * └───┴───┴───┴───┴───┘       └───┴───┴───┴───┴───┘
      *     ┌─────┐                           ┌─────┐
      *     │1,BSP├─────┐               ┌─────┤ ESC │
      *     └─────┤2,SPC├─────┐   ┌─────┤2,ENT├─────┘
      *           └─────│3,TAB│   │ SFT ├─────┘
      *                 └─────┘   └─────┘
      * NUMBER (1)
      * ┌───┬───┬───┬───┬───┐       ┌───┬───┬───┬───┬───┐
      * │   │   │   │   │ T │       │ * │ 7 │ 8 │ 9 │ + │
      * ├───┼───┼───┼───┼───┤       ├───┼───┼───┼───┼───┤
      * │ g │ o │: c│% s│   │       │ / │ 4 │ 5 │ 6 │ - │
      * ├───┼───┼───┼───┼───┤       ├───┼───┼───┼───┼───┤
      * │   │   │   │   │   │       │ 0 │ 1 │ 2 │ 3 │ . │
      * └───┴───┴───┴───┴───┘       └───┴───┴───┴───┴───┘
      *     ┌─────┐                           ┌─────┐
      *     │     ├─────┐               ┌─────┤ BSP │
      *     └─────┤     ├─────┐   ┌─────┤ ENT ├─────┘
      *           └─────│     │   │  0  ├─────┘
      *                 └─────┘   └─────┘
      * SYMBOL (2)
      * ┌───┬───┬───┬───┬───┐       ┌───┬───┬───┬───┬───┐
      * │ * │ @ │ # │ ~ │ + │       │   │ { │ } │   │   │
      * ├───┼───┼───┼───┼───┤       ├───┼───┼───┼───┼───┤
      * │% g│^ o│$ c│- s│ = │       │ " │( s│) c│' o│` g│
      * ├───┼───┼───┼───┼───┤       ├───┼───┼───┼───┼───┤
      * │ ! │ & │ | │ _ │ \ │       │ < │ [ │ ] │ > │   │
      * └───┴───┴───┴───┴───┘       └───┴───┴───┴───┴───┘
      *     ┌─────┐                           ┌─────┐
      *     │ BSP ├─────┐               ┌─────┤ BSP │
      *     └─────┤     ├─────┐   ┌─────┤ ENT ├─────┘
      *           └─────│     │   │     ├─────┘
      *                 └─────┘   └─────┘
      * NAV (3)
      * ┌───┬───┬───┬───┬───┐       ┌───┬───┬───┬───┬───┐
      * │   │   │   │   │   │       │swu│prv│ply│nxt│   │
      * ├───┼───┼───┼───┼───┤       ├───┼───┼───┼───┼───┤
      * │ g │ o │ c │ s │   │       │ ← │ ↓ │ ↑ │ → │   │
      * ├───┼───┼───┼───┼───┤       ├───┼───┼───┼───┼───┤
      * │   │   │   │   │   │       │swd│mut│ v+│ v-│   │
      * └───┴───┴───┴───┴───┘       └───┴───┴───┴───┴───┘
      *     ┌─────┐                           ┌─────┐
      *     │     ├─────┐               ┌─────┤     │
      *     └─────┤     ├─────┐   ┌─────┤     ├─────┘
      *           └─────│     │   │     ├─────┘
      *                 └─────┘   └─────┘
      */


    [_QWERTY] = LAYOUT_split_3x5_3(
        KC_Q,            KC_W,            KC_E,            KC_R,            KC_T,                 KC_Y,            KC_U,            KC_I,            KC_O,            KC_P,
        LGUI_T(KC_A),    LALT_T(KC_S),    LCTL_T(KC_D),    LSFT_T(KC_F),    MT(MOD_HYPR, KC_G),   MT(MOD_HYPR, KC_H), LSFT_T(KC_J), RCTL_T(KC_K),    LALT_T(KC_L),    KC_REV_SEMI, // RGUI_T(KC_REV_SEMI),
        KC_Z,            KC_X,            KC_C,            KC_V,            KC_B,                 KC_N,            KC_M,            KC_COMM,         KC_DOT,          KC_SLSH,
                                          LT(1,KC_BSPC),   LT(2,KC_SPC),    LT(3,KC_TAB),         OSM(MOD_LSFT),   LT(2,KC_ENT),    KC_ESC
    ),
    [_NUMBER] = LAYOUT_split_3x5_3(
        XXXXXXX,         XXXXXXX,         XXXXXXX,         XXXXXXX,         KC_T,                 KC_PAST,         KC_7,            KC_8,            KC_9,            KC_PPLS,
        KC_LGUI,         KC_LALT,         TD(TD_CTL_CLN),  TD(TD_SFT_PCT),  XXXXXXX,              KC_PSLS,         KC_4,            KC_5,            KC_6,            KC_PMNS,
        XXXXXXX,         XXXXXXX,         XXXXXXX,         XXXXXXX,         XXXXXXX,              KC_0,            KC_1,            KC_2,            KC_3,            KC_PDOT,

                                          XXXXXXX,         XXXXXXX,         XXXXXXX,              KC_0,            KC_ENT,         KC_BSPC
    ),
    [_SYMBOL] = LAYOUT_split_3x5_3(
        S(KC_8),         S(KC_2),         S(KC_3),         S(KC_GRAVE),     S(KC_EQL),            XXXXXXX,         S(KC_LBRC),      S(KC_RBRC),      XXXXXXX,         XXXXXXX,
        TD(TD_GUI_PCT),  TD(TD_ALT_CRT),  TD(TD_CTL_DOL),  LSFT_T(KC_MINS), KC_EQUAL,             S(KC_QUOT),      TD(TD_SFT_LPA),  TD(TD_CTL_RPA),  LALT_T(KC_QUOT), RGUI_T(KC_GRAVE),
        S(KC_1),         S(KC_7),         S(KC_BSLS),      S(KC_MINS),      KC_BSLS,              S(KC_COMM),      KC_LBRC,         KC_RBRC,         S(KC_DOT),       S(KC_SLSH),

                                          KC_BSPC,         XXXXXXX,         XXXXXXX,              XXXXXXX,         KC_ENT,         KC_BSPC
    ),
    [_NAV] = LAYOUT_split_3x5_3(
        XXXXXXX,         XXXXXXX,         XXXXXXX,         XXXXXXX,         XXXXXXX,              MS_WHLU,         KC_MPRV,         KC_MPLY,           KC_MNXT,         XXXXXXX,
        KC_LGUI,         KC_LALT,         KC_LCTL,         KC_LSFT,         XXXXXXX,              KC_LEFT,         KC_DOWN,         KC_UP,             KC_RIGHT,        XXXXXXX,
        XXXXXXX,         XXXXXXX,         XXXXXXX,         XXXXXXX,         XXXXXXX,              MS_WHLD,         KC_KB_MUTE,      KC_KB_VOLUME_DOWN, KC_KB_VOLUME_UP, XXXXXXX,

                                          XXXXXXX,         XXXXXXX,         XXXXXXX,              XXXXXXX,         XXXXXXX,         XXXXXXX
    )
};
