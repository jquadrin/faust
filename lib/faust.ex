defmodule Faust do
  @moduledoc """
  A Markov chain text generator for Elixir

  This module handles markov chain generation and traversing
  """
    

  # Public API

  @doc """
  Generates a markov chain from a bitstring using `order` sized terms 

  Returns a markov transition table as a `Map`
  """
  @spec generate_chain(binary, number) :: {:ok, term} 
  def generate_chain(bin, order), 
    do: {:ok, Faust.Table.generate(bin, order)}

  @doc """
  Returns the resulting string from traversing a markov chain `n` times 

  If a vertex is not found at any point in traversal it will return a partial result
  """
  @spec traverse(term, number) :: {:ok, binary} | {:error, atom}
  def traverse(chain, n), 
    do: traverse(chain, n, :default)


  @doc """
  Returns the resulting string from traversing a markov chain `n` times. 

  Traversal is guided by a specified `method` which is function that decides
  which transitions to follow.

  If a vertex is not found at any point in traversal it will return a partial result
  """
  @spec traverse(term, number, fun) :: {:ok, binary} | {:error, atom}
  def traverse(chain, n, method) do
    cond do
      is_function(method, 1) -> 
        do_traverse(chain, n, method)
      method == :default -> 
        do_traverse(chain, n, &random/1)
      true -> 
        {:error, :bad_method}
    end
  end


  # Helpers

  defp random(edges) do
    x = :random.uniform
    inspect(edges)
    {edge, _} =Enum.min_by(edges, fn {_, prob} -> prob-x end)
    edge
  end

  defp do_traverse(chain, n, method) do
    start_seq = chain |> Map.keys |> Enum.random
    do_traverse(chain, n, method, start_seq, Enum.join(start_seq, " "))
  end
  defp do_traverse(_, 0, _, _, acc) do 
    {:ok, acc}
  end
  defp do_traverse(chain, n, method, seq, acc) do
    case chain[seq] do
      nil -> {:ok, acc}
      edges ->
        term = method.(edges)
        nseq = List.delete_at(seq, 0) ++ [term]
        do_traverse(chain, n-1, method, nseq, Enum.join([acc, term], " "))
    end
  end
end
