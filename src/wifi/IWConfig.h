
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
 * IWConfig.h
 *
 *  Created on: 01.10.2013
 *      Author: Cem Akpolat
 */


#ifndef IWCONFIG_H_
#define IWCONFIG_H_
#include <string>
#include "IWConfObject.h"

using namespace std;

/*! \class IWConfig
 * \brief IW Configuration performs the network scanning and the assignment of the transmission power through iwconfig command.
 *
 */

class IWConfig {
public:
	static  std::string className;/**< Class name */

	/*! \enum DEVICE
	 *  \brief router device types
	 */
	enum DEVICE{GENERIC=1,NETGEAR=2,VOYAGE=3};

    static int const NETGEAR_MIN_TX = 3; 				/**< Minimum Netgear Router Transmission Power */
    static int const NETGEAR_MAX_TX = 26;				/**< Maximum Netgear Router Transmission Power */
    static int const GENERIC_MIN_TX = -5;				/**< Minimum Generic Router Transmission Power */
    static int const GENERIC_MAX_TX = 25;				/**< Maximum Generic Router Transmission Power */
    static int const VOYAGE_MIN_TX=0;					/**< Minimum Voyage Router Transmission Power */
    static int const VOYAGE_MAX_TX=27;					/**< Maximum Voyage Router Transmission Power */

	IWConfObject *iwobj;								/**<  IWConfObject containing the whole instantaneous scan information */
	std::string interface;								/**< Network Interface Name of the chosen router  */
	int selectedDevice;									/**< Selected Router */
	int  maxSignalThreshold,minSignalThreshold;			/**< Maximum and minimum transmission powers for selected router  */

	/**
	 * A constructor starting with default router settings
	 */
	IWConfig();
	/**
	 * A constructor starting with the given selected router configurations.
	 */
	IWConfig(int selectedDevice);
	/**
	 * Set the selected device to iwconfig for determining the scan function
	 */
	void setDeviceParameters();

	virtual ~IWConfig();

	/**
	 * Set transmission power.
	 * \param  value will be assigned
	 */
	void setTransmissionPower(int value);
	/**
	 * Scan Network through iwconfig with regard to the router specific method.
	 */
	void scan();
	/**
	 * A method being able to scan the generic Linux OS's wireless network card.
	 * Generic Linux OS iwconfig scanning result:
	 *
	 eth1     IEEE 802.11g  ESSID:"OSU_PUB"
	 Mode:Managed  Frequency:2.427 GHz  Access Point: 00:0D:9D:C6:38:2D
	 Bit Rate=48 Mb/s   Tx-Power=20 dBm   Sensitivity=8/0
	 Retry limit:7   RTS thr:off   Fragment thr:off
	 Power Management:off
	 Link Quality=91/100  Signal level=-39 dBm  Noise level=-87 dBm
	 Rx invalid nwid:0  Rx invalid crypt:860  Rx invalid frag:0
	 Tx excessive retries:0  Invalid misc:39   Missed beacon:8
	 */
	void scanGeneric();

	/**
	 * A method scanning VOYAGE  wireless network interface.
	 * Voyage OS iwconfig scanning result:
	 * wlan0     IEEE 802.11abgn  Mode:Master  Frequency:2.462 GHz  Tx-Power=27 dBm
	          Retry  long limit:7   RTS thr:off   Fragment thr:off
	          Power Management:o
	 */
	void scanVoyage();
	/**
	 * A method scanning NETGEAR network interface.
	 */
	void scanNetgear();

};

#endif /* IWCONFIG_H_ */
