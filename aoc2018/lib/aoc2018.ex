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
  Convert an ASCII letter `c` to upper case.

  ## Examples

      iex> <<downcase(?A)>>
      "a"
  """
  def downcase(c), do: c ||| 32

  @doc """
  Convert an ASCII letter `c` to lower case.

  ## Examples

      iex> <<upcase(?a)>>
      "A"
  """
  def upcase(c), do: c &&& bnot(32)

  @doc """
  Compute the Manhattan distance between two coordinates.

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
  Repeatedly apply a function `fun` `n` times on an initial value `acc`.

  ## Examples

    iex> repeat(0, 5, &(&1 + 1))
    5

    iex> repeat(1, 5, &(&1 * 2))
    32
  """
  def repeat(acc, n, fun) do
    Enum.reduce(1..n//1, acc, fn _, acc -> fun.(acc) end)
  end

  @doc """
  Repeatedly apply a function `fun` on an initial value `acc`.

  The return value for `fun` is expected to be

  - `{:cont, acc}` to continue with `acc` as the new accumulator
    or
  - `{:halt, val}` to halt and return `val`

  ## Examples

      iex> repeat_while(1, fn v ->
      ...>    if v < 10, do: {:cont, 2 * v}, else: {:halt, to_string(v)}
      ...> end)
      "16"
  """
  def repeat_while(acc, fun) do
    Stream.cycle([nil]) |> Enum.reduce_while(acc, fn nil, acc -> fun.(acc) end)
  end

  @doc """
  Return a permutation list `l` that puts `enumerable` in sorted order,
  according to the provided `sorter` function.

  ## Examples

      iex> sortperm([3, 1, 2])
      [1, 2, 0]

      iex> sortperm([:foo, :bar, :baz])
      [1, 2, 0]
  """
  def sortperm(enumerable, sorter \\ :asc) do
    enumerable
    |> Enum.with_index()
    |> Enum.sort_by(&elem(&1, 0), sorter)
    |> Enum.map(&elem(&1, 1))
  end
end
