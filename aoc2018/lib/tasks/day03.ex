defmodule Mix.Tasks.Day03 do
  @shortdoc "AoC 2018 Day 3"

  @test """
        #1 @ 1,3: 4x4
        #2 @ 3,1: 4x4
        #3 @ 5,5: 2x2
        """
        |> String.split("\n", trim: true)

  import ExUnit.Assertions
  use Mix.Task

  def parse(claims), do: Enum.map(claims, &parse_claim/1)

  def parse_claim(claim) do
    [id, l, t, w, h] =
      claim
      |> String.split(~r"[^0-9]+", trim: true)
      |> Enum.map(&String.to_integer/1)

    {id, {l, t, w, h}}
  end

  def q1(claims) do
    for {_id, {l, t, w, h}} <- claims,
        x <- l..(l + w - 1),
        y <- t..(t + h - 1),
        reduce: %{} do
      fabric -> Map.update(fabric, {x, y}, 1, &(&1 + 1))
    end
    |> Map.values()
    |> Enum.count(&(&1 > 1))
  end

  def q2(claims) do
    ids = MapSet.new(claims, &elem(&1, 0))

    [solo] =
      for {ai, ar} <- claims, {bi, br} <- claims, ai != bi, reduce: ids do
        ids ->
          if overlap?(ar, br) do
            ids |> MapSet.delete(ai) |> MapSet.delete(bi)
          else
            ids
          end
      end
      |> MapSet.to_list()

    solo
  end

  def overlap?({al, at, aw, ah}, {bl, bt, bw, bh}) do
    al < bl + bw and bl < al + aw and at < bt + bh and bt < at + ah
  end

  defp check() do
    claims = parse(@test)
    assert q1(claims) == 4
    assert q2(claims) == 3
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
      |> parse()

    {time, _} =
      :timer.tc(fn ->
        IO.puts("Q1: #{q1(lines)}")
        IO.puts("Q2: #{q2(lines)}")
      end)

    IO.puts("  #{time / 1_000_000} seconds")
  end
end
