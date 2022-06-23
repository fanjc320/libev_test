#pragma once

template<class ITEM_T>
class object_pool
{
private:
    //成员
    struct obj_item
    {        
        obj_item* pNext = nullptr;              //向下的指针
        obj_item* pPre = nullptr;               //向上的指针

        ITEM_T	Element;            //成员  
        bool	bAlloc = false;
        bool    bFirst = true;
    };

    struct obj_set
    {
        obj_item* pCurSet = nullptr;
        obj_set*  pNextSet = nullptr;
    };

public:
    object_pool(void)
        : _pObjSet(nullptr)
        , _pUnusedSet(nullptr)
    { }
    virtual ~object_pool(void) { }

private:
    obj_set*    _pObjSet = nullptr;		// 对象集
    obj_item*   _pUnusedSet = nullptr;		// 可用对象列表

    int	    _nNumOfAlloc = 0;			// 已分配对象数量
    int	    _nNumOfElements = 0;		// 对象集中对象总数量
    int	    _nNumOfSet = 0;			// 对象集数量
public:
    virtual bool create(int nNumOfElements)
    {
        _nNumOfElements = nNumOfElements;
        _nNumOfSet = 1;
        _pObjSet = new obj_set;
        _pObjSet->pNextSet = nullptr;
        _pObjSet->pCurSet = new obj_item[_nNumOfElements];

        //关联对象成员
        for (int i = 0; i < _nNumOfElements; ++i)
        {
            if (i > 0)
            {
                _pObjSet->pCurSet[i].pPre = &_pObjSet->pCurSet[i - 1];
                _pObjSet->pCurSet[i - 1].pNext = &_pObjSet->pCurSet[i];
            }
            _pObjSet->pCurSet[i].bAlloc = false;
        }
        _pObjSet->pCurSet[0].pPre = nullptr;
        _pObjSet->pCurSet[_nNumOfElements - 1].pNext = nullptr;

        _pUnusedSet = _pObjSet->pCurSet;
        _nNumOfAlloc = 0;
        return true;
    }

    virtual void destroy()
    {
        while (_pObjSet)
        {
            if (_pObjSet->pCurSet)
            {
                delete[] _pObjSet->pCurSet;
                _pObjSet->pCurSet = nullptr;
            }

            obj_set* pOld = _pObjSet;
            _pObjSet = _pObjSet->pNextSet;
            delete pOld;
        }
    }

    virtual ITEM_T* alloc()
    {
        if (!_pUnusedSet)
        {
            obj_set* pSet = new obj_set;
            pSet->pNextSet = _pObjSet;
            pSet->pCurSet = new obj_item[_nNumOfElements];

            for (int i = 0; i < _nNumOfElements; i++)
            {
                if (i > 0)
                {
                    pSet->pCurSet[i].pPre = &pSet->pCurSet[i - 1];
                    pSet->pCurSet[i - 1].pNext = &pSet->pCurSet[i];
                }
                pSet->pCurSet[i].bAlloc = false;
            }
            pSet->pCurSet[0].pPre = nullptr;
            pSet->pCurSet[_nNumOfElements - 1].pNext = nullptr;

            _pUnusedSet = pSet->pCurSet;
            _pObjSet = pSet;
            ++_nNumOfSet;
        }

        obj_item* pFreeObj;
        pFreeObj = _pUnusedSet;
        _pUnusedSet = _pUnusedSet->pNext;
        if (_pUnusedSet)
            _pUnusedSet->pPre = nullptr;

        pFreeObj->bAlloc = true;
        _nNumOfAlloc++;

        ITEM_T* p = &(pFreeObj->Element);
        if (!pFreeObj->bFirst) p = new (p)ITEM_T;
        return p;
    }

    void free(ITEM_T* pElement)
    {
        obj_item* pObject = (obj_item*) pElement;
        if (!pObject->bAlloc)
            return;

        pObject->Element.~ITEM_T();

        pObject->pNext = _pUnusedSet;
        pObject->pPre = nullptr;
        if (_pUnusedSet)
            _pUnusedSet->pPre = pObject;
        _pUnusedSet = pObject;

        _nNumOfAlloc--;

        pObject->bAlloc = false;
        pObject->bFirst = false;
    }

    int	alloc_size()
    {
        return _nNumOfAlloc;
    }

    int elem_size()
    {
        return _nNumOfElements;
    }

    int	pool_size()
    {
        return _nNumOfElements * _nNumOfSet;
    }

    bool is_created()
    {
        return _pUnusedSet != nullptr;
    }
};