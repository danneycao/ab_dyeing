#ifndef __INV_MYSQL_H
#define __INV_MYSQL_H

#include "mysql.h"
#include "util/inv_ex.h"
#include <map>
#include <vector>
#include <stdlib.h>

namespace inv
{

/////////////////////////////////////////////////
/** 
* @file  inv_mysql.h 
* @brief mysql������. 
*  
*/           
/////////////////////////////////////////////////


/**
* @brief ���ݿ��쳣��
*/
struct INV_Mysql_Exception : public INV_Exception
{
    INV_Mysql_Exception(const string &sBuffer) : INV_Exception(sBuffer){};
    ~INV_Mysql_Exception() throw(){};    
};

/**
* @brief ���ݿ����ýӿ�
*/
struct INV_DBConf
{
    /**
    * ������ַ
    */
    string _host;

    /**
    * �û���
    */
    string _user;

    /**
    * ����
    */
    string _password;

    /**
    * ���ݿ�
    */
    string _database;

    /**
     * �ַ���
     */
    string _charset;

    /**
    * �˿�
    */
    int _port;

    /**
    * �ͻ��˱�ʶ
    */
    int _flag;

    /**
    * @brief ���캯��
    */
    INV_DBConf()
        : _port(0)
        , _flag(0)
    {
    }

    /**
    * @brief ��ȡ���ݿ�����. 
    * 
    * @param mpParam ������ݿ����õ�map 
    *        dbhost: ������ַ
    *        dbuser:�û���
    *        dbpass:����
    *        dbname:���ݿ�����
    *        dbport:�˿�
    */
    void loadFromMap(const map<string, string> &mpParam)
    {
        map<string, string> mpTmp = mpParam;

        _host        = mpTmp["dbhost"];
        _user        = mpTmp["dbuser"];
        _password    = mpTmp["dbpass"];
        _database    = mpTmp["dbname"];
        _charset     = mpTmp["charset"];
        _port        = atoi(mpTmp["dbport"].c_str());
        _flag        = 0;

        if(mpTmp["dbport"] == "")
        {
            _port = 3306;
        }
    }

};

/**
* @brief Mysql���ݿ������ 
* 
* ���̰߳�ȫ��ͨ��һ���߳�һ��INV_Mysql����
* 
* ����insert/update�����и��õĺ�����װ����֤SQLע�룻
* 
* INV_Mysql::DB_INT��ʾ��װsql���ʱ�����ӡ�����ת�壻
* 
* INV_Mysql::DB_STR��ʾ��װsql���ʱ���ӡ�����ת�壻
*/
class INV_Mysql 
{
public:
    /**
    * @brief ���캯��
    */
    INV_Mysql();

    /**
    * @brief ���캯��. 
    *  
    * @param sHost        ����IP
    * @param sUser        �û�
    * @param sPasswd      ����
    * @param sDatebase    ���ݿ�
    * @param port         �˿�
    * @param iUnixSocket  socket
    * @param iFlag        �ͻ��˱�ʶ
    */
    INV_Mysql(const string& sHost, const string& sUser = "", const string& sPasswd = "", const string& sDatabase = "", const string &sCharSet = "", int port = 0, int iFlag = 0);

    /**
    * @brief ���캯��. 
    * @param tcDBConf ���ݿ�����
    */
    INV_Mysql(const INV_DBConf& tcDBConf);

    /**
    * @brief ��������.
    */
    ~INV_Mysql();

    /**
    * @brief ��ʼ��. 
    *  
    * @param sHost        ����IP
    * @param sUser        �û�
    * @param sPasswd      ����
    * @param sDatebase    ���ݿ�
    * @param port         �˿�
    * @param iUnixSocket  socket
    * @param iFlag        �ͻ��˱�ʶ
    * @return ��
    */
    void init(const string& sHost, const string& sUser  = "", const string& sPasswd  = "", const string& sDatabase = "", const string &sCharSet = "", int port = 0, int iFlag = 0);

    /**
    * @brief ��ʼ��. 
    *  
    * @param tcDBConf ���ݿ�����
    */
    void init(const INV_DBConf& tcDBConf);

    /**
    * @brief �������ݿ�. 
    *  
    * @throws INV_Mysql_Exception
    * @return ��
    */
    void connect();
 
    /**
    * @brief �Ͽ����ݿ�����. 
    * @return ��
    */
    void disconnect();

    /**
     * @brief ��ȡ���ݿ����. 
     * 
     * @return ���ݿ����
     */
    string getVariables(const string &sName);

    /**
    *  @brief ֱ�ӻ�ȡ���ݿ�ָ��. 
    *  
    * @return MYSQL* ���ݿ�ָ��
    */
    MYSQL *getMysql();

    /**
    *  @brief �ַ�ת��. 
    *  
    * @param sFrom  Դ�ַ���
    * @param sTo    ����ַ���
    * @return       ����ַ���
    */
    string escapeString(const string& sFrom);

    /**
    * @brief ���»��߲�������. 
    *  
    * @param sSql  sql���
    * @throws      INV_Mysql_Exception
    * @return
    */
    void execute(const string& sSql);

    /**
     *  @brief mysql��һ����¼
     */
    class MysqlRecord
    {
    public:
        /**
         * @brief ���캯��.
         *  
         * @param record
         */
        MysqlRecord(const map<string, string> &record);

        /**
         * @brief ��ȡ���ݣ�sһ����ָ���ݱ��ĳ���ֶ��� 
         * @param s Ҫ��ȡ���ֶ�
         * @return  ���ϲ�ѯ�����ļ�¼��s�ֶ���
         */
        const string& operator[](const string &s);
    protected:
        const map<string, string> &_record;
    };

    /**
     * @brief ��ѯ������mysql����
     */
    class MysqlData
    {
    public:
        /**
         * @brief ��������.
         * 
         * @return vector<map<string,string>>&
         */
        vector<map<string, string> >& data();

        /**
         * ���ݵļ�¼����
         * 
         * @return size_t
         */
        size_t size();

        /**
         * @brief ��ȡĳһ����¼. 
         *  
         * @param i  Ҫ��ȡ�ڼ�����¼ 
         * @return   MysqlRecord���͵����ݣ����Ը����ֶλ�ȡ�����Ϣ��
         */
        MysqlRecord operator[](size_t i);

    protected:
        vector<map<string, string> > _data;
    };

    /**
    * @brief Query Record. 
    *  
    * @param sSql sql���
    * @throws     INV_Mysql_Exception
    * @return     MysqlData���͵����ݣ����Ը����ֶλ�ȡ�����Ϣ
    */
    MysqlData queryRecord(const string& sSql);

    /**
     * @brief �����ֶ����ͣ� 
     *  DB_INT:�������� 
     *  DB_STR:�ַ�������
     */
    enum FT
    {
        DB_INT,     
        DB_STR,    
    };

     /**
     * ���ݼ�¼
     */
    typedef map<string, pair<FT, string> > RECORD_DATA;

    /**
    * @brief ���¼�¼. 
    *  
    * @param sTableName ����
    * @param mpColumns  ����/ֵ��
    * @param sCondition where�����,����:where A = B
    * @throws           INV_Mysql_Exception
    * @return           size_t Ӱ�������
    */
    size_t updateRecord(const string &sTableName, const map<string, pair<FT, string> > &mpColumns, const string &sCondition);

    /**
    * @brief �����¼. 
    *  
    * @param sTableName  ����
    * @param mpColumns  ����/ֵ��
    * @throws           INV_Mysql_Exception
    * @return           size_t Ӱ�������
    */
    size_t insertRecord(const string &sTableName, const map<string, pair<FT, string> > &mpColumns);

    /**
    * @brief �滻��¼. 
    *  
    * @param sTableName  ����
    * @param mpColumns   ����/ֵ��
    * @throws            INV_Mysql_Exception
    * @return            size_t Ӱ�������
    */
    size_t replaceRecord(const string &sTableName, const map<string, pair<FT, string> > &mpColumns);

    /**
    * @brief ɾ����¼.  
    *  
    * @param sTableName   ����
    * @param sCondition   where�����,����:where A = B
    * @throws             INV_Mysql_Exception
    * @return             size_t Ӱ�������
    */
    size_t deleteRecord(const string &sTableName, const string &sCondition = "");

    /**
    * @brief ��ȡTable��ѯ�������Ŀ. 
    *  
    * @param sTableName ���ڲ�ѯ�ı���
    * @param sCondition where�����,����:where A = B
    * @throws           INV_Mysql_Exception
    * @return           size_t ��ѯ�ļ�¼��Ŀ
    */
    size_t getRecordCount(const string& sTableName, const string &sCondition = "");

    /**
    * @brief ��ȡSql���ؽ�����ĸ���. 
    *  
    * @param sCondition where�����,����:where A = B
    * @throws           INV_Mysql_Exception
    * @return           ��ѯ�ļ�¼��Ŀ
    */
    size_t getSqlCount(const string &sCondition = "");

    /**
     * @brief ���ڼ�¼. 
     *  
     * @param sql  sql���
     * @throws     INV_Mysql_Exception
     * @return     �����Ƿ�ɹ�
     */
    bool existRecord(const string& sql);

    /**
    * @brief ��ȡ�ֶ����ֵ. 
    *  
    * @param sTableName ���ڲ�ѯ�ı���
    * @param sFieldName ���ڲ�ѯ���ֶ�
    * @param sCondition where�����,����:where A = B
    * @throws           INV_Mysql_Exception
    * @return           ��ѯ�ļ�¼��Ŀ
    */
    int getMaxValue(const string& sTableName, const string& sFieldName, const string &sCondition = "");

    /**
    * @brief ��ȡauto_increment�������ID. 
    *  
    * @return IDֵ
    */
    long lastInsertID();

    /**
    * @brief ����Insert-SQL���. 
    *  
    * @param sTableName  ����
    * @param mpColumns  ����/ֵ��
    * @return           string insert-SQL���
    */
    string buildInsertSQL(const string &sTableName, const map<string, pair<FT, string> > &mpColumns);

    /**
    * @brief ����Replace-SQL���. 
    *  
    * @param sTableName  ����
    * @param mpColumns ����/ֵ��
    * @return           string insert-SQL���
    */
    string buildReplaceSQL(const string &sTableName, const map<string, pair<FT, string> > &mpColumns);

    /**
    * @brief ����Update-SQL���. 
    *  
    * @param sTableName  ����
    * @param mpColumns   ����/ֵ��
    * @param sCondition  where�����
    * @return            string Update-SQL���
    */
    string buildUpdateSQL(const string &sTableName,const map<string, pair<FT, string> > &mpColumns, const string &sCondition);

    /**
     * @brief ��ȡ���ִ�е�SQL���.
     * 
     * @return SQL���
     */
    string getLastSQL() { return _sLastSql; }

    /**
     * @brief ��ȡ��ѯӰ����
     * @return int
     */
     size_t getAffectedRows();
protected:
    /**
    * @brief copy contructor��ֻ����,������,��֤����ʹ�� 
    */
    INV_Mysql(const INV_Mysql &tcMysql);

    /**
    * 
    * @brief ֻ����,������,��֤����ʹ��
    */
    INV_Mysql &operator=(const INV_Mysql &tcMysql);


private:

    /**
    * ���ݿ�ָ��
    */
    MYSQL       *_pstMql;

    /**
    * ���ݿ�����
    */
    INV_DBConf   _dbConf;
    
    /**
    * �Ƿ��Ѿ�����
    */
    bool        _bConnected;

    /**
     * ���ִ�е�sql
     */
    string      _sLastSql;
  
};

}
#endif //_INV_MYSQL_H
