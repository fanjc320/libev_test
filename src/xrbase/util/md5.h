#pragma once

#include <fstream>
#ifndef _MSC_VER
#include <string.h>
#else
#include <string>
#endif
#include <ctype.h>

/* Type define */
#ifndef byte
typedef unsigned char byte;
#endif // !byte
#ifndef uint32
typedef unsigned int uint32;
#endif

using std::string;
using std::ifstream;

/* MD5 declaration. */
class MD5 {
public:
	MD5();
	MD5(const void* input, size_t length);
	MD5(const string& str);
	MD5(ifstream& in);
	void update(const void* input, size_t length);
	void update(const string& str);
	void update(ifstream& in);
	const unsigned char* digest();
	string toString();
	void reset();
private:
	void update(const unsigned char* input, size_t length);
	void final();
	void transform(const unsigned char block[64]);
	void encode(const uint32* input, unsigned char* output, size_t length);
	void decode(const unsigned char* input, uint32* output, size_t length);
	string bytesToHexString(const unsigned char* input, size_t length);

	/* class uncopyable */
	MD5(const MD5&);
private:
	uint32 _state[4];	/* state (ABCD) */
	uint32 _count[2];	/* number of bits, modulo 2^64 (low-order word first) */
	unsigned char _buffer[64];	/* input buffer */
	unsigned char _digest[16];	/* message digest */
	bool _finished;		/* calculate finished ? */

	static const unsigned char PADDING[64];	/* padding for calculate */
	static const char HEX[16];
	//const static size_t BUFFER_SIZE;
};