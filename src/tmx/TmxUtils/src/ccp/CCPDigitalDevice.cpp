/*
 * CCPDigitalDevice.cpp
 *
 *  Created on: Nov 13, 2016
 *      Author: gmb
 */

#include "CCPDigitalDevice.h"
#include "../PluginLog.h"

namespace tmx {
namespace utils {

CCPDigitalDevice::CCPDigitalDevice(bool activeHigh): _actH(activeHigh)
{
	open();
}

CCPDigitalDevice::~CCPDigitalDevice()
{
	close();
}

void CCPDigitalDevice::open(int &fd, const char *devName)
{
	FILE_LOG(logINFO) << "Mode=" << (int)_mode << ", Speed=" << _speed;

	fd = ::open(devName, O_RDWR);
	if (fd <= 0)
	{
		FILE_LOG(logWARNING) << "Unable to open " << devName << ": " << strerror(errno);
		shutdown(fd, devName);
	}
	else if (ioctl(fd, SPI_IOC_WR_MODE, &_mode) < 0)
	{
		FILE_LOG(logWARNING) << "Unable to set SPI mode on " << devName << ": " << strerror(errno);
		shutdown(fd, devName);
	}
	else if (ioctl(fd, SPI_IOC_WR_BITS_PER_WORD, &_bits) < 0)
	{
		FILE_LOG(logWARNING) << "Unable to set bits per word on " << devName << ": " << strerror(errno);
		shutdown(fd, devName);
	}
	else if (ioctl(fd, SPI_IOC_WR_MAX_SPEED_HZ, &_speed) < 0)
	{
		FILE_LOG(logWARNING) << "Unable to set max speed on " << devName << ": " << strerror(errno);
		shutdown(fd, devName);
	}
	else
	{
		FILE_LOG(logINFO) << "Device " << devName << " (" << fd << ") opened and configured with " <<
				"ActiveHigh=" << std::boolalpha << IsChipSelectActiveHigh() << ", " <<
				"LSB=" << std::boolalpha << IsLeastSignificantBitFirst() << ", " <<
				"Loopback=" << std::boolalpha << IsLoopback() << ", " <<
				"PhaseClocking=" << std::boolalpha << IsPhaseClocking() << ", " <<
				"PolarityClocking=" << std::boolalpha << IsPolarityClocking() << ", " <<
				"SignalsShared=" << std::boolalpha << IsSignalsShared() << ", " <<
				"BitsPerWord=" << (int)GetBitsPerWord() << ", " <<
				"Speed(Hz)=" << GetSpeedHz() << ", " <<
				"Delay(usec)=" << GetDelay();

		// Set all pins to active low logic output
		tmx::byte_stream Tx(2);
		tmx::byte_stream Rx(2);

		Tx[0] = 0x0A;
		Tx[1] = 0xFF;
		transfer(fd, Tx, Rx, 2);

		Tx[0] = 0x10;
		Tx[1] = 0x01;
		transfer(fd, Tx, Rx, 2);

		usleep(50000);
	}
}

void CCPDigitalDevice::shutdown(int &fd, const char *devName)
{
	if (fd > 0)
	{
		if (::close(fd) < 0)
		{
			FILE_LOG(logWARNING) << "Unable to close " << devName << "(" << fd << "): " << strerror(errno);
		}

		fd = 0;
	}
}

void CCPDigitalDevice::transfer(int fd, tmx::byte_stream &tx, tmx::byte_stream &rx, int length)
{
	if (fd <= 0)
		return;

	static uint8_t txBuf[10];
	static uint8_t rxBuf[10];

	::memcpy(txBuf, tx.data(), tx.size());

	struct spi_ioc_transfer tr;
	tr.tx_buf = (unsigned long)txBuf;
	tr.rx_buf = (unsigned long)rxBuf;
	tr.len = length;
	tr.delay_usecs = GetDelay();
	tr.speed_hz = GetSpeedHz();
	tr.bits_per_word = GetBitsPerWord();
	tr.cs_change = 0x00;
	tr.pad = 0x00000000;


	FILE_LOG(logDEBUG2) << "Sending command to device " << fd << ": " << tx;

	if (ioctl(fd, SPI_IOC_MESSAGE(1), &tr) < 0)
	{
		FILE_LOG(logWARNING) << "Unable to send spi message to " << fd << ": " << strerror(errno);
	}
	else
	{
		usleep(5000);

		::memcpy(rx.data(), rxBuf, rx.size());
		FILE_LOG(logDEBUG2) << "Received response from device " << fd << ": " << rx;
	}
}

void CCPDigitalDevice::open()
{
	// Connect to the device
	close();
	open(fdL, devL);
	open(fdH, devH);
}

void CCPDigitalDevice::close()
{
	usleep(50000);

	shutdown(fdL, devL);
	shutdown(fdH, devH);
}

void CCPDigitalDevice::load()
{
	// No loading for now.  Just use as output ports
}

void CCPDigitalDevice::flush()
{
	// Set all pins to active low logic output
	tmx::byte_stream Tx(2);
	tmx::byte_stream Rx(2);

	// Write out the pins
	uint8_t i;

	uint16_t curVal = this->Get();
	for (int n = 0; n < 2; n++)
	{
		for (i = 0; i < PIN_COUNT / 2; i++)
		{
			Tx[0] = 0 | i;

			// If bit is set and active high or if bit is not set and active low,  then output a 1, i.e. XNOR
			// If bit is set and active low  or if bit is not set and active high, then output a 0, i.e. XNOR
			uint8_t bit = (curVal & 0x01) ^ (_actH ? 0x00 : 0x01);
			curVal = (curVal >> 1);

			Tx[1] = (bit ? 0x01 : 0x00);
			transfer(n ? fdH : fdL, Tx, Rx, 2);
		}
	}
}

} /* namespace utils */
} /* namespace tmx */
