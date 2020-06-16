#ifndef REFLECTOBJECT_H
#define REFLECTOBJECT_H

#include <sys/types.h>
#include <cxxabi.h>
#include <string>
#include <iostream>
#include <memory>
#include <functional>
#include <mutex>
#include <unordered_map>

class ReflectObject {

};

template<typename ...Targs>
class ReflectObjectFactory {
public:
    static ReflectObjectFactory& getInstance() {
        static std::once_flag s_once_flag;
        std::call_once(s_once_flag, [&]{
            sInstance.reset(new ReflectObjectFactory());
        });
        return *sInstance;
    }

    ReflectObject* create(const std::string& className, Targs&&... args) {
        std::cout << "Create ReflectObject: " << className << std::endl;
        auto iter = mFuctioncreators.find(className);
        if (iter == mFuctioncreators.end()) {
            std::cout << "Create ReflectObject " << className << " fail" << std::endl;
            return nullptr;
        } else {
            std::cout << "Create ReflectObject " << className << " success" << std::endl;
            return iter->second(std::forward<Targs>(args)...);
        }
    }

    bool regist(const std::string& className, std::function<ReflectObject *(Targs&&... args)> func) {
        if (nullptr == func) {
            std::cout << "Register ReflectObject " << className << " fail" << std::endl;
            return false;
        }
        std::cout << "Register ReflectObject " << className << " success" << std::endl;
        return mFuctioncreators.insert(std::make_pair(className, func)).second;
    }

private:
    ReflectObjectFactory() = default;
    ReflectObjectFactory(const ReflectObjectFactory&) = delete;
    ReflectObjectFactory& operator=(const ReflectObjectFactory&) = delete;

    std::unordered_map<std::string, std::function<ReflectObject*(Targs&&...)>> mFuctioncreators;
    static std::unique_ptr<ReflectObjectFactory<Targs...>> sInstance;
};

template<typename ...Targs>
std::unique_ptr<ReflectObjectFactory<Targs...>> ReflectObjectFactory<Targs...>::sInstance;

template<typename T, typename...Targs>
class ReflectObjectRegister {
public:
    struct Register {
        Register() {
            registObject(getClassName());
        }
        Register(std::string className) {
            registObject(className);
        }
        inline void xxx() const {}
    };

    ReflectObjectRegister() {
        sReflectObjectRegister.xxx();
    }

    virtual ~ReflectObjectRegister() {
        sReflectObjectRegister.xxx();
    }

private:
    static std::string getClassName(){
        #ifdef __GNUC__
        std::string className = abi::__cxa_demangle(typeid(T).name(), nullptr, nullptr, nullptr);
        #else
        //in this format?:
        std::string className = typeid(T).name();
        #endif
        return className;
    }

    static void registObject(std::string className) {
        //        ReflectObjectFactory<Targs...>::getInstance().regist(className, [](Targs&&... args)->T *{
        //            return new T(std::forward<Targs>(args)...);
        //        });
        ReflectObjectFactory<Targs...>::getInstance().regist(className, newObject);
    }

    static T* newObject(Targs&&... args) {
        return new T(std::forward<Targs>(args)...);
    }

    static Register sReflectObjectRegister;
};

template<typename T, typename...Targs>
typename ReflectObjectRegister<T, Targs...>::Register ReflectObjectRegister<T, Targs...>::sReflectObjectRegister;

class ReflectObjectClass {
public:
    template<typename T, typename ...Targs>
    static T* createObject(const std::string& className, Targs&&... args) {
        ReflectObject *object = ReflectObjectFactory<Targs...>::getInstance().create(className, std::forward<Targs>(args)...);

#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wold-style-cast"
        return (T *)(object);
#pragma GCC diagnostic pop
    }
};


#define ReflectObject_SpecRegister_DECLARATION(CLASS_TYPE) \
    static typename ReflectObjectRegister<CLASS_TYPE>::Register sSpecRegister

#define ReflectObject_SpecRegister_IMPLEMENT(CLASS_TYPE, CLASS_NAME) \
    ReflectObjectRegister<CLASS_TYPE>::Register CLASS_TYPE::sSpecRegister(CLASS_NAME)

#endif // REFLECTOBJECT_H
