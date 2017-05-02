#include <iostream>
#include <cstdlib>
#include<utility> //for std::move()
using namespace std;


template<typename ET>
class VecT{
	public:
		VecT(int dsize);              //default constructor
		VecT(const VecT &);       //copy constructor
      VecT(VecT&&rhs);
   	~VecT();                     //destructor
		int getSize()const;          //accessor for size
		VecT &operator=(const VecT&);//copy assignment
	   VecT &operator=(VecT &&);    //move assignment
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
VecT<ET> :: VecT(const VecT &){
	
}

//move constructor
template<typename ET>
VecT<ET> :: VecT(VecT && rhs){
	
}
//destructor
template<typename ET>
VecT<ET>:: ~VecT(){
	cout<<"destructor called"<<endl;
	delete [] ptr;
	ptr=NULL;
	size=0;
}

template<typename ET>
int VecT<ET>:: getSize()const{
	return size;
}



//overload input/output operators- global and non-friend function
/*template<typename ET>

istream &operator>>(istream &in, VecT&v){
	int size=v.getSize();
   
	for(int i=0;i<size;++i){
		
		
	}
	return in;
}
*/
//class member function definitions



//display on monitor details of the authors of code in this assignment
void info(){
	cout<<"***********************************"<<endl;
	cout<<"*   Michael Zhao         *"<<endl;
	cout<<"*           *"<<endl;
	cout<<"***********************************"<<endl<<endl<<endl;
}



int main(){
	info();
	cout<<"test constructor"<<endl;
	VecT<double> v1(20);
	VecT<char> v2(4);
	
	cout<<"test int getSize()const "<<endl;\
	cout<<v1.getSize()<<endl;
	cout<<v2.getSize()<<endl;
	//cin>>v1;
	
}
