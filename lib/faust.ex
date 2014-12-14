defmodule Faust do
  @on_load{:init, 0}

  def init do
    :ok = :erlang.load_nif('src/faust_nif', 1)
  end

  def generate(path, [{:order, order}, {:n, iterations}]) do
    :ok = call_andrey(path, order, iterations)
  end

  def call_andrey(_, _, _) do end
end
