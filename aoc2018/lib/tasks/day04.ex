defmodule Mix.Tasks.Day04 do
  @shortdoc "AoC 2018 Day 4"

  @test """
        [1518-11-01 00:00] Guard #10 begins shift
        [1518-11-01 00:05] falls asleep
        [1518-11-01 00:25] wakes up
        [1518-11-01 00:30] falls asleep
        [1518-11-01 00:55] wakes up
        [1518-11-01 23:58] Guard #99 begins shift
        [1518-11-02 00:40] falls asleep
        [1518-11-02 00:50] wakes up
        [1518-11-03 00:05] Guard #10 begins shift
        [1518-11-03 00:24] falls asleep
        [1518-11-03 00:29] wakes up
        [1518-11-04 00:02] Guard #99 begins shift
        [1518-11-04 00:36] falls asleep
        [1518-11-04 00:46] wakes up
        [1518-11-05 00:03] Guard #99 begins shift
        [1518-11-05 00:45] falls asleep
        [1518-11-05 00:55] wakes up
        """
        |> String.split("\n", trim: true)

  import ExUnit.Assertions
  use Mix.Task

  def parse(lines) do
    lines
    |> Enum.sort()
    |> Enum.map_reduce(-1, fn line, id ->
      [_, dt, event] = Regex.run(~r/^\[(.*)\] (.*)$/, line)
      {:ok, dt} = NaiveDateTime.from_iso8601("#{dt}:00")

      {id, action} =
        case event do
          "falls asleep" ->
            {id, :sleep}

          "wakes up" ->
            {id, :wakeup}

          _ ->
            id =
              Regex.run(~r/^Guard #([0-9]+) begins shift$/, event)
              |> Enum.at(1)
              |> Integer.parse()
              |> elem(0)

            {id, :begin}
        end

      {{dt, id, action}, id}
    end)
    |> elem(0)
    |> Enum.reject(fn {_, _, action} -> action === :begin end)
    |> Enum.chunk_every(2)
    |> Enum.map(fn [{s, id, :sleep}, {w, id, :wakeup}] -> {id, {s, w}} end)
    |> Enum.group_by(&elem(&1, 0), &elem(&1, 1))
  end

  def q1(sleeps) do
    laziest =
      sleeps
      |> Enum.map(fn {id, ranges} ->
        total =
          Enum.reduce(ranges, 0, fn {s, w}, total ->
            total + w.minute - s.minute
          end)

        {id, total}
      end)
      |> Enum.max_by(&elem(&1, 1))
      |> elem(0)

    best_min =
      Enum.max_by(0..59, fn min ->
        Enum.count(sleeps[laziest], fn {s, w} ->
          s.minute <= min and min < w.minute
        end)
      end)

    laziest * best_min
  end

  def q2(sleeps) do
    {freqest, best_min, _} =
      for {id, ranges} <- sleeps, min <- 0..59, reduce: {0, 0, 0} do
        {_, _, best_total} = best ->
          total =
            Enum.count(ranges, fn {s, w} ->
              s.minute <= min and min < w.minute
            end)

          if total > best_total, do: {id, min, total}, else: best
      end

    freqest * best_min
  end

  defp check() do
    sleeps = parse(@test)
    assert q1(sleeps) == 240
    assert q2(sleeps) == 4455
  end

  def run(_) do
    check()

    day =
      __MODULE__
      |> Atom.to_string()
      |> String.split(".")
      |> List.last()
      |> String.downcase()

    sleeps =
      File.stream!("input/#{day}.txt", :line)
      |> Stream.map(&String.trim_trailing(&1, "\n"))
      |> parse()

    {time, _} =
      :timer.tc(fn ->
        IO.puts("Q1: #{q1(sleeps)}")
        IO.puts("Q2: #{q2(sleeps)}")
      end)

    IO.puts("  #{time / 1_000_000} seconds")
  end
end
