defmodule Mix.Tasks.Day12 do
  @shortdoc "AoC 2018 Day 12"

  @test """
        initial state: #..#.#..##......###...###

        ...## => #
        ..#.. => #
        .#... => #
        .#.#. => #
        .#.## => #
        .##.. => #
        .#### => #
        #.#.# => #
        #.### => #
        ##.#. => #
        ##.## => #
        ###.. => #
        ###.# => #
        ####. => #
        """
        |> String.trim_trailing("\n")
        |> String.split("\n")

  import ExUnit.Assertions
  use Mix.Task

  def parse(["initial state: " <> init, "" | rules]) do
    rules =
      rules
      |> Enum.map(&List.to_tuple(String.split(&1, " => ")))
      |> Map.new()

    {init, rules}
  end

  def q1({init, rules}, generations) do
    {0, init} |> simulate(generations, rules) |> score()
  end

  def simulate(init, 0, _rules), do: init

  def simulate({n, state}, count, rules) do
    case step({n, state}, rules) do
      {m, ^state} -> {n + (m - n) * count, state}
      next -> simulate(next, count - 1, rules)
    end
  end

  def step({n, state}, rules) do
    state = grow("...." <> state <> "....", "", rules)
    trim(state, ".", n - 2)
  end

  def grow(<<a, b, c, d, e, rest::binary>>, acc, rules) do
    acc = Map.get(rules, <<a, b, c, d, e>>, ".") <> acc
    grow(<<b, c, d, e, rest::binary>>, acc, rules)
  end

  def grow("....", acc, _rules) do
    String.reverse(acc)
  end

  def trim(string, to_trim, offset \\ 0) do
    l_trimmed = String.trim_leading(string, to_trim)
    lt_trimmed = String.trim_trailing(l_trimmed, to_trim)
    {offset + String.length(string) - String.length(l_trimmed), lt_trimmed}
  end

  def score({n, state}) do
    Enum.reduce(Enum.with_index(String.to_charlist(state), n), 0, fn
      {?#, i}, total -> total + i
      _, total -> total
    end)
  end

  defp check() do
    input = parse(@test)
    assert q1(input, 20) == 325
  end

  def run(_) do
    check()

    input = Aoc2018.input(__MODULE__) |> Enum.to_list() |> parse()

    {time, _} =
      :timer.tc(fn ->
        IO.puts("Q1: #{q1(input, 20)}")
        IO.puts("Q2: #{q1(input, 50_000_000_000)}")
      end)

    IO.puts("  #{time / 1_000_000} seconds")
  end
end
