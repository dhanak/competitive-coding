defmodule Aoc2018.MixProject do
  use Mix.Project

  def project do
    [
      app: :aoc2018,
      version: "0.1.0",
      elixir: "~> 1.17",
      start_permanent: Mix.env() == :prod,
      deps: deps(),
      dialyzer: [plt_add_apps: [:ex_unit, :mix]],
      aliases: [test: ["dialyzer", "test"]]
    ]
  end

  # Run "mix help compile.app" to learn about applications.
  def application do
    [
      extra_applications: [:logger]
    ]
  end

  # Run "mix help deps" to learn about dependencies.
  defp deps do
    [
      {:dialyxir, "~> 1.1", only: [:dev, :test], runtime: false},
      {:libgraph, "~> 0.16.0"},
      {:nimble_parsec, "~> 1.0"},
      {:xpeg, "~> 0.9.1"}
    ]
  end
end
