#pragma once

#define ASSERT_ENABLED
#ifdef ASSERT_ENABLED
#define ASSERT(x) do { if(!(x)) { std::cerr << "Assertion failure: " << __LINE__ << "@" << __FILE__ << ": " << #x << std::endl; _CrtDbgBreak(); } } while(0)
#else
#define ASSERT(x)
#endif

#define ALL(container) container.begin(), container.end()

template<class Container, class T>
bool contains(const Container& Container, const T& Value)
{
	return std::find(ALL(Container), Value) != Container.end();
}

template<class T>
bool contains(const std::set<T>& Set, const T& Value)
{
	return Set.find(Value) != Set.end();
}

template<class T1, class T2>
bool contains(const std::map<T1, T2>& Map, const T1& Value)
{
	return Map.find(Value) != Map.end();
}

template <class T>
void Dump(const T& Container, std::string Label)
{
	std::cout << Label << ": [ ";
	for (auto& x : Container)
	{
		std::cout << x << " ";
	}
	std::cout << "]" << std::endl;
}
