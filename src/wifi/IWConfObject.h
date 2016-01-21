
/*
 *
 *  Version 1.0
 * Copyright Master Thesis project - 2013
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU Lesser General Public License as
 * published by  the Free Software Foundation, either version 3 of the
 * License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with this program.  If not, see
 * <http://www.gnu.org/licenses/>.
 *
 * IWConfObject.h
 *
 *  Created on: 01.10.2013
 *      Author: Cem Akpolat
 */

#ifndef IWConfObject_H_
#define IWConfObject_H_
#include <string>

/*! \class IWConfObject Class.
 *  \brief
 */
class IWConfObject {
public:
	std::string ssid;					/**< SSID */
	std::string linkQuality;			/**< Link Quality of Wireless Network Interface Card(WNIC) */
	std::string signalLev;				/**< Signal Level of WNIC Sensed by Clients */
	std::string macAddr;				/**< MAC Address of WNIC */
	std::string channel;				/**< Selected Channel of WNIC */
	//std::string interface;
	std::string mode;					/**<  Selected WNIC Mode: Master mode*/
	std::string freq;					/**<  Frequency of WNIC*/
	std::string bitrate;				/**<  Bitrate of WNIC*/
	std::string txpower;				/**<  Assigned Transmission Power of WNIC*/
	std::string noiseLevel;				/**< Noise Level of WNIC */
	std::string retryLim;				/**<  Retry Limit of WNIC*/
	int txPower;

	IWConfObject();
	virtual ~IWConfObject();
	/**
	 * \return SSID.
	 */
	std::string getSSID();
	/**
	 * \return link quality.
	 */
	std::string getLinkQuality();
	/**
	 * return signal level.
	 */
	std::string getSignalLevel();
	/**
	 * \return MAC address.
	 */
	std::string getMacAddr();
	/**
	 * \return selected channel number.
	 */
	std::string getChannel();

	std::string getTXPower();
	/**
	 * \return bitrate.
	 */
	std::string getBitRate();
	/**
	 * \return retry limit.
	 */
	std::string getRetryLimit();
	/**
	 * \return noise level.
	 */
	std::string getNoiseLevel();
	/**
	 * \return SNR(Signal Level-Noise Level in dBM) sensed by Client
	 */
	std::string getSNR();
	/**
	 * Set the channel of WNIC
	 */
	void setChannel(std::string& val);
};

#endif /* IWConfObject_H_ */
