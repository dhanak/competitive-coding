defmodule Mix.Tasks.Day01 do
  @shortdoc "AoC 2018 Day 1"

  @test "+1, -2, +3, +1"

  import ExUnit.Assertions
  use Mix.Task

  def q1(values) do
    Enum.sum(values)
  end

  def q2(values) do
    values
    |> Stream.cycle()
    |> Stream.transform({MapSet.new(), 0}, fn v, {set, sum} ->
      if MapSet.member?(set, sum) do
        {:halt, {set, sum}}
      else
        {[sum + v], {MapSet.put(set, sum), sum + v}}
      end
    end)
    |> Enum.take(-1)
    |> List.first()
  end

  defp check() do
    values =
      @test
      |> String.split(", ")
      |> Enum.map(&String.to_integer/1)

    assert q1(values) == 3
    assert q2(values) == 2
  end

  def run(_) do
    check()

    day =
      __MODULE__
      |> Atom.to_string()
      |> String.split(".")
      |> List.last()
      |> String.downcase()

    lines =
      File.stream!("input/#{day}.txt", :line)
      |> Stream.map(&String.trim_trailing(&1, "\n"))
      |> Enum.map(&String.to_integer/1)

    {time, _} =
      :timer.tc(fn ->
        IO.puts("Q1: #{q1(lines)}")
        IO.puts("Q2: #{q2(lines)}")
      end)

    IO.puts("  #{time / 1_000_000} seconds")
  end
end
