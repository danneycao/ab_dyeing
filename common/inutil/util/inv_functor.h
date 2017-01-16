#ifndef __INV_FUNCTOR_H
#define __INV_FUNCTOR_H

#include <memory>
#include <iostream>
#include "util/inv_loki.h"

using namespace std;

namespace inv
{

template<typename R, class TList>
class INV_FunctorImp
{
public:
    virtual R operator()() = 0;
    virtual INV_FunctorImp* clone() const = 0;
    virtual ~INV_FunctorImp() {};
};

template<typename R>
class INV_FunctorImp<R, TL::EmptyType>
{
public:
    virtual R operator()() = 0;
    virtual INV_FunctorImp* clone() const = 0;
    virtual ~INV_FunctorImp() {};
};

template<typename R, typename P1>
class INV_FunctorImp<R, TL::TYPELIST_1(P1)>
{
public:
    virtual R operator()(typename TL::TypeTraits<P1>::ReferenceType p1) = 0;
    virtual INV_FunctorImp* clone() const = 0;
    virtual ~INV_FunctorImp() {};
};

template<typename R, typename P1, typename P2>
class INV_FunctorImp<R, TL::TYPELIST_2(P1, P2)>
{
public:
    virtual R operator()(typename TL::TypeTraits<P1>::ReferenceType p1,
                         typename TL::TypeTraits<P2>::ReferenceType p2) = 0;
    virtual INV_FunctorImp* clone() const = 0;
    virtual ~INV_FunctorImp() {};
};

template<typename R, typename P1, typename P2, typename P3>
class INV_FunctorImp<R, TL::TYPELIST_3(P1, P2, P3)>
{
public:
    virtual R operator()(typename TL::TypeTraits<P1>::ReferenceType p1,
                         typename TL::TypeTraits<P2>::ReferenceType p2,
                         typename TL::TypeTraits<P3>::ReferenceType p3) = 0;
    virtual INV_FunctorImp* clone() const = 0;
    virtual ~INV_FunctorImp() {};
};

template<typename R, typename P1, typename P2, typename P3, typename P4>
class INV_FunctorImp<R, TL::TYPELIST_4(P1, P2, P3, P4)>
{
public:
    virtual R operator()(typename TL::TypeTraits<P1>::ReferenceType p1,
                         typename TL::TypeTraits<P2>::ReferenceType p2,
                         typename TL::TypeTraits<P3>::ReferenceType p3,
                         typename TL::TypeTraits<P4>::ReferenceType p4) = 0;
    virtual INV_FunctorImp* clone() const = 0;
    virtual ~INV_FunctorImp() {};
};

template<typename R, typename P1, typename P2, typename P3, typename P4, typename P5>
class INV_FunctorImp<R, TL::TYPELIST_5(P1, P2, P3, P4, P5)>
{
public:
    virtual R operator()(typename TL::TypeTraits<P1>::ReferenceType p1,
                         typename TL::TypeTraits<P2>::ReferenceType p2,
                         typename TL::TypeTraits<P3>::ReferenceType p3,
                         typename TL::TypeTraits<P4>::ReferenceType p4,
                         typename TL::TypeTraits<P5>::ReferenceType p5) = 0;
    virtual INV_FunctorImp* clone() const = 0;
    virtual ~INV_FunctorImp() {};
};

template<typename R, typename P1, typename P2, typename P3, typename P4, typename P5, typename P6>
class INV_FunctorImp<R, TL::TYPELIST_6(P1, P2, P3, P4, P5, P6)>
{
public:
    virtual R operator()(typename TL::TypeTraits<P1>::ReferenceType p1,
                         typename TL::TypeTraits<P2>::ReferenceType p2,
                         typename TL::TypeTraits<P3>::ReferenceType p3,
                         typename TL::TypeTraits<P4>::ReferenceType p4,
                         typename TL::TypeTraits<P5>::ReferenceType p5,
                         typename TL::TypeTraits<P6>::ReferenceType p6) = 0;
    virtual INV_FunctorImp* clone() const = 0;
    virtual ~INV_FunctorImp() {};
};

template<typename R, typename P1, typename P2, typename P3, typename P4, typename P5, typename P6, typename P7>
class INV_FunctorImp<R, TL::TYPELIST_7(P1, P2, P3, P4, P5, P6, P7)>
{
public:
    virtual R operator()(typename TL::TypeTraits<P1>::ReferenceType p1,
                         typename TL::TypeTraits<P2>::ReferenceType p2,
                         typename TL::TypeTraits<P3>::ReferenceType p3,
                         typename TL::TypeTraits<P4>::ReferenceType p4,
                         typename TL::TypeTraits<P5>::ReferenceType p5,
                         typename TL::TypeTraits<P6>::ReferenceType p6,
                         typename TL::TypeTraits<P7>::ReferenceType p7) = 0;
    virtual INV_FunctorImp* clone() const = 0;
    virtual ~INV_FunctorImp() {};
};

template<typename R, typename P1, typename P2, typename P3, typename P4, typename P5, typename P6, typename P7, typename P8>
class INV_FunctorImp<R, TL::TYPELIST_8(P1, P2, P3, P4, P5, P6, P7, P8)>
{
public:
    virtual R operator()(typename TL::TypeTraits<P1>::ReferenceType p1,
                         typename TL::TypeTraits<P2>::ReferenceType p2,
                         typename TL::TypeTraits<P3>::ReferenceType p3,
                         typename TL::TypeTraits<P4>::ReferenceType p4,
                         typename TL::TypeTraits<P5>::ReferenceType p5,
                         typename TL::TypeTraits<P6>::ReferenceType p6,
                         typename TL::TypeTraits<P7>::ReferenceType p7,
                         typename TL::TypeTraits<P8>::ReferenceType p8) = 0;
    virtual INV_FunctorImp* clone() const = 0;
    virtual ~INV_FunctorImp() {};
};

template<typename R, typename P1, typename P2, typename P3, typename P4, typename P5, typename P6, typename P7, typename P8, typename P9>
class INV_FunctorImp<R, TL::TYPELIST_9(P1, P2, P3, P4, P5, P6, P7, P8, P9)>
{
public:
    virtual R operator()(typename TL::TypeTraits<P1>::ReferenceType p1,
                         typename TL::TypeTraits<P2>::ReferenceType p2,
                         typename TL::TypeTraits<P3>::ReferenceType p3,
                         typename TL::TypeTraits<P4>::ReferenceType p4,
                         typename TL::TypeTraits<P5>::ReferenceType p5,
                         typename TL::TypeTraits<P6>::ReferenceType p6,
                         typename TL::TypeTraits<P7>::ReferenceType p7,
                         typename TL::TypeTraits<P8>::ReferenceType p8,
                         typename TL::TypeTraits<P9>::ReferenceType p9) = 0;
    virtual INV_FunctorImp* clone() const = 0;
    virtual ~INV_FunctorImp() {};
};

template<typename R, typename P1, typename P2, typename P3, typename P4, typename P5, typename P6, typename P7, typename P8, typename P9, typename P10>
class INV_FunctorImp<R, TL::TYPELIST_10(P1, P2, P3, P4, P5, P6, P7, P8, P9, P10)>
{
public:
    virtual R operator()(typename TL::TypeTraits<P1>::ReferenceType p1,
                         typename TL::TypeTraits<P2>::ReferenceType p2,
                         typename TL::TypeTraits<P3>::ReferenceType p3,
                         typename TL::TypeTraits<P4>::ReferenceType p4,
                         typename TL::TypeTraits<P5>::ReferenceType p5,
                         typename TL::TypeTraits<P6>::ReferenceType p6,
                         typename TL::TypeTraits<P7>::ReferenceType p7,
                         typename TL::TypeTraits<P8>::ReferenceType p8,
                         typename TL::TypeTraits<P9>::ReferenceType p9,
                         typename TL::TypeTraits<P10>::ReferenceType p10) = 0;
    virtual INV_FunctorImp* clone() const = 0;
    virtual ~INV_FunctorImp() {};
};

/**
 * 封装对函数对象以及函数的调用
 */
template<class ParentFunctor, typename Fun>
class INV_FunctorHandler : public INV_FunctorImp<typename ParentFunctor::ResultType, typename ParentFunctor::ParamList>
{
public:
    typedef typename ParentFunctor::ResultType ResultType;

    INV_FunctorHandler(Fun fun) : _fun(fun)
    {

    }

    INV_FunctorHandler *clone() const
    {
        return new INV_FunctorHandler(*this);
    }

    ResultType operator()()
    { return _fun(); }

    ResultType operator()(typename ParentFunctor::Reference1 p1)
	{ return _fun(p1); }

    ResultType operator()(typename ParentFunctor::Reference1 p1,
                        typename ParentFunctor::Reference2 p2)
	{ return _fun(p1, p2); }

    ResultType operator()(typename ParentFunctor::Reference1 p1,
                        typename ParentFunctor::Reference2 p2,
                        typename ParentFunctor::Reference3 p3)
	{ return _fun(p1, p2, p3); }

    ResultType operator()(typename ParentFunctor::Reference1 p1,
                        typename ParentFunctor::Reference2 p2,
                        typename ParentFunctor::Reference3 p3,
                        typename ParentFunctor::Reference4 p4)
	{ return _fun(p1, p2, p3, p4); }

    ResultType operator()(typename ParentFunctor::Reference1 p1,
                        typename ParentFunctor::Reference2 p2,
                        typename ParentFunctor::Reference3 p3,
                        typename ParentFunctor::Reference4 p4,
                        typename ParentFunctor::Reference5 p5)
	{ return _fun(p1, p2, p3, p4, p5); }

    ResultType operator()(typename ParentFunctor::Reference1 p1,
                        typename ParentFunctor::Reference2 p2,
                        typename ParentFunctor::Reference3 p3,
                        typename ParentFunctor::Reference4 p4,
                        typename ParentFunctor::Reference5 p5,
                        typename ParentFunctor::Reference6 p6)
	{ return _fun(p1, p2, p3, p4, p5, p6); }

    ResultType operator()(typename ParentFunctor::Reference1 p1,
                        typename ParentFunctor::Reference2 p2,
                        typename ParentFunctor::Reference3 p3,
                        typename ParentFunctor::Reference4 p4,
                        typename ParentFunctor::Reference5 p5,
                        typename ParentFunctor::Reference6 p6,
                        typename ParentFunctor::Reference7 p7)
	{ return _fun(p1, p2, p3, p4, p5, p6, p7); }

	ResultType operator()(typename ParentFunctor::Reference1 p1,
						typename ParentFunctor::Reference2 p2,
						typename ParentFunctor::Reference3 p3,
						typename ParentFunctor::Reference4 p4,
						typename ParentFunctor::Reference5 p5,
						typename ParentFunctor::Reference6 p6,
						typename ParentFunctor::Reference7 p7,
                        typename ParentFunctor::Reference8 p8)
	{ return _fun(p1, p2, p3, p4, p5, p6, p7, p8); }

	ResultType operator()(typename ParentFunctor::Reference1 p1,
						typename ParentFunctor::Reference2 p2,
						typename ParentFunctor::Reference3 p3,
						typename ParentFunctor::Reference4 p4,
						typename ParentFunctor::Reference5 p5,
						typename ParentFunctor::Reference6 p6,
						typename ParentFunctor::Reference7 p7,
                        typename ParentFunctor::Reference8 p8,
                        typename ParentFunctor::Reference9 p9)
	{ return _fun(p1, p2, p3, p4, p5, p6, p7, p8, p9); }

	ResultType operator()(typename ParentFunctor::Reference1 p1,
						typename ParentFunctor::Reference2 p2,
						typename ParentFunctor::Reference3 p3,
						typename ParentFunctor::Reference4 p4,
						typename ParentFunctor::Reference5 p5,
						typename ParentFunctor::Reference6 p6,
						typename ParentFunctor::Reference7 p7,
                        typename ParentFunctor::Reference8 p8,
                        typename ParentFunctor::Reference9 p9,
                        typename ParentFunctor::Reference10 p10)
	{ return _fun(p1, p2, p3, p4, p5, p6, p7, p8, p9, p10); }

private:
    Fun _fun;
};

template<class ParentFunctor, typename PointerToObj, typename PointerToMemFun>
/**
 * @brief 封装对成员函数的调用
 */
class INV_MemFunHandler
	: public INV_FunctorImp<typename ParentFunctor::ResultType, typename ParentFunctor::ParamList>
{
public:
	typedef typename ParentFunctor::ResultType ResultType;

	INV_MemFunHandler(const PointerToObj &pObj, PointerToMemFun pMemFn)
		: _pObj(pObj), _pMemFn(pMemFn)
	{
	}

	INV_MemFunHandler* clone() const
	{
		return new INV_MemFunHandler(*this);
	}

	ResultType operator()()
	{ return (_pObj->*_pMemFn)(); }

	ResultType operator()(typename ParentFunctor::Reference1 p1)
	{ return (_pObj->*_pMemFn)(p1); }

	ResultType operator()(typename ParentFunctor::Reference1 p1,
						typename ParentFunctor::Reference2 p2)
	{ return (_pObj->*_pMemFn)(p1, p2); }

	ResultType operator()(typename ParentFunctor::Reference1 p1,
						typename ParentFunctor::Reference2 p2,
						typename ParentFunctor::Reference3 p3)
	{ return (_pObj->*_pMemFn)(p1, p2, p3); }

	ResultType operator()(typename ParentFunctor::Reference1 p1,
						typename ParentFunctor::Reference2 p2,
						typename ParentFunctor::Reference3 p3,
						typename ParentFunctor::Reference4 p4)
	{ return (_pObj->*_pMemFn)(p1, p2, p3, p4); }

	ResultType operator()(typename ParentFunctor::Reference1 p1,
						typename ParentFunctor::Reference2 p2,
						typename ParentFunctor::Reference3 p3,
						typename ParentFunctor::Reference4 p4,
						typename ParentFunctor::Reference5 p5)
	{ return (_pObj->*_pMemFn)(p1, p2, p3, p4, p5); }

	ResultType operator()(typename ParentFunctor::Reference1 p1,
						typename ParentFunctor::Reference2 p2,
						typename ParentFunctor::Reference3 p3,
						typename ParentFunctor::Reference4 p4,
						typename ParentFunctor::Reference5 p5,
						typename ParentFunctor::Reference6 p6)
	{ return (_pObj->*_pMemFn)(p1, p2, p3, p4, p5, p6); }

	ResultType operator()(typename ParentFunctor::Reference1 p1,
						typename ParentFunctor::Reference2 p2,
						typename ParentFunctor::Reference3 p3,
						typename ParentFunctor::Reference4 p4,
						typename ParentFunctor::Reference5 p5,
						typename ParentFunctor::Reference6 p6,
						typename ParentFunctor::Reference7 p7)
	{ return (_pObj->*_pMemFn)(p1, p2, p3, p4, p5, p6, p7); }

	ResultType operator()(typename ParentFunctor::Reference1 p1,
						typename ParentFunctor::Reference2 p2,
						typename ParentFunctor::Reference3 p3,
						typename ParentFunctor::Reference4 p4,
						typename ParentFunctor::Reference5 p5,
						typename ParentFunctor::Reference6 p6,
						typename ParentFunctor::Reference7 p7,
                        typename ParentFunctor::Reference8 p8)
	{ return (_pObj->*_pMemFn)(p1, p2, p3, p4, p5, p6, p7, p8); }

	ResultType operator()(typename ParentFunctor::Reference1 p1,
						typename ParentFunctor::Reference2 p2,
						typename ParentFunctor::Reference3 p3,
						typename ParentFunctor::Reference4 p4,
						typename ParentFunctor::Reference5 p5,
						typename ParentFunctor::Reference6 p6,
						typename ParentFunctor::Reference7 p7,
                        typename ParentFunctor::Reference8 p8,
                        typename ParentFunctor::Reference9 p9)
	{ return (_pObj->*_pMemFn)(p1, p2, p3, p4, p5, p6, p7, p8, p9); }

	ResultType operator()(typename ParentFunctor::Reference1 p1,
						typename ParentFunctor::Reference2 p2,
						typename ParentFunctor::Reference3 p3,
						typename ParentFunctor::Reference4 p4,
						typename ParentFunctor::Reference5 p5,
						typename ParentFunctor::Reference6 p6,
						typename ParentFunctor::Reference7 p7,
                        typename ParentFunctor::Reference8 p8,
                        typename ParentFunctor::Reference9 p9,
                        typename ParentFunctor::Reference10 p10)
	{ return (_pObj->*_pMemFn)(p1, p2, p3, p4, p5, p6, p7, p8, p9, p10); }

private:
	PointerToObj	_pObj;
	PointerToMemFun _pMemFn;
};

template<class ParentFunctor>
class INV_FunctorWrapper;


template<typename R, class TList = TL::NullType>
/**
 * @brief 函数对象类, 可以封装对: 函数对象, 函数, 成员函数 
 *  	  的调用
 */
class INV_Functor
{
public:
    typedef R     ResultType;
    typedef TList ParamList;

    /**定义封装类型*/
    typedef INV_FunctorWrapper<INV_Functor<R, TList> > wrapper_type;

    /**定义类型列表中每个参数的原类型*/
    typedef typename TL::TypeTraits<typename TL::TypeAtNonStrict<TList, 0, TL::EmptyType>::Result>::ParameterType Param1;
    typedef typename TL::TypeTraits<typename TL::TypeAtNonStrict<TList, 1, TL::EmptyType>::Result>::ParameterType Param2;
    typedef typename TL::TypeTraits<typename TL::TypeAtNonStrict<TList, 2, TL::EmptyType>::Result>::ParameterType Param3;
    typedef typename TL::TypeTraits<typename TL::TypeAtNonStrict<TList, 3, TL::EmptyType>::Result>::ParameterType Param4;
    typedef typename TL::TypeTraits<typename TL::TypeAtNonStrict<TList, 4, TL::EmptyType>::Result>::ParameterType Param5;
    typedef typename TL::TypeTraits<typename TL::TypeAtNonStrict<TList, 5, TL::EmptyType>::Result>::ParameterType Param6;
    typedef typename TL::TypeTraits<typename TL::TypeAtNonStrict<TList, 6, TL::EmptyType>::Result>::ParameterType Param7;
    typedef typename TL::TypeTraits<typename TL::TypeAtNonStrict<TList, 7, TL::EmptyType>::Result>::ParameterType Param8;
    typedef typename TL::TypeTraits<typename TL::TypeAtNonStrict<TList, 8, TL::EmptyType>::Result>::ParameterType Param9;
    typedef typename TL::TypeTraits<typename TL::TypeAtNonStrict<TList, 9, TL::EmptyType>::Result>::ParameterType Param10;

    /**定义类型列表中每个参数的引用类型*/
    typedef typename TL::TypeTraits<Param1>::ReferenceType Reference1;
    typedef typename TL::TypeTraits<Param2>::ReferenceType Reference2;
    typedef typename TL::TypeTraits<Param3>::ReferenceType Reference3;
    typedef typename TL::TypeTraits<Param4>::ReferenceType Reference4;
    typedef typename TL::TypeTraits<Param5>::ReferenceType Reference5;
    typedef typename TL::TypeTraits<Param6>::ReferenceType Reference6;
    typedef typename TL::TypeTraits<Param7>::ReferenceType Reference7;
    typedef typename TL::TypeTraits<Param8>::ReferenceType Reference8;
    typedef typename TL::TypeTraits<Param9>::ReferenceType Reference9;
    typedef typename TL::TypeTraits<Param10>::ReferenceType Reference10;

public:
    INV_Functor()
    {
    }

    INV_Functor(const INV_Functor &functor)
    : _spImpl(functor._spImpl->clone())
    {
    }

    INV_Functor& operator=(const INV_Functor &functor)
    {
        if(this != &functor)
        {
            _spImpl = std::auto_ptr<Impl>(functor._spImpl->clone());
        }

        return (*this);
    }

    template<class Fun>
    INV_Functor(Fun fun)
	: _spImpl(new INV_FunctorHandler<INV_Functor, Fun>(fun))
    {
    }

	template<typename PointerToObj, typename PointerToMemFun>
	INV_Functor(const PointerToObj &pObj, PointerToMemFun pMemFn)
	: _spImpl(new INV_MemFunHandler<INV_Functor, PointerToObj, PointerToMemFun>(pObj, pMemFn))
	{
	}

	// 判断是否已经设置了functor
	// 可以这样使用if(functor)
	operator bool() const
	{
		return _spImpl.get() ? true : false;
	}

    R operator()()
	{
		return (*_spImpl)();
	}

    R operator()(Reference1 p1)
	{
		return (*_spImpl)(p1);
	}

    R operator()(Reference1 p1, Reference2 p2)
	{
		return (*_spImpl)(p1, p2);
	}

    R operator()(Reference1 p1, Reference2 p2, Reference3 p3)
	{
		return (*_spImpl)(p1, p2, p3);
	}

    R operator()(Reference1 p1, Reference2 p2, Reference3 p3, Reference4 p4)
	{
		return (*_spImpl)(p1, p2, p3, p4);
	}

    R operator()(Reference1 p1, Reference2 p2, Reference3 p3, Reference4 p4, Reference5 p5)
	{
		return (*_spImpl)(p1, p2, p3, p4, p5);
	}

    R operator()(Reference1 p1, Reference2 p2, Reference3 p3, Reference4 p4, Reference5 p5, Reference6 p6)
	{
		return (*_spImpl)(p1, p2, p3, p4, p5, p6);
	}

    R operator()(Reference1 p1, Reference2 p2, Reference3 p3, Reference4 p4, Reference5 p5, Reference6 p6, Reference7 p7)
	{
		return (*_spImpl)(p1, p2, p3, p4, p5, p6, p7);
	}

    R operator()(Reference1 p1, Reference2 p2, Reference3 p3, Reference4 p4, Reference5 p5, Reference6 p6, Reference7 p7, Reference8 p8)
	{
		return (*_spImpl)(p1, p2, p3, p4, p5, p6, p7, p8);
	}

    R operator()(Reference1 p1, Reference2 p2, Reference3 p3, Reference4 p4, Reference5 p5, Reference6 p6, Reference7 p7, Reference8 p8, Reference9 p9)
	{
		return (*_spImpl)(p1, p2, p3, p4, p5, p6, p7, p8, p9);
	}

    R operator()(Reference1 p1, Reference2 p2, Reference3 p3, Reference4 p4, Reference5 p5, Reference6 p6, Reference7 p7, Reference8 p8, Reference9 p9, Reference10 p10)
	{
		return (*_spImpl)(p1, p2, p3, p4, p5, p6, p7, p8, p9, p10);
	}


private:
    template<class ParentFunctor>
    friend class INV_FunctorWrapper;

    typedef INV_FunctorImp<R, TList> Impl;

    std::auto_ptr<Impl>     _spImpl;
};

/**
 * @brief wapper基类
 */
class INV_FunctorWrapperInterface
{
public:
    virtual void operator()() = 0;
    virtual ~INV_FunctorWrapperInterface(){}
};

template<class ParentFunctor>

/**
 * @brief 对INV_Functor进行封装, 可以让INV_Functor做到事后被调用
 */
class INV_FunctorWrapper : public INV_FunctorWrapperInterface
{
public:
    typedef typename ParentFunctor::ResultType R;
    typedef typename ParentFunctor::ParamList  TList;

    /**
     * @brief 拷贝构造
     * @param fw
     */
    INV_FunctorWrapper(const INV_FunctorWrapper &fw)
    : _spImpl(fw._spImpl->clone())
    , _p1(fw._p1)
    , _p2(fw._p2)
    , _p3(fw._p3)
    , _p4(fw._p4)
    , _p5(fw._p5)
    , _p6(fw._p6)
    , _p7(fw._p7)
    , _p8(fw._p8)
    , _p9(fw._p9)
    , _p10(fw._p10)
    {
    }

    /**
     * @brief 构造函数
     * @param tf
     */
    INV_FunctorWrapper(ParentFunctor &tf)
    : _spImpl(tf._spImpl->clone())
    {
    }

    /**
     * @brief 一个参数调用的构造函数
     * @param tf
     * @param p1
     */
    INV_FunctorWrapper(ParentFunctor &tf,
                  typename ParentFunctor::Reference1 p1)
    : _spImpl(tf._spImpl->clone()), _p1(p1)
    {
    }

    /**
     * @brief 两个参数调用的构造函数
     * @param tf
     * @param p1
     * @param p2
     */
    INV_FunctorWrapper(ParentFunctor &tf,
                  typename ParentFunctor::Reference1 p1,
                  typename ParentFunctor::Reference2 p2)
    : _spImpl(tf._spImpl->clone()), _p1(p1), _p2(p2)
    {
    }

    /**
     * @brief 三个参数调用的构造函数
     * @param tf
     * @param p1
     * @param p2
     * @param p3
     */
    INV_FunctorWrapper(ParentFunctor &tf,
                  typename ParentFunctor::Reference1 p1,
                  typename ParentFunctor::Reference2 p2,
                  typename ParentFunctor::Reference3 p3)
    : _spImpl(tf._spImpl->clone()), _p1(p1), _p2(p2), _p3(p3)
    {
    }

    /**
     * @brief 四个参数调用的构造函数
     * @param tf
     * @param p1
     * @param p2
     * @param p3
     * @param p4
     */
    INV_FunctorWrapper(ParentFunctor &tf,
                                            typename ParentFunctor::Reference1 p1,
                                            typename ParentFunctor::Reference2 p2,
                                            typename ParentFunctor::Reference3 p3,
                                            typename ParentFunctor::Reference4 p4)
    : _spImpl(tf._spImpl->clone()), _p1(p1), _p2(p2), _p3(p3), _p4(p4)
    {
    }

    /**
     * @brief 五个参数调用的构造函数
     * @param tf
     * @param p1
     * @param p2
     * @param p3
     * @param p4
     * @param p5
     */
    INV_FunctorWrapper(ParentFunctor &tf,
                                            typename ParentFunctor::Reference1 p1,
                                            typename ParentFunctor::Reference2 p2,
                                            typename ParentFunctor::Reference3 p3,
                                            typename ParentFunctor::Reference4 p4,
                                            typename ParentFunctor::Reference5 p5)
    : _spImpl(tf._spImpl->clone()), _p1(p1), _p2(p2), _p3(p3), _p4(p4), _p5(p5)
    {
    }

    /**
     * @brief 六个参数调用的构造函数
     * @param tf
     * @param p1
     * @param p2
     * @param p3
     * @param p4
     * @param p5
     * @param p6
     */
    INV_FunctorWrapper(ParentFunctor &tf,
                                            typename ParentFunctor::Reference1 p1,
                                            typename ParentFunctor::Reference2 p2,
                                            typename ParentFunctor::Reference3 p3,
                                            typename ParentFunctor::Reference4 p4,
                                            typename ParentFunctor::Reference5 p5,
                                            typename ParentFunctor::Reference6 p6)
    : _spImpl(tf._spImpl->clone()), _p1(p1), _p2(p2), _p3(p3), _p4(p4), _p5(p5), _p6(p6)
    {
    }

    /**
     * @brief 七个参数调用的构造函数
     * @param tf
     * @param p1
     * @param p2
     * @param p3
     * @param p4
     * @param p5
     * @param p6
     * @param p7
     */
    INV_FunctorWrapper(ParentFunctor &tf,
                                            typename ParentFunctor::Reference1 p1,
                                            typename ParentFunctor::Reference2 p2,
                                            typename ParentFunctor::Reference3 p3,
                                            typename ParentFunctor::Reference4 p4,
                                            typename ParentFunctor::Reference5 p5,
                                            typename ParentFunctor::Reference6 p6,
                                            typename ParentFunctor::Reference7 p7)
    : _spImpl(tf._spImpl->clone()), _p1(p1), _p2(p2), _p3(p3), _p4(p4), _p5(p5), _p6(p6), _p7(p7)
    {
    }

    /**
     * @brief 八个参数调用的构造函数
     * @param tf
     * @param p1
     * @param p2
     * @param p3
     * @param p4
     * @param p5
     * @param p6
     * @param p7
     * @param p8
     */
    INV_FunctorWrapper(ParentFunctor &tf,
                                            typename ParentFunctor::Reference1 p1,
                                            typename ParentFunctor::Reference2 p2,
                                            typename ParentFunctor::Reference3 p3,
                                            typename ParentFunctor::Reference4 p4,
                                            typename ParentFunctor::Reference5 p5,
                                            typename ParentFunctor::Reference6 p6,
                                            typename ParentFunctor::Reference7 p7,
                                            typename ParentFunctor::Reference8 p8)
    : _spImpl(tf._spImpl->clone()), _p1(p1), _p2(p2), _p3(p3), _p4(p4), _p5(p5), _p6(p6), _p7(p7), _p8(p8)
    {
    }

    /**
     * @brief 九个参数调用的构造函数
     * @param tf
     * @param p1
     * @param p2
     * @param p3
     * @param p4
     * @param p5
     * @param p6
     * @param p7
     * @param p8
     * @param p9
     */
    INV_FunctorWrapper(ParentFunctor &tf,
                                            typename ParentFunctor::Reference1 p1,
                                            typename ParentFunctor::Reference2 p2,
                                            typename ParentFunctor::Reference3 p3,
                                            typename ParentFunctor::Reference4 p4,
                                            typename ParentFunctor::Reference5 p5,
                                            typename ParentFunctor::Reference6 p6,
                                            typename ParentFunctor::Reference7 p7,
                                            typename ParentFunctor::Reference8 p8,
                                            typename ParentFunctor::Reference9 p9)
    : _spImpl(tf._spImpl->clone()), _p1(p1), _p2(p2), _p3(p3), _p4(p4), _p5(p5), _p6(p6), _p7(p7), _p8(p8), _p9(p9)
    {
    }

    /**
     * @brief 十个参数调用的构造函数
     * @param tf
     * @param p1
     * @param p2
     * @param p3
     * @param p4
     * @param p5
     * @param p6
     * @param p7
     * @param p8
     * @param p9
     * @param p10
     */
    INV_FunctorWrapper(ParentFunctor &tf,
                                            typename ParentFunctor::Reference1 p1,
                                            typename ParentFunctor::Reference2 p2,
                                            typename ParentFunctor::Reference3 p3,
                                            typename ParentFunctor::Reference4 p4,
                                            typename ParentFunctor::Reference5 p5,
                                            typename ParentFunctor::Reference6 p6,
                                            typename ParentFunctor::Reference7 p7,
                                            typename ParentFunctor::Reference8 p8,
                                            typename ParentFunctor::Reference9 p9,
                                            typename ParentFunctor::Reference10 p10)
    : _spImpl(tf._spImpl->clone()), _p1(p1), _p2(p2), _p3(p3), _p4(p4), _p5(p5), _p6(p6), _p7(p7), _p8(p8), _p9(p9), _p10(p10)
    {
    }

    /**
     * @brief 具体调用
     * @param tf
     * @param p1
     */
	virtual R operator()()
	{
		return todo(TL::Int2Type<TL::Length<TList>::value>());
	}

	virtual ~INV_FunctorWrapper(){ }

protected:
    /**
     * @brief 赋值函数不定义, 只声明
     * @param fw
     *
     * @return INV_FunctorWrapper&
     */
    INV_FunctorWrapper& operator=(const INV_FunctorWrapper &fw);

	R todo(TL::Int2Type<0>)
    {
		return (*_spImpl)();
	}

	R todo(TL::Int2Type<1>)
	{
        return (*_spImpl)(_p1);
	}

	R todo(TL::Int2Type<2>)
	{
        return (*_spImpl)(_p1, _p2);
	}

	R todo(TL::Int2Type<3>)
	{
        return (*_spImpl)(_p1, _p2, _p3);
	}

	R todo(TL::Int2Type<4>)
	{
        return (*_spImpl)(_p1, _p2, _p3, _p4);
	}

	R todo(TL::Int2Type<5>)
	{
        return (*_spImpl)(_p1, _p2, _p3, _p4, _p5);
	}

	R todo(TL::Int2Type<6>)
	{
        return (*_spImpl)(_p1, _p2, _p3, _p4, _p5, _p6);
	}

	R todo(TL::Int2Type<7>)
	{
        return (*_spImpl)(_p1, _p2, _p3, _p4, _p5, _p6, _p7);
	}

	R todo(TL::Int2Type<8>)
	{
        return (*_spImpl)(_p1, _p2, _p3, _p4, _p5, _p6, _p7, _p8);
	}

	R todo(TL::Int2Type<9>)
	{
        return (*_spImpl)(_p1, _p2, _p3, _p4, _p5, _p6, _p7, _p8, _p9);
	}

	R todo(TL::Int2Type<10>)
	{
		return (*_spImpl)(_p1, _p2, _p3, _p4, _p5, _p6, _p7, _p8, _p9, _p10);
	}

protected:
    typedef INV_FunctorImp<R, TList> Impl;

    std::auto_ptr<Impl>             _spImpl;

    typename TL::TypeTraits<typename ParentFunctor::Param1>::ParameterType   _p1;
    typename TL::TypeTraits<typename ParentFunctor::Param2>::ParameterType   _p2;
    typename TL::TypeTraits<typename ParentFunctor::Param3>::ParameterType   _p3;
    typename TL::TypeTraits<typename ParentFunctor::Param4>::ParameterType   _p4;
    typename TL::TypeTraits<typename ParentFunctor::Param5>::ParameterType   _p5;
    typename TL::TypeTraits<typename ParentFunctor::Param6>::ParameterType   _p6;
    typename TL::TypeTraits<typename ParentFunctor::Param7>::ParameterType   _p7;
    typename TL::TypeTraits<typename ParentFunctor::Param8>::ParameterType   _p8;
    typename TL::TypeTraits<typename ParentFunctor::Param9>::ParameterType   _p9;
    typename TL::TypeTraits<typename ParentFunctor::Param10>::ParameterType  _p10;
};

}

#endif

