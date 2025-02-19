defmodule Mix.Tasks.Day18 do
  @shortdoc "AoC 2018 Day 18"

  @test """
        .#.#...|#.
        .....#|##|
        .|..|...#.
        ..|#.....#
        #.#|||#|#|
        ...#.||...
        .|....|...
        ||...#|.#|
        |.||||..|.
        ...#.|..|.
        """
        |> String.split("\n", trim: true)

  import ExUnit.Assertions
  use Mix.Task

  @spec parse(Enum.t()) :: map()
  def parse(lines) do
    for {line, y} <- Enum.with_index(lines),
        {acre, x} <- Enum.with_index(String.graphemes(line)),
        into: %{} do
      {{y, x},
       case acre do
         "." -> :open
         "|" -> :tree
         "#" -> :yard
       end}
    end
  end

  @spec adjacent(map(), {integer(), integer()}) :: map()
  def adjacent(area, {y, x}) do
    for dy <- -1..1, dx <- -1..1, {y + dy, x + dx} != {y, x}, into: [] do
      Map.get(area, {y + dy, x + dx})
    end
    |> Enum.reject(&is_nil/1)
    |> Enum.frequencies()
    |> then(&Map.merge(%{open: 0, tree: 0, yard: 0}, &1))
  end

  @spec update(map()) :: map()
  def update(area) do
    Enum.map(area, fn {c, old} ->
      new =
        case {old, adjacent(area, c)} do
          {:open, %{tree: t}} when t >= 3 -> :tree
          {:tree, %{yard: l}} when l >= 3 -> :yard
          {:yard, %{tree: t, yard: l}} when t < 1 or l < 1 -> :open
          {acre, _} -> acre
        end

      {c, new}
    end)
    |> Map.new()
  end

  @spec dump(IO.device(), map()) :: :ok
  def dump(io \\ :stdio, area) do
    s = trunc(:math.sqrt(map_size(area))) - 1

    for y <- 0..s do
      for x <- 0..s do
        IO.write(
          io,
          case area[{y, x}] do
            :open -> "."
            :tree -> "|"
            :yard -> "#"
          end
        )
      end

      IO.puts(io, "")
    end

    :ok
  end

  @spec q1(input :: map(), n :: integer()) :: integer()
  def q1(input, n \\ 10) do
    %{tree: t, yard: l} =
      input
      |> Aoc2018.repeat(n, &update/1)
      |> Map.values()
      |> Enum.frequencies()

    t * l
  end

  @spec q2(
          input :: map(),
          memo :: %{map() => integer()},
          round :: integer()
        ) :: integer()
  def q2(input, memo \\ %{}, round \\ 0) do
    case Map.get(memo, input) do
      nil ->
        q2(update(input), Map.put(memo, input, round), round + 1)

      round0 ->
        n = rem(1_000_000_000 - round0, round - round0)
        q1(input, n)
    end
  end

  defp check() do
    assert q1(parse(@test)) == 1147
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
