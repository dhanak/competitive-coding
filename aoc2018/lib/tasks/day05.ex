defmodule Mix.Tasks.Day05 do
  @shortdoc "AoC 2018 Day 5"

  @test "dabAcCaCBAcCcaDA" |> String.to_charlist()

  import Aoc2018
  import ExUnit.Assertions
  use Mix.Task

  def q1(polymer) do
    polymer |> reduce() |> length()
  end

  def reduce(polymer), do: reduce(polymer, [])

  def reduce([a, b | polymer], reduced) do
    react? = a != b and downcase(a) == downcase(b)

    case {react?, reduced} do
      {true, [z | reduced]} -> reduce([z | polymer], reduced)
      {true, []} -> reduce(polymer, [])
      {false, _} -> reduce([b | polymer], [a | reduced])
    end
  end

  def reduce([a], reduced), do: Enum.reverse([a | reduced])

  def q2(polymer) do
    Enum.map(?a..?z, fn unit ->
      polymer
      |> Enum.reject(&(downcase(&1) == unit))
      |> q1()
    end)
    |> Enum.min()
  end

  defp check() do
    assert q1(@test) == 10
    assert q2(@test) == 4
  end

  def run(_) do
    check()

    day =
      __MODULE__
      |> Atom.to_string()
      |> String.split(".")
      |> List.last()
      |> String.downcase()

    [line] =
      File.stream!("input/#{day}.txt", :line)
      |> Stream.map(&String.trim_trailing(&1, "\n"))
      |> Enum.map(&String.to_charlist/1)

    {time, _} =
      :timer.tc(fn ->
        IO.puts("Q1: #{q1(line)}")
        IO.puts("Q2: #{q2(line)}")
      end)

    IO.puts("  #{time / 1_000_000} seconds")
  end
end
