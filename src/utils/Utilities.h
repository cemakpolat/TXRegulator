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
 * Utilities.h
 *
 *  Created on: 01.10.2013
 *      Author: Cem Akpolat
 */

#ifndef UTILITIES_H_
#define UTILITIES_H_
#include <string>
#include <vector>
#include <iostream>
#include <sstream>
#include <fstream>
#include <stdio.h>
#include <stdlib.h>
//#include "writer.h"
//#include "elements.h"
//#include "reader.h"
#include <algorithm>
#include <functional>
#include <cctype>
#include <locale>
#include <unistd.h>
#include <sys/stat.h>
using namespace std;

/*! \class Utilities
 *  \brief Utilities, as its name implies, is composed of utility functions
 *
 */
class Utilities {
public:

	/**
	 * A Constructor.
	 */
	Utilities();
	/**
	 *A Destructor.
	 */
	~Utilities();


	static void writeOutputInFile(std::string text) {
		string file="outputtx.txt";
		cout<<getCurrentTimeAsString()<<" "<<text<<endl;
		text=getCurrentTimeAsString()+" "+text;
		Utilities::fileAppend(file,text);
	}

	/**
	 * Check whether the given string is in the given text or not
	 * \param text
	 * \param searchParam
	 * \return true if string is involved in the second string, otherwise false
	 */
	static bool contains(std::string& text, std::string searchParam) {
		if (std::string::npos != text.find(searchParam)) {
			return true;
		} else {
			return false;
		}
	}

	/**
	 * Write console a static string
	 \param line
	 */
	static void writeConsole(const std::string& line) {
		std::cout << line << std::endl;
	}
	/**
	 * Write console a string
	 * \param line
	 */
	static void writeConsole(std::string& line) {
		std::cout << line << std::endl;
	}
	/**
	 * Convert String to Long Double
	 * \param string
	 * \return long double
	 */
	inline static long double convertStringtoLongDouble(string input) {
		long double anz;
		istringstream inStream(input);
		inStream >> anz;
		return anz;
	}
	/**
	 * Convert long double to string
	 * \param val to be converted
	 * \return value
	 */
	inline static std::string convertLongDoubleToString(long double val){
		 std::stringstream ss;
		 ss << val;
		 return ss.str();
	}
	/**
	 * Convert float to string
	 * \param  val
	 * \return string
	 */
	inline static std::string float_to_string(float val) {
		stringstream ss;
		ss << val;
		return ss.str();
	}
	/**
	 * Convert long to string
	 * \param long val
	 * \sa
	 * \return stream
	 */
	inline static std::string long_to_String(long val) {
		std::ostringstream stream;
		stream << val;
		return stream.str();
	}

	static void errorCatcher(std::string funcName, int position) {
		std::cout << "FunctionName: " << funcName << " position: " << position
				<< std::endl;
	}

	/**
	 *Convert string to long
	 * \param str
	 * \return long
	 */
	static long convertStringToLong(std::string& str) {
		try {
			return atol(str.c_str());
		} catch (std::exception& e) {
			std::cout
					<< "Utilities:Converting could not be successfully performed! "
					<< e.what() << std::endl;
			return 0;
		}
		return 0;
	}
	/**
	 * Convert string to double
	 * \param s
	 * \sa
	 * \return double
	 */
	static double convertStringDouble(string& s) {
		try {

			return atof(s.c_str());
		} catch (std::exception& e) {
			std::cout
					<< "Utilities:Converting could not be successfully performed! "
					<< e.what() << std::endl;
			return 0;
		}
	}
	/**
	 * Convert Double to string
	 * \param number
	 * \sa
	 * \return string
	 */
	static std::string convertDoubleToString(double& number) {
		std::stringstream ss; //create a stringstream
		ss << number; //add number to the stream
		return ss.str(); //return a string with the contents of the stream
	}

	/**
	 * Return given number in the form of the second
	 * \param second
	 * \return second
	 */
	static int second(int second) {
		//int oneSecond=1000000;
		return 1000000 * second;
	}
	/**
	 * Return given number in the form of minutes
	 * \param min
	 * \return min
	 */
	static unsigned int minute(int min) {
		//int oneSecond=1000000*60;
		return 1000000 * 60 * min;
	}
	/**
	 * \param string s
	 * \param delim
	 * \param elem
	 * \return elems
	 */
	static std::vector<std::string> &split(const std::string &s, char delim,
			std::vector<std::string> &elems) {
		std::stringstream ss(s);
		std::string item;
		while (std::getline(ss, item, delim)) {
			elems.push_back(item);
		}
		return elems;
	}
	/**
	 * Split the string according to the delimeter
	 * \param string to be splitted
	 * \param delimiter
	 * \return vector in which the splitted strings
	 */
	static std::vector<std::string> split(const std::string &s, char delim) {
		std::vector<std::string> elems;
		return split(s, delim, elems);
	}
	/**
	 * Cinvert String to integer
	 * \param  s
	 * \return int
	 */
	static int convertStringToInt(std::string& s) {
		try {
			return atoi(s.c_str());
		} catch (std::exception& e) {
			std::cout
					<< "Utilities:Converting could not be successfully performed! "
					<< e.what() << std::endl;
			return 0;
		}
		return 0;
	}

	/**
	 * Convert Integer To String
	 * \param number
	 * \sa
	 * \return string
	 */
	static std::string convertIntToString(int number) {
		std::stringstream ss;	//create a stringstream
		ss << number;	//add number to the stream
		return ss.str();	//return a string with the contents of the stream
	}
	/**
	 * Time compare with current time
	 * \param value
	 * \param time
	 * \sa
	 * \return true if time is smaller than given values else false
	 */
	static bool timeCompareWithCurrentTime(int value, std::string& time) {
		//std::cout<<"time in dhcp: "<<atoi(time.c_str())<<std::endl;
		//std::cout<<"current time: "<<getCurrentTimeAsSecond()<<std::endl;
		if ((getCurrentTimeAsSecond() - atoi(time.c_str())) < value) {
			return true;
		} else {
			return false;
		}

	}
	/**
	 * \return current Minute in integer format
	 */
	static int getCurrentMinute() {

		std::string time = getCurrentTimeAsString();
		std::vector<std::string> list = split(time, ':');
		int hour = convertStringToInt(list[0]);
		int min = convertStringToInt(list[1]);
		return hour * 60 + min;
	}
	/**
	 * Convert String to Integer with CPP method
	 * \param  s
	 * \return integer
	 */
	static int convertStringToIntCPP(std::string& s) {
		std::stringstream ss(s); // Could of course also have done ss("1234") directly.
		int i=-1;
		if ((ss >> i).fail()) {
			std::cout << "Utilities:Error occured" << std::endl;
		}
		//std::cout << i << std::endl;
		return i;
	}
	/**
	 * Convert String to Integer with C method
	 * \param def
	 * \return integer
	 */
	static int covertStringToIntC(std::string& def) {
		int val = 0;
		try {
			val = atoi(def.c_str());
			return val;
		} catch (std::exception& e) {
			std::cout
					<< "Utilities:Converting could not be succesfully performed! "
					<< e.what() << std::endl;
			return val;
		}
		return val;
	}
	/**
	 * Get Current Date time in the format %Y/%m/%d-%H:%M:%S
	 * \return current date time in the string
	 */
	static std::string getCurrentDateAndTimeAsString() {
		time_t rawtime;
		struct tm * timeinfo;
		char buffer[80];
		time(&rawtime);
		timeinfo = localtime(&rawtime);
		strftime(buffer, 80, "%Y/%m/%d-%H:%M:%S", timeinfo);
		std::stringstream ss;
		ss << buffer;
		return ss.str();

	}
	/**
	 * Get current time in the format "%H:%M"
	 * \return current time in string format
	 */
	static std::string getCurrentTimeHM() {
		time_t rawtime;
		struct tm * timeinfo;
		char buffer[80];
		time(&rawtime);
		timeinfo = localtime(&rawtime);
		strftime(buffer, 80, "%H:%M", timeinfo);
		std::stringstream ss;
		ss << buffer;
		return ss.str();

	}
	/**
	 * Get current time as string in format  "%H:%M:%S"
	 * \return current time in the string format
	 */
	static std::string getCurrentTimeAsString() {
		time_t rawtime;
		struct tm * timeinfo;
		char buffer[80];
		time(&rawtime);
		timeinfo = localtime(&rawtime);
		strftime(buffer, 80, "%H:%M:%S", timeinfo);
		std::stringstream ss;
		ss << buffer;
		return ss.str();

	}
	/**
	 * Convert the given time to string format  "%H:%M:%S"
	 * \param time
	 * \return string
	 */
	static std::string getTimeFromGivenTime(const time_t& time) { //time should be already assigned to time value.
		struct tm * timeinfo;
		char buffer[80];
		timeinfo = localtime(&time);
		strftime(buffer, 80, "%H:%M:%S", timeinfo);
		std::stringstream ss;
		ss << buffer;
		return ss.str();
	}
	/**
	 * Get current Date as string in the format  "%Y/%m/%d"
	 * \return string
	 */
	static std::string getCurrentDateAsString() {
		time_t rawtime;
		struct tm * timeinfo;
		char buffer[80];
		time(&rawtime);
		timeinfo = localtime(&rawtime);
		strftime(buffer, 80, "%Y/%m/%d", timeinfo);
		std::stringstream ss;
		ss << buffer;
		return ss.str();

	}
	/**
	 * Convert the given second to String
	 * \param second
	 * \sa
	 * \return string long time
	 */
	static std::string longToTimeString(long second) {
		long elapsedTime = second;
		int hours = (int) elapsedTime / (60 * 60);
		int minutes = (int) (elapsedTime / (60)) % 60;
		int seconds = (int) (elapsedTime) % 60;
		//int mili = (int) (milisecond) % (1000);
		// System.out.println("convertMiliSecondTimeToString: "+milisecond);
		// System.out.println("hour: "+hours + ":" + minutes + ":" + seconds +
		// ":" + mili);
		std::string result = convertIntToString(hours) + ":"
				+ convertIntToString(minutes) + ":"
				+ convertIntToString(seconds);
		//std::cout<<"longToString: "<<result<<std::endl;
		return result;
	}
	/**
	 * Convert the String in the date format to Milisecond
	 * \param  time
	 * \return long miliseconds
	 */
	static long convertStringToMilisecond(std::string time) {

		std::vector<std::string> strTime = Utilities::split(time, ':');
		long second = 0;
		second = atoi(strTime[0].c_str()) * 1 * 60 * 60 + second;
		second = atoi(strTime[1].c_str()) * 1 * 60 + second;
		second = atoi(strTime[2].c_str()) * 1 + second;
		//milisecond = atoi(strTime[3].c_str()) + milisecond;
		// System.out.println(" convertStringToMilisecond: "+milisecond);
		return second;
	}
	/**
	 * Get Current Time as second
	 * \return second
	 */
	static time_t getCurrentTimeAsSecond() {
		time_t seconds;
		seconds = time(NULL);
		return seconds;
	}
	/**
	 *Convert milisecond in the time format of  "%Y/%m/%d-%H:%M:%S"
	 *\param  time
	 * \return string
	 */
	static std::string convertMilisecondToString(const time_t& time) {//time should be already assigned to time value.
		struct tm * timeinfo;
		char buffer[80];
		timeinfo = localtime(&time);
		strftime(buffer, 80, "%Y/%m/%d-%H:%M:%S", timeinfo);
		std::stringstream ss;
		ss << buffer;
		return ss.str();
	}
	/**
	 * Convert data time string to Milisecond
	 * \param  time
	 * \return time_t
	 */
	static time_t convertDateTimeStringToMilisecond(const std::string& time1) {
		if (time1 != "0") {
			std::vector<std::string> date;
			std::vector<std::string> d;
			std::vector<std::string> t;
			date = split(time1, '-');
			d = split(date[0], '/');
			t = split(date[1], ':');
			time_t rawtime;
			struct tm * timeinfo;
			time(&rawtime);
			timeinfo = localtime(&rawtime);
			timeinfo->tm_year = atoi(d[0].c_str()) - 1900;
			timeinfo->tm_mon = atoi(d[1].c_str()) - 1;
			timeinfo->tm_mday = atoi(d[2].c_str());
			timeinfo->tm_hour = atoi(t[0].c_str());
			timeinfo->tm_min = atoi(t[1].c_str());
			timeinfo->tm_sec = atoi(t[2].c_str());
			time_t x = mktime(timeinfo);
			return x;
		} else {

			return 0;
		}
	}
	/**
	 * Convert time string to Milisecond
	 * \param time
	 * \return time_t milisecond
	 */
	static time_t convertTimeStringToMilisecond(const std::string& time1) {
		if (time1 != "0") {
			std::vector<std::string> t;
			t = split(time1, ':');
			time_t rawtime;
			struct tm * timeinfo;
			time(&rawtime);
			timeinfo = localtime(&rawtime);
			//	timeinfo->tm_year = atoi(d[0].c_str())-1900;
			//	timeinfo->tm_mon = atoi(d[1].c_str())-1;
			//	timeinfo->tm_mday =atoi(d[2].c_str());
			timeinfo->tm_hour = atoi(t[0].c_str());
			timeinfo->tm_min = atoi(t[1].c_str());
			timeinfo->tm_sec = atoi(t[2].c_str());
			time_t x = mktime(timeinfo);
			return x;
		} else {
			return 0;
		}
	}

	static int randomGenerator(int min, int max) {

		//time_t t;
		//time(&t);
		//srand((unsigned int) t);
		int num = rand() % max + min;
		//printf("generated number %d\n ", num);
		return num;
	}
	/**
	 * Sleep during a time randomly selected which should smaller than 5 seconds.

	 */
	static void randomBackOfftime() {

		int randResult = 1 + (rand() % 5);
		//string randStr=" in randombackoff time state: "+Utilities::convertIntToString(randResult);
		try {
			usleep(randResult * Utilities::second(1)); //usleep is in microsecond
		} catch (std::exception& e) {
			e.what();
		}

	}
	/*random variable is choosen from 1 to a given value. Given value is referred to as maximumvalue,
	 * it means the max random value could be the given value.
	 * @maxValue
	 */
	/**
	 * Sleep during a time randomly selected which should smaller than max seconds.
	 * \param max
	 */
	static void randomBackOfftime(int maxValue) {

		int randResult = 1 + (rand() % maxValue);
		//string randStr=" in randombackoff time state: "+Utilities::convertIntToString(randResult);
		try {
			usleep(randResult * Utilities::second(1)); //usleep is in microsecond
		} catch (std::exception& e) {
			e.what();
		}

	}

	/**
	 * Execute the given command on the back console then return the output of the executed command
	 * \param  command
	 * \sa
	 * \return string output
	 */
	static string exec(string cmd) {
		FILE* pipe = popen(cmd.c_str(), "r");
		if (!pipe)
			return "ERROR";
		char buffer[128];
		std::string result = "";
		while (!feof(pipe)) {
			if (fgets(buffer, 128, pipe) != NULL)
				result += buffer;
		}
		pclose(pipe);
		return result;
	}
	/**
	 * Return Interface via IWconfig command
	 * \return
	 */
	/*Get Network Interface*/
	static string getNetworkInterface() {

		string result = "";
		string str2("802.11");
		size_t found;
		//wlan0
		result = exec("iwconfig wlan0");
		found = result.find(str2);
		if (found != string::npos) {
			//cout<<"device is wlan0"<<endl;
			return "wlan0";
		}
		//eth1
		result = exec("iwconfig eth1");
		found = result.find(str2);
		if (found != string::npos) {
			return "eth1";
		}
		//en1
		result = exec("iwconfig en1");
		found = result.find(str2);
		if (found != string::npos) {
			return "en1";
		}
		//eth0
		result = exec("iwconfig eth0");
		found = result.find(str2);
		if (found != string::npos) {
			return "eth0";
		}

	}

	//Ad here also JSON TUTORIAL which might ease its usage
	/**
	 * Write the text in the file
	 * \param fileName
	 * \param string will be written
	 */
	static void fileWrite(string& fileName, string& outputString) {
		try {
			ofstream file;
			file.open(fileName.c_str());
			if (file.is_open()) {
				file << outputString;
				file << "\n";
				file.close();
			}
		} catch (exception& e) {
			cerr << " " << e.what() << endl;
		}
	}
	/**
	 * Append the text to the provided file
	 * \param fileName
	 * \param text will be appended
	 */
	static void fileAppend(string& fileName, string& outputString) {
		try {
			ofstream file;
			file.open(fileName.c_str(), ios::app);
			if (file.is_open()) {
				file << outputString;
				file << "\n";
				file.close();
			}
		} catch (exception& e) {
			cerr << " " << e.what() << endl;
		}
	}

	/**
	 *Read the given file and return file content in a string
	 * \param string s
	 * \sa
	 * \return
	 */
	static string readFile(string& fileName) {
		ifstream infile;
		infile.open(fileName.c_str());
		string sLine;
		while (!infile.eof()) {
			getline(infile, sLine);
			cout << sLine << endl;
			sLine += sLine;
		}
		infile.close();
		return sLine;
	}

	/**
	 *Read the given file and return file content in a string via append method
	 * \param fileName
	 * \return the content of the file
	 */
	static string readFile2(string& fileName) {
		ifstream infile;
		string text;
		infile.open(fileName.c_str());
		string sLine;
		while (!infile.eof()) {
			getline(infile, sLine);

			text.append(sLine);
		}
		infile.close();
		return text;
	}

	/**
	 *Find the root path of the given file.
	 * \param
	 * \sa
	 * \return
	 */
	static string pathFinder(const string folderName, const string fileName) {
		char resolved_path[200];

		//locate the requested paths
		string x = "./" + folderName;
		realpath(x.c_str(), resolved_path);
		//printf("\n%s\n", resolved_path);

		//check whether path refers to an object or not
		struct stat sb;
		if (!(stat(resolved_path, &sb) == 0 && S_ISDIR(sb.st_mode))) {
			string cmd = "mkdir " + folderName;
			system(cmd.c_str());
		}
		string path = resolved_path;
		path = path + "/" + fileName;
		//cout << path << endl;

		return path;
	}

	/**
	 *trim from start
	 * \param  s
	 * \sa
	 * \return
	 */
	static inline std::string &ltrim(std::string &s) {
		s.erase(s.begin(),
				std::find_if(s.begin(), s.end(),
						std::not1(std::ptr_fun<int, int>(std::isspace))));
		return s;
	}
	/**
	 * trim from end
	 * \param  s
	 * \sa
	 * \return
	 */
	static inline std::string &rtrim(std::string &s) {
		s.erase(
				std::find_if(s.rbegin(), s.rend(),
						std::not1(std::ptr_fun<int, int>(std::isspace))).base(),
				s.end());
		return s;
	}

	/**
	 * trim from both ends
	 * \param string s
	 * \return
	 */
	static inline std::string &trim(std::string &s) {
		return ltrim(rtrim(s));
	}
	/*
	 * Scan all Access Point around the current AP return as a json string
	 * including MAC,ESSID and signal level
	 */
	/*static string scanAllAP() {
	 string result = "";
	 string s="sudo iwlist "+getNetworkInterface()+" scanning|awk '/Address:/{print $4$5}/ESSID:/{print $1}/Quality/{print $3}'>scanningResult";
	 result = exec(s);
	 std::ifstream infile("/root/scanningResult");
	 try {
	 std::string line;
	 size_t found;
	 //define json array Object
	 Array sessionsPerMin;
	 //define json object
	 Object sessionSample;
	 sessionSample["Address"]=String("");
	 sessionSample["ESSID"]=String("");
	 sessionSample["SNR"]=String("");
	 while (std::getline(infile, line)) {
	 string str("Address");
	 found = line.find(str);
	 if (found != string::npos) {
	 sessionSample["Address"] = String(line);
	 }
	 str = "ESSID";
	 found = line.find(str);
	 if (found != string::npos) {
	 sessionSample["ESSID"] = String(line);
	 }
	 str = "level";
	 found = line.find(str);
	 if (found != string::npos) {
	 sessionSample["SNR"] = String(line);
	 sessionsPerMin.Insert(sessionSample);//last value, then add in the list
	 }
	 }
	 //convert in string
	 std::stringstream stream1;
	 Writer::Write(sessionsPerMin, stream1);
	 return stream1.str();
	 } catch (Exception& e) {
	 e.what();

	 }
	 }*/
	/*
		//////////////////////////////	 JSON OPERATIONS  ////////////////////////////////////

	  //Read JSON file and return as JSON Object.
	 static json::Object readJSONFile(string& fileName1) {

		string path = "";
		//pathFinder(DataAccess::userModelFolderName,userId);
		const char *fileName = (path + ".txt").c_str();

		std::stringstream buffer;
		json::Object elemRootFile1;

		std::ifstream file(fileName);
		try {
			if (file) {
				cout << "JsonDataAccess: User's Profile exists" << endl;
				buffer << file.rdbuf();
				file.close();

				try {
					Reader::Read(elemRootFile1, buffer);
					return elemRootFile1;
				} catch (json::Reader::ParseException& e) {

				}
			} else {
				cout << "File does not exist" << endl;
			}
		} catch (exception& e) {
			cout << "File does not exist" << endl;
		}
		return elemRootFile1;
	}

	// Write the given object in a file as a JSON object.
	static void fileJSONWrite(const Object& tobeWritten,
			const string& fileName) {

		bool succes = false;
		ofstream myfile;
		std::stringstream stream1;
		//string path;
		try {

			Writer::Write(tobeWritten, stream1);
			//	path = pathFinder(DataAccess::userModelFolderName,fileName);
			//const char *fileName = (path + ".txt").c_str();
			myfile.open(fileName.c_str());
			if (myfile.is_open()) {
				myfile << stream1.str();
				myfile.close();
			}
			succes = true;

		} catch (Exception& e) {
			e.what();
		}
	}


	//Convert the json format string to JSON Object
	static json::Object convertStringToJSON(std::string& str) {
		using namespace json;
		try {
			std::istringstream sBadDocument(str);
			Object jsonObj;
			Reader::Read(jsonObj, sBadDocument);
			return jsonObj;
		} catch (Reader::ParseException& e) {
			std::cout << "Utilities:2:Caught json::ParseException: " << e.what()
					<< ", Line/offset: " << e.m_locTokenBegin.m_nLine + 1 << '/'
					<< e.m_locTokenBegin.m_nLineOffset + 1 << std::endl
					<< std::endl;
		} catch (json::Exception& e) {
			e.what();
		}
		Object *obj = NULL;
		return *obj;
	}


	//Convert the given the json object to String
	static string convertJSONToString(Object jo) {
		std::stringstream stream1;
		try {
			Writer::Write(jo, stream1);
		} catch (json::Exception &e) {
			cerr << "JSON Writing Exception " << e.what() << endl;
		}
		return stream1.str();
	}
	*/
	//add here tutorial how to use JSON
	//add object
	//get object
	//convert to json object

};

#endif /* UTILITIES_H_ */
