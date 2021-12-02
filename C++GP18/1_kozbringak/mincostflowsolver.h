#pragma once

#include <utility>
#include <boost/graph/adjacency_list.hpp>
#include "solver.h"

class MinCostFlowSolver : public Solver
{
  public:
	MinCostFlowSolver(matrix M) : Solver(M) {}
	void solve() override;

  private:
	struct VertexProp
	{
		int row = -1;
		int col = -1;
	};
	struct EdgeProp
	{
		int capacity = 0;
		int weight = 0;
		int residual_capacity = 0;
	};
	using Graph = boost::adjacency_list<boost::vecS, boost::vecS, boost::directedS, VertexProp, EdgeProp>;
	using Vertex = boost::graph_traits<Graph>::vertex_descriptor;
	using Edge = boost::graph_traits<Graph>::edge_descriptor;

	void constructGraph();
	void decodeSolution();
	void addEdge(Vertex u, Vertex v, int capacity, int weight = 0);

	Graph graph;
	Vertex main_source;
	Vertex main_sink;
	std::set<Vertex> sources;
	std::set<Vertex> sinks;
	std::map<Edge, Edge> reverseEdgeMap;
};
