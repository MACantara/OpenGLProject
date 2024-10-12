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

// Define the window dimensions
const int WINDOW_WIDTH = 640, WINDOW_HEIGHT = 480;
const std::string WINDOW_TITLE = "Lighting of Cube and Sphere";

// Define the mathematical constants
const float M_PI = 3.14159265358979323846f;
const float M_PI_2 = M_PI / 2.0f;

// Define the text instruction parameters
const int TEXT_INSTRUCTION_WIDTH = 250;
const int TEXT_INSTRUCTION_HEIGHT = 100;
const int TEXT_INSTRUCTION_LEFT_MARGIN = 10;
const int TEXT_INSTRUCTION_POS_X = (WINDOW_WIDTH - TEXT_INSTRUCTION_WIDTH) - TEXT_INSTRUCTION_LEFT_MARGIN;
const int TEXT_INSTRUCTION_POS_Y = 10;

// Define the orbit and rotation speed slider parameters
const int SLIDER_POS_X = 10;
const int SLIDER_POS_Y = 200;

// Define the camera parameters
glm::vec3 initialCameraPos = glm::vec3(0.75f, 1.0f, 3.0f);  // Adjusted camera position (closer and above the sphere)
float initialYaw = -90.0f;  // Default yaw value (this can remain the same)
float initialPitch = -20.0f;  // Adjusted pitch to look down at the sphere
glm::vec3 initialCameraFront = glm::vec3(0.0f, -0.2f, -1.0f);  // Slightly adjusted front direction to look down at the sphere

glm::vec3 cameraPos = glm::vec3(0.75f, 1.0f, 3.0f);  // Updated position
glm::vec3 cameraFront = glm::vec3(0.0f, -0.2f, -1.0f);  // Updated front direction
glm::vec3 cameraUp = glm::vec3(0.0f, 1.0f, 0.0f);  // Up vector remains unchanged
float cameraSpeed = 0.1f;  // Adjust this speed as needed

float cameraYaw = -90.0f;  // Yaw is initialized to -90.0 degrees (pointing towards negative z-axis)
float cameraPitch = 0.0f;  // Pitch is initialized to 0.0 degrees
float lastX = WINDOW_WIDTH / 2.0f;  // Last x-coordinate of the mouse
float lastY = WINDOW_HEIGHT / 2.0f; // Last y-coordinate of the mouse
bool firstMouse = true;  // Flag to ignore the first mouse movement
bool cameraMovementEnabled = true; // Camera movement toggle

// Define the mouse sensitivity
float mouseSensitivity = 0.1f;

// Define an array to hold texture IDs
std::array<unsigned int, 9> textureIds;

// Define the paths and scale factors for the textures
const std::array<std::string, 9> texturePaths = {
    "textures/sun.jpg",
    "textures/mercury.jpg",
    "textures/venus.jpg",
    "textures/earth.jpg",
    "textures/mars.jpg",
    "textures/jupiter.jpg",
    "textures/saturn.jpg",
    "textures/uranus.jpg",
    "textures/neptune.jpg"
};

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
    10.0f, // Jupiter
    12.0f, // Saturn
    14.0f, // Uranus
    16.0f  // Neptune
};

// Orbital speeds for each planet (in radians per second)
const std::array<float, 9> originalAngularVelocities = {
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
const std::array<float, 9> originalRotationSpeeds = {
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

// Variables to hold current speeds
std::array<float, 9> angularVelocities;
std::array<float, 9> rotationSpeeds;

// Scale factor (default to 1.0)
float scaleFactor = 1.0f;

// Function to update speeds based on scale factor
void UpdateSpeeds() {
    for (size_t i = 0; i < 9; ++i) {
        angularVelocities[i] = originalAngularVelocities[i] * scaleFactor;
        rotationSpeeds[i] = originalRotationSpeeds[i] * scaleFactor;
    }
}

// Global time variable
float deltaTime = 0.0f; // Time between frames
float lastFrame = 0.0f; // Time of the last frame

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

void framebuffer_size_callback(GLFWwindow* window, int width, int height)
{
    // Adjust the viewport based on the new window dimensions
    glViewport(0, 0, width, height);
}

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

void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods) {
    if (glfwGetKey(window, GLFW_KEY_C) == GLFW_PRESS) {
        cameraMovementEnabled = !cameraMovementEnabled; // Toggle camera movement
        if (cameraMovementEnabled) {
            glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED); // Hide cursor
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

    for (size_t i = 0; i < textureIds.size(); ++i) {
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
};

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

    /* Loop until the user closes the window */
    while (!glfwWindowShouldClose(window))
    {
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

        // Process input
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

        // Define a static position for the light source
        glm::vec3 lightPos = glm::vec3(0.0f, 0.0f, 0.0f);

        // Set up the locations of the shader uniforms
        int lightPosLoc = glGetUniformLocation(shader, "lightPos");
        int viewPosLoc = glGetUniformLocation(shader, "viewPos");
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
        glUniform3fv(lightPosLoc, 1, glm::value_ptr(lightPos));
        glUniform3fv(viewPosLoc, 1, glm::value_ptr(cameraPos));
        glUniform3f(lightColorLoc, 1.0f, 1.0f, 1.0f); // White light
        glUniform3f(objectColorLoc, 0.5f, 0.1f, 0.3f); // Object color

        glActiveTexture(GL_TEXTURE0); // Activate texture unit 0

        glEnable(GL_LINE_SMOOTH);
        glLineWidth(1.0f); // Set line width if needed

        // Enable blending
        glEnable(GL_BLEND);
        glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

        // Set the alpha value for orbit lines
        glUniform1f(glGetUniformLocation(shader, "orbitAlpha"), 0.25f);

        // For orbit lines
        glUniform1i(glGetUniformLocation(shader, "isOrbitLine"), true);
        glUniform3f(glGetUniformLocation(shader, "orbitColor"), 1.0f, 1.0f, 1.0f); // Example: White color for orbit lines

        // Draw orbits for each planet
        for (int i = 1; i < positions.size(); i++) {  // Start from 1 to skip the Sun
            glColor3f(1.0f, 1.0f, 1.0f); // Set orbit color (white)
            drawOrbit(orbitalRadii[i], 100); // 100 segments for smoothness
        }

        // For textured objects
        glUniform1i(glGetUniformLocation(shader, "isOrbitLine"), false);
        renderSpheres(shader, modelLoc, sphereVao, sphereIndices);

        // Start the ImGui frame
        ImGui_ImplOpenGL3_NewFrame();
        ImGui_ImplGlfw_NewFrame();
        ImGui::NewFrame();

        // Get window size
        int width, height;
        glfwGetWindowSize(window, &width, &height);

        // Text Instructions Window
        ImGui::Begin("Instructions", nullptr, ImGuiWindowFlags_AlwaysAutoResize);
        // Set the position of the ImGui Instructions window
        ImGui::SetWindowPos(ImVec2(width - TEXT_INSTRUCTION_WIDTH - TEXT_INSTRUCTION_LEFT_MARGIN, TEXT_INSTRUCTION_POS_Y), ImGuiCond_Always);

        ImGui::Text("Use WASD to move around.");
        ImGui::Text("Press ESC to exit.");
		ImGui::Text("Press R to reset camera position.");
		ImGui::Text("Press C to toggle camera movement.");

        ImGui::End();

		// Camera control
        ImGui::Begin("Camera Control", nullptr, ImGuiWindowFlags_AlwaysAutoResize);
        ImGui::SliderFloat("Camera Speed", &cameraSpeed, 0.1f, 5.0f, "Speed: %.1f"); // Add a slider for camera speed
        ImGui::End(); // End the ImGui window

        // Mouse sensitivity control
        ImGui::Begin("Mouse Sensitivity", nullptr, ImGuiWindowFlags_AlwaysAutoResize);
        ImGui::SliderFloat("Mouse Sensitivity", &mouseSensitivity, 0.1f, 5.0f, "Speed: %.1f");
        ImGui::End();

        ImGui::Begin("Planet Orbit and Rotation Speed Control", nullptr, ImGuiWindowFlags_AlwaysAutoResize);

        // Set the position of the ImGui sliders window to the bottom left
        ImGui::SetWindowPos(ImVec2(SLIDER_POS_X, height - SLIDER_POS_Y), ImGuiCond_Always); 

        // Define the multiplier buttons
        if (ImGui::Button("1x")) {
            scaleFactor = 1.0f;
            UpdateSpeeds();
        }
        ImGui::SameLine(); // Keep buttons in the same line
        if (ImGui::Button("2x")) {
            scaleFactor = 2.0f;
            UpdateSpeeds();
        }
        ImGui::SameLine();
        if (ImGui::Button("3x")) {
            scaleFactor = 3.0f;
            UpdateSpeeds();
        }
        ImGui::SameLine();
        if (ImGui::Button("5x")) {
            scaleFactor = 5.0f;
            UpdateSpeeds();
        }
        ImGui::SameLine();
        if (ImGui::Button("10x")) {
            scaleFactor = 10.0f;
            UpdateSpeeds();
        }
        ImGui::SameLine();
        if (ImGui::Button("15x")) {
            scaleFactor = 15.0f;
            UpdateSpeeds();
        }
        ImGui::SameLine();
        if (ImGui::Button("20x")) {
            scaleFactor = 20.0f;
            UpdateSpeeds();
        }

        // Display current speeds
        for (size_t i = 1; i < 9; ++i) { // Start from 1 to skip the Sun
            ImGui::Text("Planet %zu: Orbital Speed: %.3f, Rotation Speed: %.3f",
                i,
                angularVelocities[i],
                rotationSpeeds[i]);
        }

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