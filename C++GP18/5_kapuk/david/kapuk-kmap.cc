#include <algorithm>
#include <array>
#include <cassert>
#include <iostream>
#include <map>
#include <set>
#include <string>
#include <cstdio>
#include <vector>

template <class T>
class optional
{
  public:
	optional() : t(), b(false) {}
	optional(T const &t) : t(t), b(true) {}
	optional(optional const &o) : t(o.t), b(o.b) {}
	T &operator*() { return assert(b), t; }
	T const &operator*() const { return assert(b), t; }
	T *operator->() { return assert(b), &t; }
	T const *operator->() const { return assert(b), &t; }
	operator bool() const { return b; }
	bool has_value() const { return b; }
	T &value() { return assert(b), t; }
	T const &value() const { return assert(b), t; }
	T value_or(T const &v) const { return b ? t : v; }
	optional &operator=(T const &v) { return b = true, t = v, *this; }
	optional &operator=(optional const &o) { return b = o.b, t = o.t, *this; }
	bool operator==(T const &v) const { return b && t == v; }
	bool operator==(optional const &o) const { return b == o.b && (!b || t == o.t); }

  private:
	T t;
	bool b;
};

template <typename T>
struct reversion_wrapper
{
	T &iterable;
};

template <typename T>
auto begin(reversion_wrapper<T> w) { return std::rbegin(w.iterable); }

template <typename T>
auto end(reversion_wrapper<T> w) { return std::rend(w.iterable); }

template <typename T>
reversion_wrapper<T> reverse(T &&iterable) { return {iterable}; }

using point = std::pair<uint32_t, uint32_t>;

std::ostream &operator<<(std::ostream &str, point const &p)
{
	return str << p.first << ',' << p.second;
}

template <class T>
class matrix_t
{
  public:
	using row_t = std::vector<T>;
	using base_t = std::vector<row_t>;

	matrix_t() {}
	explicit matrix_t(size_t R, size_t C, T const &value = T()) : m(R, row_t(C, value)) {}

	void resize(size_t R, size_t C, T const &value = T()) { m.resize(R, row_t(C, value)); }
	row_t &operator[](uint32_t p) { return m[p]; }
	row_t const &operator[](uint32_t p) const { return m[p]; }
	T &operator[](point p) { return m[p.first][p.second]; }
	T const &operator[](point p) const { return m[p.first][p.second]; }
	size_t height() const { return m.size(); }
	size_t width() const { return m.empty() ? 0 : m[0].size(); }
	point dim() const { return {height(), width()}; }

	typename base_t::iterator begin() { return m.begin(); }
	typename base_t::iterator end() { return m.end(); }
	typename base_t::const_iterator begin() const { return m.begin(); }
	typename base_t::const_iterator end() const { return m.end(); }

  protected:
	base_t m;
};

using matrix = matrix_t<optional<uint32_t>>;

struct rect
{
	point pos;
	point dim;
	bool operator<(rect const &o) const { return pos < o.pos || (pos == o.pos && dim < o.dim); }
};

using rects = std::set<rect>;

std::ostream &operator<<(std::ostream &str, rect const &r)
{
	return str << r.pos << ':' << r.dim.first << '*' << r.dim.second;
}

std::ostream &operator<<(std::ostream &str, rects const &rs)
{
	bool first = true;
	for (auto const &r : rs)
	{
		if (!first)
			str << ' ';
		str << r;
		first = false;
	}
	return str;
}

struct minterm
{
	uint32_t value = 0;
	uint32_t mask = 0;
	bool operator<(minterm const &o) const { return value < o.value || (value == o.value && mask < o.mask); }
};

using minterms = std::set<minterm>;

std::ostream &operator<<(std::ostream &str, minterm mt)
{
	size_t n = 0;
	bool first = true;
	while (mt.mask)
	{
		if (mt.mask & 1)
		{
			if (!first)
				str << '&';
			if (!(mt.value & 1))
				str << '!';
			str << n;
			first = false;
		}
		mt.mask >>= 1;
		mt.value >>= 1;
		++n;
	}
	return str;
}

std::ostream &operator<<(std::ostream &str, minterms const &mts)
{
	bool first = true;
	for (auto const &mt : mts)
	{
		if (!first)
			str << " | ";
		str << mt;
		first = false;
	}
	return str;
}

enum gate_type
{
	NOT,
	AND,
	OR,
	XOR,
	NXOR,

	GATE_COUNT
};

constexpr std::array<const char *, GATE_COUNT> gate_type_names{"NOT", "AND", "OR", "XOR", "NXOR"};

struct gate
{
	gate_type gt;
	std::vector<size_t> in;
	bool operator<(gate const &o) const { return gt < o.gt || (gt == o.gt && std::lexicographical_compare(in.begin(), in.end(), o.in.begin(), o.in.end())); }
};

uint32_t binary2gray(uint32_t v)
{
	return v ^ (v >> 1);
}

uint32_t gray2binary(uint32_t v)
{
	v = v ^ (v >> 16);
	v = v ^ (v >> 8);
	v = v ^ (v >> 4);
	v = v ^ (v >> 2);
	v = v ^ (v >> 1);
	return v;
}

void dump(matrix const &m, size_t nbit)
{
	for (auto const &r : m)
	{
		for (auto const &v : r)
		{
			bool const bit = (v.value_or(0) >> nbit) & 1;
			std::cerr << (!v ? '*' : bit ? '1' : '0');
		}
		std::cerr << std::endl;
	}
}

bool contains(uint32_t ca, uint32_t cl, uint32_t a, uint32_t l, uint32_t m)
{
	return l <= cl &&
		   ((ca == 0 && cl == m) ||					// full
			(ca <= a && a + l <= ca + cl) ||		// classic
			(m < ca + cl && a + l <= ca + cl - m)); // wrap around
}

bool contains(rect const &rc, rect const &r, point const &dim)
{
	return contains(rc.pos.first, rc.dim.first, r.pos.first, r.dim.first, dim.first) &&
		   contains(rc.pos.second, rc.dim.second, r.pos.second, r.dim.second, dim.second);
}

rects optimize(matrix const &m, size_t nbit)
{
	point const mdim = m.dim();
	rects minterms;
	rects rs;
	rects next_rs;

	point p;
	for (p.first = 0; p.first < mdim.first; ++p.first)
		for (p.second = 0; p.second < mdim.second; ++p.second)
		{
			rect r{p, {1, 1}};
			bool const bit = (m[p].value_or(0) >> nbit) & 1;
			if (!m[p] || bit)
			{
				rs.insert(r);
			}
			if (bit)
			{
				minterms.insert(r);
			}
		}

	while (!rs.empty())
	{
		for (auto const &r : rs)
		{
			rect const rd{{(r.pos.first + r.dim.first) % mdim.first, r.pos.second}, r.dim};
			rect const r_rd{r.pos, {2 * r.dim.first, r.dim.second}};
			if ((r_rd.dim.first < mdim.first || (r_rd.pos.first == 0 && r_rd.dim.first == mdim.first)) && rs.count(rd))
			{
				next_rs.insert(r_rd);
				if (minterms.count(r) || minterms.count(rd))
				{
					minterms.insert(r_rd);
				}
			}

			rect const rr{{r.pos.first, (r.pos.second + r.dim.second) % mdim.second}, r.dim};
			rect const r_rr{r.pos, {r.dim.first, 2 * r.dim.second}};
			if ((r_rr.dim.second < mdim.second || (r_rr.pos.second == 0 && r_rr.dim.second == mdim.second)) && rs.count(rr))
			{
				next_rs.insert(r_rr);
				if (minterms.count(r) || minterms.count(rr))
				{
					minterms.insert(r_rr);
				}
			}
		}
		for (auto const &r : rs)
		{
			for (auto const &nr : next_rs)
			{
				if (contains(nr, r, mdim))
				{
					minterms.erase(r);
					break;
				}
			}
		}
		rs = std::move(next_rs);
	}

	return minterms;
}

minterm decode_range(uint32_t s, uint32_t l, uint32_t m)
{
	uint32_t const e = (s + l) % m;
	uint32_t all = -1;
	uint32_t any = 0;
	do
	{
		uint32_t const sb = gray2binary(s);
		all &= sb;
		any |= sb;
		s = (s + 1) % m;
	} while (s != e);
	return {all, ~(all ^ any) % m};
}

minterms decode(rects const &rs, point const &mdim, uint32_t C)
{
	minterms mts;
	for (auto const &r : rs)
	{
		minterm mt_r = decode_range(r.pos.first, r.dim.first, mdim.first);
		minterm mt_c = decode_range(r.pos.second, r.dim.second, mdim.second);
		minterm mt{(mt_r.value << C) | mt_c.value, (mt_r.mask << C | mt_c.mask)};
		mts.insert(mt);
	}
	return mts;
}

void build(matrix const &m, size_t K, size_t L)
{
	std::map<gate, size_t> gm;
	std::vector<gate> gv;

	auto get_gate = [&gm, &gv](gate const &g) {
		auto ib = gm.insert({g, gv.size()});
		if (ib.second)
		{
			gv.push_back(g);
		}
		return ib.first->second;
	};
	auto use_gate = [K, &get_gate](gate const &g) {
		assert(!g.in.empty());
		return g.in.size() > 1 ? K + get_gate(g) : g.in[0];
	};

	gate const gzero{XOR, {0, 0}}; // constant 0
	gate const gone{NXOR, {0, 0}}; // constant 1
	gate gnot{NOT, {0}};
	gate gor{OR};
	gate gand{AND};
	std::vector<size_t> out(L);

	for (size_t l = 0; l < L; ++l)
	{
		// std::cerr << "Bit #" << l << std::endl;
		// dump(m, l);
		rects rect_minterms = optimize(m, l);
		// std::cerr << rect_minterms << std::endl;
		minterms alg_minterms = decode(rect_minterms, m.dim(), K / 2);
		// std::cerr << alg_minterms << std::endl
		// 		  << std::endl;

		if (alg_minterms.empty())
		{
			out[l] = K + get_gate(gzero);
		}
		else
		{
			for (auto mt : alg_minterms)
			{
				for (size_t i = 0; mt.mask; ++i)
				{
					if (mt.mask & 1)
					{
						if ((mt.value & 1) == 0)
						{
							gnot.in[0] = i;
							gand.in.push_back(K + get_gate(gnot));
						}
						else
						{
							gand.in.push_back(i);
						}
					}
					mt.mask >>= 1;
					mt.value >>= 1;
				}
				if (!gand.in.empty())
				{
					gor.in.push_back(use_gate(gand));
				}
				gand.in.clear();
			}
			out[l] = use_gate(gor.in.empty() ? gone : gor);
			gor.in.clear();
		}
	}

	std::cout << gv.size() << std::endl;
	for (auto const &g : gv)
	{
		std::cout << gate_type_names[g.gt] << ' ' << g.in.size();
		for (auto n : g.in)
		{
			std::cout << ' ' << n + 1;
		}
		std::cout << std::endl;
	}
	for (auto n : out)
	{
		std::cout << n + 1 << std::endl;
	}
}

uint32_t str2bin(std::string s)
{
	uint32_t v = 0;
	for (char ch : reverse(s))
	{
		v = (v << 1) | (ch == '1');
	}
	return v;
}

int main(int argc, const char *argv[])
{
	if (argc > 1)
	{
		std::freopen(argv[1], "r", stdin);
	}

	size_t K, L;
	std::cin >> K >> L;
	assert(2 <= K && K <= 32);
	assert(1 <= L && L <= 32);

	size_t const C = K / 2;
	size_t const R = K - C;
	matrix m(1 << R, 1 << C);

	size_t N;
	std::cin >> N;
	assert(1 <= N && N <= 1u << std::min<int>(K, 15));
	while (N-- > 0)
	{
		std::string ins, outs;
		std::cin >> ins >> outs;
		uint32_t v = str2bin(ins);
		point p{binary2gray(v >> C), binary2gray(v & ((1 << C) - 1))};
		m[p] = str2bin(outs);
	}

	build(m, K, L);

	return 0;
}
