defmodule Mix.Tasks.Day14 do
  @shortdoc "AoC 2018 Day 14"

  import ExUnit.Assertions
  use Mix.Task

  @initial_state %{recipes: %{0 => 3, 1 => 7}, a: 0, b: 1}

  def q1(input) do
    recipes =
      Stream.cycle([nil])
      |> Enum.reduce_while(@initial_state, fn _, %{recipes: recipes} = state ->
        if map_size(recipes) >= input + 10 do
          {:halt, recipes}
        else
          {state, _} = mix(state)
          {:cont, state}
        end
      end)

    input..(input + 9) |> Enum.map(&recipes[&1]) |> Enum.join()
  end

  # This is dead slow, takes more than a minute!
  def q2(input) do
    input_length = String.length(input)

    Stream.cycle([nil])
    |> Enum.reduce_while({@initial_state, "37", 0}, fn _, {state0, tail, n} ->
      {state1, added} = mix(state0)
      tail = tail <> Enum.join(added)

      case :binary.match(tail, input) do
        :nomatch ->
          {dropped, tail} = String.split_at(tail, 1 - input_length)
          {:cont, {state1, tail, n + String.length(dropped)}}

        {at, ^input_length} ->
          {:halt, n + at}
      end
    end)
  end

  def mix(%{recipes: recipes, a: a, b: b}) do
    digits = Integer.digits(recipes[a] + recipes[b])
    recipes = Enum.reduce(digits, recipes, &Map.put(&2, map_size(&2), &1))
    length = map_size(recipes)
    a = rem(a + recipes[a] + 1, length)
    b = rem(b + recipes[b] + 1, length)
    {%{recipes: recipes, a: a, b: b}, digits}
  end

  defp check() do
    assert q1(5) == "0124515891"
    assert q1(9) == "5158916779"
    assert q1(18) == "9251071085"
    assert q1(2018) == "5941429882"
    assert q2("01245") == 5
    assert q2("51589") == 9
    assert q2("92510") == 18
    assert q2("59414") == 2018
  end

  def run(_) do
    check()

    input = 894_501

    {time, _} =
      :timer.tc(fn ->
        IO.puts("Q1: #{q1(input)}")
        IO.puts("Q2: #{q2(Integer.to_string(input))}")
      end)

    IO.puts("  #{time / 1_000_000} seconds")
  end
end
