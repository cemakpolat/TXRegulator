
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
 * TCPConnection.h
 *
 *  Created on: 01.10.2013
 *      Author: Cem Akpolat
 */


#ifndef TCPCONNECTION_H_
#define TCPCONNECTION_H_
#include <string>
#include "PracticalSocket.h"

/*! \class TCPConnection
 *  \brief This class is responsible for establishing the external and internal socket communication.
 * \sa PracticalSocket.h
 *
 */
class TCPConnection {
public:
	static const unsigned int RCVBUFSIZE = 32;/**< TCP Socket Received Buffer Size  */
	TCPConnection();
	virtual ~TCPConnection();

	/*TCP Sockets Communications */

	/* TCP External Socket method
	 * A method Get Message from TCP Socket
	 * \param sock
	 * \return the received message from the external client in string format
	 *
	 */
	std::string getMessageInSmallChunks(TCPSocket* sock);

	/* TCP External Socket method
	 * The same method as getMessageInSmallChunks, unique difference the IP address of incoming packet is returned to the invoquer of this
	 * function.
	 *\return the received Message from the external client in string format.
	 */
	std::string getMessageInSmallChunks(TCPSocket* sock, string& ip);
	/* TCP External Socket method
	 * \param ip ip address of the client to which data will be sent
	 * \param text the string will be sent
	 * \param echoServPort port number to which data will be sent
	 * \return if sending process is successful return true
	 *
	 */
	bool sendMessageInSmallChunks(string ip, string& text, int echoServPort);

	/* TCP External Socket method
	 * This method works in two steps, firstly get the data length in order to allocate the buffer, then receive the data itself.
	 * \param sock
	 * \return the incoming message in string format
	 */
	std::string getMessageFirstSizeInInt(TCPSocket *sock);

	/* TCP External Socket method
	 * Transmit data to Client with respect to its IP address.
	 * Send firstly the lenght of the packet will be trasmitted, then send packet itself.
	 * \param ip ip address of the client to which data will be sent
	 * \param text the string will be sent
	 * \param echoServPort port number to which data will be sent
	 * \return if sending process is successful return true
	 */
	bool sendMessageFirstSizeInInt(string ip, string text, int port);
	/*TCP Internal Socket method
	 *Get Local Message From Client by means of two socket communications.
	 *\param sock
	 *\return the received message in string format
	 */
	std::string getMessageLocal(TCPSocket *sock);
	/*TCP Internal Socket method
	 * Send data to the local user via two socket transmission(2 message subsequently)
	 * There is any conversion from network-to-host or host-to-network
	 * \param text the string data to be sent.
	 * \param port to which the data will be sent.
	 * \return true in case successful
	 */

	bool sendMessageLocal(string text, int port);
	/*
	 * Send Request Message to Client,
	 * Once its response comes, return it to the caller
	 */
};

#endif /* TCPCONNECTION_H_ */
