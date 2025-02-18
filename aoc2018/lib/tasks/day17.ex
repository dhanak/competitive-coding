defmodule Mix.Tasks.Day17 do
  @shortdoc "AoC 2018 Day 17"

  @test """
        x=495, y=2..7
        y=7, x=495..501
        x=501, y=3..7
        x=498, y=2..4
        x=506, y=1..2
        x=498, y=10..13
        x=504, y=10..13
        y=13, x=498..504
        """
        |> String.split("\n", trim: true)

  import ExUnit.Assertions
  use Mix.Task

  def parse(lines) do
    ground =
      Enum.reduce(lines, %{}, fn line, ground ->
        {xr, yr} = parse_line(line)

        for x <- xr, y <- yr, into: ground do
          {{x, y}, :clay}
        end
      end)

    {ymin, ymax} =
      ground
      |> Map.keys()
      |> Enum.map(&elem(&1, 1))
      |> Enum.min_max()

    fill(ground, {500, ymin}, ymax)
  end

  defp parse_line(line) do
    [_, c1, v, c2, a, b] = Regex.run(~r/(.)=(.+), (.)=(.+)\.\.(.+)/, line)
    [v, a, b] = Enum.map([v, a, b], &String.to_integer/1)

    case {c1, c2} do
      {"x", "y"} -> {v..v, a..b}
      {"y", "x"} -> {a..b, v..v}
    end
  end

  def fill(ground, {_, y}, ymax) when y > ymax, do: ground

  def fill(ground, {x, y} = water, ymax) do
    ground = Map.put(ground, water, :flow)

    case Map.get(ground, {x, y + 1}) do
      nil ->
        fill(ground, {x, y + 1}, ymax)

      _ ->
        {{gl, l}, {gr, r}} = flow_horizontally(ground, {x, y})

        case {gl, gr} do
          {:clay, :clay} ->
            for x <- l..r, into: ground do
              {{x, y}, :water}
            end
            |> fill({x, y - 1}, ymax)

          _ ->
            ground =
              for x <- l..r, into: ground do
                {{x, y}, :flow}
              end

            Enum.reduce([{gl, l}, {gr, r}], ground, fn
              {nil, x}, ground -> fill(ground, {x, y + 1}, ymax)
              _, ground -> ground
            end)
        end
    end
  end

  def flow_horizontally(ground, c) do
    left = flow_horizontally(ground, c, -1)
    right = flow_horizontally(ground, c, +1)
    {left, right}
  end

  def flow_horizontally(ground, {x, y}, dx) do
    case {Map.get(ground, {x, y}), Map.get(ground, {x, y + 1})} do
      {:clay, _} -> {:clay, x - dx}
      {_, :flow} -> {:flow, x}
      {_, nil} -> {nil, x}
      _ -> flow_horizontally(ground, {x + dx, y}, dx)
    end
  end

  def dump(ground, io \\ :stdio) do
    {xmin, xmax} =
      ground
      |> Map.keys()
      |> Enum.map(&elem(&1, 0))
      |> Enum.min_max()

    {ymin, ymax} =
      ground
      |> Map.keys()
      |> Enum.map(&elem(&1, 1))
      |> Enum.min_max()

    for y <- ymin..ymax do
      for x <- xmin..xmax do
        IO.write(
          io,
          case Map.get(ground, {x, y}) do
            nil -> "."
            :clay -> "#"
            :water -> "~"
            :flow -> "|"
          end
        )
      end

      IO.puts(io, "")
    end

    ground
  end

  def q1(ground) do
    Map.values(ground) |> Enum.count(&(&1 == :water || &1 == :flow))
  end

  def q2(ground) do
    Map.values(ground) |> Enum.count(&(&1 == :water))
  end

  def check() do
    input = parse(@test)
    assert q1(input) == 57
    assert q2(input) == 29
  end

  def run(_) do
    check()

    input = Aoc2018.input(__MODULE__) |> parse()

    {time, _} =
      :timer.tc(fn ->
        IO.puts("Q1: #{q1(input)}")
        IO.puts("Q2: #{q2(input)}")
      end)

    IO.puts("  #{time / 1_000_000} seconds")
  end
end
