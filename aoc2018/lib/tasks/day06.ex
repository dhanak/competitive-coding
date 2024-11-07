defmodule Mix.Tasks.Day06 do
  @shortdoc "AoC 2018 Day 6"

  @test """
        1, 1
        1, 6
        8, 3
        3, 4
        5, 5
        8, 9
        """
        |> String.split("\n", trim: true)

  import Aoc2018
  import ExUnit.Assertions
  use Mix.Task

  def parse(lines) do
    coords = Enum.map(lines, &parse_coordinate/1)
    {l, r} = coords |> Enum.map(&elem(&1, 0)) |> Enum.min_max()
    {t, b} = coords |> Enum.map(&elem(&1, 1)) |> Enum.min_max()
    {coords, {l, t, r, b}}
  end

  def parse_coordinate(line) do
    [x, y] =
      line
      |> String.split(", ")
      |> Enum.map(fn s ->
        {i, ""} = Integer.parse(s)
        i
      end)

    {x, y}
  end

  def q1({coords, {l, t, r, b}}) do
    grid =
      for x <- l..r, y <- t..b do
        {cs, _} =
          Enum.reduce(coords, {[], Inf}, fn coord, {min_cs, min_d} ->
            d = manhattan_distance({x, y}, coord)

            cond do
              d < min_d -> {[coord], d}
              d == min_d -> {[coord | min_cs], d}
              d > min_d -> {min_cs, min_d}
            end
          end)

        {{x, y}, cs}
      end
      |> Map.new()

    edge =
      (for(x <- l..r, do: [{x, t}, {x, b}]) ++
         for(y <- t..b, do: [{l, y}, {r, y}]))
      |> Enum.concat()
      |> Enum.flat_map(fn c ->
        case grid[c] do
          [v] -> [v]
          _ -> []
        end
      end)
      |> MapSet.new()

    {_, s} =
      grid
      |> Map.values()
      |> Enum.flat_map(fn
        [c] -> if MapSet.member?(edge, c), do: [], else: [c]
        _ -> []
      end)
      |> Enum.frequencies()
      |> Enum.max_by(&elem(&1, 1))

    s
  end

  def q2(_) do
    0
  end

  defp check() do
    problem = parse(@test)
    assert q1(problem) == 17
    assert q2(problem) == 0
  end

  def run(_) do
    check()

    day =
      __MODULE__
      |> Atom.to_string()
      |> String.split(".")
      |> List.last()
      |> String.downcase()

    problem =
      File.stream!("input/#{day}.txt", :line)
      |> Stream.map(&String.trim_trailing(&1, "\n"))
      |> parse()

    {time, _} =
      :timer.tc(fn ->
        IO.puts("Q1: #{q1(problem)}")
        IO.puts("Q2: #{q2(problem)}")
      end)

    IO.puts("  #{time / 1_000_000} seconds")
  end
end
