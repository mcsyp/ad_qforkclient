#ifndef __BLE_HDKC_H__
#define __BLE_HDKC_H__
#include <stdint.h>

#define HDLC_MAX_RX_SIZE 128
#define HDLC_MAX_TX_SIZE 128
typedef void (* hdlc_frame_encoded)(uint8_t * encoded, uint16_t encoded_len);
typedef void (* hdlc_frame_decoded)(const uint8_t * decoded, uint16_t decoded_len);

void ble_hdlc_init(hdlc_frame_encoded,hdlc_frame_decoded);
void ble_hdlc_frame_encode(const uint8_t* raw, uint16_t raw_len);
void ble_hdlc_rx_byte(uint8_t b);
#endif


