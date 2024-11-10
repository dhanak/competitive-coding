defmodule Mix.Tasks.Day11 do
  @shortdoc "AoC 2018 Day 11"

  import ExUnit.Assertions
  use Mix.Task

  def q1(sn) do
    squares =
      for x <- 1..298, y <- 1..298 do
        {{x, y}, square_power(sn, {x, y}, 3)}
      end

    {{x, y}, _} = Enum.max_by(squares, &elem(&1, 1))
    {x, y}
  end

  def cell_power(sn, {x, y}) do
    rem(div(((x + 10) * y + sn) * (x + 10), 100), 10) - 5
  end

  def q2(sn) do
    powers =
      for x <- 1..300, y <- 1..300 do
        {{x, y, 1}, cell_power(sn, {x, y})}
      end
      |> Map.new()

    {_, best} =
      for s <- 2..300,
          x <- 1..(300 - s + 1),
          y <- 1..(300 - s + 1),
          reduce: {powers, %{power: 0}} do
        {powers, best} ->
          power = square_power(powers, {x, y}, s)
          powers = Map.put(powers, {x, y, s}, power)
          current = %{at: {x, y, s}, power: power}
          best = Enum.max_by([best, current], & &1.power)
          {powers, best}
      end

    best.at
  end

  def square_power(sn, {x, y}, size) when is_integer(sn) do
    Enum.sum(
      for sx <- x..(x + size - 1), sy <- y..(y + size - 1) do
        cell_power(sn, {sx, sy})
      end
    )
  end

  def square_power(powers, {x, y}, size) when is_map(powers) do
    tl_big = powers[{x, y, size - 1}]
    br_big = powers[{x + 1, y + 1, size - 1}]
    middle = Map.get(powers, {x + 1, y + 1, size - 2}, 0)
    bl_1x1 = powers[{x, y + size - 1, 1}]
    tr_1x1 = powers[{x + size - 1, y, 1}]
    tl_big + br_big - middle + bl_1x1 + tr_1x1
  end

  defp check() do
    assert cell_power(8, {3, 5}) == 4
    assert square_power(18, {33, 45}, 3) == 29
    assert q1(18) == {33, 45}
    assert q1(42) == {21, 61}
    assert square_power(18, {90, 269}, 16) == 113
    assert square_power(42, {232, 251}, 12) == 119
    assert q2(18) == {90, 269, 16}
    assert q2(42) == {232, 251, 12}
  end

  def run(_) do
    check()

    {time, _} =
      :timer.tc(fn ->
        IO.puts("Q1: #{inspect(q1(1718))}")
        IO.puts("Q2: #{inspect(q2(1718))}")
      end)

    IO.puts("  #{time / 1_000_000} seconds")
  end
end
