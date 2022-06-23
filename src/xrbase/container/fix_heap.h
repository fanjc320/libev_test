#pragma once
#include <stdint.h>
#include "log/LogMgr.h"

typedef bool (*FUN_COMPARE_HEAP)(int64_t nKey1, int64_t nKey2);

namespace fix
{
    struct fix_heap_node
    {
        int64_t llKey = 0;
        int32_t nIdx = 0;
    };

    template <size_t N>
    struct fix_heap
    {
    public:
        fix_heap(void) { }
        ~fix_heap(void) { }

        bool init(FUN_COMPARE_HEAP pFunc)
        {
            clear();
            _funcCompare = pFunc;

            return true;
Exit0:
            return false;
        }

        void uninit(void)
        {
            _nNodeCnt = 0;
        }

        void clear(void)
        {
            memset(_pNodeList, 0, N * sizeof(fix_heap_node*));
            _nNodeCnt = 0;
        }

        bool add_node(fix_heap_node* pNode)
        {
            CHECK_LOG_RETVAL(_nNodeCnt < N, false);

            _pNodeList[_nNodeCnt] = pNode;
            pNode->nIdx = _nNodeCnt;
            __heap_shift_up(_nNodeCnt);

            ++_nNodeCnt;

            return true;
Exit0:
            return false;
        }

        fix_heap_node* pop(void)
        {
            fix_heap_node* pNode = nullptr;
            int32_t nRetCode = 0;
            ERROR_LOG_EXIT0(_nNodeCnt > 0);

            pNode = _pNodeList[0];

            __heap_swap_node(0, _nNodeCnt - 1);

            _pNodeList[_nNodeCnt - 1] = nullptr;
            --_nNodeCnt;

            pNode->nIdx = -1;

            if (_nNodeCnt > 0)
            {
                __heap_shift_down(0);
            }

            return pNode;
Exit0:
            return nullptr;
        }

        fix_heap_node* top(void)
        {
            return _pNodeList[0];
        }

        fix_heap_node* del_node(int32_t nIdx)
        {
            fix_heap_node* pNode = nullptr;
            ERROR_LOG_EXIT0(nIdx >= 0 && nIdx < _nNodeCnt);

            pNode = _pNodeList[nIdx];
            ERROR_LOG_EXIT0(_nNodeCnt > 0);

            pNode->llKey = 0x80000000;
            __heap_shift_up(nIdx);
            pNode = pop();

            return pNode;
Exit0:
            return nullptr;
        }

        fix_heap_node* del_node(fix_heap_node* pNode)
        {
            int32_t nRetCode = 0;
            int32_t nIdx = pNode->nIdx;

            pNode = del_node(pNode->nIdx);
            ERROR_LOG_EXIT0(pNode);

            return pNode;
Exit0:
            return nullptr;
        }

        bool check(void)
        {
            int32_t nRetCode = 0;

            for (int32_t i = 0; i < _nNodeCnt; ++i)
            {
                int32_t nLeftChildIdx = i * 2 + 1;
                int32_t nRightChildIdx = nLeftChildIdx + 1;

                if (nLeftChildIdx < _nNodeCnt)
                {
                    nRetCode = (*_funcCompare)(_pNodeList[i]->llKey, _pNodeList[nLeftChildIdx]->llKey);
                    ERROR_LOG_EXIT0(nRetCode);
                }

                if (nRightChildIdx < _nNodeCnt)
                {
                    nRetCode = (*_funcCompare)(_pNodeList[i]->llKey, _pNodeList[nRightChildIdx]->llKey);
                    ERROR_LOG_EXIT0(nRetCode);
                }
            }

            return true;
Exit0:
            return false;
        }

        int32_t node_cnt(void) { return _nNodeCnt; }

        fix_heap_node* get_node(int32_t nIdx)
        {
            ERROR_LOG_EXIT0(nIdx >= 0 && nIdx < N);
            return _pNodeList[nIdx];
Exit0:
            return nullptr;
        }

    protected:

        void __heap_swap_node(int32_t nIdx1, int32_t nIdx2)
        {
            fix_heap_node* pNode = _pNodeList[nIdx1];
            _pNodeList[nIdx1] = _pNodeList[nIdx2];
            _pNodeList[nIdx2] = pNode;
            _pNodeList[nIdx1]->nIdx = nIdx1;
            _pNodeList[nIdx2]->nIdx = nIdx2;
        }

        void __get_target_child(int32_t nParentIdx, int32_t& nTargetChildIdx)
        {
            int32_t nLeftChildIdx = nParentIdx * 2 + 1;
            int32_t nRightChildIdx = nLeftChildIdx + 1;

            if (nLeftChildIdx >= _nNodeCnt)
            {
                nTargetChildIdx = -1;
            }
            else if (nRightChildIdx >= _nNodeCnt)
            {
                nTargetChildIdx = nLeftChildIdx;
            }
            else
            {
                if ((*_funcCompare)(_pNodeList[nLeftChildIdx]->llKey, _pNodeList[nRightChildIdx]->llKey))
                    nTargetChildIdx = nLeftChildIdx;
                else
                    nTargetChildIdx = nRightChildIdx;
            }
        }

        void __heap_shift_up(int32_t nFromIdx)
        {
            int32_t nParentIdx = nFromIdx;
            ERROR_LOG_EXIT0(nFromIdx > 0);

            do
            {
                nParentIdx = (nFromIdx - 1) / 2;

                if ((*_funcCompare)(_pNodeList[nFromIdx]->llKey, _pNodeList[nParentIdx]->llKey))
                {
                    __heap_swap_node(nParentIdx, nFromIdx);
                }
                nFromIdx = nParentIdx;

            } while (nParentIdx > 0);

            return;
Exit0:
            return;
        }

        void __heap_shift_down(int32_t nFromIdx)
        {
            int32_t nTargetChildIdx = 0;
            ERROR_LOG_EXIT0(nFromIdx >= 0 && nFromIdx < _nNodeCnt);

            __get_target_child(nFromIdx, nTargetChildIdx);

            while (nTargetChildIdx > 0)
            {
                if ((*_funcCompare)(_pNodeList[nTargetChildIdx]->llKey, _pNodeList[nFromIdx]->llKey))
                {
                    __heap_swap_node(nTargetChildIdx, nFromIdx);
                    nFromIdx = nTargetChildIdx;
                    __get_target_child(nFromIdx, nTargetChildIdx);
                    continue;
                }
                break;
            }

Exit0:
            return;
        }

    private:
        fix_heap_node*  _pNodeList[N] = {};
        int32_t		    _nNodeCnt = 0;

        FUN_COMPARE_HEAP _funcCompare = nullptr;
    };
}
