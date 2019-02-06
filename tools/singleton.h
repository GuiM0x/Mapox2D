#ifndef SINGLETON_H
#define SINGLETON_H

#include <stdexcept>
#include <typeinfo>
#include <string>

template<typename T>
class Singleton
{
public:
    template<typename ...Args>
    static T& createInstance(Args ...args)
    {
        ++m_count;
        if(m_count > 1){
            std::string typeInfo = typeid(T).name();
            throw(std::logic_error{typeInfo +
                                   " - More than one instance not authorized"});
        }
        static T obj{(args)...};
        return obj;
    }

private:
    static int m_count;
};

template<typename T>
int Singleton<T>::m_count = 0;

#endif // SINGLETON_H
