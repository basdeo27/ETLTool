#ifndef DRIVER_H
#define DRIVER_H
// Driver.h
#include <string>
#include <unordered_map>

using namespace std;

//namespace ODBCConnectorAPI
//{
	class Driver
	{
	public:
		// Constructor
		Driver(unordered_map<string, int> capabilities);

		// Sets a capability for the app
		void setAppCapability(string capability, int value);

		// Returns the drivers name
		string getDriverName();

		string getDriverNameW();

		string getDriverId();

		string getDriverLicenseId();

		~Driver();

	private:
		int majorVersion, minorVersion;

		string driverName;

		string description;
		
		string id, licenseId;

		unordered_map<string, int> capabilities;
	};
//}

#endif