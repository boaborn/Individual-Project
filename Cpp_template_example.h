#ifndef A2P2_H
#define A2P2_H
#include<cassert>
#include <iostream>
#include <cstdlib>
#include<utility> //for std::move()
using namespace std;


template<typename ET>
class VecT{
	public:
		VecT(int dsize);              //default constructor
		VecT(const VecT &);           //copy constructor
      VecT(VecT && rhs);            //move constructor
   	~VecT();                      //destructor
		int getSize()const;           //accessor for size
		void setValue(int& i, ET&value);//set a value to array
		ET getValue(int & i);          
		VecT<ET>& operator=(const VecT<ET>&right);//copy assignment
	   VecT<ET>& operator=(VecT &&);    //move assignment
		bool operator==(const VecT & right);
		bool operator!=(const VecT & right);
		ET &operator[](int);
		ET operator[](int) const;
		
	private:
		int size;
		ET *ptr;
};

//default constructor
template<typename ET>
VecT<ET> :: VecT(int dsize){
	size= (dsize>0 ? dsize:5);
	ptr=new ET[size];
}

//copy constructor
template<typename ET>
VecT<ET> :: VecT(const VecT & rhs):size(rhs.size){
	//cout<<"copy constructor called"<<endl;
	ptr=new ET[size];
	for(int i=0;i<size;++i){
		ptr[i]=rhs.ptr[i];
	}
}

//move constructor
template<typename ET>
VecT<ET> :: VecT(VecT<ET> && rhs):size{rhs.size},ptr{rhs.ptr}{
	//cout<<"move constructor called"<<endl;
	rhs.size=0;
	rhs.ptr=nullptr;
}

//destructor
template<typename ET>
VecT<ET>:: ~VecT(){
	//cout<<"destructor called"<<endl;
	delete [] ptr;
	ptr=NULL;
	size=0;
}

//returns the size 
template<typename ET>
int VecT<ET>:: getSize()const{
	return size;
}

//set a value to array
template<typename ET>
void VecT<ET> :: setValue(int &i, ET&value){
	ptr[i]=value;
}

//returns a value from position[i]
template<typename ET>
ET VecT<ET>:: getValue(int & i){
	return ptr[i];
}

//copy assignment
template<typename ET>
VecT<ET> & VecT<ET>:: operator=(const VecT<ET>&right){
	//cout<<"copy assignment = called"<<endl;
	if(&right!=this){
		if(size!=right.size){
			delete[]ptr;
			size=right.size;
			ptr=new ET[size];
		}
		for(int i=0;i<size;++i){
			ptr[i]=right.ptr[i];
		}
	}
}
//move assignment
template<typename ET>
VecT<ET>& VecT<ET>::operator=(VecT &&right){
	//cout<<"move assignment called"<<endl;
	delete[] ptr;
	ptr=right.ptr;
	size=right.size;
	right.ptr=nullptr;
	right.size=0;
	return *this;
}

//overload equal operator
template<typename ET>
bool VecT<ET>:: operator==(const VecT<ET> & right){
	if(size==right.size){
		for(int i=0;i<size;++i){
			if(ptr[i]!=right.ptr[i]){
				return false;
			}
		}
		return true;
	}
	return false;
}

//overload not equal operator
template<typename ET>
bool VecT<ET>:: operator!=(const VecT<ET> & right){
	if(size==right.size){
		for(int i =0;i<size;++i){
			if(ptr[i]!=right.ptr[i]){
				return true;
			}
		}
		return false;
	}
	return true;
}

//subscript operator for non-const objects
//read&write
template<typename ET>
ET& VecT<ET>::operator[](int i){
	assert((i>=0)&&(i<size));
	return ptr[i];
}
//read only
template<typename ET>
ET VecT<ET>::operator[](int i) const{
	assert((i>=0)&&(i<size));
	return ptr[i];
}


//overload input/output operators- global and non-friend function,reading [size] of  ET type data
//overload input
template<typename ET>
istream &operator>>(istream &in, VecT<ET>&v){
	int size=v.getSize();
   ET val;
	for(int i=0;i<size;++i){
		in>>val;
		v.setValue(i,val);
	}
	return in;
}

//overload output
template<typename ET>
ostream &operator<<(ostream &out,VecT<ET>&v){
	int size=v.getSize();
	ET val;
	for(int i=0;i<size;++i){
		val=v.getValue(i);
		out<<val<<" ";
	}
	out<<endl;
	return out;
}
//class member function definitions



//display on monitor details of the authors of code in this assignment
void info(){
	cout<<"***********************************"<<endl;
	cout<<"*   159.234  -   A2 Part2 Solution*"<<endl;
	cout<<"*   Jia Qi Zhao, 12171900         *"<<endl;
	cout<<"*   Solomon Boe, 14132392         *"<<endl;
	cout<<"***********************************"<<endl<<endl<<endl;
}

/*I put my makefile in here, in case that you might need it. This header file only compiler in QB lab computer, it doesnt works on computers in AT lab. I guess the compiler in AT Lab might be old version
main.exe		: main.o   
	g++ -Wl,-s -std=c++11 -o main.exe main.o  

main.o		:  main.cpp A2P2.h 
	g++ -c -std=c++11 -fpermissive -fconserve-space main.cpp
A2P2.o		:    A2P2.h 
	g++ -c -std=c++11 -fpermissive -fconserve-space 
*/

#endif
