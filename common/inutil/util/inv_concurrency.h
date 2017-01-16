#include <map>
#include <boost/function.hpp>
#include <boost/bind.hpp>
#include <string>
#include <vector>

#include "co_routine.h"

typedef boost::function<void ()> CoHandler;

typedef struct INV_CoArg
{
public:
	INV_CoArg(CoHandler hh, int* cc) : h(hh), count(cc) { }
	CoHandler	h;
	int*		count; 	
} INV_CoArg;						
							
inline void* inv_co_run(void* arg)
{
	INV_CoArg* p = (INV_CoArg*)arg;

    try {
	    (p->h)();
    }
    catch (...)
    {
        //协程中不能出现异常，注册函数中需要做处理
        printf("Oh, my God!!!, catch exception in inv_co_run!!!\n");
    }

	(*p->count)--;
	
	return NULL;
}

inline int inv_co_check(void* arg)
{
	int* pcount = (int*)arg;
	if ((*pcount) > 0)
		return 0;
	return -1;
}

#define INV_CO_BEGIN() do 														\
				{																\
					int __co_count__ = 0;										\
					vector<INV_CoArg> __co_args__;

#define INV_CO_CALL(fun, ...)													\
				__co_args__.push_back(INV_CoArg(							    \
										boost::bind(&fun, __VA_ARGS__), 		\
										&__co_count__)); 						

#define INV_CO_END()	__co_count__ = (int)__co_args__.size();					\
				if (!__co_count__)												\
				{																\
					break;														\
				}																\
																				\
				vector<stCoRoutine_t*> __colst__;								\
				for (int i = 0; i < (int)__co_args__.size(); i++)				\
				{																\
					stCoRoutine_t* co = NULL;									\
					co_create(&co, NULL, inv_co_run, (void*)&__co_args__[i]);	\
					__colst__.push_back(co);									\
					co_resume(co);												\
				}																\
																				\
				co_eventloop(co_get_epoll_ct(), 								\
											inv_co_check, (void*)&__co_count__);\
																				\
				for (int j = 0; j < (int)__colst__.size(); j++)					\
				{																\
					if (__colst__[j])											\
					{															\
						free(__colst__[j]);										\
						__colst__[j] = NULL;									\
					}															\
				} 																\
			} while(0);	
