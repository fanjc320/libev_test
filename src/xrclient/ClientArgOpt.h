#pragma  once
#include "dtype.h"
#include "server/ArgOptBase.h"
class ClientArgOpt : public ArgOptBase
{
public:
    ClientArgOpt();

    void Man();
    bool Argv(int argc, char* argv[]);

    const std::string& GetUserName() { return _userName; }
    const std::string& GetUserPwd() { return _userPwd; }
    const std::string& GetHostIp() { return _hostIp; }

private:
    std::string _userName;
    std::string _userPwd;
    std::string _hostIp;
};