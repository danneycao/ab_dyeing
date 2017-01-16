#include "util/inv_ex.h"
#include <execinfo.h>
#include <string>
#include <string.h>
#include <stdlib.h>
#include <cerrno>

namespace inv
{

INV_Exception::INV_Exception(const string &buffer)
:_buffer(buffer), _code(0)
{
//    getBacktrace();
}

INV_Exception::INV_Exception(const string &buffer, int err)
{
	_buffer = buffer + string(" :") + strerror(err);
    _code   = err;
//    getBacktrace();
}

INV_Exception::~INV_Exception() throw()
{
}

const char* INV_Exception::what() const throw()
{
    return _buffer.c_str();
}

void INV_Exception::getBacktrace()
{
    void * array[64];
    int nSize = backtrace(array, 64);
    char ** symbols = backtrace_symbols(array, nSize);

    for (int i = 0; i < nSize; i++)
    {
        _buffer += symbols[i];
        _buffer += "\n";
    }
	free(symbols);
}

}
