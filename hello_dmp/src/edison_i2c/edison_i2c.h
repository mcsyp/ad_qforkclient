#ifndef EDISON_I2C
#define EDISON_I2C
extern "C" {
#include <string.h>
#include <sys/types.h>
#include <unistd.h>
#include <stdlib.h>
#include <stdint.h>
#include <stdio.h>
}

#define BUFFER_LENGTH 32

class EdisonI2C  {
public:
	void begin();
	void end();
	void beginTransmission(uint8_t);
	void beginTransmission(int);
	uint8_t endTransmission(void);
    	uint8_t endTransmission(uint8_t);
	uint8_t requestFrom(uint8_t, uint8_t);
    	uint8_t requestFrom(uint8_t, uint8_t, uint8_t);
	uint8_t requestFrom(int, int);
    	uint8_t requestFrom(int, int, int);
	virtual size_t write(uint8_t);
	virtual size_t write(const uint8_t *, size_t);
	virtual int available(void);
	virtual int read(void);
	virtual int peek(void);
	virtual void flush(void);

    inline size_t write(unsigned long n) { return write((uint8_t)n); }
    inline size_t write(long n) { return write((uint8_t)n); }
    inline size_t write(unsigned int n) { return write((uint8_t)n); }
    inline size_t write(int n) { return write((uint8_t)n); }
	void onService(void);
private:
	// RX Buffer
	uint8_t rxBuffer[BUFFER_LENGTH];
	uint8_t rxBufferIndex;
	uint8_t rxBufferLength;

	// TX Buffer
	uint8_t txAddress;
	uint8_t txBuffer[BUFFER_LENGTH];
	uint8_t txBufferLength;

	// Service buffer
	uint8_t srvBuffer[BUFFER_LENGTH];
	uint8_t srvBufferIndex;
	uint8_t srvBufferLength;
	
	// Timeouts 
	static const uint32_t RECV_TIMEOUT = 100000;
	static const uint32_t XMIT_TIMEOUT = 100000;

	uint8_t adapter_nr;
	int i2c_fd;
	int i2c_transfer;
};

#endif
