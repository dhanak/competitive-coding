defmodule Mix.Tasks.Day07 do
  @shortdoc "AoC 2018 Day 7"

  @test """
        Step C must be finished before step A can begin.
        Step C must be finished before step F can begin.
        Step A must be finished before step B can begin.
        Step A must be finished before step D can begin.
        Step B must be finished before step E can begin.
        Step D must be finished before step E can begin.
        Step F must be finished before step E can begin.
        """
        |> String.split("\n", trim: true)

  import ExUnit.Assertions
  use Mix.Task

  def parse(lines) do
    graph =
      lines
      |> Enum.map(&parse_line/1)
      |> then(&Graph.add_edges(Graph.new(), &1))

    assert Graph.is_acyclic?(graph)
    graph
  end

  def parse_line(line) do
    [_, a, b] = Regex.run(~r/^Step (.).*before step (.)/, line)
    {String.to_atom(a), String.to_atom(b)}
  end

  def q1(graph) do
    graph
    |> roots()
    |> Enum.sort()
    |> traverse(graph, [])
    |> Enum.join()
  end

  def q2(graph, n_workers, base_cost) do
    ready = Enum.sort(roots(graph))
    workers = %{idle: n_workers, busy: []}
    schedule(graph, ready, workers, base_cost, 0)
  end

  def roots(graph), do: roots(graph, Graph.vertices(graph))

  def roots(graph, candidates) do
    Enum.filter(candidates, fn node ->
      Graph.in_degree(graph, node) == 0
    end)
  end

  def delete_vertices(graph, vertices) do
    reduced_graph = Graph.delete_vertices(graph, vertices)

    root_candidates =
      vertices
      |> Enum.flat_map(&Graph.out_neighbors(graph, &1))
      |> Enum.uniq()

    {reduced_graph, roots(reduced_graph, root_candidates)}
  end

  def traverse([], _graph, traversed), do: Enum.reverse(traversed)

  def traverse([node | rest], graph, traversed) do
    {graph, new} = delete_vertices(graph, [node])
    traverse(Enum.sort(rest ++ new), graph, [node | traversed])
  end

  def schedule(graph, ready, workers, base_cost, time) do
    {complete, workers} = stop_jobs(workers, time)
    {graph, new} = delete_vertices(graph, complete)
    {start, ready} = Enum.split(Enum.sort(ready ++ new), workers.idle)
    workers = start_jobs(workers, start, time + base_cost)

    case workers.busy do
      [] ->
        time

      _ ->
        time = workers.busy |> Enum.map(& &1.finish) |> Enum.min()
        schedule(graph, ready, workers, base_cost, time)
    end
  end

  def stop_jobs(workers, now) do
    {stop, busy} = Enum.split_with(workers.busy, &(&1.finish <= now))
    complete = Enum.map(stop, & &1.job)
    {complete, %{idle: workers.idle + length(stop), busy: busy}}
  end

  def start_jobs(workers, jobs, offset) do
    work =
      Enum.map(jobs, fn job ->
        finish = offset + hd(Atom.to_charlist(job)) - 64
        %{job: job, finish: finish}
      end)

    %{idle: workers.idle - length(work), busy: workers.busy ++ work}
  end

  defp check() do
    graph = parse(@test)
    assert q1(graph) == "CABDFE"
    assert q2(graph, 2, 0) == 15
  end

  def run(_) do
    check()

    graph =
      Aoc2018.input(__MODULE__)
      |> parse()

    {time, _} =
      :timer.tc(fn ->
        IO.puts("Q1: #{q1(graph)}")
        IO.puts("Q2: #{q2(graph, 5, 60)}")
      end)

    IO.puts("  #{time / 1_000_000} seconds")
  end
end
