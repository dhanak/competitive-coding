#include <cassert>
#include <cmath>
#include <boost/graph/successive_shortest_path_nonnegative_weights.hpp>
#include <boost/property_map/property_map.hpp>

#include "mincostflowsolver.h"

void MinCostFlowSolver::solve()
{
	constructGraph();
	boost::successive_shortest_path_nonnegative_weights(
		graph, main_source, main_sink,
		boost::capacity_map(boost::get(&EdgeProp::capacity, graph))
			.residual_capacity_map(boost::get(&EdgeProp::residual_capacity, graph))
			.weight_map(boost::get(&EdgeProp::weight, graph))
			.reverse_edge_map(boost::make_assoc_property_map(reverseEdgeMap)));
	decodeSolution();
}

void MinCostFlowSolver::constructGraph()
{
	for (int r = 0; r < (int)R; ++r)
	{
		for (int c = 0; c < (int)C; ++c)
		{
			int const v = M[r][c];
			if (v == 0)
				continue;

			Vertex vx = boost::add_vertex(VertexProp{r, c}, graph);
			(v > 0 ? sources : sinks).insert(vx);
		}
	}

	for (auto u : sources)
	{
		auto const &up = graph[u];
		for (auto v : sinks)
		{
			auto const &vp = graph[v];
			int const capacity = std::min(M[up.row][up.col], -M[vp.row][vp.col]);
			int const weight = std::abs(up.row - vp.row) + std::abs(up.col - vp.col);
			addEdge(u, v, capacity, weight);
		}
	}

	main_source = boost::add_vertex(graph);
	for (auto u : sources)
	{
		auto const &up = graph[u];
		addEdge(main_source, u, M[up.row][up.col]);
	}

	main_sink = boost::add_vertex(graph);
	for (auto v : sinks)
	{
		auto const &vp = graph[v];
		addEdge(v, main_sink, -M[vp.row][vp.col]);
	}
}

void MinCostFlowSolver::decodeSolution()
{
	for (auto u : sources)
	{
		auto edges = boost::out_edges(u, graph);
		for (auto it = edges.first; it != edges.second; ++it)
		{
			auto v = boost::target(*it, graph);
			if (v == main_source)
				continue;
			auto const &ep = graph[*it];
			auto const &up = graph[u];
			auto const &vp = graph[v];
			move(up.row, up.col, vp.row, vp.col, ep.capacity - ep.residual_capacity);
		}
	}
}

void MinCostFlowSolver::addEdge(Vertex u, Vertex v, int capacity, int weight /* = 0 */)
{
	assert(capacity > 0 && weight >= 0);
	Edge e1 = boost::add_edge(u, v, EdgeProp{capacity, weight}, graph).first;
	Edge e2 = boost::add_edge(v, u, EdgeProp{0, -weight}, graph).first;
	reverseEdgeMap[e1] = e2;
	reverseEdgeMap[e2] = e1;
}
