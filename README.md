Faust
======


Markov Text Generator


#### Generate text
```elixir
Faust.generate("Faust_by_Goethe.txt", order: 3, n: 270)

# Arguments: path, Markov Chain order, number of words to return 

# Results are sent to the caller's pid as one word messages

```

##### NOTE
Currently experimental, each word is passed as an individual message

##### TODO
- Pass entire result as single message
- Newline stripping optional
- Default arguments 
- Tests

#### License
[MIT](http://opensource.org/licenses/MIT "MIT License") Copyright &copy; 2014 Joseph Quadrino
