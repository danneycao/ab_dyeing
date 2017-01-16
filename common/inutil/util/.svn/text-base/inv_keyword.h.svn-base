#ifndef _INV_KEYWORD_H_
#define _INV_KEYWORD_H_

#include <vector>
#include <map>
#include <iterator>
#include <string>
#include <stdexcept>
#include <cassert>
#include <string.h>

using namespace std;

namespace inv
{

struct stCheckRes
{
    unsigned  pos;
    char      level;
    int      type;
	int		iReplace;
    char    content[128];
	char    replaceContent[128];
};

struct SyntaxTreeNode
{
public:
    map<unsigned, unsigned>* position;
    char level;
    int      type;
	int	iReplace;
    string *content;
	string *replaceContent;

	SyntaxTreeNode()
	{
		position = NULL;
		content = NULL;
		replaceContent = NULL;
	}
};

typedef int ItemHandle;

class TreeRoot
{
public:
    SyntaxTreeNode *node  ;
    ItemHandle  freenode  ;
    unsigned    ifreenode ;
    unsigned    nodenum   ;

    TreeRoot()
    {
        node = NULL;
    }

    ItemHandle getFreeNode()
    {
        unsigned iFreeNode = freenode;
        if(ifreenode>0&&iFreeNode<nodenum)
        {
            ItemHandle hnode=freenode;
            ifreenode--; freenode++;
            
            memset(&(node[hnode]),0,sizeof(SyntaxTreeNode));
            return hnode;
        }
        return -1;
    }
    unsigned getFreeNodeN() { return ifreenode; }
     
    void init(int num)
    {
        nodenum = num; freenode = 1; ifreenode= num-1;
        if( node != NULL)
        {
            delete [] node;
            node = NULL;
        }
        node = new SyntaxTreeNode[nodenum];
        memset(node, 0x00, nodenum*sizeof(SyntaxTreeNode));
    }

    void clear()
    {
        if(node == NULL) {
            return;
        }

    	for(unsigned int i=0;i<nodenum;i++)
    	{
    		if(node[i].position != NULL)
    		{
    			node[i].position->clear();
    			delete node[i].position;
    			node[i].position = NULL;
    		}

    		if(node[i].content != NULL)
    		{
    			delete node[i].content;
    			node[i].content = NULL;
    		}

    		if(node[i].replaceContent != NULL)
    		{
    			delete node[i].replaceContent;
    			node[i].replaceContent = NULL;
    		}
    	}
    }
};

class KeyWordSearch
{
public:
    KeyWordSearch(){};
    ~KeyWordSearch(){
        _KeyWordTreeRoot.clear();
    }

    void Init(int nodenum)throw();
    void AddKeyWord(const char* buf, const char* dst, int iReplace, char level, int type)throw(runtime_error);
    void Check(const char* buf,int maxlevel, vector<stCheckRes>& vtres)throw();

    //支持utf8
    void AddKeyWordUtf8(const string &keyword);
    int CheckUtf8(const string &content);

    unsigned int freeNodeNum()
    {
        return _KeyWordTreeRoot.getFreeNodeN();
    }
private:

    struct stSyntax
    {
        unsigned  pos  ;
        unsigned  len  ;
        unsigned  code ;
    };

    TreeRoot   _KeyWordTreeRoot;

private:
    void Compile(const char* buf, vector<stSyntax>& destbuf)throw();
    void SearchKeyWordTree(vector<stSyntax>::iterator& bit, vector<stSyntax>::iterator& eit, stCheckRes& res)throw();
};

}

#endif
