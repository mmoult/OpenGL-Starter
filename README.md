# Setup Instructions
Install and compile GLFW and GLAD for your OS. (Try https://www.glfw.org/download.html
for GLFW, and https://glad.dav1d.de/ for GLAD.) Then, follow the steps
below to configure your project in some IDE (Visual Studio recommended).

## C++ Files
- Add glfw/include as an additional include directory
- Add glad/include as an additional include directory
- Add glad.c (which should be in glad/src) as an external source file

## Linking
- Add the following as additional library directories, where X is your version of glfw:
  + glfwX.dll
  + glfwX.lib
  + glfwX_mt.lib
  + glfwXdll.lib

  Alternatively, if you are using Visual Studio, add the folder in glfw corresponding to the version you are using.
- In input, add the following as additional dependencies:
  + glfwX.lib, where X is your version of glfw (for example, glfw3.lib)
  + opengl32.lib
  + user32.lib
  + gdi32.lib
  + shell32.lib
