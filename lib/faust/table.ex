defmodule Faust.Table do
  @moduledoc false

  def generate(bin, order), 
    do: init(bin, order, [], << >>) 

  defp init(bin, 0, acc, _), 
    do: build(bin, {%{}, acc}, << >>)
  defp init(<< ?\s, rest :: bits >>, order, acc, term),
    do: init(trim(rest), order-1, [term|acc], << >>)
  defp init(<< ?\t, rest :: bits >>, order, acc, term),
    do: init(trim(rest), order-1, [term|acc], << >>)
  defp init(<< ?\r, rest :: bits >>, order, acc, term),
    do: init(trim(rest), order, acc, term)
  defp init(<< ?\n, rest :: bits >>, order, acc, << >>),
    do: init(trim(rest), order, acc, << >>)
  defp init(<< ?\n, rest :: bits >>, order, acc, term),
    do: init(trim(rest), order, acc, << term :: binary, " " >>)
  defp init(<< char, rest :: bits >>, order, acc, term),
    do: init(rest, order, acc, << term :: binary, char >>)

  defp build(<< >>, {table, _}, << >>), 
    do: table |> normalize 
  defp build(<< >>, s, term), 
    do: update(s, term) |> elem(0) |> normalize 
  defp build(<< ?\s, rest :: bits >>, s, term),
    do: build(trim(rest), update(s, term), << >>) 
  defp build(<< ?\t, rest :: bits >>, s, term),
    do: build(trim(rest), update(s, term), << >>) 
  defp build(<< ?\r, rest :: bits >>, s, term),
    do: build(trim(rest), s, term)
  defp build(<< ?\n, rest :: bits >>, s, << >>),
    do: build(trim(rest), s, << >>)
  defp build(<< ?\n, rest :: bits >>, s, term),
    do: build(trim(rest), s, << term :: binary, " " >>)
  defp build(<< char, rest :: bits >>, s, term),
    do: build(rest, s, << term :: binary, char >>)

  defp update({table, seq}, term) do
    table = 
      Map.update(table, Enum.reverse(seq), %{term => 1}, 
        fn tmap -> 
          Map.update(tmap, term, 1, fn val -> val+1 end) 
        end)
    {table, [term|List.delete_at(seq, -1)]}
  end

  defp normalize(table) do
    Enum.reduce(table, %{}, 
      fn {node, transitions}, acc ->
        sum = transitions |> Map.values |> Enum.sum
        Map.put(acc, node, Enum.reduce(transitions, %{},
          fn {transition, val}, tacc ->
            Map.put(tacc, transition, val/sum)
          end)
        )
      end)
  end

  defp trim(<< ?\s, rest :: bits >>), do: trim(rest)
  defp trim(<< ?\t, rest :: bits >>), do: trim(rest)
  defp trim(bin), do: bin 
end
