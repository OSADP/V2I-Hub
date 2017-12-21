/*
 * @file CCPDigitalDevice.h
 *
 *  Created on: Nov 13, 2016
 *      @authoer Gregory M. Baumgardner
 */

#ifndef SRC_CCP_CCPDIGITALDEVICE_H_
#define SRC_CCP_CCPDIGITALDEVICE_H_

#include "../DigitalDevice.h"
#include "CCPRegisterDefinitions.h"

#include <fcntl.h>
#include <sys/ioctl.h>
#include <linux/types.h>
#include <linux/spi/spidev.h>

namespace tmx {
namespace utils {

class CCPDigitalDevice: public DigitalDevice<20> {
public:
	CCPDigitalDevice(bool activeHigh = true);
	virtual ~CCPDigitalDevice();

	using DigitalDevice<20>::operator=;

#define MODECFG(Nm, Bit) \
	inline bool Is ## Nm () { return _mode & Bit; } \
	inline void Set ## Nm () { _mode |= Bit; } \
	inline void Clear ## Nm () { _mode &= ~Bit; }

	// Configuration data
	MODECFG(ChipSelectActiveHigh, SPI_CS_HIGH)
	MODECFG(LeastSignificantBitFirst, SPI_LSB_FIRST)
	MODECFG(Loopback, SPI_LOOP)
	MODECFG(PhaseClocking, SPI_CPHA)
	MODECFG(PolarityClocking, SPI_CPOL)
	MODECFG(SignalsShared, SPI_3WIRE)

#undef MODECFG

#define VALCFG(Nm, Type, Var) \
	inline Type Get ## Nm () { return Var; } \
	inline void Set ## Nm(Type val) { if (val > 0) Var = val; }

	VALCFG(BitsPerWord, uint8_t, _bits)
	VALCFG(SpeedHz, uint32_t, _speed)
	VALCFG(Delay, uint16_t, _delay)

#undef VALCFG

	// Device I/O operations
	void load();
	void flush();
	void open();
	void close();
private:
	bool _actH;

	int fdL = 0;
	int fdH = 0;

	uint8_t _mode = SPI_MODE_2;
	uint8_t _bits = 8;
	uint32_t _speed = 500000;
	uint16_t _delay = 0;

	const char *devL = "/dev/spidev1.0";
	const char *devH = "/dev/spidev1.1";

	void open(int &, const char *);
	void shutdown(int &, const char *);
	void transfer(int, tmx::byte_stream &, tmx::byte_stream &, int);
};

} /* namespace utils */
} /* namespace tmx */

#endif /* SRC_CCP_CCPDIGITALDEVICE_H_ */
