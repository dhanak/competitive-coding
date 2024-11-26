defmodule Mix.Tasks.Day15 do
  @shortdoc "AoC 2018 Day 15"

  @power 3

  @testcases %{
    1 => """
    #######
    #.G...#
    #...EG#
    #.#.#G#
    #..G#E#
    #.....#
    #######
    """,
    2 => """
    #######
    #G..#E#
    #E#E.E#
    #G.##.#
    #...#E#
    #...E.#
    #######
    """,
    3 => """
    #######
    #E..EG#
    #.#G.E#
    #E.##E#
    #G..#.#
    #..E#.#
    #######
    """,
    4 => """
    #######
    #E.G#.#
    #.#G..#
    #G.#.G#
    #G..#.#
    #...E.#
    #######
    """,
    5 => """
    #######
    #.E...#
    #.#..G#
    #.###.#
    #E#G#G#
    #...#G#
    #######
    """,
    6 => """
    #########
    #G......#
    #.E.#...#
    #..##..G#
    #...##..#
    #...#...#
    #.G...G.#
    #.....G.#
    #########
    """
  }

  import ExUnit.Assertions
  use Mix.Task

  def testcase(n), do: parse(String.split(@testcases[n], "\n", trim: true))

  def q1_scores(), do: [27730, 36334, 39514, 27755, 28944, 18740]

  def q2_scores() do
    [{15, 4988}, nil, {4, 31284}, {15, 3478}, {12, 6474}, {34, 1140}]
  end

  def parse(lines) do
    for {l, y} <- Enum.with_index(lines),
        {f, x} <- Enum.with_index(to_charlist(l)),
        reduce: %{arena: MapSet.new(), units: %{}} do
      bg ->
        arena =
          if f == ?# do
            bg.arena
          else
            MapSet.put(bg.arena, {y, x})
          end

        units =
          if f in ~c[EG] do
            type = if f == ?E, do: :elf, else: :goblin
            unit = %{on: {y, x}, type: type, hp: 200}
            Map.put(bg.units, map_size(bg.units), unit)
          else
            bg.units
          end

        %{arena: arena, units: units}
    end
  end

  def play(battleground, elf_power \\ @power, round \\ 0) do
    case do_round(battleground, elf_power) do
      {:ok, battleground} -> play(battleground, elf_power, round + 1)
      {:end, battleground} -> {:ok, round, battleground}
      {:elf_death, battleground} -> {:elf_death, round, battleground}
    end
  end

  def do_round(battleground, elf_power \\ @power) do
    battleground.units
    |> Enum.sort_by(fn {_, %{on: point}} -> point end)
    |> Enum.map(&elem(&1, 0))
    |> then(&do_round(battleground, &1, elf_power))
  end

  def do_round(battleground, [i | is], elf_power) do
    case do_turn(battleground, i, elf_power) do
      {:ok, battleground} -> do_round(battleground, is, elf_power)
      result -> result
    end
  end

  def do_round(battleground, [], _), do: {:ok, battleground}

  def do_turn(battleground, i, elf_power \\ @power) do
    unit = Map.get(battleground.units, i, false)
    power = if unit && unit.type == :elf, do: elf_power, else: @power
    elves_can_die = elf_power == @power

    case unit && attack_target(battleground, unit) do
      false ->
        {:ok, battleground}

      j when is_number(j) ->
        attack(battleground, j, power, elves_can_die)

      nil ->
        case move_to(battleground, unit) do
          {:err, :no_target} ->
            {:end, battleground}

          {:err, :no_route} ->
            {:ok, battleground}

          {:ok, to} ->
            {battleground, unit} = move(battleground, i, to)

            case attack_target(battleground, unit) do
              j when is_number(j) ->
                attack(battleground, j, power, elves_can_die)

              nil ->
                {:ok, battleground}
            end
        end
    end
  end

  def neighbor?({ay, ax}, {by, bx}), do: abs(ay - by) + abs(ax - bx) == 1

  def neighbors({y, x}), do: [{y - 1, x}, {y, x - 1}, {y, x + 1}, {y + 1, x}]

  def neighbors(%{on: point}), do: neighbors(point)

  def free?(battleground, point) do
    MapSet.member?(battleground.arena, point) and
      unit_on(battleground, point) === nil
  end

  def unit_on(%{units: units}, on) do
    Enum.find(units, fn {_, %{on: point}} -> point == on end)
  end

  def free_neighbors(battleground, point) do
    neighbors(point) |> Enum.filter(&free?(battleground, &1))
  end

  def targets(battleground, unit, opts \\ []) do
    all = opts[:neighbors] !== true

    battleground.units
    |> Enum.filter(fn {_, %{on: point, type: type}} ->
      type !== unit.type and (all or neighbor?(point, unit.on))
    end)
    |> Enum.sort_by(fn
      {_, %{on: point, hp: hp}} -> {hp, point}
    end)
  end

  def attack_target(battleground, unit) do
    case targets(battleground, unit, neighbors: true) |> List.first() do
      {target, _} -> target
      nil -> nil
    end
  end

  def move_to(battleground, unit) do
    move_to(
      battleground,
      free_neighbors(battleground, unit),
      targets(battleground, unit)
    )
  end

  defp move_to(_, _, []), do: {:err, :no_target}

  defp move_to(_, [], _), do: {:err, :no_route}

  defp move_to(battleground, starts, goals) do
    squares =
      goals
      |> Enum.flat_map(fn
        {_, goal} -> free_neighbors(battleground, goal)
      end)
      |> Enum.sort()
      |> Enum.uniq()

    target =
      starts
      |> Enum.map(fn point ->
        shortest_path(point, squares, &free_neighbors(battleground, &1))
      end)
      |> Enum.reject(&is_nil/1)
      |> Enum.sort()
      |> List.first()

    if target === nil, do: {:err, :no_route}, else: {:ok, target[:start]}
  end

  def shortest_path(start, goals, next) do
    shortest_path(%{
      d: 0,
      current: [start],
      future: [],
      visited: MapSet.new(),
      start: start,
      goals: goals,
      next: next
    })
  end

  defp shortest_path(%{current: [], future: []}), do: nil

  defp shortest_path(%{d: d, current: [], future: future} = state) do
    shortest_path(%{state | d: d + 1, current: Enum.sort(future), future: []})
  end

  defp shortest_path(%{current: [point | current]} = state) do
    cond do
      Enum.member?(state.goals, point) ->
        [d: state.d, goal: point, start: state.start]

      MapSet.member?(state.visited, point) ->
        shortest_path(%{state | current: current})

      true ->
        shortest_path(%{
          state
          | current: current,
            future: state.next.(point) ++ state.future,
            visited: MapSet.put(state.visited, point)
        })
    end
  end

  def attack(battleground, i, power \\ @power, elves_can_die \\ true) do
    unit = Map.update!(battleground.units[i], :hp, &(&1 - power))
    state = if unit.hp <= 0, do: :dead, else: :alive

    case {state, elves_can_die || unit.type} do
      {:dead, :elf} ->
        {:elf_death, battleground}

      {:dead, _} ->
        {:ok, %{battleground | units: Map.delete(battleground.units, i)}}

      {:alive, _} ->
        {:ok, %{battleground | units: %{battleground.units | i => unit}}}
    end
  end

  def move(battleground, i, to) do
    unit = battleground.units[i]
    unit = %{unit | on: to}
    battleground = %{battleground | units: %{battleground.units | i => unit}}
    {battleground, unit}
  end

  def total_hp(battleground) do
    battleground.units |> Map.values() |> Enum.map(& &1.hp) |> Enum.sum()
  end

  def dump(io \\ :stdio, battleground) do
    h = Enum.max(Enum.map(battleground.arena, &elem(&1, 0))) + 1
    w = Enum.max(Enum.map(battleground.arena, &elem(&1, 1))) + 1
    chars = for y <- 0..h, x <- 0..w, into: Map.new(), do: {{y, x}, "#"}
    chars = for point <- battleground.arena, into: chars, do: {point, "."}

    chars =
      for {_, %{on: point, type: type}} <- battleground.units, into: chars do
        {point, typechar(type)}
      end

    for y <- 0..h do
      for x <- 0..w do
        IO.write(io, chars[{y, x}])
      end

      IO.write(io, "   ")

      battleground.units
      |> Map.values()
      |> Enum.filter(&match?(%{on: {^y, _}}, &1))
      |> Enum.sort_by(& &1.on)
      |> Enum.map(fn %{type: type, hp: hp} -> "#{typechar(type)}(#{hp})" end)
      |> Enum.join(", ")
      |> then(&IO.puts(io, &1))
    end

    :ok
  end

  def simulate(io \\ :stdio, battleground), do: simulate(io, battleground, 0)

  defp simulate(io, battleground, round) do
    IO.puts(io, "Round #{round}:")
    dump(io, battleground)
    IO.puts(io, "")

    case do_round(battleground) do
      {:ok, battleground} ->
        simulate(io, battleground, round + 1)

      {:end, battleground} ->
        IO.puts(io, "Final round #{round + 1}:")
        dump(io, battleground)
        IO.puts(io, "Combat ends after #{round} full rounds")
        hp = total_hp(battleground)

        [winners] =
          battleground.units
          |> Enum.frequencies_by(&elem(&1, 1).type)
          |> Map.keys()

        IO.puts(io, "#{typestr(winners)} win with #{hp} total hit points left")
        IO.puts(io, "Outcome: #{round} * #{hp} = #{round * hp}")
    end
  end

  defp typechar(:elf), do: "E"
  defp typechar(:goblin), do: "G"

  defp typestr(:elf), do: "Elves"
  defp typestr(:goblin), do: "Gnomes"

  def q1(battleground) do
    {:ok, round, battleground} = play(battleground)
    round * total_hp(battleground)
  end

  def q2(battleground, min_power \\ 3, max_power \\ 200)

  def q2(battleground, min_power, max_power) when max_power == min_power + 1 do
    {:ok, round, battleground} = play(battleground, max_power)
    {max_power, round * total_hp(battleground)}
  end

  def q2(battleground, min_power, max_power) do
    power = div(min_power + max_power, 2)

    case play(battleground, power) |> elem(0) do
      :elf_death -> q2(battleground, power, max_power)
      :ok -> q2(battleground, min_power, power)
    end
  end

  defp check() do
    IO.write("Checks: " <> IO.ANSI.green())
    puzzles = Enum.map(1..6, &testcase/1)
    assert length(puzzles) == length(q1_scores())
    assert length(puzzles) == length(q2_scores())

    for {puzzle, score} <- Enum.zip(puzzles, q1_scores()) do
      assert q1(puzzle) == score
      IO.write(".")
    end

    for {puzzle, score} <- Enum.zip(puzzles, q2_scores()) do
      assert score == nil or q2(puzzle) == score
      IO.write(".")
    end

    IO.puts(IO.ANSI.reset())
  end

  def run(_) do
    check()

    puzzle = Aoc2018.input(__MODULE__) |> parse()

    {time, _} =
      :timer.tc(fn ->
        IO.puts("Q1: #{q1(puzzle)}")
        IO.puts("Q2: #{elem(q2(puzzle), 1)}")
      end)

    IO.puts("  #{time / 1_000_000} seconds")
  end
end
