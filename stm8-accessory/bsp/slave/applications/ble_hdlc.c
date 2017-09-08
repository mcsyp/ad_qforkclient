#include "ble_hdlc.h"
#include "rtthread.h"

/* User must define a function, that sends a 8bit char over the chosen interface, usart, spi, i2c etc. */
static hdlc_frame_encoded fptr_encoded_=RT_NULL;
/* User must define a function, that will process the valid received frame */
/* This function can act like a command router/dispatcher */
static hdlc_frame_decoded fptr_decoded_=RT_NULL;

uint8_t 	escape_character_=0;
uint8_t   receive_frame_buffer_[HDLC_MAX_RX_SIZE];
uint8_t 	frame_position_=0;
// 16bit CRC sum for _crc_ccitt_update
uint16_t 	frame_checksum_=0;

uint8_t   tx_encoded_buffer_[HDLC_MAX_TX_SIZE];
uint16_t  tx_encoded_length_=0;
/* HDLC Asynchronous framing */
/* The frame boundary octet is 01111110, (7E in hexadecimal notation) */
#define FRAME_BOUNDARY_OCTET 0x7E

/* A "control escape octet", has the bit sequence '01111101', (7D hexadecimal) */
#define CONTROL_ESCAPE_OCTET 0x7D

/* If either of these two octets appears in the transmitted data, an escape octet is sent, */
/* followed by the original data octet with bit 5 inverted */
#define INVERT_OCTET 0x20

/* The frame check sequence (FCS) is a 16-bit CRC-CCITT */
/* AVR Libc CRC function is _crc_ccitt_update() */
/* Corresponding CRC function in Qt (www.qt.io) is qChecksum() */
#define CRC16_CCITT_INIT_VAL 0xFFFF

/* 16bit low and high bytes copier */
#define low(x)    ((x) & 0xFF)
#define high(x)   (((x)>>8) & 0xFF)
uint16_t _crc_ccitt_update (uint16_t crc, uint8_t data)
{
    data ^= low(crc);
    data ^= data << 4;

    return ((((uint16_t)data << 8) | low (crc)) ^ (uint8_t)(data >> 4) 
            ^ ((uint16_t)data << 3));
}
static void push_to_txqueue(uint8_t data){
	tx_encoded_buffer_[tx_encoded_length_] = data;
	++tx_encoded_length_;
}
static void flush_txqueue(void){
	if(fptr_encoded_){
		fptr_encoded_(tx_encoded_buffer_,tx_encoded_length_);
	}
	tx_encoded_length_=0;
}

void ble_hdlc_init(hdlc_frame_encoded fenc,hdlc_frame_decoded fdec){
	frame_position_=0;
	frame_checksum_ = CRC16_CCITT_INIT_VAL;
	escape_character_ =RT_FALSE;
	fptr_encoded_ = fenc;
	fptr_decoded_ = fdec;
	tx_encoded_length_ = 0;
}

void ble_hdlc_rx_byte(uint8_t b){
		/* FRAME FLAG */
	if(b == FRAME_BOUNDARY_OCTET)
	{
		if(escape_character_ == RT_TRUE)
		{
			escape_character_ = RT_FALSE;
		}
		/* If a valid frame is detected */
		else if( (frame_position_ >= 2) &&
						 (frame_checksum_ == ((receive_frame_buffer_[frame_position_-1] << 8 ) | (receive_frame_buffer_[frame_position_-2] & 0xff)) ) )  // (msb << 8 ) | (lsb & 0xff)
		{
				/* Call the user defined function and pass frame to it */
			if(fptr_decoded_) fptr_decoded_(receive_frame_buffer_,frame_position_-2);
		}
		frame_position_ = 0;
		frame_checksum_ = CRC16_CCITT_INIT_VAL;
		return;
	}

	if(escape_character_)
	{
			escape_character_ = RT_FALSE;
			b ^= INVERT_OCTET;
	}
	else if(b == CONTROL_ESCAPE_OCTET)
	{
			escape_character_ = RT_TRUE;
			return;
	}

	receive_frame_buffer_[frame_position_] = b;

	if(frame_position_-2 >= 0) {
			frame_checksum_ = _crc_ccitt_update(frame_checksum_, receive_frame_buffer_[frame_position_-2]);
	}

	++frame_position_;

	if(frame_position_ == HDLC_MAX_RX_SIZE)
	{
		frame_position_ = 0;
		frame_checksum_ = CRC16_CCITT_INIT_VAL;
	}
}
//void Arduhdlc::frameDecode(const char *framebuffer, uint8_t frame_length)
void ble_hdlc_frame_encode(const uint8_t* raw, uint16_t raw_len){
	uint8_t data;
	uint16_t fcs = CRC16_CCITT_INIT_VAL;

	push_to_txqueue((uint8_t)FRAME_BOUNDARY_OCTET);

	while(raw_len)
	{
		data = *raw++;
		fcs = _crc_ccitt_update(fcs, data);
		if((data == CONTROL_ESCAPE_OCTET) || (data == FRAME_BOUNDARY_OCTET))
		{
				push_to_txqueue((uint8_t)CONTROL_ESCAPE_OCTET);
				data ^= INVERT_OCTET;
		}
		push_to_txqueue((uint8_t)data);
		raw_len--;
	}
	data = low(fcs);
	if((data == CONTROL_ESCAPE_OCTET) || (data == FRAME_BOUNDARY_OCTET))
	{
		push_to_txqueue((uint8_t)CONTROL_ESCAPE_OCTET);
		data ^= (uint8_t)INVERT_OCTET;
	}
	push_to_txqueue((uint8_t)data);
	data = high(fcs);
	if((data == CONTROL_ESCAPE_OCTET) || (data == FRAME_BOUNDARY_OCTET))
	{
		push_to_txqueue(CONTROL_ESCAPE_OCTET);
		data ^= INVERT_OCTET;
	}
	push_to_txqueue(data);
	push_to_txqueue(FRAME_BOUNDARY_OCTET);
	flush_txqueue();
}
