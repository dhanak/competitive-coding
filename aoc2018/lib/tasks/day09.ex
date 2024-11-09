defmodule Mix.Tasks.Day09 do
  @shortdoc "AoC 2018 Day 9"

  import ExUnit.Assertions
  import Aoc2018
  use Mix.Task

  def q1(n_elves, last_marble) do
    elves = Map.new(for i <- 1..n_elves, do: {i, 0})
    marbles = Enum.to_list(1..last_marble)
    play(marbles, 1, elves, {[], [0]})
  end

  def play([], _elf, elves, _ring) do
    elves |> Map.values() |> Enum.max()
  end

  def play([marble | marbles], elf, elves, ring) do
    {score, ring} = move(marble, ring)
    elves = if score > 0, do: Map.update!(elves, elf, &(&1 + score)), else: elves
    elf = rem(elf, map_size(elves)) + 1
    play(marbles, elf, elves, ring)
  end

  def move(marble, ring) when rem(marble, 23) == 0 do
    {circle_cw, [removed | circle_ccw]} = step(ring, -7)
    {marble + removed, step({circle_cw, circle_ccw})}
  end

  def move(marble, ring) do
    {circle_cw, circle_ccw} = step(ring)
    {0, {circle_cw, [marble | circle_ccw]}}
  end

  def step(ring, n) when n > 0 do
    repeat(ring, n, &step/1)
  end

  def step({fwd, rev}, n) when n < 0 do
    {rev, fwd} = repeat({rev, fwd}, -n, &step/1)
    {fwd, rev}
  end

  def step({[], []}), do: {[], []}
  def step({[a | fwd], rev}), do: {fwd, [a | rev]}
  def step({[], rev}), do: step({Enum.reverse(rev), []})

  defp check() do
    assert q1(9, 25) == 32
    assert q1(10, 1618) == 8317
    assert q1(13, 7999) == 146_373
    assert q1(17, 1104) == 2764
    assert q1(21, 6111) == 54718
    assert q1(30, 5807) == 37305
  end

  def run(_) do
    check()

    {time, _} =
      :timer.tc(fn ->
        IO.puts("Q1: #{q1(478, 71240)}")
        IO.puts("Q2: #{q1(478, 7_124_000)}")
      end)

    IO.puts("  #{time / 1_000_000} seconds")
  end
end
