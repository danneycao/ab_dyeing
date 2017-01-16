#ifndef __INV_FILE_H_
#define __INV_FILE_H_

#include <iostream>
#include <fstream>
#include <unistd.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <dirent.h>
#include <fnmatch.h>
#include "util/inv_ex.h"
#include "util/inv_util.h"

namespace inv
{

struct INV_File_Exception : public INV_Exception
{
    INV_File_Exception(const string &buffer) : INV_Exception(buffer){};
    INV_File_Exception(const string &buffer, int err) : INV_Exception(buffer, err){};
    ~INV_File_Exception() throw(){};
};


class INV_File
{
public:

    static int removeFile(const string &sFullFileName, bool bRecursive=true);

    static string load2str(const string &sFullFileName);

    static void save2file(const string &sFullFileName, const string &sFileData);

    static int save2file(const string &sFullFileName, const char *sFileData, size_t length);

    static ifstream::pos_type getFileSize(const string &sFullFileName);
    
    static bool isFileExist(const string &sFullFileName, mode_t iFileType = S_IFREG);
    
    static bool isFileExistEx(const string &sFullFileName, mode_t iFileType = S_IFREG);
    
    static string simplifyDirectory(const string& path);
 
    static bool makeDir(const string &sDirectoryPath, mode_t iFlag = S_IRUSR | S_IWUSR | S_IXUSR | S_IRGRP | S_IXGRP | S_IROTH |  S_IXOTH);

    static bool makeDirRecursive(const string &sDirectoryPath, mode_t iFlag = S_IRUSR | S_IWUSR | S_IXUSR | S_IRGRP | S_IXGRP | S_IROTH |  S_IXOTH);

    static string extractFileName(const string &sFullFileName);

    static string extractFilePath(const string &sFullFileName);

    typedef int (*FILE_SELECT)(const dirent *);

    static size_t scanDir(const string &sFilePath, vector<string> &vtMatchFiles, FILE_SELECT f, int iMaxSize );

    static void listDirectory(const string &path, vector<string> &files, bool bRecursive);

};

}
#endif // INV_FILE_H
