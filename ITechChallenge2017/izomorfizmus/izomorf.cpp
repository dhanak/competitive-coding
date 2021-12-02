#include <algorithm>
#include <array>
#include <cmath>
#include <functional>
#include <initializer_list>
#include <iostream>
#include <unordered_set>
#include <vector>

//#define DEBUG
#ifdef DEBUG
#define check(cond) ((cond) ? (cerr<<(#cond)<<'\n',true) : false)
#define check2(cond,prn) ((cond) ? (cerr<<(#cond)<<"\n\t"<<prn<<'\n',true) : false)
#else
#define check(cond) (cond)
#define check2(cond,prn) (cond)
#endif

using namespace std;

class point
{
public:
	point() : D{{}}, V{{}} {}
	point(int64_t x, int64_t y, int64_t z) : D{{}}, V{{x,y,z}} {}
	uint64_t length_sq() const { return V[0]*V[0] + V[1]*V[1] + V[2]*V[2]; }
	int64_t &operator[](size_t i) { return V[i]; }
	int64_t operator[](size_t i) const { return V[i]; }
	point &operator*=(int64_t a) { for (auto &v : V) v *= a; return *this; }
	point &operator/=(int64_t a) { for (auto &v : V) v /= a; return *this; }
	point &operator%=(int64_t a) { for (auto &v : V) v %= a; return *this; }
	point &operator+=(point const &p) { for (size_t i = V.size(); i--; ) V[i] += p[i]; return *this; }
	point &operator-=(point const &p) { for (size_t i = V.size(); i--; ) V[i] -= p[i]; return *this; }
	point operator*(int64_t v) const { return point(*this) *= v; }
	point operator/(int64_t v) const { return point(*this) /= v; }
	point operator%(int64_t v) const { return point(*this) %= v; }
	point operator+(point const &o) const { return point(*this) += o; }
	point operator-(point const &o) const { return point(*this) -= o; }
	bool operator==(point const &p) const { return V == p.V && D == p.D; }
	bool operator!=(point const &p) const { return V != p.V || D != p.D; }
	bool operator<(point const &p) const { return V < p.V || (V == p.V && D < p.D); }

	array<uint,3> D;
protected:
	array<int64_t,3> V;
};

class matrix
{
public:
	matrix() : M{{{}}} {}
	matrix(initializer_list<initializer_list<int64_t>> init)
	{
		size_t r = 0;
		for (auto vs : init)
		{
			size_t c = 0;
			for (auto v : vs)
				M[r][c++] = v;
			r++;
		}
	}
	array<int64_t,3> const &operator[](size_t r) const { return M[r]; }
	array<int64_t,3> &operator[](size_t r) { return M[r]; }
	matrix operator*(matrix const &o) const
	{
		matrix res;
		for (int r = 0; r < 3; r++)
			for (int c = 0; c < 3; c++)
				for (int i = 0; i < 3; i++)
					res[r][c] += M[r][i] * o[i][c];
		return res;
	}
	point operator*(point const &p) const
	{
		point res;
		for (int i = 0; i < 3; i++)
			for (int j = 0; j < 3; j++)
				res[i] += M[i][j] * p[j];
		res.D = p.D;
		return res;
	}
	bool operator==(matrix const &o) const { return M == o.M; }

	static matrix const unit, rot_x, rot_y, rot_z;
protected:
	array<array<int64_t,3>,3> M;
};
matrix const matrix::unit{{1,0,0},{0,1,0},{0,0,1}};
matrix const matrix::rot_x{{1,0,0},{0,0,-1},{0,1,0}};
matrix const matrix::rot_y{{0,0,1},{0,1,0},{-1,0,0}};
matrix const matrix::rot_z{{0,-1,0},{1,0,0},{0,0,1}};

class bounding_box
{
public:
	point bl, tr;
	void include(point const &p)
	{
		for (uint i = 0; i < 3; ++i)
		{
			bl[i] = min(bl[i], p[i]);
			tr[i] = max(tr[i], p[i]);
		}
	}
	bounding_box rotate(matrix const &m)
	{
		bounding_box bb;
		bb.include(m*bl);
		bb.include(m*tr);
		return bb;
	}
	bool operator==(bounding_box const &o) const { return bl == o.bl && tr == o.tr; }
	bool operator!=(bounding_box const &o) const { return !(*this == o); }
};

ostream &operator<<(ostream &s, point const &p)
{
	return s << '(' << p[0] << ',' << p[1] << ',' << p[2] << ')';
}

ostream &operator<<(ostream &s, matrix const &m)
{
	s << '[';
	for (size_t r = 0; r < 3; r++)
		for (size_t c = 0; c < 3; c++)
			s << m[r][c] << (c < 2 ? ',' : r < 2 ? ';' : ']');
	return s;
}

ostream &operator<<(ostream &s, bounding_box const &bb)
{
	return s << '<' << bb.bl << ';' << bb.tr << '>';
}

class building
{
	friend ostream &operator<<(ostream &s, building const &b);
	
private:
	typedef pair<uint,uint> point_range; // index range in points
	typedef array<uint,4> edge; // pair of indices in points + side degree + hole degree
	typedef pair<uint,uint> edge_range; // index range in edge_refs
	typedef pair<uint,uint> hole_range; // index range in holes
	
	struct side
	{
		edge_range edges;
		hole_range holes;
		side(uint eb, uint ee, uint hb, uint he) : edges(eb,ee), holes(hb,he) {}
	};

	point center;
	bounding_box bb;

	vector<point> points;
	vector<edge> edges;
	vector<side> sides;

	vector<uint> edge_refs;	// indices in edges, indexed by sides and holes (below)
	vector<edge_range> holes; // index ranges in edge_refs, indexed by holes

	vector<uint> sorted_points;	// indices in points, geometrically sorted
	vector<uint> sorted_points_rev; // reverse mapping of sorted_points
	vector<uint> sorted_edges; // indices in edges, geometrically sorted
	vector<uint> sorted_edges_rev; // reverse mapping of sorted_edges

	class canonical_edge_iterator
	{
	public:
		canonical_edge_iterator(building const &_b, uint _i) : b(_b), i(_i) {}
		canonical_edge_iterator& operator++() { ++i; return *this; }
		uint operator*() const { return b.sorted_edges_rev[b.edge_refs[i]]; }
		bool operator==(canonical_edge_iterator const &o) const { return i == o.i; }
		bool operator!=(canonical_edge_iterator const &o) const { return i != o.i; }
	private:
		building const &b;
		uint i;
	};

	bool edge_ranges_equal(edge_range const &er, building const &b, edge_range const &ber)
	{
		return er.second - er.first == ber.second - ber.first &&
			equal(
				canonical_edge_iterator(*this, er.first),
				canonical_edge_iterator(*this, er.second),
				canonical_edge_iterator(b, ber.first));
	}

	bool compare_edge_ranges(edge_range const &a, edge_range const &b)
	{
		return lexicographical_compare(
			edge_refs.begin()+a.first, edge_refs.begin()+a.second,
			edge_refs.begin()+b.first, edge_refs.begin()+b.second,
			[this](uint a, uint b){ return sorted_edges_rev[a] < sorted_edges_rev[b]; });
	}

	void sort_edge_range(edge_range const &a)
	{
		sort(edge_refs.begin()+a.first, edge_refs.begin()+a.second,
			 [this](uint a, uint b){ return sorted_edges_rev[a] < sorted_edges_rev[b]; });
	}

	edge canonical_edge(edge e) const
	{
		e[0] = sorted_points_rev[e[0]];
		e[1] = sorted_points_rev[e[1]];
		if (e[0] > e[1]) swap(e[0], e[1]);
		return e;
	}

	void update_reverse_map(vector<uint> const &m, vector<uint> &rm)
	{
		rm.resize(m.size());
		for (size_t i = m.size(); i--; )
			rm[m[i]] = i;
	}
	
	void sort_points(bool reverse = true)
	{   // sort points in geometrical order
		sort(sorted_points.begin(), sorted_points.end(), [this](uint a, uint b){
				return points[a] < points[b];
			});
		if (reverse)
			update_reverse_map(sorted_points, sorted_points_rev);
	}

	void sort_edges(bool reverse = true)
	{   // sort edges according to sorted point order
		sort(sorted_edges.begin(), sorted_edges.end(), [this](uint a, uint b){
				return canonical_edge(edges[a]) < canonical_edge(edges[b]);
			});
		if (reverse)
			update_reverse_map(sorted_edges, sorted_edges_rev);
	}

	void sort_sides()
	{
		for (side &s : sides)
		{
			sort_edge_range(s.edges);
			for (uint h = s.holes.first; h < s.holes.second; ++h)
			{
				sort_edge_range(holes[h]);
			}
			sort(holes.begin()+s.holes.first, holes.begin()+s.holes.second,
				 [this](edge_range const &h1, edge_range const &h2){
					 return compare_edge_ranges(h1, h2);
				 });
		}
		sort(sides.begin(), sides.end(), [this](side const &a, side const &b){
				return compare_edge_ranges(a.edges, b.edges);
			});
	}

	struct reftype { enum T { point, side, hole }; };
	void add_edge_ref(uint ref, reftype::T type)
	{
		edge_refs.push_back(ref-1);
		edge &e = edges[ref-1];
		e[type+1]++;
		points[e[0]].D[type]++;
		points[e[1]].D[type]++;
	}

	bool rotated_identical(matrix const &rot, matrix &rot_diff, building const &other)
	{
		// quick center & bounding box check
		if (check2((rot*center - other.center) % points.size() != point(),
				   rot<<' '<<rot*center<<" vs "<<other.center))
			return false;

		bounding_box bbr = bb.rotate(rot_diff);
		if (check2(bbr != other.bb, rot<<' '<<bbr<<" vs "<<other.bb))
			return false;

		// apply rotation on BB and all points
		bb = bbr;
		for (auto &p : points)
		{
			p = rot_diff*p;
		}
		rot_diff = matrix::unit;
		
		return check_points_and_all(other);
	}

	template <class T, class Compare>
	void collect_equal_ranges(vector<uint> const &sorted, vector<T> &objects,
							  Compare comp, vector<pair<uint,uint>> &equals)
	{
		uint start, end;
		for (start = 0, end = 1; end < sorted.size(); ++end)
		{
			if (!comp(objects[sorted[start]], objects[sorted[end]]))
			{
				if (start+1 != end)
				{
					equals.emplace_back(start, end);
				}
				start = end;
			}
		}
		if (start+1 != end)
		{
			equals.emplace_back(start, end);
		}
	}

	template <class Iter1, class Iter2>
	bool test_permutations(Iter1 begin, Iter1 end, Iter2 sorted, building const &other,
						   function<bool(building&, building const&)> test)
	{
		auto it = begin;
		for (; it != end; it++)
		{
			sort(sorted+it->first, sorted+it->second);
		}
		while (1)
		{
			if (it == end)
			{
				if (test(*this, other))
					return true;
			}
			else if (next_permutation(sorted+it->first, sorted+it->second))
			{
				it = end;
				continue;
			}
			if (it == begin)
				return false;
			--it;
		}
	}

	bool check_points_and_all(building const &other)
	{
		sort_points(false);
		for (uint N = points.size(); N--; )
		{
			if (check2(points[sorted_points[N]] != other.points[other.sorted_points[N]],
					   /*rot<<' '<<*/sorted_points[N]<<' '<<other.sorted_points[N]))
			{
				//for (N = points.size(); N--; )
				//	cerr<<points[sorted_points[N]]<<other.points[other.sorted_points[N]]<<endl;
				return false;
			}
		}
		vector<point_range> equal_points;
		collect_equal_ranges(sorted_points, points, equal_to<point const &>(), equal_points);
		return test_permutations(equal_points.begin(), equal_points.end(),
								 sorted_points.begin(), other, &building::check_edges_and_all);
	}

	bool check_edges_and_all(building const &other)
	{
		// check all edges
		update_reverse_map(sorted_points, sorted_points_rev);
		sort_edges(false);
		for (uint K = edges.size(); K--; )
		{
			if (check2(canonical_edge(edges[sorted_edges[K]]) !=
					   other.canonical_edge(other.edges[other.sorted_edges[K]]),
					   /*rot<<' '<<*/sorted_edges[K]<<' '<<other.sorted_edges[K]))
			{
				// for (K = edges.size(); K--; )
				// {
				// 	edge const &e1 = canonical_edge(edges[sorted_edges[K]]);
				// 	edge const &e2 = other.canonical_edge(other.edges[other.sorted_edges[K]]);
				// 	cerr<<e1.first<<','<<e1.second<<' '<<e2.first<<','<<e2.second<<endl;
				// }
				return false;
			}
		}

		vector<edge_range> equal_edges;
		collect_equal_ranges(sorted_edges, edges, [this](edge const &a, edge const &b){
				return canonical_edge(a) == canonical_edge(b);
			}, equal_edges);
		return test_permutations(equal_edges.begin(), equal_edges.end(),
								 sorted_edges.begin(), other, &building::check_sides);
	}

	bool check_sides(building const &other)
	{
		// check all sides
		update_reverse_map(sorted_edges, sorted_edges_rev);
		sort_sides();
		for (uint P = sides.size(); P--; )
		{
			side const &sa = sides[P];
			side const &sb = other.sides[P];
			if (check(!edge_ranges_equal(sa.edges, other, sb.edges)) ||
				check(sa.holes.second - sa.holes.first != sb.holes.second - sb.holes.first))
				return false;
			for (uint h = sa.holes.second - sa.holes.first; h--; )
			{
				if (check(!edge_ranges_equal(
							  holes[sa.holes.first + h],
							  other, holes[sb.holes.first + h])))
					return false;
			}
		}
		
		return true;
	}

	void recenter()
	{
		uint const N = points.size();
		for (auto p : points)
		{
			center += p;
		}
		for (auto &p : points)
		{
			p = p*N - center;
			bb.include(p);
		}
	}

	uint64_t edge_length_sq(uint i) const
	{
		edge const& e = edges[i];
		return (points[e[0]] - points[e[1]]).length_sq();
	}

	bool edge_lengths_equal(building const &other)
	{
		unordered_multiset<uint64_t> lengths;
		lengths.reserve(edges.size());
		for (uint K = edges.size(); K--; )
		{
			//cerr << edge_length_sq(K) << ' ';
			lengths.insert(edge_length_sq(K));
		}
		//cerr << '\n';
		for (uint K = other.edges.size(); K--; )
		{
			//cerr << other.edge_length_sq(K) << ' ';
			auto it = lengths.find(other.edge_length_sq(K));
			if (it == lengths.end())
				return false;
			lengths.erase(it);
		}
		//cerr << '\n';
		return lengths.empty();
	}

public:
	void read()
	{
		uint N, K, P;
		cin >> N;
		points.reserve(N);
		sorted_points.reserve(N);
		while (N--)
		{
			uint64_t x, y, z;
			cin >> x >> y >> z;
			points.emplace_back(x, y, z);
			sorted_points.push_back(sorted_points.size());
		}
		cin >> K;
		edges.reserve(K);
		sorted_edges.reserve(K);
		while (K--)
		{
			uint a, b;
			cin >> a >> b;
			edges.push_back({a-1, b-1});
			points[a-1].D[reftype::point]++;
			points[b-1].D[reftype::point]++;
			sorted_edges.push_back(sorted_edges.size());
		}
		cin >> P;
		sides.reserve(P);
		while (P--)
		{
			uint E;
			cin >> E;
			uint const eb = edge_refs.size();
			while (E--)
			{
				uint e;
				cin >> e;
				add_edge_ref(e, reftype::side);
			}
			uint const ee = edge_refs.size();

			uint H;
			cin >> H;
			uint const hb = holes.size();
			while (H--)
			{
				uint M;
				cin >> M;
				uint const heb = edge_refs.size();
				while (M--)
				{
					uint e;
					cin >> e;
					add_edge_ref(e, reftype::hole);
				}
				uint const hee = edge_refs.size();
				holes.emplace_back(heb, hee);
			}
			uint const he = holes.size();
			sides.emplace_back(eb, ee, hb, he);
		}
		recenter();
	}

	void sort_all()
	{
		sort_points();
		sort_edges();
		sort_sides();
	}

	bool isomorphic(building const &other)
	{
		// trivial checks
		if (check(points.size() != other.points.size()) ||
			check(edges.size() != other.edges.size()) ||
			check(sides.size() != other.sides.size()) ||
			check(edge_refs.size() != other.edge_refs.size()) ||
			check(holes.size() != other.holes.size()) ||
			check(!edge_lengths_equal(other)))
			return false;

		// rotation checks
		vector<matrix> rotations;
		matrix rot(matrix::unit);
		matrix rot_diff(matrix::unit);
		for (int rx = 0; rx < 4; rx++)
		{
			for (int ry = 0; ry < 4; ry++)
			{
				for (int rz = 0; rz < 4; rz++)
				{
					if (find(rotations.begin(), rotations.end(), rot) == rotations.end())
					{
						rotations.push_back(rot);
						if (rotated_identical(rot, rot_diff, other))
						{
							//cerr << "Transform: " << rot << ' ' <<
							//	(other.center-rot*center)/points.size() << "\n\n";
							return true;
						}
					}
					rot = matrix::rot_z*rot;
					rot_diff = matrix::rot_z*rot_diff;
				}
				rot = matrix::rot_y*rot;
				rot_diff = matrix::rot_y*rot_diff;
			}
			rot = matrix::rot_x*rot;
			rot_diff = matrix::rot_x*rot_diff;
		}
		return false;
	}
};

namespace std
{
	template <>
	class iterator_traits<building::canonical_edge_iterator>
	{
	public:
		typedef uint value_type;
	};
}

ostream &operator<<(ostream &s, building const &b)
{
	s << "c:" << b.center << " bb:" << b.bb;
	return s;
}

int main()
{
	uint L;
	cin >> L;

	building B1;
	B1.read();
	B1.sort_all();
	
	vector<uint> iso;
	for (uint l = 1; l < L; ++l)
	{
		building B2;
		B2.read();
		if (B2.isomorphic(B1))
			iso.push_back(l+1);
	}
	
	for (uint i = 0; i < iso.size(); ++i)
		cout << (i?" ":"") << iso[i];
	cout << endl;
	
	return 0;
}
