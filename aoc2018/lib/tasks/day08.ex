defmodule Mix.Tasks.Day08 do
  @shortdoc "AoC 2018 Day 8"

  @test "2 3 0 3 10 11 12 1 1 0 1 99 2 1 1 2"

  import ExUnit.Assertions
  import Aoc2018
  use Mix.Task

  def parse(line) do
    values =
      line
      |> String.split(" ")
      |> Enum.map(&String.to_integer/1)

    {tree, []} = parse_node(values)
    tree
  end

  def parse_node([n_children, n_metadata | values]) do
    {children, values} =
      repeat({[], values}, n_children, fn {children, values} ->
        {child, values} = parse_node(values)
        {[child | children], values}
      end)

    {metadata, values} = Enum.split(values, n_metadata)
    {%{children: Enum.reverse(children), metadata: metadata}, values}
  end

  def q1(%{children: children, metadata: metadata}) do
    Enum.reduce(children, Enum.sum(metadata), fn child, sum ->
      sum + q1(child)
    end)
  end

  def q2(%{metadata: metadata, children: children}) do
    case children do
      [] ->
        metadata

      children ->
        metadata
        |> Enum.map(fn i ->
          case Enum.at(children, i - 1, :skip) do
            :skip -> 0
            child -> q2(child)
          end
        end)
    end
    |> Enum.sum()
  end

  defp check() do
    tree = parse(@test)
    assert q1(tree) == 138
    assert q2(tree) == 66
  end

  def run(_) do
    check()

    day =
      __MODULE__
      |> Atom.to_string()
      |> String.split(".")
      |> List.last()
      |> String.downcase()

    tree =
      File.stream!("input/#{day}.txt", :line)
      |> Enum.map(&String.trim_trailing(&1, "\n"))
      |> hd()
      |> parse()

    {time, _} =
      :timer.tc(fn ->
        IO.puts("Q1: #{q1(tree)}")
        IO.puts("Q2: #{q2(tree)}")
      end)

    IO.puts("  #{time / 1_000_000} seconds")
  end
end
