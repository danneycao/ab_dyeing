#include "util/inv_pgsql.h"
#include <string.h>

namespace inv
{

INV_Pgsql::INV_Pgsql()
    :_conn(NULL)
{
}

INV_Pgsql::INV_Pgsql(const string& sHost, 
                     const string& sPort, 
                     const string& sUser, 
                     const string& sPasswd, 
                     const string& sDatabase, 
                     const string& sOptions, 
                     const string& sTty)
    : _conn(NULL)
{
    init(sHost, sPort, sUser, sPasswd, sDatabase, sOptions, sTty);
}

INV_Pgsql::INV_Pgsql(const INV_PgConf& conf)
    : _conn(NULL), _conf(conf)
{
}

INV_Pgsql::~INV_Pgsql()
{
    disconnect();
}

void INV_Pgsql::init(const string& sHost, 
                     const string& sPort, 
                     const string& sUser, 
                     const string& sPasswd, 
                     const string& sDatabase, 
                     const string& sOptions, 
                     const string& sTty)
{
    _conf._host = sHost;
    _conf._port = sPort;
    _conf._user = sUser;
    _conf._password = sPasswd;
    _conf._database = sDatabase;
    _conf._options = sOptions;
    _conf._tty = sTty;
}

void INV_Pgsql::init(const INV_PgConf& conf)
{
    _conf = conf;
}

void INV_Pgsql::connectIfNot()
{
    if (PQstatus(_conn) == CONNECTION_OK) return;

    disconnect();
    _conn = PQsetdbLogin(_conf._host.c_str(), 
                         _conf._port.c_str(),
                         _conf._options.c_str(),
                         _conf._tty.c_str(),
                         _conf._database.c_str(),
                         _conf._user.c_str(),
                         _conf._password.c_str());

    if (PQstatus(_conn) != CONNECTION_OK)
    {
        throw INV_Pgsql_Exception(string("INV_Pgsql::connectIfNot: PQsetdbLogin: ") 
                + PQerrorMessage(_conn));
    }

    return;
}

void INV_Pgsql::disconnect()
{
    PQfinish(_conn);
    _conn = NULL;
}


INV_Pgsql::PgsqlData INV_Pgsql::queryRecord(const string& sSql)
{
    connectIfNot();

    _sLastSql = sSql;

    PGresult *res = PQexec(_conn, sSql.c_str());
    if (PQresultStatus(res) != PGRES_TUPLES_OK)
    {
        PQclear(res);
        throw INV_Pgsql_Exception(string("INV_Pgsql::queryRecord: PQexec: ") 
                + PQerrorMessage(_conn) + "\n" + PQresultErrorMessage(res));
    }

    int rowNum = PQntuples(res);
    int colNum = PQnfields(res);
    vector<string> colNames;
    PgsqlData   data;
    for (int i = 0; i < colNum; i++)
    {
        colNames.push_back(PQfname(res, i));
    }
    for (int j = 0; j < rowNum; j++)
    {
        map<string, string> row;
        for (int k = 0; k < colNum; k++)
        {
            row[colNames[k]] = PQgetvalue(res, j, k);
        }
        data.data().push_back(row);
    }

    PQclear(res);

    return data;
}

//////////////////////////////////////////////////////////////////////////////////////////////////////////////
INV_Pgsql::PgsqlRecord::PgsqlRecord(const map<string, string> &record)
: _record(record)
{
}

const string& INV_Pgsql::PgsqlRecord::operator[](const string &s) const
{
    map<string, string>::const_iterator it = _record.find(s);
    if(it == _record.end())
    {
        throw INV_Pgsql_Exception("field '" + s + "' not exists.");
    }
    return it->second;
}

//////////////////////////////////////////////////////////////////////////////////////////////////////////////

vector<map<string, string> >& INV_Pgsql::PgsqlData::data()
{
    return _data;
}

size_t INV_Pgsql::PgsqlData::size()
{
    return _data.size();
}

INV_Pgsql::PgsqlRecord INV_Pgsql::PgsqlData::operator[](size_t i) const 
{
    return _data[i];
}

}

