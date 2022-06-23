/*
 *
 *      Author: venture
 */

#include "pch.h"
#ifndef _MSC_VER
#include <sys/stat.h>
#include <sys/sysinfo.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <sys/epoll.h>
#include <sys/socket.h>
#include <netinet/tcp.h>
#include <fcntl.h>
#include <sys/types.h>
#include <netdb.h>
#include <sys/ioctl.h>
#include <net/if.h>
#else
#include <windows.h>
#include <process.h>
#include <shellapi.h>
#include <stdint.h>
#include <tlhelp32.h>

#include <Psapi.h>   
#include <Iprtrmib.h>   
#include <Winsock2.h>

#pragma  comment(lib,"Psapi.lib")
#pragma  comment(lib,"Iphlpapi.Lib")
//#pragma  comment(lib,"WS2_32.lib")
#endif // _MSC_VER

#include "func_proc.h"
#include "string/str_format.h"
#include "string/str_num.h"
#include "file_func.h"
#include "log/LogMgr.h"
#include "dtype.h"

#ifdef _MSC_VER

typedef struct
{
	DWORD dwState;      //连接状态   
	DWORD dwLocalAddr;  //本地地址   
	DWORD dwLocalPort;  //本地端口   
	DWORD dwRemoteAddr; //远程地址   
	DWORD dwRemotePort; //远程端口   
	DWORD dwProcessId;  //进程标识 
}MIB_TCPEXROW, * PMIB_TCPEXROW;

typedef struct
{
	DWORD dwLocalAddr;  //本地地址   
	DWORD dwLocalPort;  //本地端口   
	DWORD dwProcessId;  //进程标识   

}MIB_UDPEXROW, * PMIB_UDPEXROW;


typedef struct
{
	DWORD dwState;      //连接状态   
	DWORD dwLocalAddr;  //本地地址   
	DWORD dwLocalPort;  //本地端口   
	DWORD dwRemoteAddr; //远程地址   
	DWORD dwRemotePort; //远程端口   
	DWORD dwProcessId;  //进程标识   
	DWORD Unknown;      //待定标识
}MIB_TCPEXROW_VISTA, * PMIB_TCPEXROW_VISTA;


typedef struct
{
	DWORD dwNumEntries;
	MIB_TCPEXROW table[ANY_SIZE];
}MIB_TCPEXTABLE, * PMIB_TCPEXTABLE;


typedef struct
{
	DWORD dwNumEntries;
	MIB_TCPEXROW_VISTA table[ANY_SIZE];
}MIB_TCPEXTABLE_VISTA, * PMIB_TCPEXTABLE_VISTA;


typedef struct
{
	DWORD dwNumEntries;
	MIB_UDPEXROW table[ANY_SIZE];
}MIB_UDPEXTABLE, * PMIB_UDPEXTABLE;


//=====================================================================================//   
//Name: DWORD AllocateAndGetTcpExTableFromStack()                                      //                                                                //   
//Descripion: 该函数仅仅只在 Windows XP，Windows Server 2003 下有效                    //   
//=====================================================================================//   
typedef DWORD(WINAPI* PFNAllocateAndGetTcpExTableFromStack)(
	PMIB_TCPEXTABLE* pTcpTabel,
	bool bOrder,
	HANDLE heap,
	DWORD zero,
	DWORD flags
	);

//=====================================================================================//   
//Name: DWORD AllocateAndGetUdpExTableFromStack()                                      //   
//Descripion: 该函数仅仅只在 XP，Windows Server 2003 下有效                            //   
//=====================================================================================//   
typedef DWORD(WINAPI* PFNAllocateAndGetUdpExTableFromStack)(
	PMIB_UDPEXTABLE* pUdpTable,
	bool bOrder,
	HANDLE heap,
	DWORD zero,
	DWORD flags
	);

//=====================================================================================//   
//Name: DWORD InternalGetTcpTable2()                                                   //   
//Descripion: 该函数在 Windows Vista 以及 Windows 7 下面效                             //   
//=====================================================================================//   
typedef DWORD(WINAPI* PFNInternalGetTcpTable2)(
	PMIB_TCPEXTABLE_VISTA* pTcpTable_Vista,
	HANDLE heap,
	DWORD flags
	);

//=====================================================================================//   
//Name: DWORD InternalGetUdpTableWithOwnerPid()                                        //   
//Descripion: 该函数在 Windows Vista 以及 Windows 7 下面效                             //   
//=====================================================================================//   
typedef DWORD(WINAPI* PFNInternalGetUdpTableWithOwnerPid)(
	PMIB_UDPEXTABLE* pUdpTable,
	HANDLE heap,
	DWORD flags
	);


//=====================================================================================//   
//Name: DWORD GetProcessIdByPort()                                                     //   
//Descripion: 根据端口号得到打开该端口号的进程ID(支持 XP，Server 2003，Vista，Win7)   //   
//=====================================================================================//   
DWORD GetProcessIdByPort(DWORD dwPort, bool bTcp = true)
{
	HMODULE hModule = LoadLibraryW(L"iphlpapi.dll");
	if (hModule == nullptr)
	{
		return 0;
	}

	if (bTcp)
	{
		// 表明查询的是 TCP 信息   
		PFNAllocateAndGetTcpExTableFromStack pAllocateAndGetTcpExTableFromStack;
		pAllocateAndGetTcpExTableFromStack =
			(PFNAllocateAndGetTcpExTableFromStack)GetProcAddress(hModule, "AllocateAndGetTcpExTableFromStack");
		if (pAllocateAndGetTcpExTableFromStack != nullptr)
		{
			// 表明为 XP 或者 Server 2003 操作系统   
			PMIB_TCPEXTABLE pTcpExTable = nullptr;
			if (pAllocateAndGetTcpExTableFromStack(&pTcpExTable, TRUE, GetProcessHeap(), 0, AF_INET) != 0)
			{
				if (pTcpExTable)
				{
					HeapFree(GetProcessHeap(), 0, pTcpExTable);
				}

				FreeLibrary(hModule);
				hModule = nullptr;

				return 0;
			}

			for (UINT i = 0; i < pTcpExTable->dwNumEntries; i++)
			{
				// 过滤掉数据，只查询我们需要的进程数据   
				if (dwPort == ntohs(0x0000FFFF & pTcpExTable->table[i].dwLocalPort))
				{
					DWORD dwProcessId = pTcpExTable->table[i].dwProcessId;
					if (pTcpExTable)
					{
						HeapFree(GetProcessHeap(), 0, pTcpExTable);
					}

					FreeLibrary(hModule);
					hModule = nullptr;

					return dwProcessId;
				}
			}

			if (pTcpExTable)
			{
				HeapFree(GetProcessHeap(), 0, pTcpExTable);
			}

			FreeLibrary(hModule);
			hModule = nullptr;

			return 0;
		}
		else
		{
			// 表明为 Vista 或者 7 操作系统   
			PMIB_TCPEXTABLE_VISTA pTcpExTable = nullptr;
			PFNInternalGetTcpTable2 pInternalGetTcpTable2 =
				(PFNInternalGetTcpTable2)GetProcAddress(hModule, "InternalGetTcpTable2");
			if (pInternalGetTcpTable2 == nullptr)
			{
				if (pTcpExTable)
				{
					HeapFree(GetProcessHeap(), 0, pTcpExTable);
				}

				FreeLibrary(hModule);
				hModule = nullptr;

				return 0;
			}

			if (pInternalGetTcpTable2(&pTcpExTable, GetProcessHeap(), 1))
			{
				if (pTcpExTable)
				{
					HeapFree(GetProcessHeap(), 0, pTcpExTable);
				}

				FreeLibrary(hModule);
				hModule = nullptr;

				return 0;
			}

			for (UINT i = 0; i < pTcpExTable->dwNumEntries; i++)
			{
				// 过滤掉数据，只查询我们需要的进程数据   
				if (dwPort == ntohs(0x0000FFFF & pTcpExTable->table[i].dwLocalPort))
				{
					DWORD dwProcessId = pTcpExTable->table[i].dwProcessId;
					if (pTcpExTable)
					{
						HeapFree(GetProcessHeap(), 0, pTcpExTable);
					}

					FreeLibrary(hModule);
					hModule = nullptr;

					return dwProcessId;
				}
			}

			if (pTcpExTable)
			{
				HeapFree(GetProcessHeap(), 0, pTcpExTable);
			}

			FreeLibrary(hModule);
			hModule = nullptr;

			return 0;
		}
	}
	else
	{
		// 表明查询的是 UDP 信息   
		PMIB_UDPEXTABLE pUdpExTable = nullptr;
		PFNAllocateAndGetUdpExTableFromStack pAllocateAndGetUdpExTableFromStack;
		pAllocateAndGetUdpExTableFromStack =
			(PFNAllocateAndGetUdpExTableFromStack)GetProcAddress(hModule, "AllocateAndGetUdpExTableFromStack");
		if (pAllocateAndGetUdpExTableFromStack != nullptr)
		{
			// 表明为 XP 或者 Server 2003 操作系统   
			if (pAllocateAndGetUdpExTableFromStack(&pUdpExTable, TRUE, GetProcessHeap(), 0, AF_INET) != 0)
			{
				if (pUdpExTable)
				{
					HeapFree(GetProcessHeap(), 0, pUdpExTable);
				}

				FreeLibrary(hModule);
				hModule = nullptr;

				return 0;
			}

			for (UINT i = 0; i < pUdpExTable->dwNumEntries; i++)
			{
				// 过滤掉数据，只查询我们需要的进程数据   
				if (dwPort == ntohs(0x0000FFFF & pUdpExTable->table[i].dwLocalPort))
				{
					DWORD dwProcessId = pUdpExTable->table[i].dwProcessId;
					if (pUdpExTable)
					{
						HeapFree(GetProcessHeap(), 0, pUdpExTable);
					}

					FreeLibrary(hModule);
					hModule = nullptr;

					return dwProcessId;
				}
			}

			if (pUdpExTable)
			{
				HeapFree(GetProcessHeap(), 0, pUdpExTable);
			}

			FreeLibrary(hModule);
			hModule = nullptr;

			return 0;
		}
		else
		{
			// 表明为 Vista 或者 7 操作系统   
			PFNInternalGetUdpTableWithOwnerPid pInternalGetUdpTableWithOwnerPid;
			pInternalGetUdpTableWithOwnerPid =
				(PFNInternalGetUdpTableWithOwnerPid)GetProcAddress(hModule, "InternalGetUdpTableWithOwnerPid");
			if (pInternalGetUdpTableWithOwnerPid != nullptr)
			{
				if (pInternalGetUdpTableWithOwnerPid(&pUdpExTable, GetProcessHeap(), 1))
				{
					if (pUdpExTable)
					{
						HeapFree(GetProcessHeap(), 0, pUdpExTable);
					}

					FreeLibrary(hModule);
					hModule = nullptr;

					return 0;
				}

				for (UINT i = 0; i < pUdpExTable->dwNumEntries; i++)
				{
					// 过滤掉数据，只查询我们需要的进程数据   
					if (dwPort == ntohs(0x0000FFFF & pUdpExTable->table[i].dwLocalPort))
					{
						DWORD dwProcessId = pUdpExTable->table[i].dwProcessId;
						if (pUdpExTable)
						{
							HeapFree(GetProcessHeap(), 0, pUdpExTable);
						}

						FreeLibrary(hModule);
						hModule = nullptr;

						return dwProcessId;
					}
				}
			}

			if (pUdpExTable)
			{
				HeapFree(GetProcessHeap(), 0, pUdpExTable);
			}

			FreeLibrary(hModule);
			hModule = nullptr;

			return 0;
		}
	}

	FreeLibrary(hModule);
	hModule = nullptr;

	return -1;
}

#endif // _MSC_VER

uint16_t get_cpu_num()
{
	uint16_t ncpu = 1;
#ifdef _MSC_VER
	SYSTEM_INFO sysinfo;
	GetSystemInfo(&sysinfo);
	ncpu = (uint16_t)sysinfo.dwNumberOfProcessors;
#ifdef _DEBUG
	return 1;
#endif
#else
	ncpu = (uint16_t)get_nprocs();
#endif // _MSC_VER
	//ncpu = ncpu * 2; //关闭双倍线程数
	if (ncpu == 0 || ncpu == UINT16_MAX || ncpu == INT16_MAX)
		return 1;
	return ncpu;
}


int get_pid()
{
#ifdef _MSC_VER
    return _getpid();
#else
	return getpid();
#endif // _MSC_VER
}

umap<int32_t, std::string> get_proc_list(const std::string& strCmd)
{
	umap<int32_t, std::string> vRet;

#ifdef _MSC_VER
	HANDLE hSnapshot = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);
	if (INVALID_HANDLE_VALUE == hSnapshot)
		return vRet;

	PROCESSENTRY32 pe = { sizeof(pe) };
	for (bool ret = Process32First(hSnapshot, &pe); ret; ret = Process32Next(hSnapshot, &pe))
	{
		std::string strTmp = pe.szExeFile;
		if (strTmp.find(strCmd) != std::string::npos)
		{
			vRet[(int32_t)pe.th32ProcessID] = strTmp;
		}
	}
	CloseHandle(hSnapshot);

#else
	char szCmd[2048];
	sprintf_safe(szCmd, "ps ax | grep %s | grep -v grep", strCmd.c_str());

	FILE* pFile = nullptr;
	char buff[2048];
	pFile = popen(szCmd, "r");
	if (pFile == nullptr)
		return vRet;

	while (fgets(buff, sizeof(buff), pFile))
	{
		vRet[(int32_t)stoll_x(buff)] = buff;
	}

	pclose(pFile);
#endif // _MSC_VER
	return vRet;
}

bool is_exsit_proc(const std::string& strCmd)
{
	return get_proc_list(strCmd).size() > 0;
}

bool is_listen_port(unsigned long dwPort, bool bTcp /*= true*/)
{
	if (dwPort == 0) return false;

#ifdef _MSC_VER
	return GetProcessIdByPort(dwPort, bTcp) != 0;
#else
	char szCmd[1024];

	//  sprintf_safe(szCmd, "netstat -an | grep :%u", dwPort);

	if (bTcp)
		sprintf_safe(szCmd, "netstat -an | grep :%u | grep LISTEN", dwPort);
	else
		sprintf_safe(szCmd, "netstat -an | grep 0.0.0.0:%u | grep udp", dwPort);

	std::string strRes = run_cmd_res(szCmd);
	if (strRes.size() > 0)
	{
		return true;
	}
	return false;
#endif // _MSC_VER
}

int64_t get_proc_mem()
{
#ifdef _MSC_VER
	HANDLE handle = GetCurrentProcess();
	PROCESS_MEMORY_COUNTERS pmc;
	GetProcessMemoryInfo(handle, &pmc, sizeof(pmc));
	return (int64_t)(pmc.WorkingSetSize) / 1000;
#else
	char file_name[1024] = { 0 };
	FILE* fd;
	char line_buff[1024] = { 0 };
	sprintf(file_name, "/proc/%d/status", get_pid());

	fd = fopen(file_name, "r");
	if (nullptr == fd) {
		return 0;
	}

	char name[1024];
	int vmrss;
	const int VMRSS_LINE = 17;
	for (int i = 0; i < VMRSS_LINE - 1; i++) {
		fgets(line_buff, sizeof(line_buff), fd);
	}

	fgets(line_buff, sizeof(line_buff), fd);
	sscanf(line_buff, "%s %d", name, &vmrss);
	fclose(fd);
	return vmrss;
#endif // _MSC_VER
}

std::string run_cmd_res(const std::string& strCmd)
{
	std::string ret;

#ifndef _MSC_VER
	FILE* pFile = nullptr;
	char buff[1024];
	pFile = popen(strCmd.c_str(), "r");
	if (pFile == nullptr)
		return ret;

	while (fgets(buff, sizeof(buff), pFile))
	{
		ret.append(buff);
	}

	pclose(pFile);
#endif // _MSC_VER

	return ret;
}

void pr_exit(int status)
{
#ifndef _MSC_VER
	if (WIFEXITED(status))
	{
		LOG_INFO("Normal termination, exit status = %d", WEXITSTATUS(status));
	}
	else if (WIFSIGNALED(status))
	{
		LOG_INFO("abnormal termination, exit signed = %d", WTERMSIG(status));
	}
	else if (WIFSTOPPED(status))
	{
		LOG_INFO("child stop, exit signed = %d", WSTOPSIG(status));
	}
#endif // !_MSC_VER
}

bool run_cmd(const std::string& strCmd, bool bFork)
{
#ifndef _MSC_VER
	if (!bFork)
	{
		try
		{
			int status = system(strCmd.c_str());
			if (status < 0)
			{
				LOG_ERROR("system error! status:%d, cmd:%s", status, strCmd.c_str());
			}
			pr_exit(status);
			return status >= 0;
		}
		catch (const std::exception& e)
		{
			LOG_ERROR("system error! error:%s, cmd:%s", e.what(), strCmd.c_str());
			return false;
		}
	}
	else
	{
		if (vfork() == 0)
		{
			execl("/bin/sh", "sh", strCmd.c_str(), nullptr);
			exit(0);
		}
		else
			return false;

		return true;
	}
	return true;
	//     string strRet = run_cmd_res(strCmd);
	//     return strRet.size() > 0;
#else
    STARTUPINFO si;
    PROCESS_INFORMATION pi;

    ZeroMemory(&si, sizeof(si));
    ZeroMemory(&pi, sizeof(pi));

	LPSTR cmdLine = (char*)strCmd.c_str();
	BOOL ret = CreateProcess(file::get_file_name(strCmd).c_str(), cmdLine, NULL, NULL, FALSE, NULL, NULL, NULL, &si, &pi);
	if (ret)
	{
		CloseHandle(pi.hProcess);  
		CloseHandle(pi.hThread); 
	}
	return ret == TRUE;
#endif
}

std::string get_local_ip()
{
	std::string strIp = "127.0.0.1";
#ifdef _MSC_VER
	char szHostName[260] = { 0 };
	int nRetCode;
	hostent* hostinfo = nullptr;
	nRetCode = gethostname(szHostName, sizeof(szHostName));
	if (nRetCode != 0)
	{
		LOG_ERROR("get_local_ip is null!, default use 127.0.0.1");
		return "127.0.0.1";
	}

	hostinfo = gethostbyname(szHostName);
	if (hostinfo == nullptr)
	{
		LOG_ERROR("get_local_ip is null!, default use 127.0.0.1");
		return "127.0.0.1";
	}

	for (size_t i = 0; hostinfo->h_addr_list[i] != nullptr; ++i)
	{
		strIp = inet_ntoa(*(struct in_addr*)(hostinfo->h_addr_list[i]));
		strIp = trim_s(strIp);
		if (strIp.compare("127.0.0.1") != 0)
			break;
	}
	return strIp;
#else
	const size_t MAXINTERFACES = 16;
	char* ip = nullptr;
	int fd, intrface, retn = 0;
	struct ifreq buf[MAXINTERFACES];
	struct ifconf ifc;

	if ((fd = socket(AF_INET, SOCK_DGRAM, 0)) >= 0)
	{
		ifc.ifc_len = sizeof(buf);
		ifc.ifc_buf = (caddr_t)buf;
		if (!ioctl(fd, SIOCGIFCONF, (char*)&ifc))
		{
			intrface = ifc.ifc_len / sizeof(struct ifreq);

			while (intrface-- > 0)
			{
				if (!(ioctl(fd, SIOCGIFADDR, (char*)&buf[intrface])))
				{
					ip = (inet_ntoa(((struct sockaddr_in*)(&buf[intrface].ifr_addr))->sin_addr));
					if (strcmp(ip, "127.0.0.1") != 0)
						break;
				}
			}
		}
		close(fd);
		if (ip != nullptr)
		{
			strIp = ip;
		}
	}
	else
	{
		LOG_ERROR("get_local_ip is null!, default use 127.0.0.1");
	}
	return strIp;
#endif // _MSC_VER
}

std::string get_net_ip()
{
#ifdef _MSC_VER
	return get_local_ip();
#else
	//curl -s ip.cn | grep -oE '([0-9]{1,3}\\.){3}[0-9]{1,3}'
	std::string strShell = file::get_local_path("sh/netip.sh");
	strShell = "sh " + strShell;
	const char* pCmd = strShell.c_str();
	std::string strIp = run_cmd_res(pCmd);
	if (strIp.empty())
	{
		strIp = get_local_ip();
		LOG_ERROR("get_net_ip is null!, default use get_local_ip:%s", strIp.c_str());
		return strIp;
	}
	else
	{
		strIp = trim_s(strIp);
		return strIp;
	}
#endif // _MSC_VER
	return "";
}

int32_t get_local_ip_num(void)
{
    int32_t nIpAddr = 0;

#if defined _MSC_VER
    char szHostName[1024] = { 0 };
    hostent* pHost = nullptr;
    char* pCharIP = nullptr;

    gethostname(szHostName, sizeof(szHostName));

    pHost = gethostbyname(szHostName);
    ERROR_LOG_EXIT0(pHost);
    ERROR_LOG_EXIT0(pHost->h_addr_list[0]);

    pCharIP = inet_ntoa(*(struct in_addr*) pHost->h_addr_list[0]);
    nIpAddr = inet_addr(pCharIP);

#else
    int inet_sock;
    struct ifreq ifr;

    inet_sock = socket(AF_INET, SOCK_DGRAM, 0);
    ERROR_LOG_EXIT0(inet_sock > 0);

	strncpy_safe(ifr.ifr_name, "eth1", strlen("eth1"));
    ioctl(inet_sock, SIOCGIFADDR, &ifr);
    nIpAddr = *(uint32_t*) (&(((struct sockaddr_in*) &ifr.ifr_addr)->sin_addr));

    close(inet_sock);
#endif
Exit0:
    return nIpAddr;
}