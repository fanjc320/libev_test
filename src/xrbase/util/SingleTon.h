/*
 *
 *      Author: venture
 */
#pragma once

template <typename T>
class SingleTon
{
public:
    static T* GetInstance()
    {
        if (nullptr == pT)
            pT = new T();

        return pT;
    }

    static void DestroyInstance()
    {
        if (nullptr == pT) return;

        delete pT;
        pT = nullptr;
    }

    static void SetInstance(T* p)
    {
        if (pT != nullptr)
            DestroyInstance();
        pT = p;
    }

protected:
    SingleTon() = default;
    virtual ~SingleTon() = default;

    SingleTon(SingleTon&) = delete;
    SingleTon(const SingleTon&) = delete;
    void operator = (SingleTon const&) = delete;

private:
    static T* pT;
};

template <typename T>
T* SingleTon<T>::pT = nullptr;
