defmodule Markov do
@on_load{:init, 0}

  def init do
    :ok = :erlang.load_nif('src/markov_nif', 1)
  end

def d do
  IO.inspect _start_("/home/ubuntu/code/markov/faust.txt")
  loop
end

def loop do
  receive do
    something -> IO.inspect something
    _ -> IO.puts "Uncaught"
  end
  loop
end
  def _start_(_) do end

end
