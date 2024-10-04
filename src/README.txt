Processes needed to set up an OpenGL Project

1. Clone this project

2. Change the configuration to All Configurations

3. Set up the include and library directories:
	- Right click on the project -> Properties -> C/C++ -> General -> Additional Include Directories
		- Add the following includes directories: 
			-$(SolutionDir)Dependencies\GLFW\include;$(SolutionDir)Dependencies\GLEW\include;$(SolutionDir)Dependencies\GLM;$(SolutionDir)Dependencies\;
	- Right click on the project -> Properties -> Linker -> General -> Additional Library Directories
		- Add the following library directories: $(SolutionDir)Dependencies\GLEW\lib\Release\x64;$(SolutionDir)Dependencies\GLFW\lib-vc2022
	- Right click on the project -> Properties -> Linker -> Input -> Additional Dependencies
		- Add the following dependencies: glew32s.lib;glfw3.lib;opengl32.lib;User32.lib;Gdi32.lib;Shell32.lib;glu32.lib;soil2-debug.lib

4. Set up the preprocessor definitions:
	- Right click on the project -> Properties -> C/C++ -> Preprocessor -> Preprocessor Definitions
		- Add the following definitions: GLEW_STATIC;%(PreprocessorDefinitions)

5. Build

6. Run

7. Enjoy!