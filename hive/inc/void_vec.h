#ifndef VOIDVEC_H
#define VOIDVEC_H

#include <iostream>


class void_vec {

	std::vector<std::pair<void *, std::string>> pv;

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

	size_t size() const {return pv.size();}

	void push_back(int * i) {
		pv.push_back(std::pair<void *, std::string>(i, typeid(int).name()));
	}

	void push_back(double * d) {
		pv.push_back(std::pair<void *, std::string>(d, typeid(double).name()));
	}

	std::string const & get_type(size_t const s) const {
		return pv.at(s).second;
	}

	template <class T>
		T * at(size_t const s) const {
			if(typeid(T).name() != get_type(s)) {
				std::cout << __LINE__ << " " << __PRETTY_FUNCTION__ << "\n"
					<< "ERROR: wrong requested type\n";
				exit(1);
			}
			return (T*)pv.at(s).first;
		}

	template <class T>
		T * front() const {
			return this->at<T>(0);
		}

	template <class T>
		T * back() const {
			return this->at<T>(pv.size()-1);
		}

	void print() {
		for(std::pair<void *, std::string> const & p : pv) {
			if(p.second == typeid(int).name()) {
				std::cout <<p.second<<" "<< *(int*)p.first << "\n";
			}
			else if(p.second == typeid(double).name()) {
				std::cout <<p.second<<" "<< *(double*)p.first << "\n";
			}
		}
	}

};



#endif
