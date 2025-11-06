Build & Run

- Using Make:

```
make -C @complier2
echo "int main(){return 0;}" | @complier2/your_compiler
```

- Using CMake:

```
cd @complier2
cmake -S . -B build
cmake --build build -j
echo "int main(){return 0;}" | build/your_compiler
```

I/O

- stdin: ToyC source code
- stdout: "accept" or "reject" then 1-based error line numbers, one per line


