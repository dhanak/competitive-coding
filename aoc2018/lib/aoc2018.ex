defmodule Aoc2018 do
  import Bitwise

  def input(module) do
    day =
      module
      |> Atom.to_string()
      |> String.split(".")
      |> List.last()
      |> String.downcase()

    File.stream!("input/#{day}.txt", :line)
    |> Stream.map(&String.trim_trailing(&1, "\n"))
  end

  @doc """
  Convert an ASCII letter to upper case.

  ## Examples

      iex> <<downcase(?A)>>
      "a"
  """
  def downcase(c), do: c ||| 32

  @doc """
  Convert an ASCII letter to lower case.

  ## Examples

      iex> <<upcase(?a)>>
      "A"
  """
  def upcase(c), do: c &&& bnot(32)

  @doc """
  Compute the Manhattan distance between two coordinates

  ## Examples

    iex> manhattan_distance({0, 0}, {2, 3})
    5
    iex> manhattan_distance({1, 4}, {-2, 7})
    6
  """
  def manhattan_distance({ax, ay}, {bx, by}), do: abs(ax - bx) + abs(ay - by)

  @doc """
  Return the value and the index of the smallest element in an enumerable.

  ## Examples

    iex> argmin([1, 2, 3])
    {1, 0}

    iex> argmin([2, 1, 3])
    {1, 1}

    iex> argmin([~D[2017-03-31], ~D[2017-04-01]])
    {~D[2017-04-01], 1}

    iex> argmin([~D[2017-03-31], ~D[2017-04-01]], Date)
    {~D[2017-03-31], 0}
  """
  def argmin(
        enum,
        sorter \\ &<=/2,
        empty_fallback \\ fn -> raise Enum.EmptyError end
      ) do
    enum
    |> Enum.with_index()
    |> Enum.min_by(&elem(&1, 0), sorter, empty_fallback)
  end

  @doc """
  Repeatedly apply a function f n times on an initial value.

  ## Examples

    iex> repeat(0, 5, &(&1 + 1))
    5

    iex> repeat(1, 5, &(&1 * 2))
    32
  """
  def repeat(init, n, f) do
    Enum.reduce(1..n//1, init, fn _, acc -> f.(acc) end)
  end
end
