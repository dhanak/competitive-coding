#include <algorithm>
#include <array>
#include <cassert>
#include <iostream>
#include <map>
#include <set>
#include <string>
#include <cstdio>
#include <vector>

using in_t = uint8_t;
constexpr size_t max_K = 8;
constexpr size_t max_L = 32;
static_assert(std::numeric_limits<in_t>::max() >= (1ull << max_K) - 1, "too large max_K");

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
auto begin(reversion_wrapper<T> w)
{
	return std::rbegin(w.iterable);
}

template <typename T>
auto end(reversion_wrapper<T> w)
{
	return std::rend(w.iterable);
}

template <typename T>
reversion_wrapper<T> reverse(T &&iterable)
{
	return {iterable};
}

using data_t = std::vector<optional<uint32_t>>;

class minterm
{
  public:
	in_t const value;
	in_t const mask;

	minterm() : value(0), mask(0), size(0) {}
	minterm(in_t v, in_t m) : value(v), mask(m), size(0)
	{
		for (in_t m = mask; m; m >>= 1)
		{
			if (m & 1)
				++size;
		}
	}
	bool operator<(minterm const &o) const { return size < o.size || (size == o.size && mask < o.mask) || (size == o.size && mask == o.mask && value < o.value); }
	bool operator==(minterm const &o) const { return (value & mask) == (o.value & o.mask); }

  private:
	uint8_t size;
};

using minterms = std::set<minterm>;

std::ostream &operator<<(std::ostream &str, minterm const &mt)
{
	in_t v = mt.value;
	in_t m = mt.mask;

	if (m == 0)
	{
		str << 1;
	}
	else
	{
		for (size_t n = 0; m; ++n)
		{
			if (m & 1)
			{
				if (!(v & 1))
					str << '!';
				str << static_cast<char>('a' + n);
			}
			m >>= 1;
			v >>= 1;
		}
	}
	return str;
}

std::ostream &operator<<(std::ostream &str, minterms const &mts)
{
	if (mts.empty())
	{
		str << 0;
	}
	else
	{
		std::string sep;
		for (auto const &mt : mts)
		{
			str << sep;
			str << mt;
			sep = " + ";
		}
	}
	return str;
}

enum gate_type
{
	XOR,
	NXOR,
	NOT,
	AND,
	OR,

	GATE_COUNT
};

constexpr std::array<const char *, GATE_COUNT> gate_type_names{"XOR", "NXOR", "NOT", "AND", "OR"};

struct gate
{
	gate_type const gt;
	std::vector<size_t> in;

	void sort() { std::sort(in.begin(), in.end()); }
	void print(std::ostream &str) const;
	bool operator<(gate const &o) const
	{
		return gt < o.gt ||
			   (gt == o.gt && in.size() < o.in.size()) ||
			   (gt == o.gt && in.size() == o.in.size() && std::lexicographical_compare(in.begin(), in.end(), o.in.begin(), o.in.end()));
	}
	bool subsetOf(gate const &o) const
	{
		return std::includes(o.in.begin(), o.in.end(), in.begin(), in.end());
	}
};

std::ostream &operator<<(std::ostream &str, gate const &g)
{
	g.print(str);
	return str;
}

class circuit
{
  public:
	explicit circuit(size_t K, size_t L) : K(K), L(L), out(L) {}
	void create(data_t const &data);
	void print(std::ostream &ostr) const;

  private:
	size_t const K;
	size_t const L;
	gate const gzero{XOR, {0, 0}}; // constant 0
	gate const gone{NXOR, {0, 0}}; // constant 1

	std::map<gate, size_t> gatemap;
	std::vector<gate> ordered_gates;
	std::vector<gate const *> gates;
	std::vector<size_t> out;

	size_t get_gate(gate const &g)
	{
		auto result = gatemap.insert({g, gates.size()});
		if (result.second)
		{
			gates.push_back(&result.first->first);
		}
		return result.first->second;
	}

	size_t use_gate(gate const &g)
	{
		assert(!g.in.empty());
		return g.in.size() > 1 ? K + get_gate(g) : g.in[0];
	}

	minterms optimize(data_t const &data, size_t l);
	void build(size_t l, minterms const &mts);
	void reduce();
};

std::ostream &operator<<(std::ostream &str, circuit const &c)
{
	c.print(str);
	return str;
}

void circuit::create(data_t const &data)
{
	for (size_t l = 0; l < L; ++l)
	{
		minterms mts = optimize(data, l);
		std::cerr << '#' << l << ": " << mts << std::endl;
		build(l, mts);
	}
	reduce();
}

minterms circuit::optimize(data_t const &data, size_t l)
{
	minterms all_mts, curr_mts, next_mts;

	using u8set = std::set<in_t>;
	u8set ones, zeroes;
	for (uint64_t i = (1ull << K); i-- > 0;)
	{
		minterm const mt(
			i, static_cast<in_t>((1ull << K) - 1));
		if (data[i])
		{
			const bool one = ((*data[i] >> l) & 1);
			(one ? ones : zeroes).insert(i);
			if (one)
			{
				curr_mts.insert(mt);
				all_mts.insert(mt);
			}
		}
		else
		{
			all_mts.insert(mt);
		}
	}

	// special cases
	if (ones.empty())
	{
		return {};
	}
	else if (zeroes.empty())
	{
		return {minterm()};
	}

	// minimize
	while (!curr_mts.empty())
	{
		for (auto const &mt : curr_mts)
		{
			for (size_t k = 0; k < K; ++k)
			{
				in_t const bit = (1ul << k);
				minterm const other_mt{static_cast<in_t>(mt.value ^ bit), mt.mask};
				minterm const merged_mt{static_cast<in_t>(mt.value & other_mt.value), static_cast<in_t>(mt.mask & ~bit)};
				if (all_mts.count(other_mt))
				{
					all_mts.insert(merged_mt);
					next_mts.insert(merged_mt);
				}
			}
		}
		curr_mts = std::move(next_mts);
	}

	// reduce
	for (auto const &mt : all_mts)
	{
		bool use = false;
		for (in_t o : u8set(ones))
		{
			if ((mt.value & mt.mask) == (o & mt.mask))
			{
				ones.erase(o);
				use = true;
			}
		}
		if (use)
		{
			curr_mts.insert(mt);
		}
		if (ones.empty())
			break;
	}

	return curr_mts;
}

void circuit::build(size_t l, minterms const &mts)
{
	gate gnot{NOT, {0}};
	gate gor{OR};
	gate gand{AND};

	if (mts.empty())
	{
		out[l] = K + get_gate(gzero);
	}
	else
	{
		for (auto mt : mts)
		{
			in_t v = mt.value;
			in_t m = mt.mask;
			for (size_t i = 0; m; ++i)
			{
				if (m & 1)
				{
					if ((v & 1) == 0)
					{
						gnot.in[0] = i;
						gand.in.push_back(K + get_gate(gnot));
					}
					else
					{
						gand.in.push_back(i);
					}
				}
				m >>= 1;
				v >>= 1;
			}
			if (!gand.in.empty())
			{
				gand.sort();
				gor.in.push_back(use_gate(gand));
				gand.in.clear();
			}
		}
		gor.sort();
		out[l] = use_gate(gor.in.empty() ? gone : gor);
		gor.in.clear();
	}
}

void gate::print(std::ostream &ostr) const
{
	ostr << gate_type_names[gt] << ' ' << in.size();
	for (auto n : in)
	{
		ostr << ' ' << n + 1;
	}
}

void circuit::print(std::ostream &ostr) const
{
	ostr << gates.size() << std::endl;
	for (auto const &g : gates)
	{
		ostr << *g << std::endl;
	}
	for (auto n : out)
	{
		ostr << n + 1 << std::endl;
	}
}

void circuit::reduce()
{
	// prepare remapping
	std::vector<size_t> order_map(gates.size());
	ordered_gates.reserve(gates.size());
	size_t i = 0;
	for (auto iThis = gatemap.begin(); iThis != gatemap.end(); ++iThis)
	{
		ordered_gates.push_back(iThis->first);
		order_map[iThis->second] = i++;
	}

	// reduce
	i = gates.size() - 1;
	for (auto iThis = gatemap.rbegin(); iThis != gatemap.rend(); ++iThis, --i)
	{
		gate &gthis = ordered_gates[i];
		if (gthis.gt == NOT)
			break;

		auto iNext = iThis;
		++iNext;
		for (size_t j = i - 1; iNext != gatemap.rend() && iThis->first.gt == iNext->first.gt; ++iNext, --j)
		{
			gate const &gother = ordered_gates[j];
			if (gother.subsetOf(gthis))
			{
				auto in1 = gother.in.begin();
				auto in2 = gthis.in.begin();
				while (in1 != gother.in.end())
				{
					if (*in1 == *in2)
					{
						++in1;
						in2 = gthis.in.erase(in2);
					}
					else
					{
						++in2;
					}
				}
				gthis.in.push_back(K + iNext->second);
			}
		}
	}

	// remap
	for (i = 0; i < gates.size(); ++i)
	{
		gates[i] = &ordered_gates[i];
		for (auto &v : ordered_gates[i].in)
		{
			if (v >= K)
			{
				v = K + order_map[v - K];
			}
		}
	}
	for (auto &v : out)
	{
		if (v >= K)
		{
			v = K + order_map[v - K];
		}
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
	assert(2 <= K && K <= max_K);
	assert(1 <= L && L <= max_L);

	size_t N;
	std::cin >> N;
	assert(1 <= N && N <= 1u << std::min<int>(K, 15));

	data_t data(1u << std::min<int>(K, 15));
	while (N-- > 0)
	{
		std::string ins, outs;
		std::cin >> ins >> outs;
		assert(ins.size() == K && outs.size() == L);
		data[str2bin(ins)] = str2bin(outs);
	}

	circuit c(K, L);
	c.create(data);
	std::cout << c;

	return 0;
}
