#ifndef __INV_PGSQL_H
#define __INV_PGSQL_H

#include "util/inv_ex.h"
#include <libpq-fe.h>
#include <map>
#include <vector>
#include <stdlib.h>

namespace inv
{

/////////////////////////////////////////////////
/** 
* @file  inv_pgsql.h 
* @brief postgresql操作类. 
*  
*/           
/////////////////////////////////////////////////


/**
* @brief 数据库异常类
*/
struct INV_Pgsql_Exception : public INV_Exception
{
    INV_Pgsql_Exception(const string &sBuffer) : INV_Exception(sBuffer){};
    ~INV_Pgsql_Exception() throw(){};    
};

/**
* @brief 数据库配置接口
*/
struct INV_PgConf
{
    /**
    * 主机地址
    */
    string _host;

    /**
    * 端口
    */
    string _port;

    /**
    * 用户名
    */
    string _user;

    /**
    * 密码
    */
    string _password;

    /**
    * 数据库
    */
    string _database;

    /**
     * 选项
     */
    string _options;

    /**
    * tty
    */
    string _tty;

    /**
    * @brief 读取数据库配置. 
    * 
    * @param mpParam 存放数据库配置的map 
    *        dbhost: 主机地址
    *        dbuser:用户名
    *        dbpass:密码
    *        dbname:数据库名称
    *        dbport:端口
    */
    void loadFromMap(const map<string, string> &mpParam)
    {
        map<string, string> mpTmp = mpParam;

        _host        = mpTmp["dbhost"];
        _port        = mpTmp["dbport"];
        _user        = mpTmp["dbuser"];
        _password    = mpTmp["dbpass"];
        _database    = mpTmp["dbname"];
        _options     = mpTmp["options"];
        _tty         = mpTmp["tty"];

        if(mpTmp["dbport"] == "")
        {
            _port = "5432";
        }
    }

};

/**
* @brief Pgsql数据库操作类 
* 
* 非线程安全，通常一个线程一个INV_Pgsql对象；
* 
* 对于insert/update可以有更好的函数封装，保证SQL注入；
* 
* INV_Pgsql::DB_INT表示组装sql语句时，不加””和转义；
* 
* INV_Pgsql::DB_STR表示组装sql语句时，加””并转义；
*/
class INV_Pgsql 
{
public:
    /**
    * @brief 构造函数
    */
    INV_Pgsql();

    /**
    * @brief 构造函数. 
    *  
    * @param sHost        主机IP
    * @param sPort        端口
    * @param sUser        用户
    * @param sPasswd      密码
    * @param sDatebase    数据库
    * @param sOptions     选项
    * @param sTty         tty
    */
    INV_Pgsql(const string& sHost, 
              const string& sPort = "", 
              const string& sUser = "", 
              const string& sPasswd = "", 
              const string& sDatabase = "", 
              const string& sOptions = "", 
              const string& sTty = "");

    /**
    * @brief 构造函数. 
    * @param conf 数据库配置
    */
    INV_Pgsql(const INV_PgConf& conf);

    /**
    * @brief 析构函数.
    */
    ~INV_Pgsql();

    /**
    * @brief 初始化. 
    *  
    * @param sHost        主机IP
    * @param sPort        端口
    * @param sUser        用户
    * @param sPasswd      密码
    * @param sDatebase    数据库
    * @param sOptions     选项
    * @param sTty         tty
    * @return 无
    */
    void init(const string& sHost, 
              const string& sPort = "", 
              const string& sUser = "", 
              const string& sPasswd = "", 
              const string& sDatabase = "", 
              const string& sOptions = "", 
              const string& sTty = "");

    /**
    * @brief 初始化. 
    *  
    * @param conf 数据库配置
    */
    void init(const INV_PgConf& conf);

    /**
    * @brief 连接数据库. 
    *  
    * @throws INV_Pgsql_Exception
    * @return 无
    */
    void connectIfNot();
 
    /**
    * @brief 断开数据库连接. 
    * @return 无
    */
    void disconnect();

    /**
     *  @brief pgsql的一条记录
     */
    class PgsqlRecord
    {
    public:
        /**
         * @brief 构造函数.
         *  
         * @param record
         */
        PgsqlRecord(const map<string, string> &record);

        /**
         * @brief 获取数据，s一般是指数据表的某个字段名 
         * @param s 要获取的字段
         * @return  符合查询条件的记录的s字段名
         */
        const string& operator[](const string &s) const;
    protected:
        const map<string, string> &_record;
    };

    /**
     * @brief 查询出来的pgsql数据
     */
    class PgsqlData
    {
    public:
        /**
         * @brief 所有数据.
         * 
         * @return vector<map<string,string>>&
         */
        vector<map<string, string> >& data();

        /**
         * 数据的记录条数
         * 
         * @return size_t
         */
        size_t size();

        /**
         * @brief 获取某一条记录. 
         *  
         * @param i  要获取第几条记录 
         * @return   PgsqlRecord类型的数据，可以根据字段获取相关信息，
         */
        PgsqlRecord operator[](size_t i) const;

    protected:
        vector<map<string, string> > _data;
    };

    /**
    * @brief Query Record. 
    *  
    * @param sSql sql语句
    * @throws     INV_Pgsql_Exception
    * @return     PgsqlData类型的数据，可以根据字段获取相关信息
    */
    PgsqlData queryRecord(const string& sSql);

    /**
     * @brief 定义字段类型， 
     *  DB_INT:数字类型 
     *  DB_STR:字符串类型
     */
    enum FT
    {
        DB_INT,     
        DB_STR,    
    };

     /**
     * 数据记录
     */
    typedef map<string, pair<FT, string> > RECORD_DATA;

protected:
    /**
    * @brief copy contructor，只申明,不定义,保证不被使用 
    */
    INV_Pgsql(const INV_Pgsql &pgsql);

    /**
    * 
    * @brief 只申明,不定义,保证不被使用
    */
    INV_Pgsql &operator=(const INV_Pgsql &pgsql);


private:

    /**
    * 数据库连接
    */
    PGconn *_conn;

    /**
    * 数据库配置
    */
    INV_PgConf   _conf;

    /**
     * 最后执行的sql
     */
    string      _sLastSql;
  
};

}
#endif //_INV_PGSQL_H
