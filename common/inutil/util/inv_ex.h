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
	 * @brief ���캯�����ṩ��һ�����Դ���errno�Ĺ��캯���� 
	 *  
	 *  	  �쳣�׳�ʱֱ�ӻ�ȡ�Ĵ�����Ϣ
	 *  
	 * @param buffer �쳣�ĸ澯��Ϣ 
     */
	explicit INV_Exception(const string &buffer);

    /**
	 * @brief ���캯��,�ṩ��һ�����Դ���errno�Ĺ��캯���� 
	 *  
	 *  	  �쳣�׳�ʱֱ�ӻ�ȡ�Ĵ�����Ϣ
	 *  
     * @param buffer �쳣�ĸ澯��Ϣ 
     * @param err    ������, ����strerror��ȡ������Ϣ
     */
	INV_Exception(const string &buffer, int err);

    /**
     * @brief ��������
     */
    virtual ~INV_Exception() throw();

    /**
     * @brief ������Ϣ.
     *
     * @return const char*
     */
    virtual const char* what() const throw();

    /**
     * @brief ��ȡ������
     * 
     * @return �ɹ���ȡ����0
     */
    int getErrCode() { return _code; }

private:
    void getBacktrace();

private:
    /**
	 * �쳣�������Ϣ
     */
    string  _buffer;

	/**
	 * ������
     */
    int     _code;

};

}
#endif

