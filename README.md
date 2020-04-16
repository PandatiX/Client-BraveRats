# Client BraveRats
Client BraveRats is a C/C++11 client prototype to play BraveRats.

## Dependencies
- [GLFW][1]
- [GLAD][2]
- [JSON for Modern C++][3]

[1]: https://github.com/glfw/glfw
[2]: https://github.com/Dav1dde/glad
[3]: https://github.com/nlohmann/json

## How to compile
After cloning the repo, run:
```shell
pushd <repoDir>
cmake -S . -B cmake-build-debug
pushd cmake-build-debug
```
Execute `./ClientBraveRats` to run the program.
