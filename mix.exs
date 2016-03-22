defmodule Faust.Mixfile do
  use Mix.Project

  @version "0.1.0"
  @source_url "https://github.com/jquadrin/faust"

  def project do
    [app: :faust,
     version: @version,
     elixir: "~> 1.2",
     deps: deps,
     docs: docs,
     name: "Faust",
     description: description,
     package: package,
     source_url: @source_url]
  end

  def application do
    [applications: []]
  end

  defp deps do
    []
  end

  defp docs do
    [source_ref: "v#{@version}", 
     main: "readme", 
     extras: ["README.md"]]
  end

  defp description do
  """
  A Markov chain text generator for Elixir.
  """
  end

  defp package do
    [maintainers: ["Joe Quadrino"],
     licenses: ["Apache 2.0"],
     links: %{"GitHub" => @source_url}]
  end

end
