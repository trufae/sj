# sj

The Unix Javascript Shell

--pancake

# Current status

* SJ is just a javascript api that is loaded on top of quickjs runtime
* Provides a shell repl and shell-scripting with javascript
* No plans to make a new language, maybe just an improved repl with api is enough

# Design ideas

Those are a list of ideas/proposals to implement or discuss in order to improve its usability.

This project is just a proof-of-concept, but may probably evolve into something useful over time.

## Properties

* Native JSON and 64bit number support
* Sync operations only, no async stuff
* Inspired by Posix Shell, Awk/Perl, V and Javascript
* Single and small executable, no external dependencies
* Written in C for portability reasons
* Designed to replace shellscripts in a safe and portable way
* Usable as a stdin/stdout stream processor
* Time,System,

## Differences with Javascript

* fn instead of 'function'
* `` is used to get the output of a shell command
* "" is used for interpolated scripts
* '' is used for plain strings
* # can be used for comments/preprocessor rules

## Preprocessor

These directives must be found at the begining of the line

* #include 
* # line comment
* #if 0 block 
* #else
* #endif
