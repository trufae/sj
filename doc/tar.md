# tar

compressing files requires

* output
* files / directories
* gzip/bzip2/..

```js
dump('ls.tgz', tar.gzip('/bin/ls'));
```

