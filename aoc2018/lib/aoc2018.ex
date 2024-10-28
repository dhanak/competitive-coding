defmodule Aoc2018 do
  import Bitwise

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
end
