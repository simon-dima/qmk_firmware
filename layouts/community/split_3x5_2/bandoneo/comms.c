#include QMK_KEYBOARD_H
#include "transactions.h"
#include "comms.h"
#include "print.h"

typedef struct {
    uint8_t red;
    uint8_t green;
    uint8_t blue;
} rgb_to_slave_t;

#define COMMS_SYNC_INTERVAL 500 // milliseconds
#define MSG_SIZE 128
#define MSG_SIZE_STRING "128"

typedef struct {
    char msg[MSG_SIZE+1];
    bool has_msg;
    uint32_t missed_messages;
} comms_to_master_t;

char msg_buffer[MSG_SIZE];
bool msg_buffer_fill = 0;
uint32_t missed_messages = 0;

void setrgb_master(uint8_t r, uint8_t g, uint8_t b) {
    rgblight_setrgb(r, g, b);
}

void setrgb_slave(uint8_t r, uint8_t g, uint8_t b) {
    rgb_to_slave_t r2s = {r, g, b};
    transaction_rpc_send(USER_RGB_RPC, sizeof(r2s), &r2s);
}

void rgb_slave_handler(uint8_t in_buflen, const void* in_data, uint8_t out_buflen, void* out_data) {
    const rgb_to_slave_t *r2s = (const rgb_to_slave_t*)in_data;
    rgblight_setrgb(r2s->red, r2s->green, r2s->blue);
}

void slave_print(char* msg) {
    if (is_keyboard_master()) {
	uprintf("slave_print called from master!\n");
        uprintf(msg);
    } else if (msg_buffer_fill) {
        missed_messages++;
    } else {
        strlcpy(msg_buffer, msg, sizeof(msg_buffer));
	msg_buffer_fill = 1;
    }
}

void comms_slave_handler(uint8_t in_buflen, const void* in_data, uint8_t out_buflen, void* out_data) {
    comms_to_master_t *s2m = (comms_to_master_t*)out_data;
    if ((s2m->has_msg = msg_buffer_fill)) {
        strlcpy(s2m->msg, msg_buffer, sizeof(s2m->msg));
        s2m->missed_messages = missed_messages;
    }
    // reset counters
    missed_messages = 0;
    msg_buffer_fill = 0;
}

void recv_comms_master(void) {
    static uint32_t last_sync = 0;
    if (timer_elapsed32(last_sync) >= COMMS_SYNC_INTERVAL) {
        comms_to_master_t s2m;
        if(transaction_rpc_recv(USER_COMMS_RPC, sizeof(s2m), &s2m)) {
            last_sync = timer_read32();
	    if (s2m.has_msg) {
	        uprintf("[slave, missed %ld]: %s\n", s2m.missed_messages, s2m.msg);
	    }
        } else {
            uprintf("Slave comms sync failed!\n");
        }
    }
}

void register_user_rpcs(void) {
  transaction_register_rpc(USER_RGB_RPC, rgb_slave_handler);
  transaction_register_rpc(USER_COMMS_RPC, comms_slave_handler);
}
