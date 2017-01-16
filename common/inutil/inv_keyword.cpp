#include <iostream>
#include <fstream>
#include <string>
#include "util/inv_keyword.h"
#include "util/inv_encoder.h"

using namespace std;

namespace inv
{

#define DCHAR2CODE(lo,hi) ((((unsigned char)(lo))<<8)+((unsigned char)(hi)))

void KeyWordSearch::Init(int nodenum)throw()
{
    _KeyWordTreeRoot.clear();
    _KeyWordTreeRoot.init(nodenum);
}

void KeyWordSearch::AddKeyWordUtf8(const string &keyword)
{
    AddKeyWord(INV_Encoder::utf82gbk(keyword).c_str(), "find", 1, 1, 1);
}
    
int KeyWordSearch::CheckUtf8(const string &content)
{
    string strgbk = INV_Encoder::utf82gbk(content);
    vector<stCheckRes> vtres;
    Check(strgbk.c_str(), 1, vtres);
    return vtres.size();
}

void KeyWordSearch::AddKeyWord(const char* buf, const char* dst, int iReplace, char level, int type)throw(runtime_error)
{
    unsigned pos=0, len=strlen(buf);

    ItemHandle beginnode=0, hnode=0;

    for(unsigned i=0;i<len;)
    {
        char s[3] = {0, 0, 0};
        s[0] = buf[i];
        if ((unsigned)buf[i] > 0x80)
        {
            s[1]=buf[i+1];
            pos = DCHAR2CODE(buf[i],  buf[i+1]);
            i += 2;
        }
        else
        {
            pos = (unsigned)buf[i];
            i++;
        }
        SyntaxTreeNode& cur = (_KeyWordTreeRoot.node)[beginnode];
        if (!cur.position)
        {
             cur.position=new map<unsigned, unsigned>;
        }
        map<unsigned, unsigned>& mp=*(cur.position);
        if (mp.find(pos) == mp.end())
        {
            hnode=_KeyWordTreeRoot.getFreeNode();
            if (hnode < 1)
            {
                throw runtime_error("couldn't found freenode");
            }
            mp[pos] = hnode;
        }
        else
        {
            hnode = mp[pos];
        }

        beginnode=hnode;
    }

    (_KeyWordTreeRoot.node)[hnode].level=level;
    (_KeyWordTreeRoot.node)[hnode].iReplace = iReplace;
    (_KeyWordTreeRoot.node)[hnode].type = type;

    if((_KeyWordTreeRoot.node)[hnode].content == NULL)
    {
        (_KeyWordTreeRoot.node)[hnode].content = new string(buf);
    }

    if((_KeyWordTreeRoot.node)[hnode].replaceContent == NULL)
    {
        (_KeyWordTreeRoot.node)[hnode].replaceContent = new string(dst);
    }
}

void KeyWordSearch::Compile(const char* buf, vector<stSyntax>& destbuf)throw()
{
    for(int i=0; buf[i]!=0; )
    {
        size_t step = ((unsigned)buf[i] > 0x80) ? 2:1;
        size_t pos = (step == 1) ? (unsigned)buf[i] : DCHAR2CODE(buf[i], buf[i+1]);

        stSyntax syntax;
        syntax.pos  = i;
        syntax.len  = step;
        syntax.code = pos;
        destbuf.push_back(syntax);

        i += step;
    }
}

void KeyWordSearch::SearchKeyWordTree(vector<stSyntax>::iterator& bit, vector<stSyntax>::iterator& eit, stCheckRes& res)throw()
{
    vector<stSyntax>::iterator preit=bit;
    ItemHandle hnode=0, hprenode=0;

    memset(&res, 0x00, sizeof(res));
    for(; bit!=eit; bit++)
    {
        SyntaxTreeNode& cur = (_KeyWordTreeRoot.node)[hnode];
        if (!cur.position)
        {
            if(bit==preit)bit++; break;
        }
        map<unsigned, unsigned>& mp=*(cur.position);
        if (mp.find((*bit).code) == mp.end())
        {
            if(bit==preit)bit++; break;
        }
        hnode = mp[(*bit).code];
        hprenode=hnode; preit=bit;
    }

    res.level=(_KeyWordTreeRoot.node)[hprenode].level;
    res.iReplace=(_KeyWordTreeRoot.node)[hprenode].iReplace;
    res.type=(_KeyWordTreeRoot.node)[hprenode].type;

    if (res.level > 0)
    {
        string& s1= *((_KeyWordTreeRoot.node)[hprenode].content);
        strcpy(res.content, s1.c_str());

        string& s= *((_KeyWordTreeRoot.node)[hprenode].replaceContent);
        strcpy(res.replaceContent, s.c_str());
    
    }
}

void KeyWordSearch::Check(const char* buf,int maxlevel, vector<stCheckRes>& vtres)throw()
{
    vector<stSyntax>  vtcode;

    Compile(buf, vtcode);

    vector<stSyntax>::iterator bit, preit, eit=vtcode.end();
    stCheckRes res;

    for(bit=vtcode.begin(); bit!=vtcode.end(); )
    {
        preit = bit;
        memset(&res, 0x00, sizeof(res));
        SearchKeyWordTree(bit, eit, res);
        if(res.level>0 && res.level<=maxlevel)
        {
            res.pos =(*preit).pos;
            vtres.push_back(res);
        }
        else
        {
            bit=++preit;
        }
    }
}

}
