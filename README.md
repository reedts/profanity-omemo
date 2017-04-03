# profanity-omemo

## Build
The following dependencies are needed to build the plugin:
* `cmake`
* `libxml2`
* `libsignal-protocol-c.a`

`libsignal-protocol-c.a` is linked to the plugin statically so it is not a runtime dependency and can be deleted after building `profanity-omemo` whereas `libxml2` is needed at runtime.

The plugin can be built with the following steps:
```
cd profanity-omemo
mkdir build
cd build
cmake (for debug purposes: -DCMAKE_BUILD_TYPE=Debug) ..
make
```
You can run the (for now minimal test suite):
```
lib/test_device_list
```
