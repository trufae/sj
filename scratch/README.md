# SI

The *Small Interpreter* is a small, easy embeddable parser and
interpreter for a programming language that's simple, 

Some of the guidelines.

* Language is not yet defined, may change and improve over time
* Comments can be nested `/* foo /* bar */ */`
* Strings are interpolable, you can use `0x${var:x}` or `${var}`
* $var is not considered valid as it makes parsing much more complex
* You can escape any char `\{`, `\_`
* Numbers are ut64, no floating point arithmetics.
