#ifndef __INV_EX_H
#define __INV_EX_H

#include <stdexcept>
#include <string>
using namespace std;

namespace inv
{

class INV_Exception : public exception
{
public:
    /**
	 * @brief 构造函数，提供了一个可以传入errno的构造函数， 
	 *  
	 *  	  异常抛出时直接获取的错误信息
	 *  
	 * @param buffer 异常的告警信息 
     */
	explicit INV_Exception(const string &buffer);

    /**
	 * @brief 构造函数,提供了一个可以传入errno的构造函数， 
	 *  
	 *  	  异常抛出时直接获取的错误信息
	 *  
     * @param buffer 异常的告警信息 
     * @param err    错误码, 可用strerror获取错误信息
     */
	INV_Exception(const string &buffer, int err);

    /**
     * @brief 析够数函
     */
    virtual ~INV_Exception() throw();

    /**
     * @brief 错误信息.
     *
     * @return const char*
     */
    virtual const char* what() const throw();

    /**
     * @brief 获取错误码
     * 
     * @return 成功获取返回0
     */
    int getErrCode() { return _code; }

private:
    void getBacktrace();

private:
    /**
	 * 异常的相关信息
     */
    string  _buffer;

	/**
	 * 错误码
     */
    int     _code;

};

}
#endif

