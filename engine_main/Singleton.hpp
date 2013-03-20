#pragma once


template<class T>
class Singleton {
protected:
	static T* singleton;
public:
	static T* getSingleton() {
		if(singleton == 0) singleton = new T();
		
		return singleton;
	}
};

template<class T>
T* Singleton<T>::singleton = 0;