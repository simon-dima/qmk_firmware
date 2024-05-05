#include "transactions.h"
#include "rgb.h"

typedef struct {
    uint8_t red;
    uint8_t green;
    uint8_t blue;
} rgb_to_slave_t;

void master_setrgb(uint8_t r, uint8_t g, uint8_t b) {
    rgblight_setrgb(r, g, b);
}

void slave_setrgb(uint8_t r, uint8_t g, uint8_t b) {
    rgb_to_slave_t r2s = {r, g, b};
    transaction_rpc_send(USER_RGB_RPC, sizeof(r2s), &r2s);
}

void rgb_slave_handler(uint8_t in_buflen, const void* in_data, uint8_t out_buflen, void* out_data) {
    const rgb_to_slave_t *r2s = (const rgb_to_slave_t*)in_data;
    rgblight_setrgb(r2s->red, r2s->green, r2s->blue);
}

void register_rgb_rpc(void) {
  transaction_register_rpc(USER_RGB_RPC, rgb_slave_handler);
}
