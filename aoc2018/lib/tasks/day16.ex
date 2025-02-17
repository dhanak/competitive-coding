defmodule Mix.Tasks.Day16 do
  @shortdoc "AoC 2018 Day 16"

  @test """
        Before: [3, 2, 1, 1]
        9 2 1 2
        After:  [3, 2, 2, 1]



        """
        |> String.split("\n")

  import ExUnit.Assertions
  import Bitwise
  use Mix.Task

  def parse(lines) do
    [samples, program] = Enum.join(lines, "\n") |> String.split("\n\n\n")

    samples =
      samples
      |> String.split("\n", trim: true)
      |> Enum.filter(&(&1 != ""))
      |> Enum.chunk_every(3)
      |> Enum.map(&parse_sample/1)

    program =
      program
      |> String.split("\n", trim: true)
      |> Enum.map(fn line ->
        line
        |> String.split(" ")
        |> Enum.map(fn s ->
          {i, ""} = Integer.parse(s)
          i
        end)
      end)

    {samples, program}
  end

  def parse_sample(["Before: " <> pre, inst, "After: " <> post]) do
    [op, a, b, c] =
      String.split(inst, " ")
      |> Enum.map(&elem(Integer.parse(&1), 0))

    pre = parse_registers(pre)
    post = parse_registers(post)
    %{before: pre, inst: [op, a, b, c], after: post}
  end

  def parse_registers(s) do
    {regs, []} = Code.eval_string(s)
    regs |> Enum.with_index(fn v, i -> {i, v} end) |> Map.new()
  end

  def addr(regs, [a, b, c]), do: %{regs | c => regs[a] + regs[b]}
  def addi(regs, [a, b, c]), do: %{regs | c => regs[a] + b}
  def mulr(regs, [a, b, c]), do: %{regs | c => regs[a] * regs[b]}
  def muli(regs, [a, b, c]), do: %{regs | c => regs[a] * b}
  def banr(regs, [a, b, c]), do: %{regs | c => band(regs[a], regs[b])}
  def bani(regs, [a, b, c]), do: %{regs | c => band(regs[a], b)}
  def borr(regs, [a, b, c]), do: %{regs | c => bor(regs[a], regs[b])}
  def bori(regs, [a, b, c]), do: %{regs | c => bor(regs[a], b)}
  def setr(regs, [a, _, c]), do: %{regs | c => regs[a]}
  def seti(regs, [a, _, c]), do: %{regs | c => a}
  def gtir(regs, [a, b, c]), do: %{regs | c => if(a > regs[b], do: 1, else: 0)}
  def gtri(regs, [a, b, c]), do: %{regs | c => if(regs[a] > b, do: 1, else: 0)}
  def gtrr(regs, [a, b, c]), do: %{regs | c => if(regs[a] > regs[b], do: 1, else: 0)}
  def eqir(regs, [a, b, c]), do: %{regs | c => if(a == regs[b], do: 1, else: 0)}
  def eqri(regs, [a, b, c]), do: %{regs | c => if(regs[a] == b, do: 1, else: 0)}
  def eqrr(regs, [a, b, c]), do: %{regs | c => if(regs[a] == regs[b], do: 1, else: 0)}

  def ops() do
    [
      &addr/2,
      &addi/2,
      &mulr/2,
      &muli/2,
      &banr/2,
      &bani/2,
      &borr/2,
      &bori/2,
      &setr/2,
      &seti/2,
      &gtir/2,
      &gtri/2,
      &gtrr/2,
      &eqir/2,
      &eqri/2,
      &eqrr/2
    ]
  end

  def q1(samples) do
    Enum.count(samples, fn %{before: pre, after: post, inst: [_ | args]} ->
      Enum.count(ops(), fn op ->
        op.(pre, args) == post
      end) >= 3
    end)
  end

  def q2(samples, program) do
    # resolve opcodes
    opcodes = Map.from_keys(Range.to_list(0..15), MapSet.new(ops()))

    opcodes =
      Enum.reduce(samples, opcodes, fn sample, opcodes ->
        %{before: pre, after: post, inst: [opc | args]} = sample

        Map.update!(opcodes, opc, fn ops ->
          MapSet.filter(ops, fn op -> op.(pre, args) == post end)
        end)
      end)
      |> resolve

    # run program
    regs = Map.from_keys(Range.to_list(0..3), 0)

    regs =
      Enum.reduce(program, regs, fn [op | args], regs ->
        opcodes[op].(regs, args)
      end)

    regs[0]
  end

  def resolve(opcodes), do: resolve(opcodes, %{})

  def resolve(input, resolved) when input == %{}, do: resolved

  def resolve(input, resolved) do
    {opc, ops} = Enum.find(input, fn {_, ops} -> MapSet.size(ops) == 1 end)
    [op] = MapSet.to_list(ops)

    input =
      Enum.map(Map.delete(input, opc), fn {opc, ops} ->
        {opc, MapSet.delete(ops, op)}
      end)
      |> Map.new()

    resolved = Map.put(resolved, opc, op)

    resolve(input, resolved)
  end

  defp check() do
    {samples, []} = parse(@test)
    assert q1(samples) == 1
  end

  def run(_) do
    check()

    {samples, program} = Aoc2018.input(__MODULE__) |> Enum.to_list() |> parse()

    {time, _} =
      :timer.tc(fn ->
        IO.puts("Q1: #{q1(samples)}")
        IO.puts("Q2: #{q2(samples, program)}")
      end)

    IO.puts("  #{time / 1_000_000} seconds")
  end
end
