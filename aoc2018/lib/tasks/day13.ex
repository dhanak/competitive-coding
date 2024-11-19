defmodule Mix.Tasks.Day13 do
  @shortdoc "AoC 2018 Day 13"

  @test1 ~S"""
         /->-\
         |   |  /----\
         | /-+--+-\  |
         | | |  | v  |
         \-+-/  \-+--/
           \------/
         """
         |> String.split("\n", trim: true)

  @test2 ~S"""
         />-<\
         |   |
         | /<+-\
         | | | v
         \>+</ |
           |   ^
           \<->/
         """
         |> String.split("\n", trim: true)

  import ExUnit.Assertions
  use Mix.Task

  def parse(lines) do
    for {line, y} <- Enum.with_index(lines),
        {v, x} <- Enum.with_index(String.to_charlist(line)),
        reduce: {%{}, %{}} do
      {tracks, carts} ->
        cond do
          v in ~C[ -|\/+] ->
            {Map.put(tracks, {x, y}, v), carts}

          v in ~C[^v] ->
            {Map.put(tracks, {x, y}, ?|), Map.put(carts, {x, y}, {v, :left})}

          v in ~C[<>] ->
            {Map.put(tracks, {x, y}, ?-), Map.put(carts, {x, y}, {v, :left})}
        end
    end
  end

  def q1(input), do: solve(input, :crash)

  def q2(input), do: solve(input, :remove)

  def solve({tracks, carts}, strategy) do
    Aoc2018.repeat_while(carts, &step(&1, tracks, strategy))
  end

  def step(carts, tracks, strategy) do
    carts =
      carts
      |> Map.keys()
      |> Enum.sort()
      |> Enum.reduce_while(carts, &move_cart(&1, &2, tracks, strategy))

    case Map.keys(carts) do
      [c] -> {:halt, c}
      _ -> {:cont, carts}
    end
  end

  def move_cart(c, carts, tracks, strategy) do
    case Map.pop(carts, c) do
      {nil, carts} ->
        # skip carts that have been removed already
        {:cont, carts}

      {{d, t}, carts} ->
        c = move(c, d)

        case {Map.has_key?(carts, c), strategy} do
          {false, _} ->
            # no collision, turn as required
            {d, t} =
              case tracks[c] do
                v when v in ~C[-|] -> {d, t}
                v when v in ~C[\/] -> {turn(d, v), t}
                ?+ -> {turn(d, t), next_direction(t)}
              end

            {:cont, Map.put(carts, c, {d, t})}

          {true, :crash} ->
            # Q1: stop right here
            {:halt, %{c => nil}}

          {true, :remove} ->
            # Q2: remove colliding carts
            {:cont, Map.delete(carts, c)}
        end
    end
  end

  defp move({x, y}, ?^), do: {x, y - 1}
  defp move({x, y}, ?v), do: {x, y + 1}
  defp move({x, y}, ?<), do: {x - 1, y}
  defp move({x, y}, ?>), do: {x + 1, y}

  defp turn(?^, ?/), do: ?>
  defp turn(?v, ?/), do: ?<
  defp turn(?>, ?\\), do: ?v
  defp turn(?<, ?\\), do: ?^
  defp turn(?^, :right), do: ?>
  defp turn(?v, :right), do: ?<
  defp turn(?>, :right), do: ?v
  defp turn(?<, :right), do: ?^

  defp turn(?^, ?\\), do: ?<
  defp turn(?v, ?\\), do: ?>
  defp turn(?>, ?/), do: ?^
  defp turn(?<, ?/), do: ?v
  defp turn(?^, :left), do: ?<
  defp turn(?v, :left), do: ?>
  defp turn(?>, :left), do: ?^
  defp turn(?<, :left), do: ?v

  defp turn(t, :straight), do: t

  defp next_direction(:left), do: :straight
  defp next_direction(:straight), do: :right
  defp next_direction(:right), do: :left

  defp check() do
    assert q1(parse(@test1)) == {7, 3}
    assert q2(parse(@test2)) == {6, 4}
  end

  def run(_) do
    check()

    lines = Aoc2018.input(__MODULE__) |> parse()

    {time, _} =
      :timer.tc(fn ->
        IO.puts("Q1: #{inspect(q1(lines))}")
        IO.puts("Q2: #{inspect(q2(lines))}")
      end)

    IO.puts("  #{time / 1_000_000} seconds")
  end
end
