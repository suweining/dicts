/*
 * Singleton.h
 *
 *  Created on: 2014年5月19日
 *      Author: quanliangliang
 */

#ifndef SINGLETON_H_
#define SINGLETON_H_


template <class T>
class Singleton {
public:
    static T* GetInstance() {
        static T* s_instance = new T();
        return s_instance;
    }
protected:
    Singleton() {};
    virtual ~Singleton() {};

private:
    Singleton(const Singleton&);
    Singleton& operator=(const Singleton&);
};


#endif /* SINGLETON_H_ */
