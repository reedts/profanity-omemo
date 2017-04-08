# profanity-omemo

## Build
The following dependencies are needed to build the plugin:
* `cmake`
* `libxml2`
* `libgcrypt`
* `libsignal-protocol-c.a`

`libsignal-protocol-c.a` is linked to the plugin statically so it is not a runtime dependency and can be deleted after building
`profanity-omemo` whereas `libxml2` and `libgcrypt` is needed at runtime. The signal library will be built and linked with the
OMEMO plugin automatically and will not install anything to your system.

The plugin can be built with the following steps:
```
$ cd profanity-omemo
$ mkdir build
$ cd build
$ cmake (for debug purposes: -DCMAKE_BUILD_TYPE=Debug; enable testing: -DBUILD_TESTING=ON) ..
$ make
```
You can run the (for now minimal) test suite:
```
$ make test
```
which will show a summary of all run tests. You can also run a single test binary for more detailed informations.
