#include <algorithm>
#include <array>
#include <cassert>
#include <cstdlib>
#include <functional>
#include <iostream>
#include <random>
#include <set>
#include <vector>

class gate_type
{
  public:
	std::string name;
	std::function<bool(bool, bool)> fn;
	int cost;

	bool operator()(bool a, bool b) const { return fn(a, b); }
};

std::array<gate_type, 4> const gate_types{
	gate_type{"NOT", [](bool v, bool) { return !v; }, 2},
	gate_type{"OR", [](bool a, bool b) { return a || b; }, 3},
	gate_type{"AND", [](bool a, bool b) { return a && b; }, 3},
	gate_type{"XOR", [](bool a, bool b) { return a ^ b; }, 3}};

class circuit
{
  public:
	circuit(size_t K) : K(K), Kfree(K), nodes(K) {}

	template <class Generator>
	void add_gate(size_t gt, Generator &gen);
	bool calculate(std::vector<bool> const &in);
	void print(size_t G0) const;
	size_t gate_count() const { return gates.size(); }

  protected:
	struct node
	{
		bool value = false;
		bool used = false;
	};

	struct gate
	{
		size_t gt = 0, a = 0, b = 0, out = 0;
	};

	size_t const K;
	size_t Kfree;
	std::vector<node> nodes;
	std::vector<gate> gates;

	template <class Generator>
	size_t use_random_free_node(Generator &gen);
};

template <class Generator>
void circuit::add_gate(size_t gt, Generator &gen)
{
	gate g{gt};
	g.a = use_random_free_node(gen);
	if (gate_types[gt].cost == 3)
	{
		g.b = Kfree > 0 ? use_random_free_node(gen) : std::uniform_int_distribution<>(0, nodes.size() - 1)(gen);
	}
	g.out = nodes.size();
	nodes.push_back(node());
	gates.push_back(g);
	++Kfree;
}

bool circuit::calculate(std::vector<bool> const &in)
{
	assert(in.size() == K);
	for (size_t i = 0; i < K; ++i)
	{
		nodes[i].value = in[i];
	}
	for (gate const &g : gates)
	{
		nodes[g.out].value = gate_types[g.gt](nodes[g.a].value, nodes[g.b].value);
	}
	return nodes.back().value;
}

void circuit::print(size_t G0) const
{
	auto global = [this, G0](size_t i) { return i < K ? i + 1 : i + G0 + 1; };

	for (gate const &g : gates)
	{
		size_t const cost = gate_types[g.gt].cost;
		std::cerr << gate_types[g.gt].name << ' ' << cost - 1 << ' ' << global(g.a);
		if (cost == 3)
		{
			std::cerr << ' ' << global(g.b);
		}
		std::cerr << std::endl;
	}
}

template <class Generator>
size_t circuit::use_random_free_node(Generator &gen)
{
	assert(Kfree > 0);
	int n = std::uniform_int_distribution<>(0, Kfree - 1)(gen);
	size_t i;
	for (i = 0; i < nodes.size() - 1; ++i)
	{
		if (!nodes[i].used && n-- == 0)
			break;
	}

	assert(!nodes[i].used);
	nodes[i].used = true;
	--Kfree;

	return i;
}

void usage(const char *name)
{
	std::cout << "Usage: " << name << " <number_of_inputs> <number_of_outputs> <gate_cost> <number_of_samples> [seed]" << std::endl;
	exit(1);
}

int main(int argc, const char *argv[])
{
	if (argc < 5)
	{
		usage(argv[0]);
	}

	size_t const K = std::atoi(argv[1]); // number of imputs
	size_t const L = std::atoi(argv[2]); // number of outputs
	int T = std::atoi(argv[3]);			 // target cost
	int N = std::atoi(argv[4]);			 // number of samples

	assert(2 <= K && K <= 8);
	assert(1 <= L && L <= 32);
	assert(1 <= N && N <= 1 << K);
	assert(2 <= T && T <= 10000);

	auto seed = argc > 5 ? std::atoll(argv[5]) : std::random_device()();
	//std::cerr << "random seed: " << seed << std::endl;
	std::mt19937 gen(seed);

	std::vector<circuit> circuits(L, circuit(K));

	std::uniform_int_distribution<> rnd_l(0, L - 1);
	std::uniform_int_distribution<> rnd_gt(0, gate_types.size() - 1);
	std::uniform_int_distribution<> rnd_gt2(1, gate_types.size() - 1);
	while (T > 0)
	{
		int const l = rnd_l(gen);
		size_t const gt = T == 3 ? rnd_gt2(gen) : T < 5 ? 0 : rnd_gt(gen);
		circuits[l].add_gate(gt, gen);
		T -= gate_types[gt].cost;
	}
	assert(T == 0);

	std::cout << K << ' ' << L << std::endl;
	std::cout << N << std::endl;

	std::vector<bool> in(K);
	std::uniform_int_distribution<int64_t> rnd_sample(0, (1ll << static_cast<int64_t>(K)) - 1);
	std::set<uint32_t> ss;
	while (N-- > 0)
	{
		uint32_t s;
		do
		{
			s = rnd_sample(gen);
		} while (ss.count(s));
		ss.insert(s);

		for (size_t k = 0; k < K; ++k)
		{
			std::cout << (in[k] = (s >> k) & 1);
		}
		std::cout << ' ';
		for (auto &c : circuits)
		{
			std::cout << c.calculate(in);
		}
		std::cout << std::endl;
	}

	size_t G = 0;
	for (auto &c : circuits)
	{
		G += c.gate_count();
	}
	std::cerr << G << std::endl;
	G = 0;
	for (auto &c : circuits)
	{
		c.print(G);
		G += c.gate_count();
	}
	G = K;
	for (auto &c : circuits)
	{
		G += c.gate_count();
		std::cerr << (c.gate_count() ? G : K) << std::endl;
	}

	return 0;
}
