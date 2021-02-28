# Lemon-Cpp

## Building
```
# Generate Sources
cmake -S . -B build
# Build all targets
cmake --build build --parallel
```

## Running Tests
You'll need to do the generate sources step with `-DBUILD_TESTING=on`.
```
./build/test/lemon_tests
```