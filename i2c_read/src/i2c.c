/*
 * i2c.c - the i2c library
 *
 * Copyright (c) 2013 Parav Nagarsheth
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 2.1 of the License, or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General
 * Public License along with this library; if not, write to the
 * Free Software Foundation, Inc., 59 Temple Place, Suite 330,
 * Boston, MA  02111-1307  USA
 */

#include <dirent.h>
#include <errno.h>
#include <stdio.h>
#include <string.h>
#include <fcntl.h>
#include <sys/ioctl.h>
#include <unistd.h>
#include <stdint.h>
#include "i2c-dev-user.h"

#define MAX_BUF 100
#define LINUX_I2C_ADAPTER		0
#define MY_TRACE_PREFIX "i2c"

struct i2c_msg i2c_buf[2];
unsigned int i2c_buf_count = 0;

int i2c_openadapter(const char * path)
{
  if(path==NULL) return -1;
  int i2c_fd=-1;	
  if ((i2c_fd = open(path, O_RDWR)) < 1) {
		perror("Failed to open adapter");
		return -1;
	}
	return i2c_fd;
}

int i2c_setslave(int i2c_fd, uint8_t addr)
{
	if (ioctl(i2c_fd, I2C_SLAVE_FORCE, addr) < 0) {
		fprintf(stderr, "Failed to set slave address %d:", addr);		
		perror("");
		return -1;	
	}
	return 0;
}

int i2c_writebyte(int i2c_fd, uint8_t byte)
{
	if (i2c_smbus_write_byte(i2c_fd, byte ) < 0) {
		perror("Failed to write byte to I2C slave");
		return -1;	
	}
	return 0;
}

int i2c_writebytes(int i2c_fd, uint8_t *bytes, uint8_t length)
{
	if(i2c_smbus_write_i2c_block_data(i2c_fd, bytes[0], length-1, bytes+1) 
									< 0) {
		perror("Failed to write bytes to I2C slave");
		return -1;
	}
	return 0;
}

int i2c_readbyte(int i2c_fd)
{
	int byte;
	if ((byte = i2c_smbus_read_byte(i2c_fd)) < 0) {
		// perror("Failed to read byte from I2C slave");
		return -1;
	}
	return byte;
}

int i2c_readbytes(int i2c_fd, uint8_t *buf, int length)
{
	if (read(i2c_fd, buf, length) == length)
		return length;
	else
		return -1;
}

int i2c_readwrite(int i2c_fd)
{
	struct i2c_rdwr_ioctl_data packets;
	packets.msgs = i2c_buf;
	packets.nmsgs = i2c_buf_count;
	if (ioctl(i2c_fd, I2C_RDWR, &packets) < 0) {
		perror("Unable to send data");
		i2c_buf_count = 0;
		return -1;
	}
	i2c_buf_count = 0;
	return 0;
}

int i2c_add_to_buf(uint8_t addr, uint8_t rw, uint8_t *value, int length)
{
	if(i2c_buf_count < 2) {
		i2c_buf[i2c_buf_count].addr = addr;
		i2c_buf[i2c_buf_count].flags = rw ? I2C_M_RD : 0;
		i2c_buf[i2c_buf_count].len = length;
		i2c_buf[i2c_buf_count].buf = (char *)value;
		return ++i2c_buf_count;
	} else
		return -1;
}
