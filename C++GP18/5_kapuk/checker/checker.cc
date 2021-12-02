#include <algorithm>
#include <functional>
#include <fstream>
#include <iostream>
#include <map>
#include <vector>

#define check(cond) check_condition((cond), __FILE__, __LINE__, #cond)

void check_condition(bool cond, const char *file, int line, const char *conds)
{
	if (!cond)
	{
		std::cerr << "Condition '" << conds << "' failed at " << file << ':' << line << std::endl;
		std::cout << -1 << std::endl;
		exit(1);
	}
}

class gate_type
{
  public:
	std::function<bool(std::vector<bool> const &)> fn;
	size_t min = 1;
	size_t max = std::numeric_limits<size_t>::max();
	bool operator()(std::vector<bool> const &in) const { return fn(in); }
};

std::map<std::string, gate_type> const gate_types{
	{"NOT", gate_type{[](std::vector<bool> const &in) { return !in[0]; }, 1, 1}},
	{"OR", gate_type{[](std::vector<bool> const &in) { return std::any_of(in.begin(), in.end(), [](bool b) { return b; }); }, 2}},
	{"AND", gate_type{[](std::vector<bool> const &in) { return std::all_of(in.begin(), in.end(), [](bool b) { return b; }); }, 2}},
	{"XOR", gate_type{[](std::vector<bool> const &in) { return std::count(in.begin(), in.end(), 1) == 1; }, 2, 2}},
	{"NOR", gate_type{[](std::vector<bool> const &in) { return !std::any_of(in.begin(), in.end(), [](bool b) { return b; }); }, 2}},
	{"NAND", gate_type{[](std::vector<bool> const &in) { return !std::all_of(in.begin(), in.end(), [](bool b) { return b; }); }, 2}},
	{"NXOR", gate_type{[](std::vector<bool> const &in) { return std::count(in.begin(), in.end(), 1) != 1; }, 2, 2}},
};

class circuit
{
  public:
	circuit(size_t K, size_t G);
	void add_gate(gate_type const &gt, std::vector<size_t> const &in);
	void calculate(std::vector<bool> const &in);
	bool operator[](size_t n) const { return nodes[n]; }

  protected:
	struct gate
	{
		gate_type const &gt;
		std::vector<size_t> in;
		size_t out;
	};

	size_t const K;
	std::vector<bool> nodes;
	std::vector<gate> gates;
};

circuit::circuit(size_t K, size_t G) : K(K), nodes(K)
{
	nodes.reserve(K + G);
	gates.reserve(G);
}

void circuit::add_gate(gate_type const &gt, std::vector<size_t> const &in)
{
	check(gt.min <= in.size() && in.size() <= gt.max);
	check(std::all_of(in.begin(), in.end(), [this](size_t n) { return n < nodes.size(); }));
	gates.push_back(gate{gt, in, nodes.size()});
	nodes.push_back(false);
}

void circuit::calculate(std::vector<bool> const &in)
{
	check(in.size() == K);
	for (size_t i = 0; i < K; ++i)
	{
		nodes[i] = in[i];
	}

	std::vector<bool> gin;
	for (gate const &g : gates)
	{
		gin.clear();
		gin.reserve(g.in.size());
		for (size_t i : g.in)
		{
			gin.push_back(nodes[i]);
		}
		nodes[g.out] = g.gt(gin);
	}
}

void usage(const char *name)
{
	std::cout << "Usage: " << name << " <intput> <solution>" << std::endl;
	exit(1);
}

int main(int argc, const char *argv[])
{
	if (argc < 3)
	{
		usage(argv[0]);
	}

	std::ifstream inf(argv[1]);
	std::ifstream solf(argv[2]);
	if (!inf.is_open() || !solf.is_open())
	{
		usage(argv[0]);
	}

	size_t K, L; // number of inputs and outputs
	inf >> K >> L;
	check(!inf.fail());

	int cost = 0;

	size_t G; // number of gates
	solf >> G;
	circuit c(K, G);
	check(!solf.fail());

	std::vector<size_t> gin;
	while (G-- > 0)
	{
		std::string gt;
		std::size_t nin;
		solf >> gt >> nin;
		check(!solf.fail());
		cost += nin + 1;
		gin.resize(nin);
		for (size_t i = 0; i < nin; ++i)
		{
			solf >> gin[i];
			check(!solf.fail());
			--gin[i];
		}
		auto igate = gate_types.find(gt);
		check(igate != gate_types.end());
		c.add_gate(igate->second, gin);
	}

	std::vector<size_t> out(L);
	for (size_t l = 0; l < L; ++l)
	{
		solf >> out[l];
		check(!solf.fail());
		--out[l];
	}
	solf >> std::ws;
	check(!solf.fail() && solf.eof());

	size_t N;
	inf >> N;
	check(!inf.fail());
	std::vector<bool> in(K);
	bool ok = true;
	for (size_t n = 0; n < N; ++n)
	{
		std::string ins, outs;
		inf >> ins >> outs;
		check(!inf.fail());
		check(ins.size() == K && outs.size() == L);
		for (size_t k = 0; k < K; ++k)
		{
			in[k] = ins[k] == '1';
		}
		c.calculate(in);
		for (size_t l = 0; l < L; ++l)
		{
			if (c[out[l]] != (outs[l] == '1'))
			{
				std::cerr << "Bit #" << l << " of sample " << n << " does not match!" << std::endl;
				ok = false;
			}
		}
	}

	std::cout << (ok ? cost : -1) << std::endl;
	return 0;
}
