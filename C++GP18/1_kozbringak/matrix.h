#pragma once

#include <vector>

#include "common.h"

template <class T>
class matrix_t
{
  public:
	using row_t = std::vector<T>;
	using base_t = std::vector<row_t>;

	matrix_t() {}
	explicit matrix_t(size_t R, size_t C, T const &value = T()) : m(R, row_t(C, value)) {}

	void resize(size_t R, size_t C, T const &value = T()) { m.resize(R, row_t(C, value)); }
	row_t &operator[](size_t p) { return m[p]; }
	row_t const &operator[](size_t p) const { return m[p]; }
	T &operator[](point p) { return m[p.first][p.second]; }
	T const &operator[](point p) const { return m[p.first][p.second]; }
	size_t height() const { return m.size(); }
	size_t width() const { return m.empty() ? 0 : m[0].size(); }

	typename base_t::iterator begin() { return m.begin(); }
	typename base_t::iterator end() { return m.end(); }
	typename base_t::const_iterator begin() const { return m.begin(); }
	typename base_t::const_iterator end() const { return m.end(); }

  protected:
	base_t m;
};

using matrix = matrix_t<int>;
