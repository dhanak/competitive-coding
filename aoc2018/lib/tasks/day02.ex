defmodule Mix.Tasks.Day02 do
  @shortdoc "AoC 2018 Day 2"

  @test1 """
         abcdef
         bababc
         abbcde
         abcccd
         aabcdd
         abcdee
         ababab
         """
         |> String.split("\n", trim: true)

  @test2 """
         abcde
         fghij
         klmno
         pqrst
         fguij
         axcye
         wvxyz
         """
         |> String.split("\n", trim: true)

  import ExUnit.Assertions
  use Mix.Task

  def q1(boxes) do
    boxes
    |> Enum.reduce({0, 0}, fn box, {double, triple} ->
      freqs =
        box
        |> String.to_charlist()
        |> Enum.frequencies()
        |> Map.values()

      double = double + if Enum.any?(freqs, &(&1 == 2)), do: 1, else: 0
      triple = triple + if Enum.any?(freqs, &(&1 == 3)), do: 1, else: 0
      {double, triple}
    end)
    |> Tuple.product()
  end

  def q2(boxes) do
    boxes = Enum.map(boxes, &String.to_charlist/1)
    pairs = for x <- boxes, y <- boxes, x !== y, do: {x, y}

    {x, y} =
      Enum.find(pairs, fn {x, y} ->
        Enum.count(Enum.zip(x, y), fn {a, b} -> a != b end) == 1
      end)

    common = for {a, b} <- Enum.zip(x, y), a == b, do: a
    List.to_string(common)
  end

  defp check() do
    assert q1(@test1) == 12
    assert q2(@test2) == "fgij"
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
