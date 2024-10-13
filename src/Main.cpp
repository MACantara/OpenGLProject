#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <SOIL2/SOIL2.h>
#include "imgui.h"
#include "backends/imgui_impl_glfw.h"   // ImGui GLFW backend
#include "backends/imgui_impl_opengl3.h"   // ImGui OpenGL3 backend

#include <iostream>
#include <fstream>
#include <string>
#include <sstream>
#include <vector>
#include <array>
#include <cstdlib> // For rand() and srand()
#include <ctime>   // For time()

// Define the window dimensions
const int WINDOW_WIDTH = 800, WINDOW_HEIGHT = 600;
const std::string WINDOW_TITLE = "Lighting of Cube and Sphere";

// Define the mathematical constants
const float M_PI = 3.14159265358979323846f;
const float M_PI_2 = M_PI / 2.0f;

// Define the text instruction position
const int TEXT_INSTRUCTION_WIDTH = 250;
const int TEXT_INSTRUCTION_HEIGHT = 100;
const int TEXT_INSTRUCTION_LEFT_MARGIN = 10;
const int TEXT_INSTRUCTION_POS_X = (WINDOW_WIDTH - TEXT_INSTRUCTION_WIDTH) - TEXT_INSTRUCTION_LEFT_MARGIN;
const int TEXT_INSTRUCTION_POS_Y = 10;

// Define the camera position window parameters
const float CAMERA_PARAMETERS_MARGIN_TOP = 20.0f;
const float CAMERA_PARAMETERS_MARGIN_RIGHT = 95.0f;

// Define the initial camera position
glm::vec3 initialCameraPos = glm::vec3(-27.55f, 11.88f, 5.53f);
float initialYaw = -5.10f;
float initialPitch = -25.50f;
glm::vec3 initialCameraFront = glm::normalize(glm::vec3(0.90f, -0.43f, -0.08f));

// Define the current camera position
glm::vec3 cameraPos = glm::vec3(-27.55f, 11.88f, 5.53f);
float cameraYaw = -5.10f;
float cameraPitch = -25.50f;
glm::vec3 cameraFront = glm::normalize(glm::vec3(0.90f, -0.43f, -0.08f));

// Define the camera up vector and speed
glm::vec3 cameraUp = glm::vec3(0.0f, 1.0f, 0.0f);
float cameraSpeed = 0.1f; 

// Define the mouse sensitivity
float lastX = WINDOW_WIDTH / 2.0f;  // Last x-coordinate of the mouse
float lastY = WINDOW_HEIGHT / 2.0f; // Last y-coordinate of the mouse
bool firstMouse = true;  // Flag to ignore the first mouse movement
bool cameraMovementEnabled = true; // Camera movement toggle

// Define the mouse sensitivity
float mouseSensitivity = 0.1f;

// Define an array to hold texture IDs
std::array<unsigned int, 12> textureIds;

// Define the paths and scale factors for the textures
const std::array<std::string, 12> texturePaths = {
    "textures/sun.jpg",
    "textures/mercury.jpg",
    "textures/venus.jpg",
    "textures/earth.jpg",
    "textures/mars.jpg",
    "textures/jupiter.jpg",
    "textures/saturn.jpg",
    "textures/uranus.jpg",
    "textures/neptune.jpg",
    "textures/asteroid.jpg",
    "textures/moon.jpg",
    "textures/saturn_ring.png"
};

// Define the scales for each planet
const std::array<float, 9> scales = {
    2.0f, // Sun
    0.2f, // Mercury
    0.45f, // Venus
    0.5f, // Earth
    0.5f, // Mars
    1.5f, // Jupiter
    1.0f, // Saturn
    1.0f, // Uranus
    1.0f  // Neptune
};

// Define the positions for each planet
const std::array<float, 9> positions = {
    0.0f,  // Sun
    2.0f,  // Mercury
    4.0f,  // Venus
    6.0f,  // Earth
    8.0f,  // Mars
    10.0f, // Jupiter
    12.0f, // Saturn
    14.0f, // Uranus
    16.0f  // Neptune
};

// Orbital radii (distance from the Sun) for each planet
const std::array<float, 9> orbitalRadii = {
    0.0f,  // Sun has no orbit
    2.0f,  // Mercury
    4.0f,  // Venus
    6.0f,  // Earth
    8.0f,  // Mars
    14.0f, // Jupiter (adjusted to be beyond the asteroid belt)
    16.0f, // Saturn
    18.0f, // Uranus
    20.0f  // Neptune
};

// Orbital speeds for each planet (in radians per second)
const std::array<float, 9> angularVelocities = {
    0.0f,    // Sun
    0.033f,  // Mercury
    0.023f,  // Venus
    0.017f,  // Earth
    0.013f,  // Mars
    0.01f,   // Jupiter
    0.007f,  // Saturn
    0.004f,  // Uranus
    0.003f   // Neptune
};

// Rotation speeds for each planet (in radians per second)
const std::array<float, 9> rotationSpeeds = {
    0.0f,    // Sun
    0.25f,   // Mercury
    0.125f,  // Venus
    0.15f,   // Earth
    0.1f,    // Mars
    0.075f,  // Jupiter
    0.05f,   // Saturn
    0.025f,  // Uranus
    0.0225f  // Neptune
};

// Global time variable
float deltaTime = 0.0f; // Time between frames
float lastFrame = 0.0f; // Time of the last frame

// Define constants for the asteroid belt
const int NUM_ASTEROIDS = 10000;
const float ASTEROID_MIN_RADIUS = 0.001f;
const float ASTEROID_MAX_RADIUS = 0.030f; 
const float BELT_INNER_RADIUS = 10.0f; // Between Mars (8.0f) and Jupiter (14.0f)
const float BELT_OUTER_RADIUS = 12.0f;
const float MIN_ROTATION_SPEED = 0.0001f;
const float MAX_ROTATION_SPEED = 0.001f;

// Moon parameters
const float moonScale = 0.15f;
const float moonOrbitRadius = 0.75f; // Distance from Earth
const float moonOrbitSpeed = 0.05f; // Speed of orbit around Earth

// Constants for Saturn's ring
const int NUM_RING_ASTEROIDS = 5000; // Number of small asteroids in the ring
const float RING_INNER_RADIUS = 0.75f; // Inner radius of the ring
const float RING_OUTER_RADIUS = 1.0f; // Outer radius of the ring
const float RING_ASTEROID_MIN_RADIUS = 0.001f; // Minimum radius of the asteroids
const float RING_ASTEROID_MAX_RADIUS = 0.010f; // Maximum radius of the asteroids

// Load texture function
GLuint loadTexture(const char* filePath) {
    GLuint textureID;
    glGenTextures(1, &textureID);
    glBindTexture(GL_TEXTURE_2D, textureID);

    // Load the texture using SOIL
    int width, height;
    unsigned char* image = SOIL_load_image(filePath, &width, &height, 0, SOIL_LOAD_RGB);

    if (image == nullptr) {
        std::cerr << "Failed to load texture: " << filePath << std::endl;
        return 0;
    }

    // Set texture parameters
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    // Upload the texture to GPU
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, image);
    glGenerateMipmap(GL_TEXTURE_2D);

    // Free the image memory
    SOIL_free_image_data(image);

    return textureID;
}

// Function to handle window resizing
void framebuffer_size_callback(GLFWwindow* window, int width, int height)
{
    // Adjust the viewport based on the new window dimensions
    glViewport(0, 0, width, height);
}

// Function to parse shader files
struct ShaderProgramSource
{
    std::string VertexSource;
    std::string FragmentSource;
};

static ShaderProgramSource ParseShader(const std::string& filepath)
{
    std::ifstream stream(filepath);

    enum class ShaderType
    {
        NONE = -1, VERTEX = 0, FRAGMENT = 1
    };

    std::string line;
    std::stringstream ss[2];
	ShaderType type = ShaderType::NONE;
    while (getline(stream, line))
    {
        if (line.find("#shader") != std::string::npos)
        {
            if (line.find("vertex") != std::string::npos)
            {
                type = ShaderType::VERTEX;
            } 
            else if (line.find("fragment") != std::string::npos)
            {
                type = ShaderType::FRAGMENT;
            }
        }
        else
        {
            ss[(int)type] << line << "\n";
        }
    }

    return { ss[0].str(), ss[1].str() };
}

// Function to compile shaders
static unsigned int CompileShader(unsigned int type, const std::string& source)
{
    unsigned int id = glCreateShader(type);
    const char* src = source.c_str();
    glShaderSource(id, 1, &src, nullptr);
	glCompileShader(id);

	int result;
    glGetShaderiv(id, GL_COMPILE_STATUS, &result);
    if (result == GL_FALSE)
    {
        int length;
		glGetShaderiv(id, GL_INFO_LOG_LENGTH, &length);
        char* message = (char*)alloca(length * sizeof(char));
        glGetShaderInfoLog(id, length, &length, message);
        std::cout << "Failed to compile " << ( type == GL_VERTEX_SHADER ? "vertex" : "fragment") << " shader!" << std::endl;
        std::cout << message << std::endl;
		glDeleteShader(id);
		return 0;
    }

    return id;
}

// Function to create a shader program
static unsigned int CreateShader(const std::string& vertexShader, const std::string& fragmentShader)
{
    unsigned int program = glCreateProgram();
    unsigned int vs = CompileShader(GL_VERTEX_SHADER, vertexShader);
	unsigned int fs = CompileShader(GL_FRAGMENT_SHADER, fragmentShader); 

    glAttachShader(program, vs);
	glAttachShader(program, fs);
	glLinkProgram(program);
	glValidateProgram(program);

	glDeleteShader(vs);
	glDeleteShader(fs);

	return program;
}

// Function to generate sphere vertices and indices
void generateSphere(float radius, unsigned int rings, unsigned int sectors, std::vector<float>& vertices, std::vector<unsigned int>& indices) {
    float const R = 1.0f / (float)(rings - 1);
    float const S = 1.0f / (float)(sectors - 1);
    vertices.clear();
    indices.clear();

    // Vertex generation
    for (unsigned int r = 0; r < rings; r++) {
        for (unsigned int s = 0; s < sectors; s++) {
            float const y = sin(-M_PI_2 + M_PI * r * R);
            float const x = cos(2 * M_PI * s * S) * sin(M_PI * r * R);
            float const z = sin(2 * M_PI * s * S) * sin(M_PI * r * R);

            // Vertex position
            vertices.push_back(x * radius); // x
            vertices.push_back(y * radius); // y
            vertices.push_back(z * radius); // z

            // Normal
            vertices.push_back(x);           // nx
            vertices.push_back(y);           // ny
            vertices.push_back(z);           // nz

            // Texture coordinates (u, v)
            float u = s * S;
            float v = r * R;
            vertices.push_back(u);           // u
            vertices.push_back(v);           // v
        }
    }

    // Index generation
    for (unsigned int r = 0; r < rings - 1; r++) {
        for (unsigned int s = 0; s < sectors - 1; s++) {
            unsigned int first = r * sectors + s;
            unsigned int second = r * sectors + (s + 1);
            unsigned int third = (r + 1) * sectors + (s + 1);
            unsigned int fourth = (r + 1) * sectors + s;

            // Two triangles per quad
            indices.push_back(first);
            indices.push_back(second);
            indices.push_back(third);
            indices.push_back(third);
            indices.push_back(fourth);
            indices.push_back(first);
        }
    }
}

// Function to handle mouse movement
void mouse_callback(GLFWwindow* window, double xpos, double ypos) {
    if (!cameraMovementEnabled) return; // Do not update if movement is disabled

    if (firstMouse) {
        lastX = xpos;
        lastY = ypos;
        firstMouse = false;
    }

    float xoffset = xpos - lastX;
    float yoffset = lastY - ypos;
    lastX = xpos;
    lastY = ypos;

    xoffset *= mouseSensitivity;
    yoffset *= mouseSensitivity;

    cameraYaw += xoffset;
    cameraPitch += yoffset;

    // Constrain pitch
    if (cameraPitch > 89.0f) cameraPitch = 89.0f;
    if (cameraPitch < -89.0f) cameraPitch = -89.0f;

    // Update camera front vector
    glm::vec3 front;
    front.x = cos(glm::radians(cameraYaw)) * cos(glm::radians(cameraPitch));
    front.y = sin(glm::radians(cameraPitch));
    front.z = sin(glm::radians(cameraYaw)) * cos(glm::radians(cameraPitch));
    cameraFront = glm::normalize(front);
}

// Function to handle key presses
void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods) {
    if (glfwGetKey(window, GLFW_KEY_C) == GLFW_PRESS) {
        cameraMovementEnabled = !cameraMovementEnabled; // Toggle camera movement
        if (cameraMovementEnabled) {
            glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED); // Hide cursor
            firstMouse = true; // Reset the firstMouse flag
        }
        else {
            glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_NORMAL); // Show cursor
        }
    }
}

// Function to load textures
void loadTextures() {
    for (size_t i = 0; i < texturePaths.size(); ++i) {
        textureIds[i] = SOIL_load_OGL_texture(
            texturePaths[i].c_str(),
            SOIL_LOAD_AUTO,
            SOIL_CREATE_NEW_ID,
            SOIL_FLAG_INVERT_Y | SOIL_FLAG_TEXTURE_REPEATS
        );

        if (textureIds[i] == 0) {
            std::cerr << "Failed to load texture: " << texturePaths[i] << std::endl;
        };
    };
};

// Function to render spheres
void renderSpheres(GLuint shader, GLuint modelLoc, GLuint sphereVao, const std::vector<unsigned int>& sphereIndices) {
    float currentTime = glfwGetTime();

    for (size_t i = 0; i < textureIds.size() - 1; ++i) { // Exclude moon for now
        glBindTexture(GL_TEXTURE_2D, textureIds[i]); // Bind the current texture

        // Calculate angle and position for orbiting planets
        float angle = angularVelocities[i] * currentTime; // Calculate angle based on orbital speed
        float x = orbitalRadii[i] * cos(angle); // X position based on angle
        float z = orbitalRadii[i] * sin(angle); // Z position based on angle

        // Create the model matrix for the current planet
        glm::mat4 model = glm::translate(glm::mat4(1.0f), glm::vec3(x, 0.0f, z)); // Position based on orbit
        model = glm::scale(model, glm::vec3(scales[i])); // Scale the planet

        // Calculate rotation based on time
        float rotationAngle = rotationSpeeds[i] * currentTime; // Rotation angle based on rotation speed
        model = glm::rotate(model, rotationAngle, glm::vec3(0.0f, 1.0f, 0.0f)); // Rotate around Y axis

        glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model)); // Send the model matrix to the shader

        glUniform1i(glGetUniformLocation(shader, "textureSampler"), 0); // Assuming your shader uses "textureSampler"

        glBindVertexArray(sphereVao); // Use the same VAO for sphere geometry
        glDrawElements(GL_TRIANGLES, sphereIndices.size(), GL_UNSIGNED_INT, 0); // Draw the sphere

        glBindVertexArray(0);
        glBindTexture(GL_TEXTURE_2D, 0); // Unbind the texture
    }

    // Render the moon orbiting Earth
    glBindTexture(GL_TEXTURE_2D, textureIds[9]); // Bind the moon texture

    // Calculate Earth's position
    float earthAngle = angularVelocities[3] * currentTime;
    float earthX = orbitalRadii[3] * cos(earthAngle);
    float earthZ = orbitalRadii[3] * sin(earthAngle);

    // Calculate Moon's position relative to Earth
    float moonAngle = moonOrbitSpeed * currentTime;
    float moonX = earthX + moonOrbitRadius * cos(moonAngle);
    float moonZ = earthZ + moonOrbitRadius * sin(moonAngle);

    // Create the model matrix for the moon
    glm::mat4 moonModel = glm::translate(glm::mat4(1.0f), glm::vec3(moonX, 0.0f, moonZ));
    moonModel = glm::scale(moonModel, glm::vec3(moonScale));

    glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(moonModel)); // Send the model matrix to the shader

    glBindVertexArray(sphereVao);
    glDrawElements(GL_TRIANGLES, sphereIndices.size(), GL_UNSIGNED_INT, 0);

    glBindVertexArray(0);
    glBindTexture(GL_TEXTURE_2D, 0);
};

// Function to draw orbit lines
void drawOrbit(float radius, int segments) {
    glBegin(GL_LINE_STRIP);
    for (int i = 0; i <= segments; i++) {
        float theta = 2.0f * M_PI * float(i) / float(segments);
        float x = radius * cosf(theta);
        float z = radius * sinf(theta);
        glVertex3f(x, 0.0f, z);
    }
    glEnd();
};

// Function to draw the moon's orbit around the Earth
void drawMoonOrbit(float earthX, float earthZ, float radius, int segments) {
    glBegin(GL_LINE_STRIP);
    for (int i = 0; i <= segments; i++) {
        float theta = 2.0f * M_PI * float(i) / float(segments);
        float x = earthX + radius * cosf(theta);
        float z = earthZ + radius * sinf(theta);
        glVertex3f(x, 0.0f, z);
    }
    glEnd();
}

// Function to generate random float between min and max
float randomFloat(float min, float max) {
    return min + static_cast<float>(rand()) / (static_cast<float>(RAND_MAX / (max - min)));
}

// Function to generate asteroid data
void generateAsteroids(std::vector<glm::vec3>& positions, std::vector<float>& sizes, std::vector<float>& rotationSpeeds) {
    srand(static_cast<unsigned int>(time(0))); // Seed for random number generation

    for (int i = 0; i < NUM_ASTEROIDS; ++i) {
        float angle = randomFloat(0.0f, 2.0f * M_PI);
        float distance = randomFloat(BELT_INNER_RADIUS, BELT_OUTER_RADIUS);
        float x = distance * cos(angle);
        float z = distance * sin(angle);
        float y = randomFloat(-0.5f, 0.5f); // Small vertical variation

        positions.push_back(glm::vec3(x, y, z));
        sizes.push_back(randomFloat(ASTEROID_MIN_RADIUS, ASTEROID_MAX_RADIUS));
        rotationSpeeds.push_back(randomFloat(MIN_ROTATION_SPEED, MAX_ROTATION_SPEED));
    }
}

// Function to update asteroid positions
void updateAsteroids(std::vector<glm::vec3>& positions, const std::vector<float>& rotationSpeeds) {
    for (size_t i = 0; i < positions.size(); ++i) {
        float angle = atan2(positions[i].z, positions[i].x);
        float distance = glm::length(glm::vec2(positions[i].x, positions[i].z));
        angle += rotationSpeeds[i];

        positions[i].x = distance * cos(angle);
        positions[i].z = distance * sin(angle);
    }
}

// Function to render asteroids
void renderAsteroids(GLuint shader, GLuint modelLoc, GLuint sphereVao, const std::vector<unsigned int>& sphereIndices, GLuint asteroidTexture, const std::vector<glm::vec3>& positions, const std::vector<float>& sizes) {
    glBindTexture(GL_TEXTURE_2D, asteroidTexture);

    for (size_t i = 0; i < positions.size(); ++i) {
        glm::mat4 model = glm::translate(glm::mat4(1.0f), positions[i]);
        model = glm::scale(model, glm::vec3(sizes[i]));

        glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));

        glBindVertexArray(sphereVao);
        glDrawElements(GL_TRIANGLES, sphereIndices.size(), GL_UNSIGNED_INT, 0);
    }

    glBindVertexArray(0);
    glBindTexture(GL_TEXTURE_2D, 0);
}

// Function to generate asteroid positions for Saturn's ring
void generateRingAsteroids(std::vector<glm::vec3>& positions, std::vector<float>& sizes) {
    srand(static_cast<unsigned int>(time(0))); // Seed for random number generation

    for (int i = 0; i < NUM_RING_ASTEROIDS; ++i) {
        float angle = randomFloat(0.0f, 2.0f * M_PI);
        float distance = randomFloat(RING_INNER_RADIUS, RING_OUTER_RADIUS);
        float x = distance * cos(angle);
        float z = distance * sin(angle);
        float y = randomFloat(-0.01f, 0.01f); // Small vertical variation for thickness

        positions.push_back(glm::vec3(x, y, z));
        sizes.push_back(randomFloat(RING_ASTEROID_MIN_RADIUS, RING_ASTEROID_MAX_RADIUS));
    }
}

// Function to render Saturn's ring asteroids
void renderRingAsteroids(GLuint shader, GLuint modelLoc, GLuint sphereVao, const std::vector<unsigned int>& sphereIndices, GLuint asteroidTexture, const std::vector<glm::vec3>& positions, const std::vector<float>& sizes, float saturnX, float saturnZ) {
    glBindTexture(GL_TEXTURE_2D, asteroidTexture);

    for (size_t i = 0; i < positions.size(); ++i) {
        glm::mat4 model = glm::translate(glm::mat4(1.0f), glm::vec3(saturnX, 0.0f, saturnZ) + positions[i]);
        model = glm::scale(model, glm::vec3(sizes[i]));

        glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));

        glBindVertexArray(sphereVao);
        glDrawElements(GL_TRIANGLES, sphereIndices.size(), GL_UNSIGNED_INT, 0);
    }

    glBindVertexArray(0);
    glBindTexture(GL_TEXTURE_2D, 0);
}

int main(void)
{
    GLFWwindow* window;

    /* Initialize the library */
    if (!glfwInit())
        return -1;

    /* Create a windowed mode window and its OpenGL context */
    window = glfwCreateWindow(WINDOW_WIDTH, WINDOW_HEIGHT, WINDOW_TITLE.c_str(), NULL, NULL);
    if (!window)
    {
        glfwTerminate();
        return -1;
    }

    /* Make the window's context current */
    glfwMakeContextCurrent(window);
    glfwSwapInterval(1);

    if (glewInit() != GLEW_OK)
        std::cout << "Failed to initialize GLEW" << std::endl;

	std::cout << glGetString(GL_VERSION) << std::endl;

    // Set the window resize callback
    glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);

    // Set key callback function
    glfwSetKeyCallback(window, key_callback);

    // Set mouse callback function
    glfwSetCursorPosCallback(window, mouse_callback);

    // Hide the mouse cursor
    glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);

    // Setup initial viewport size
    glViewport(0, 0, WINDOW_WIDTH, WINDOW_HEIGHT);

    // Create sphere
    std::vector<float> sphereVertices;
    std::vector<unsigned int> sphereIndices;
    generateSphere(0.5f, 20, 20, sphereVertices, sphereIndices);

    unsigned int sphereVao, sphereVbo, sphereIbo;
    glGenVertexArrays(1, &sphereVao);
    glBindVertexArray(sphereVao);

    // Vertex Buffer Object
    glGenBuffers(1, &sphereVbo);
    glBindBuffer(GL_ARRAY_BUFFER, sphereVbo);
    glBufferData(GL_ARRAY_BUFFER, sphereVertices.size() * sizeof(float), sphereVertices.data(), GL_STATIC_DRAW);

    // Element Buffer Object
    glGenBuffers(1, &sphereIbo);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, sphereIbo);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sphereIndices.size() * sizeof(unsigned int), sphereIndices.data(), GL_STATIC_DRAW);

    // Position attribute
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(float) * 8, (const void*)0); // Position
    glEnableVertexAttribArray(0);

    // Normal attribute
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(float) * 8, (const void*)(sizeof(float) * 3)); // Normal
    glEnableVertexAttribArray(1);

    // Texture coordinates attribute
    glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(float) * 8, (const void*)(sizeof(float) * 6)); // Texture coordinates
    glEnableVertexAttribArray(2);

    // Unbind the VAO to avoid accidental modification
    glBindVertexArray(0);

    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(float) * 6, (void*)0); // Position
    glEnableVertexAttribArray(0);

    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(float) * 6, (void*)(sizeof(float) * 3)); // Normal
    glEnableVertexAttribArray(1);

	// Load shaders
    ShaderProgramSource source = ParseShader("res/shaders/Basic.shader");
    std::cout << "VERTEX SHADERS" << std::endl;
    std::cout << source.VertexSource << std::endl;
    std::cout << "FRAGMENT SHADERS" << std::endl;
	std::cout << source.FragmentSource << std::endl;

    unsigned int shader = CreateShader(source.VertexSource, source.FragmentSource);
    glUseProgram(shader);

    // Define the model matrices for the cube and the sphere
    glm::mat4 modelCube = glm::translate(glm::mat4(1.0f), glm::vec3(-0.75f, 0.0f, 0.0f)); // Move the cube to the left
    glm::mat4 modelSphere = glm::translate(glm::mat4(1.0f), glm::vec3(0.75f, 0.0f, 0.0f)); // Move the sphere to the right

    // Define the view and projection matrices
    // Use camera position and front vector for view matrix
    glm::mat4 view = glm::lookAt(cameraPos, cameraPos + cameraFront, cameraUp);

    glm::mat4 projection = glm::perspective(glm::radians(45.0f), (float)WINDOW_WIDTH / (float)WINDOW_HEIGHT, 0.1f, 100.0f);
    glm::mat4 model = glm::mat4(1.0f); // Identity matrix for the model

    // Get uniform locations
    int modelLoc = glGetUniformLocation(shader, "model");
    int viewLoc = glGetUniformLocation(shader, "view");
    int projLoc = glGetUniformLocation(shader, "projection");

    glEnable(GL_DEPTH_TEST);

    loadTextures();

    // Setup ImGui context
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGuiIO& io = ImGui::GetIO(); (void)io;

    // Setup Dear ImGui style
    ImGui::StyleColorsDark();

    // Initialize ImGui for GLFW and OpenGL3
    ImGui_ImplGlfw_InitForOpenGL(window, true);  // Your GLFW window
    ImGui_ImplOpenGL3_Init("#version 130");  // GLSL version (adjust as needed)

    // Load the asteroid texture
    GLuint asteroidTexture = loadTexture("textures/asteroid.jpg");

    // Generate asteroid data
    std::vector<glm::vec3> asteroidPositions;
    std::vector<float> asteroidSizes;
    std::vector<float> asteroidRotationSpeeds;
    generateAsteroids(asteroidPositions, asteroidSizes, asteroidRotationSpeeds);

    std::vector<glm::vec3> ringAsteroidPositions;
    std::vector<float> ringAsteroidSizes;
    generateRingAsteroids(ringAsteroidPositions, ringAsteroidSizes);

    /* Loop until the user closes the window */
    while (!glfwWindowShouldClose(window))
    {
        float currentTime = glfwGetTime();

        // Close window on pressing ESC
        if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
            glfwSetWindowShouldClose(window, true);

        // Reset camera on pressing 'R'
        if (glfwGetKey(window, GLFW_KEY_R) == GLFW_PRESS)
        {
            cameraPos = initialCameraPos;
            cameraYaw = initialYaw;
            cameraPitch = initialPitch;
            cameraFront = initialCameraFront;
        }

        // Keyboard input for camera movement
        float deltaTime = 0.1f;  // Can adjust or calculate based on actual time elapsed
        if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
            cameraPos += cameraSpeed * deltaTime * cameraFront;
        if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
            cameraPos -= cameraSpeed * deltaTime * cameraFront;
        if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
            cameraPos -= glm::normalize(glm::cross(cameraFront, cameraUp)) * cameraSpeed * deltaTime;
        if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
            cameraPos += glm::normalize(glm::cross(cameraFront, cameraUp)) * cameraSpeed * deltaTime;

        /* Render here */
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        // Set the uniform matrices
        glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));
        glUniformMatrix4fv(viewLoc, 1, GL_FALSE, glm::value_ptr(view));
        glUniformMatrix4fv(projLoc, 1, GL_FALSE, glm::value_ptr(projection));

        // Set up the locations of the shader uniforms
        int lightColorLoc = glGetUniformLocation(shader, "lightColor");
        int objectColorLoc = glGetUniformLocation(shader, "objectColor");

        // Camera/View transformation
        glm::mat4 view = glm::lookAt(cameraPos, cameraPos + cameraFront, cameraUp);
        glUniformMatrix4fv(viewLoc, 1, GL_FALSE, glm::value_ptr(view));

        glm::mat4 projection = glm::perspective(glm::radians(45.0f), (float)WINDOW_WIDTH / (float)WINDOW_HEIGHT, 0.1f, 100.0f);

        // Set the projection matrix in the shader
        unsigned int projectionLoc = glGetUniformLocation(shader, "projection");
        glUniformMatrix4fv(projectionLoc, 1, GL_FALSE, glm::value_ptr(projection));

        // Pass light and view data to the shader
        glUniform3f(lightColorLoc, 1.0f, 1.0f, 1.0f); // White light
        glUniform3f(objectColorLoc, 0.5f, 0.1f, 0.3f); // Object color

        glActiveTexture(GL_TEXTURE0); // Activate texture unit 0

        glEnable(GL_LINE_SMOOTH);
        glLineWidth(1.0f);

        // Enable blending
        glEnable(GL_BLEND);
        glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

        // Set the alpha value for orbit lines
        glUniform1f(glGetUniformLocation(shader, "orbitAlpha"), 0.25f);

        // For orbit lines
        glUniform1i(glGetUniformLocation(shader, "isOrbitLine"), true);
        glUniform3f(glGetUniformLocation(shader, "orbitColor"), 1.0f, 1.0f, 1.0f); // Example: White color for orbit lines

        // Rendering the sun
        glUniform1i(glGetUniformLocation(shader, "isSun"), true);

        // Set emission color to orange-ish yellow
        glm::vec3 emissionColor = glm::vec3(1.0f, 0.65f, 0.0f); // RGB values for orange-ish yellow
        float emissionStrength = 0.10f; // Adjust strength based on desired brightness

        int emissionColorLoc = glGetUniformLocation(shader, "emissionColor");
        int emissionStrengthLoc = glGetUniformLocation(shader, "emissionStrength");

        glUniform3fv(emissionColorLoc, 1, glm::value_ptr(emissionColor));
        glUniform1f(emissionStrengthLoc, emissionStrength);

        // Rendering the planets
        glUniform1i(glGetUniformLocation(shader, "isSun"), false);

        // Draw orbits for each planet
        for (int i = 1; i < positions.size(); i++) {  // Start from 1 to skip the Sun
            glColor3f(1.0f, 1.0f, 1.0f); // Set orbit color (white)
            drawOrbit(orbitalRadii[i], 100); // 100 segments for smoothness
        }

        // Calculate Earth's position
        float earthAngle = angularVelocities[3] * currentTime;
        float earthX = orbitalRadii[3] * cos(earthAngle);
        float earthZ = orbitalRadii[3] * sin(earthAngle);

        // Draw the moon's orbit around the Earth
        glColor3f(0.5f, 0.5f, 0.5f); // Set orbit color (gray)
        drawMoonOrbit(earthX, earthZ, moonOrbitRadius, 100); // 100 segments for smoothness

        // For textured objects
        glUniform1i(glGetUniformLocation(shader, "isOrbitLine"), false);
        renderSpheres(shader, modelLoc, sphereVao, sphereIndices);

        // Calculate Saturn's position
        float saturnAngle = angularVelocities[6] * currentTime;
        float saturnX = orbitalRadii[6] * cos(saturnAngle);
        float saturnZ = orbitalRadii[6] * sin(saturnAngle);

        renderRingAsteroids(shader, modelLoc, sphereVao, sphereIndices, asteroidTexture, ringAsteroidPositions, ringAsteroidSizes, saturnX, saturnZ);

        // Update asteroid positions
        updateAsteroids(asteroidPositions, asteroidRotationSpeeds);

        // Render the asteroid belt
        renderAsteroids(shader, modelLoc, sphereVao, sphereIndices, asteroidTexture, asteroidPositions, asteroidSizes);

        // Start the ImGui frame
        ImGui_ImplOpenGL3_NewFrame();
        ImGui_ImplGlfw_NewFrame();
        ImGui::NewFrame();

        // Get window size
        int width, height;
        glfwGetWindowSize(window, &width, &height);

        // Text Instructions Window
        ImGui::Begin("Instructions", nullptr, ImGuiWindowFlags_AlwaysAutoResize);
        ImGui::SetWindowPos(ImVec2(width - TEXT_INSTRUCTION_WIDTH - TEXT_INSTRUCTION_LEFT_MARGIN, TEXT_INSTRUCTION_POS_Y), ImGuiCond_Always);

        ImGui::Text("Use the mouse to look around.");
        ImGui::Text("Use WASD to move around.");
        ImGui::Text("Press ESC to exit.");
		ImGui::Text("Press R to reset camera position.");
		ImGui::Text("Press C to toggle camera movement.");

        ImGui::End();

        // Display camera position
        ImGui::Begin("Camera Positions", nullptr, ImGuiWindowFlags_AlwaysAutoResize);
        ImGui::SetWindowPos(ImVec2(width - TEXT_INSTRUCTION_WIDTH - CAMERA_PARAMETERS_MARGIN_RIGHT, TEXT_INSTRUCTION_POS_Y + ImGui::GetWindowHeight() + CAMERA_PARAMETERS_MARGIN_TOP), ImGuiCond_Always);

        ImGui::Text("Initial Camera Position: (%.2f, %.2f, %.2f)", cameraPos.x, cameraPos.y, cameraPos.z);
        ImGui::Text("Initial Yaw: %.2f", cameraYaw);
        ImGui::Text("Initial Pitch: %.2f", cameraPitch);
        ImGui::Text("Initial Camera Front: (%.2f, %.2f, %.2f)", cameraFront.x, cameraFront.y, cameraFront.z);

        ImGui::End();

		// Camera control
        ImGui::Begin("Camera Control", nullptr, ImGuiWindowFlags_AlwaysAutoResize);
        ImGui::SliderFloat("Camera Speed", &cameraSpeed, 0.1f, 5.0f, "Speed: %.1f");
        ImGui::End();

        // Mouse sensitivity control
        ImGui::Begin("Mouse Sensitivity", nullptr, ImGuiWindowFlags_AlwaysAutoResize);
        ImGui::SliderFloat("Mouse Sensitivity", &mouseSensitivity, 0.1f, 5.0f, "Speed: %.1f");
        ImGui::End();

        // Rendering ImGui
        ImGui::Render();
        ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

        // Swap buffers and poll IO
        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    glDeleteProgram(shader);
    glDeleteVertexArrays(1, &sphereVao);
    glDeleteBuffers(1, &sphereVbo);
    glDeleteBuffers(1, &sphereIbo);
    ImGui_ImplOpenGL3_Shutdown();
    ImGui_ImplGlfw_Shutdown();
    ImGui::DestroyContext();

    glfwTerminate();
    return 0;
}