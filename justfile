set ignore-comments

# compiler
cc := "gcc"
#compiler flags
cflags := "-Wall -Wextra"

# directory of the executable
bin_dir := "bin"
# library directory
lib_dir := bin_dir
# include directory
inc_dir := "include"
# source directory
src_dir := "src"

# source files
src := src_dir / "shader.c " + src_dir / "main.c"
# path of the executable
bin := bin_dir / "mandelbrot.exe"
# libraries
libs := "-lopengl32 -lglew32 -lglfw3"

# glfw-release type
glfw_type := "WIN64"
# glew-release type
glew_type := "win32"


run: build
    ./{{bin}}

# compile the sources and copy the shaders to the bin directory
build:
    {{cc}} {{cflags}} -I {{inc_dir}} {{src}} -o {{bin}} -L {{lib_dir}} {{libs}}
    cp -r shaders {{bin_dir}}

# remove old libraries download new ones and build the application
install: rmlibs getGLFW getGLEW build
    rm -rf tmp

# remove the libraries
rmlibs:
    rm -rf tmp {{bin_dir}}/*.dll include/GLFW include/GL

create_lib_dirs:
    mkdir -p tmp
    mkdir -p {{bin_dir}}

# download latest glfw release and move the includes/.dll into the include/bin directory
getGLFW: create_lib_dirs
    curl -s https://api.github.com/repos/glfw/glfw/releases/latest | jq -r ".assets[] | select(.name | test(\"{{glfw_type}}\")) | .browser_download_url" | wget -O tmp/glfw.zip -i - && unzip -d tmp tmp/glfw.zip
    mv tmp/glfw*/include/GLFW include/
    mv tmp/glfw*/lib-mingw-w64/glfw3.dll {{bin_dir}}

# download latest glew release and move the includes/.dll into the include/bin directory
getGLEW: create_lib_dirs
    curl -s https://api.github.com/repos/nigels-com/glew/releases/latest | jq -r ".assets[] | select(.name | test(\"{{glew_type}}\")) | .browser_download_url" | wget -O tmp/glew.zip -i - && unzip -d tmp tmp/glew.zip
    mv tmp/glew*/include/GL include/
    mv tmp/glew*/bin/Release/x64/glew32.dll {{bin_dir}}

# remove the executable and shaders
clean:
    rm -rf {{bin}} {{bin_dir}}/shaders/