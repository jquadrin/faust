# Faust

[![Build Status](https://travis-ci.org/jquadrin/faust.svg?branch=master)](https://travis-ci.org/jquadrin/faust)

Markov chain text generator for Elixir.

Documentation: http://hexdocs.pm/faust/0.1.0

## Example
```iex
iex(1)> {:ok, text} = File.read("Franz_Kafka-Metamorphosis.txt")
...
iex(2)> lowercase_text = text |> String.downcase
...
iex(2)> {:ok, chain} = Faust.generate_chain(lowercase_text, 3)
...
iex(3)> Faust.traverse(chain, 10)
{:ok,
 "time during the day, no-one came, and the keys were in the other room catching"}
iex(4)> Faust.traverse(chain, 50)
{:ok,
 "as possible for him by removing the furniture that got in his way, especially the chest of drawers and
the desk. now, this was not something for which gregor could be sacked on the spot. and it seemed to
gregor much more sensible to leave him now in peace instead of disturbing him with talking at him and"}
iex(5)> Faust.traverse(chain, 100)
{:ok,
 "from their cigarettes upwards from their mouth and noses. yet gregor's sister was playing so
beautifully. her face was leant to one side, following the lines of music with a careful and melancholy
expression. gregor crawled a little further forward, keeping his head close to the ground and broke; a
splinter cut gregor's face, some kind of action for damages from you, and believe me it would be better
if his mother came in, not every day of course, but one day a week, perhaps; she could understand
everything much better than his sister who, for all her courage, was still just a child after all, and
really might not have had an adult's"}
```

## License

Copyright 2016 Joe Quadrino

Licensed under the Apache License, Version 2.0 (the "License");
you may not use this file except in compliance with the License.
You may obtain a copy of the License at
```
    http://www.apache.org/licenses/LICENSE-2.0
```
Unless required by applicable law or agreed to in writing, software
distributed under the License is distributed on an "AS IS" BASIS,
WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
See the License for the specific language governing permissions and
limitations under the License.
