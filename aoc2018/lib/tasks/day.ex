defmodule Mix.Tasks.Day do
  @shortdoc "AoC 2018 Day"

  @test """
        """
        |> String.split("\n", trim: true)

  import ExUnit.Assertions
  use Mix.Task

  def q1(_) do
    0
  end

  def q2(_) do
    0
  end

  defp check() do
    assert q1(@test) == 0
    assert q2(@test) == 0
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

    {time, _} =
      :timer.tc(fn ->
        IO.puts("Q1: #{q1(lines)}")
        IO.puts("Q2: #{q2(lines)}")
      end)

    IO.puts("  #{time / 1_000_000} seconds")
  end
end
