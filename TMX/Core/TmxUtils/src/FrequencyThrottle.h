/*
 * FrequencyRegulator.h
 *
 *  Created on: Jun 7, 2016
 *      Author: ivp
 */

#ifndef SRC_FREQUENCYTHROTTLE_H_
#define SRC_FREQUENCYTHROTTLE_H_

#include <map>
#include <boost/chrono.hpp>

namespace tmx {
namespace utils {

/**
 * This class is used to monitor a data source so that it can be throttled.
 */
template <class KeyType>
class FrequencyThrottle
{
public:
	FrequencyThrottle();
	/**
	 * @param frequency The frequency at which Monitor returns true.
	 */
	FrequencyThrottle(boost::chrono::milliseconds frequency);
	virtual ~FrequencyThrottle();

	/**
	 * Update the timestamp for the key specified and return true if the maxFrequency
	 * specified in the constructor has elapsed since the last time this method returned true
	 * for the matching key.
	 * Monitor will always return true the first time it is called for each unique key.
	 *
	 * @param key The unique key to monitor.
	 * @returns true if the frequency has elapsed since this method last returned true for the key.
	 */
	bool Monitor(KeyType key);

	/**
	 * Remove the data associated with any stale keys.
	 * This method should be called periodically if it is expected for keys to no longer be relevant.
	 */
	void RemoveStaleKeys();

	/**
	 * Get the frequency at which Monitor returns true.
	 */
	boost::chrono::milliseconds get_Frequency();

	/**
	 * Set the frequency at which Monitor returns true.
	 */
	void set_Frequency(boost::chrono::milliseconds frequency);

private:
	boost::chrono::milliseconds _frequency;
	boost::chrono::milliseconds _staleDuration;
	std::map<KeyType, boost::chrono::system_clock::time_point> _mapLastTime;
};

template <class KeyType>
FrequencyThrottle<KeyType>::FrequencyThrottle()
{
	set_Frequency(boost::chrono::milliseconds(0));
}

template <class KeyType>
FrequencyThrottle<KeyType>::FrequencyThrottle(boost::chrono::milliseconds frequency)
{
	set_Frequency(frequency);
}

template <class KeyType>
FrequencyThrottle<KeyType>::~FrequencyThrottle()
{
}

template <class KeyType>
boost::chrono::milliseconds FrequencyThrottle<KeyType>::get_Frequency()
{
	return _frequency;
}

template <class KeyType>
void FrequencyThrottle<KeyType>::set_Frequency(boost::chrono::milliseconds frequency)
{
	_frequency = frequency;

	// A key is determined to be stale if it has been this long since Monitor has been called.
	_staleDuration = _frequency + boost::chrono::milliseconds(5000);

}

template <class KeyType>
bool FrequencyThrottle<KeyType>::Monitor(KeyType key)
{
	// Find the key in the map.
	typename std::map<KeyType, boost::chrono::system_clock::time_point>::iterator it = _mapLastTime.find(key);

	// If key not found, store the current time, then return true to indicate it is time to do any processing.
	if (it == _mapLastTime.end())
	{
		_mapLastTime.insert(std::pair<KeyType, boost::chrono::system_clock::time_point>(key, boost::chrono::system_clock::now()));
		return true;
	}

	// Determine duration since last time that true was returned.
	boost::chrono::system_clock::time_point now = boost::chrono::system_clock::now();
	boost::chrono::milliseconds duration = boost::chrono::duration_cast<boost::chrono::milliseconds> (now - it->second);

	// If duration surpassed, store new time and return true.
	if (duration >= _frequency)
	{
		it->second = now;
		return true;
	}

	return false;
}

template <class KeyType>
void FrequencyThrottle<KeyType>::RemoveStaleKeys()
{
	boost::chrono::system_clock::time_point now = boost::chrono::system_clock::now();

	typename std::map<KeyType, boost::chrono::system_clock::time_point>::iterator it = _mapLastTime.begin();

	while (it != _mapLastTime.end())
	{
		boost::chrono::milliseconds duration = boost::chrono::duration_cast<boost::chrono::milliseconds> (now - it->second);

		if (duration >= _staleDuration)
	    {
			typename std::map<KeyType, boost::chrono::system_clock::time_point>::iterator toErase = it;
			it++;
			//std::cout << "Erasing " << toErase->first << std::endl;
			_mapLastTime.erase(toErase);
	    }
	    else
	    {
	       it++;
	    }
	}
}

}} // namespace tmx::utils

#endif /* SRC_FREQUENCYTHROTTLE_H_ */
