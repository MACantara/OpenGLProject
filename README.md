# How to set up this OpenGL Project

1. Clone this Github repository

2. Use Visual Studio Community Edition 2022

3. Right click on the project -> Change the configuration to All Configurations

4. Set up the include and library directories:
	- Right click on the project -> Properties -> C/C++ -> General -> Additional Include Directories
		- Add the following include directories: 
			`$(SolutionDir)Dependencies\GLFW\include;$(SolutionDir)Dependencies\GLEW\include;$(SolutionDir)Dependencies\GLM;$(SolutionDir)Dependencies\;`
	- Right click on the project -> Properties -> Linker -> General -> Additional Library Directories
		- Add the following library directories:
   			`$(SolutionDir)Dependencies\GLEW\lib\Release\x64;$(SolutionDir)Dependencies\GLFW\lib-vc2022;`
	- Right click on the project -> Properties -> Linker -> Input -> Additional Dependencies
		- Add the following dependencies:
			`glew32s.lib;glfw3.lib;opengl32.lib;User32.lib;Gdi32.lib;Shell32.lib;glu32.lib;`
  	- Right click on the project -> Properties -> VC++ Directories -> Include Directories
   		- Add the following include directories:
     			`$(SolutionDir)Dependencies\ImGui\;$(SolutionDir)Dependencies\ImGui\backends;$(IncludePath)`

5. Set up the preprocessor definitions:
	- Right click on the project -> Properties -> C/C++ -> Preprocessor -> Preprocessor Definitions
		- Add the following definitions: 
			`GLEW_STATIC;%(PreprocessorDefinitions)`

6. Build

7. Run

8. Enjoy!
