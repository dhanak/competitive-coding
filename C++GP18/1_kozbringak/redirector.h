#pragma once

#include <string>
#include <vector>
#include <fstream>
#include <iostream>

class Redirector
{
  public:
	template <class... T>
	explicit Redirector(T &&... t) : Redirector(std::vector<std::string>(std::forward<T>(t)...)) {}
	explicit Redirector(std::vector<std::string> args);
	~Redirector();

  private:
	std::ifstream fin;
	std::ofstream fout;
	std::basic_streambuf<char> *rdcin = nullptr;
	std::basic_streambuf<char> *rdcout = nullptr;
};
