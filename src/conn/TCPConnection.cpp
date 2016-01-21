/*
 * TCPConnection.cpp
 *
 *  Created on: Mar 7, 2013
 *      Author: cem
 */

#include "TCPConnection.h"
#include <iostream>
#include <string.h>
#include "Utilities.h"
#include <netinet/in.h>


TCPConnection::TCPConnection() {

}

TCPConnection::~TCPConnection() {
}

/**
 * The following functions are implemented for TCP Socket communications between internal and external component so as to facilitate
 * their inter-communications.
 */


bool TCPConnection::sendMessageInSmallChunks(string ip, string& text, int echoServPort) {
	string servAddress = ip;

	const char *echoString = text.c_str();
	int echoStringLen = strlen(echoString);   // Determine input length
	try {
		// Establish connection with the echo server
		TCPSocket sock(servAddress, echoServPort);
		// Send the string to the echo server
		sock.send(echoString, echoStringLen);
		return true;
	} catch (SocketException &e) {
		cerr << e.what() << endl;
		//exit(1);
		return false;
	}
	return false;
}

std::string TCPConnection::getMessageInSmallChunks(TCPSocket *sock,string& ip) { //,string servAddress,int echoServPort
	std::string str;
	try {
		ip=sock->getForeignAddress();

		cout << sock->getForeignAddress() << ":";cout << sock->getForeignPort();
		char echoBuffer[RCVBUFSIZE];
		int recvMsgSize;
		while ((recvMsgSize = sock->recv(echoBuffer, RCVBUFSIZE)) > 0) { // Zero means// end of transmission

			echoBuffer[recvMsgSize] = '\0';
			cout << ":echo " << echoBuffer << endl;
			str.append(echoBuffer);
		}
		cout << ":all " << str << endl;
		delete sock;

	} catch (SocketException &e) {
		cerr << e.what() << endl;

	}
	return str;
}

std::string TCPConnection::getMessageInSmallChunks(TCPSocket *sock) { //,string servAddress,int echoServPort
	std::string str;
	try {

		cout << sock->getForeignAddress() << ":";cout << sock->getForeignPort();
		char echoBuffer[RCVBUFSIZE];
		int recvMsgSize;
		while ((recvMsgSize = sock->recv(echoBuffer, RCVBUFSIZE)) > 0) { // Zero means// end of transmission

			echoBuffer[recvMsgSize] = '\0';
			cout << ":echo " << echoBuffer << endl;
			str.append(echoBuffer);
		}
		cout << ":all " << str << endl;
		delete sock;

	} catch (SocketException &e) {
		cerr << e.what() << endl;

	}
	return str;
}

std::string TCPConnection::getMessageFirstSizeInInt(TCPSocket *sock) {
	int val[1];
	try {
		//get packet size
		sock->recv(val, sizeof(val));
		uint32_t size = ntohl(val[0]);
		char echoBuffer[size];
		//read packet itself
		sock->recv(echoBuffer, size);
		echoBuffer[ val[0]] = '\0';
		delete sock;
		return echoBuffer;
	} catch (SocketException &e) {
		cerr << e.what() << endl;
	}
	return "";
}

bool TCPConnection::sendMessageFirstSizeInInt(string ip, string text,
		int port) {
	try {
		TCPSocket *sock = new TCPSocket(ip, port);
		uint32_t size = htonl(strlen(text.c_str()));
		//sent packet size
		sock->send(&size, sizeof(size));
		//sent packet itself
		sock->send(text.c_str(), size);
		delete sock;
		return true;
	} catch (SocketException &e) {
		cerr << e.what() << endl;
		return false;
	}
	return false;
}

std::string TCPConnection::getMessageLocal(TCPSocket *sock) {

	try {
		int val[1];
		//get packet size
		sock->recv(val, sizeof(val));
		cout<<val[0]<<endl;
		char echoBuffer[val[0]];
		//read packet itself
		sock->recv(echoBuffer, val[0]);
		echoBuffer[ val[0]] = '\0';

		Utilities::errorCatcher("getMessageLocal",1);
		cout<<echoBuffer<<endl;
		string a=echoBuffer;
		delete sock;
		return a;
	} catch (SocketException &e) {
		cerr << e.what() << endl;
		sleep(3);
		getMessageLocal(sock);
	}
	return "";
}

bool TCPConnection::sendMessageLocal(string text,int port) {
	try {

		TCPSocket *sock = new TCPSocket("127.0.0.1", port);
		const char *echoString =text.c_str();
		int val[1];
		val[0]=strlen(echoString);
		sock->send(val, sizeof(val));
		sock->send(echoString,val[0]);
		delete sock;
		Utilities::errorCatcher("sendMessageLocal",1);

		return true;
	} catch (SocketException &e) {
		cerr << e.what() << endl;
		return false;
	}
	return false;
}



 /*A SIMPLE EXAMPLE FOR CLIENT AND SERVER
//CLIENT
int main(int argc, char *argv[]) {

	string servAddress = argv[1]; // First arg: server address
	char *echoString = argv[2];   // Second arg: string to echo

	try {
		TCPConnection *con = new TCPConnection();
		for (;;) {
			//con->sendMessageInSmallChunks("192.168.1.1",echoString,12121);

			con->sendMessageFirstSizeInInt("192.168.1.1", echoString, 12121);
			//sendMessage();
			sleep(2);
		}
	} catch (SocketException &e) {
		cerr << e.what() << endl;
		exit(1);
	}

	return 0;
}
//SERVER
int main(int argc, char *argv[]) {

	try {
		TCPConnection *con = new TCPConnection();
		// Server Socket object
		TCPServerSocket servSock(12121);
		for (;;) {   // Run forever
			//cout<<con->getMessageInSmallChunks(servSock.accept())<<endl;
			cout << con->getMessageFirstSizeInInt(servSock.accept()) << endl;

		}
	} catch (SocketException &e) {
		cerr << e.what() << endl;
		exit(1);
	}
	// NOT REACHED

	return 0;
}
*/

