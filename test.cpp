
#include <iostream>
#include <typeinfo>
#include <vector>

class void_vec {

  std::vector<std::pair<void*, std::string>> pv;
  
  std::string const & get_type(size_t const s) {
    return pv.at(s).second;
  }

public:

  ~void_vec() {
    for(std::pair<void*, std::string> & p : pv) {
      if(p.second == typeid(int).name()) {
	delete (int*)p.first;
      }
      else if(p.second == typeid(double).name()) {
	delete (double*)p.first;
      }
    }
  }

  void push_back(int * i) {
    pv.push_back(std::pair<void *, std::string>(i, typeid(int).name()));
  }
  
  void push_back(double * d) {
    pv.push_back(std::pair<void *, std::string>(d, typeid(double).name()));
  }

  template <class T>
  T * at(size_t const s) {
    if(typeid(T).name() != get_type(s)) {
      std::cout << __LINE__ << " " << __PRETTY_FUNCTION__ << "\n"
		<< "ERROR: wrong requested type\n";
      exit(1);
    }
    return (T*)pv.at(s).first;
  }

};




int main() {

  void_vec v;

  int * i = new int(1);

  double * d = new double(1.2);

  v.push_back(i);
  v.push_back(d);

  int * i2 = v.at<int>(0);
  double * d2 = v.at<double>(1);

  std::cout << *v.at<int>(0) << "\n";
  std::cout << *v.at<double>(1) << "\n";

  //v.at<double>(0);
  //v.at<int>(1);

  return 0;

}
