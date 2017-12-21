/*
 * j2735dump.cpp
 *
 *  Created on: Dec 15, 2016
 *      @author: gmb
 */

#include <ccp/CCPDigitalDevice.h>
#include <PluginExec.h>

using namespace std;
using namespace tmx;
using namespace tmx::messages;
using namespace tmx::utils;

namespace gpiotest
{

class GPIOTest: public Runnable
{
public:
	GPIOTest(): Runnable(), msg("-")
	{
	}

	inline int Main()
	{
		if (msg == "-")
		{
			char buf[4000];
			cin.getline(buf, sizeof(buf));
			msg = string(buf);
		}

		PLOG(logDEBUG) << "Setting GPIO to: " << msg;

		CCPDigitalDevice dev;
		dev = atol(msg.c_str());
		cout << dev.BinStr() << endl;

		return (0);
	}

	inline bool ProcessOptions(const boost::program_options::variables_map &opts)
	{
		Runnable::ProcessOptions(opts);

		if (opts.count(INPUT_FILES_PARAM))
		{
			vector<string> files = opts[INPUT_FILES_PARAM].as< vector<string> >();
			if (files.size())
				msg = files[0];
		}

		return true;
	}

private:
	string msg;
};

} /* End namespace */

int main(int argc, char *argv[])
{
	FILELog::ReportingLevel() = logERROR;
	Output2Eventlog::Enable() = false;

	try
	{
		gpiotest::GPIOTest myExec;
		run("", argc, argv, myExec);
	}
	catch (TmxException &ex)
	{
		cerr << boost::diagnostic_information(ex) << endl;
	}
	catch (exception &ex)
	{
		cerr << ExceptionToString(ex) << endl;
	}
}

