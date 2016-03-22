defmodule FaustTest do
  use ExUnit.Case

  test "generate_chain/2 order of 1" do
    text = lorem_ipsum
    {:ok, chain} = Faust.generate_chain(text, 1)
    assert chain == %{
      ["I"] => %{"do" => 1.0}, 
      ["In"] => %{"a" => 1.0}, 
      ["With"] => %{"a" => 1.0}, 
      ["Would"] => %{"you" => 1.0}, 
      ["a"] => %{"house." => 0.25, "house?" => 0.25, "mouse" => 0.25, "mouse?" => 0.25}, 
      ["do"] => %{"not" => 1.0}, 
      ["house."] => %{"I" => 1.0}, 
      ["house?"] => %{"Would" => 1.0}, 
      ["like"] => %{"them" => 1.0}, 
      ["mouse?"] => %{"I" => 1.0}, 
      ["not"] => %{"like" => 1.0}, 
      ["them"] => %{"In" => 0.5, "With" => 0.5}, 
      ["you"] => %{"like" => 1.0}
    }
  end

  test "generate_chain/2 order of 2" do
    text = lorem_ipsum
    {:ok, chain} = Faust.generate_chain(text, 2)
    assert chain == %{
      ["I", "do"] => %{"not" => 1.0}, 
      ["In", "a"] => %{"house." => 0.5, "house?" => 0.5}, 
      ["With", "a"] => %{"mouse" => 0.5, "mouse?" => 0.5}, 
      ["Would", "you"] => %{"like" => 1.0}, 
      ["a", "house."] => %{"I" => 1.0}, 
      ["a", "house?"] => %{"Would" => 1.0}, 
      ["a", "mouse?"] => %{"I" => 1.0}, 
      ["do", "not"] => %{"like" => 1.0}, 
      ["house.", "I"] => %{"do" => 1.0}, 
      ["house?", "Would"] => %{"you" => 1.0}, 
      ["like", "them"] => %{"In" => 0.5, "With" => 0.5}, 
      ["mouse?", "I"] => %{"do" => 1.0}, 
      ["not", "like"] => %{"them" => 1.0}, 
      ["them", "In"] => %{"a" => 1.0}, 
      ["them", "With"] => %{"a" => 1.0}, 
      ["you", "like"] => %{"them" => 1.0}
    }
  end

  test "generate_chain/2 order of 10" do
    text = lorem_ipsum
    {:ok, chain} = Faust.generate_chain(text, 10)
    assert chain == %{
      ["I", "do", "not", "like", "them", "In", "a", "house.", "I", "do"] => %{"not" => 1.0}, 
      ["In", "a", "house.", "I", "do", "not", "like", "them", "With", "a"] => %{"mouse" => 1.0},
      ["In", "a", "house?", "Would", "you", "like", "them", "With", "a", "mouse?"] => %{"I" => 1.0},
      ["With", "a", "mouse?", "I", "do", "not", "like", "them", "In", "a"] => %{"house." => 1.0},
      ["Would", "you", "like", "them", "In", "a", "house?", "Would", "you", "like"] => %{"them" => 1.0},
      ["Would", "you", "like", "them", "With", "a", "mouse?", "I", "do", "not"] => %{"like" => 1.0},
      ["a", "house?", "Would", "you", "like", "them", "With", "a", "mouse?", "I"] => %{"do" => 1.0},
      ["a", "mouse?", "I", "do", "not", "like", "them", "In", "a", "house."] => %{"I" => 1.0}, 
      ["do", "not", "like", "them", "In", "a", "house.", "I", "do", "not"] => %{"like" => 1.0},
      ["house?", "Would", "you", "like", "them", "With", "a", "mouse?", "I", "do"] => %{"not" => 1.0},
      ["like", "them", "In", "a", "house.", "I", "do", "not", "like", "them"] => %{"With" => 1.0},
      ["like", "them", "In", "a", "house?", "Would", "you", "like", "them", "With"] => %{"a" => 1.0},
      ["like", "them", "With", "a", "mouse?", "I", "do", "not", "like", "them"] => %{"In" => 1.0},
      ["mouse?", "I", "do", "not", "like", "them", "In", "a", "house.", "I"] => %{"do" => 1.0},
      ["not", "like", "them", "In", "a", "house.", "I", "do", "not", "like"] => %{"them" => 1.0},
      ["them", "In", "a", "house.", "I", "do", "not", "like", "them", "With"] => %{"a" => 1.0},
      ["them", "In", "a", "house?", "Would", "you", "like", "them", "With", "a"] => %{"mouse?" => 1.0},
      ["them", "With", "a", "mouse?", "I", "do", "not", "like", "them", "In"] => %{"a" => 1.0},
      ["you", "like", "them", "In", "a", "house?", "Would", "you", "like", "them"] => %{"With" => 1.0},
      ["you", "like", "them", "With", "a", "mouse?", "I", "do", "not", "like"] => %{"them" => 1.0}
    }
  end

  test "generate_chain/2 ignores newlines" do
    text = lorem_ipsum |> String.replace("  ", "\n")
    {:ok, chain} = Faust.generate_chain(text, 1)
    assert chain == %{
      ["In"] => %{"a" => 1.0}, 
      ["With"] => %{"a" => 1.0}, 
      ["Would"] => %{"you" => 1.0}, 
      ["a"] => %{"house. I" => 0.25, "house? Would" => 0.25, "mouse" => 0.25, "mouse? I" => 0.25}, 
      ["do"] => %{"not" => 1.0}, 
      ["house. I"] => %{"do" => 1.0}, 
      ["house? Would"] => %{"you" => 1.0}, 
      ["like"] => %{"them" => 1.0}, 
      ["mouse? I"] => %{"do" => 1.0}, 
      ["not"] => %{"like" => 1.0}, 
      ["them"] => %{"In" => 0.5, "With" => 0.5}, 
      ["you"] => %{"like" => 1.0}
    }
  end

  test "generate_chain/2 ignores carriage returns" do
    text = lorem_ipsum |> String.replace("  ", "\r\n")
    {:ok, chain} = Faust.generate_chain(text, 1)
    assert chain == %{
      ["In"] => %{"a" => 1.0}, 
      ["With"] => %{"a" => 1.0}, 
      ["Would"] => %{"you" => 1.0}, 
      ["a"] => %{"house. I" => 0.25, "house? Would" => 0.25, "mouse" => 0.25, "mouse? I" => 0.25}, 
      ["do"] => %{"not" => 1.0}, 
      ["house. I"] => %{"do" => 1.0}, 
      ["house? Would"] => %{"you" => 1.0}, 
      ["like"] => %{"them" => 1.0}, 
      ["mouse? I"] => %{"do" => 1.0}, 
      ["not"] => %{"like" => 1.0}, 
      ["them"] => %{"In" => 0.5, "With" => 0.5}, 
      ["you"] => %{"like" => 1.0}
    }
  end

  test "traverse/2" do
    text = lorem_ipsum
    {:ok, chain} = Faust.generate_chain(text, 1)
    assert {:ok, _} = Faust.traverse(chain, 10)
  end

  test "traverse/3" do
    text = lorem_ipsum
    {:ok, chain} = Faust.generate_chain(text, 1)
    assert {:ok, _} = Faust.traverse(chain, 10, fn edges -> edges |> Map.keys |> hd end)
  end

  test "traverse/3 bad method" do
    text = lorem_ipsum
    {:ok, chain} = Faust.generate_chain(text, 1)
    assert {:error, :bad_method} = Faust.traverse(chain, 10, fn edges, cats -> {edges, cats} end)
  end

  defp lorem_ipsum do
    "Would you like them In a house?  Would you like them With a mouse?  I do not like them In a house.  I do not like them With a mouse"  
  end
end
