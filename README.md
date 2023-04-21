# sj

The Shell is

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
