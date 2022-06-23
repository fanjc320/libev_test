#include "pch.h"
#include "ClientArgOpt.h"
#include "string/str_num.h"

ClientArgOpt::ClientArgOpt()
{
	_userName = "test";
	_userPwd = "pwd";
	_hostIp = "127.0.0.1";
}

void ClientArgOpt::Man()
{
	printf(" -u userName.\n");
	printf(" -p userPwd.\n");
	printf(" -i host ip.\n");
	printf(" -h: help.\n");
}

bool ClientArgOpt::Argv(int argc, char* argv[])
{
	int32_t opt;
	while ((opt = getopt(argc, argv, "u:p:i:h")) != -1)
	{
		switch (opt)
		{
		case 'u':
			_userName = optarg;
			break;
		case 'p':
			_userPwd = optarg;
			break;
		case 'i':
			_hostIp = optarg;
			break;
		case 'h':
			Man();
			return false;
			break;
		default:
			LOG_ERROR("argv error, use -h help!");
			return false;
			break;
		}
	}

	return true;
}
