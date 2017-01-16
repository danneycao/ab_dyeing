#include "util/inv_file.h"
#include <string.h>

namespace inv
{

ifstream::pos_type INV_File::getFileSize(const string &sFullFileName)
{
    ifstream ifs(sFullFileName.c_str());
    ifs.seekg(0, ios_base::end);
    return ifs.tellg();
}

string INV_File::load2str(const string &sFullFileName)
{
    ifstream ifs(sFullFileName.c_str());

    return string(istreambuf_iterator<char>(ifs), istreambuf_iterator<char>());
}

void INV_File::save2file(const string &sFullFileName, const string &sFileData)
{
    ofstream ofs((sFullFileName).c_str());
    ofs << sFileData;
    ofs.close();
}

int INV_File::save2file(const string &sFullFileName, const char *sFileData, size_t length)
{
    FILE *fp = fopen(sFullFileName.c_str(), "wb");
    if(fp == NULL)
    {
        return -1;
    }

    size_t ret = fwrite((void*)sFileData, 1, length, fp);
    fclose(fp);

    if(ret == length)
    {
        return 0;
    }
    return -1;
}

size_t INV_File::scanDir(const string &sFilePath, vector<string> &vtMatchFiles, FILE_SELECT f, int iMaxSize )
{
    vtMatchFiles.clear();

    struct dirent **namelist;
    int n = scandir(sFilePath.c_str(), &namelist, f, alphasort);

    if (n < 0)
    {
        return 0;
    }
    else
    {
        while(n-- )
        {
            if(iMaxSize > 0 && vtMatchFiles.size() >= (size_t)iMaxSize )
            {
                free(namelist[n]);
                break;
            }
            else
            {
                vtMatchFiles.push_back(namelist[n]->d_name);
                free(namelist[n]);
            }
        }
        free(namelist);
    }

    return vtMatchFiles.size();
}

void INV_File::listDirectory(const string &path, vector<string> &files, bool bRecursive)
{
    vector<string> tf;
    scanDir(path, tf, 0, 0);

    for(size_t i = 0; i < tf.size(); i++)
    {
        if(tf[i] == "." || tf[i] == "..")
            continue;

        string s = path + "/" + tf[i];

        if(isFileExist(s, S_IFDIR))
        {
            files.push_back(simplifyDirectory(s));
            if(bRecursive)
            {
                listDirectory(s, files, bRecursive);
            }
        }
        else
        {
            files.push_back(simplifyDirectory(s));
        }
    }
}

int INV_File::removeFile(const string &sFullFileName, bool bRecursive)
{
    string path = simplifyDirectory(sFullFileName);

    if(isFileExist(path, S_IFDIR))
    {
        if(bRecursive)
        {
            vector<string> files;
            listDirectory(path, files, false);
            for(size_t i = 0; i < files.size(); i++)
            {
                removeFile(files[i], bRecursive);
            }

            if(path != "/")
            {
                if(::rmdir(path.c_str()) == -1)
                {
                    return -1;
                }
                return 0;
            }
        }
        else
        {
            if(::rmdir(path.c_str()) == -1)
            {
                return -1;
            }
        }
    }
    else
    {
        if(::remove(path.c_str()) == -1)
        {
            return -1;
        }
    }

    return 0;
}

bool INV_File::isFileExist(const string &sFullFileName, mode_t iFileType)
{
    struct stat f_stat;

    if (lstat(sFullFileName.c_str(), &f_stat) == -1)
    {
        return false;
    }

    if (!(f_stat.st_mode & iFileType))
    {
        return false;
    }

    return true;
}

bool INV_File::isFileExistEx(const string &sFullFileName, mode_t iFileType)
{
    struct stat f_stat;

    if (stat(sFullFileName.c_str(), &f_stat) == -1)
    {
        return false;
    }

    if (!(f_stat.st_mode & iFileType))
    {
        return false;
    }

    return true;
}

bool INV_File::makeDir(const string &sDirectoryPath, mode_t iFlag)
{
    int iRetCode = mkdir(sDirectoryPath.c_str(), iFlag);
    if(iRetCode < 0 && errno == EEXIST)
    {
        return isFileExistEx(sDirectoryPath, S_IFDIR);
    }

    return iRetCode == 0;
}

bool INV_File::makeDirRecursive(const string &sDirectoryPath, mode_t iFlag)
{
    string simple = simplifyDirectory(sDirectoryPath);

    string::size_type pos = 0;
    for(; pos != string::npos; )
    {
        pos = simple.find("/", pos + 1);
        string s;
        if(pos == string::npos)
        {
            s = simple.substr(0, simple.size());
            return makeDir(s.c_str(), iFlag);
        }
        else
        {
            s = simple.substr(0, pos);
            if(!makeDir(s.c_str(), iFlag)) return false;
        }
    }
    return true;
}

string INV_File::simplifyDirectory(const string& path)
{
    string result = path;

    string::size_type pos;

    pos = 0;
    while((pos = result.find("//", pos)) != string::npos)
    {
        result.erase(pos, 1);
    }

    pos = 0;
    while((pos = result.find("/./", pos)) != string::npos)
    {
        result.erase(pos, 2);
    }

    while(result.substr(0, 4) == "/../")
    {
        result.erase(0, 3);
    }

    if(result == "/.")
    {
       return result.substr(0, result.size() - 1);
    }

    if(result.size() >= 2 && result.substr(result.size() - 2, 2) == "/.")
    {
        result.erase(result.size() - 2, 2);
    }

    if(result == "/")
    {
        return result;
    }

    if(result.size() >= 1 && result[result.size() - 1] == '/')
    {
        result.erase(result.size() - 1);
    }

    if(result == "/..")
    {
        result = "/";
    }

    return result;
}

string INV_File::extractFileName(const string &sFullFileName)
{
    if(sFullFileName.length() <= 0)
    {
        return "";
    }

    string::size_type pos = sFullFileName.rfind('/');
    if(pos == string::npos)
    {
        return sFullFileName;
    }

    return sFullFileName.substr(pos + 1);
}

string INV_File::extractFilePath(const string &sFullFileName)
{
    if(sFullFileName.length() <= 0)
    {
        return "./";
    }

    string::size_type pos = 0;

    for(pos = sFullFileName.length(); pos != 0 ; --pos)
    {
        if(sFullFileName[pos-1] == '/')
        {
            return sFullFileName.substr(0, pos);
        }
    }

    return "./";
}


}

