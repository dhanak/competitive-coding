defmodule Mix.Tasks.Day10 do
  @shortdoc "AoC 2018 Day 10"

  @test """
        position=< 9,  1> velocity=< 0,  2>
        position=< 7,  0> velocity=<-1,  0>
        position=< 3, -2> velocity=<-1,  1>
        position=< 6, 10> velocity=<-2, -1>
        position=< 2, -4> velocity=< 2,  2>
        position=<-6, 10> velocity=< 2, -2>
        position=< 1,  8> velocity=< 1, -1>
        position=< 1,  7> velocity=< 1,  0>
        position=<-3, 11> velocity=< 1, -2>
        position=< 7,  6> velocity=<-1, -1>
        position=<-2,  3> velocity=< 1,  0>
        position=<-4,  3> velocity=< 2,  0>
        position=<10, -3> velocity=<-1,  1>
        position=< 5, 11> velocity=< 1, -2>
        position=< 4,  7> velocity=< 0, -1>
        position=< 8, -2> velocity=< 0,  1>
        position=<15,  0> velocity=<-2,  0>
        position=< 1,  6> velocity=< 1,  0>
        position=< 8,  9> velocity=< 0, -1>
        position=< 3,  3> velocity=<-1,  1>
        position=< 0,  5> velocity=< 0, -1>
        position=<-2,  2> velocity=< 2,  0>
        position=< 5, -2> velocity=< 1,  2>
        position=< 1,  4> velocity=< 2,  1>
        position=<-2,  7> velocity=< 2, -2>
        position=< 3,  6> velocity=<-1, -1>
        position=< 5,  0> velocity=< 1,  0>
        position=<-6,  0> velocity=< 2,  0>
        position=< 5,  9> velocity=< 1, -2>
        position=<14,  7> velocity=<-2,  0>
        position=<-3,  6> velocity=< 2, -1>
        """
        |> String.split("\n", trim: true)

  import ExUnit.Assertions
  use Mix.Task

  def parse_line(line) do
    ~r/^p.*n=< *(-?[0-9]+), *(-?[0-9]+)> v.*y=< *(-?[0-9]+), *(-?[0-9]+)>$/
    |> Regex.run(line)
    |> tl()
    |> Enum.map(&String.to_integer/1)
    |> Enum.chunk_every(2)
    |> Enum.map(&List.to_tuple/1)
    |> List.to_tuple()
  end

  def q1(lights, height) do
    t = run_length(lights, height)
    lights = move(lights, t)
    plot(lights)
  end

  def run_length(lights, height) do
    {t, t} =
      for l1 <- lights, l2 <- lights, l1 !== l2, reduce: {0, Inf} do
        acc -> intersect(acc, run_length(l1, l2, height))
      end

    t
  end

  def intersect({amin, amax}, {bmin, bmax}) do
    {max(amin, bmin), min(amax, bmax)}
  end

  def run_length({{_, pa}, {_, v}}, {{_, pb}, {_, v}}, height) do
    assert abs(pa - pb) < height
    {0, Inf}
  end

  def run_length({{_, pa}, {_, va}}, {{_, pb}, {_, vb}}, height) when va > vb do
    tmin = ceil((pb - pa - height + 1) / (va - vb))
    tmax = floor((pb - pa + height - 1) / (va - vb))
    assert tmin <= tmax
    {tmin, tmax}
  end

  def run_length(a, b, height), do: run_length(b, a, height)

  def move(lights, n) do
    Enum.map(lights, fn {{x, y}, {vx, vy}} ->
      {x + vx * n, y + vy * n}
    end)
  end

  def plot(lights) do
    l = lights |> Enum.map(&elem(&1, 0)) |> Enum.min()
    t = lights |> Enum.map(&elem(&1, 1)) |> Enum.min()
    lights = Enum.sort_by(lights, fn {x, y} -> {y, x} end)
    plot(lights, t, l)
  end

  def plot([], _, _), do: nil

  def plot(lights, y, l) do
    {line, lights} = Enum.split_with(lights, fn {_, ly} -> ly == y end)
    plot_line(line, l)
    plot(lights, y + 1, l)
  end

  def plot_line([], _), do: IO.puts("")

  def plot_line([{lx, _} | lights], x) when lx < x do
    plot_line(lights, x)
  end

  def plot_line([{lx, _} | lights], x) do
    IO.write(String.pad_leading("#", lx - x + 1))
    plot_line(lights, lx + 1)
  end

  def q2(lights, height) do
    run_length(lights, height)
  end

  defp check() do
    lights = @test |> Enum.map(&parse_line/1)
    height = 8
    q1(lights, height)
    assert q2(lights, height) == 3
  end

  def run(_) do
    check()

    lights =
      Aoc2018.input(__MODULE__)
      |> Enum.map(&parse_line/1)

    height = 10

    {time, _} =
      :timer.tc(fn ->
        IO.puts("Q1:")
        q1(lights, height)
        IO.puts("Q2: #{q2(lights, height)}")
      end)

    IO.puts("  #{time / 1_000_000} seconds")
  end
end
