#shader vertex
#version 330 core

layout(location = 0) in vec3 position;
layout(location = 1) in vec3 normal;
layout(location = 2) in vec2 texCoord; // Add texture coordinate attribute

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;

out vec3 Normal;  // Pass the normal to the fragment shader
out vec3 FragPos; // Pass the fragment position
out vec2 TexCoord; // Pass the texture coordinates to the fragment shader

void main()
{
    FragPos = vec3(model * vec4(position, 1.0));
    Normal = mat3(transpose(inverse(model))) * normal; // Transform normal to world coordinates
    TexCoord = texCoord; // Pass texture coordinates
    gl_Position = projection * view * vec4(FragPos, 1.0);
}

#shader fragment
#version 330 core

layout(location = 0) out vec4 color;

in vec3 Normal;  // Interpolated normal from the vertex shader
in vec3 FragPos; // Fragment position
in vec2 TexCoord; // Interpolated texture coordinates

// Uniforms for lighting
uniform vec3 lightPos;    // Light position (static)
uniform vec3 viewPos;     // Camera position (for specular calculation)
uniform vec3 lightColor;  // Light color
uniform sampler2D textureSampler; // Texture sampler

// New uniform for orbit color
uniform vec3 orbitColor; // Color of the orbit lines
uniform float orbitAlpha; // Alpha value for the orbit lines

// New uniform to determine if the fragment is part of an orbit line
uniform bool isOrbitLine; // Boolean to indicate if this fragment is an orbit line

// Emission properties
uniform vec3 emissionColor; // Color of the emission from the sun's surface
uniform float emissionStrength; // Strength of the emission effect

void main()
{
    // Ambient lighting
    float ambientStrength = 0.1;
    vec3 ambient = ambientStrength * lightColor;

    // Diffuse lighting
    vec3 norm = normalize(Normal);
    vec3 lightDir = normalize(lightPos - FragPos);
    float diff = max(dot(norm, lightDir), 0.0);
    vec3 diffuse = diff * lightColor;

    // Specular lighting
    float specularStrength = 0.5;
    vec3 viewDir = normalize(viewPos - FragPos);
    vec3 reflectDir = reflect(-lightDir, norm);
    float spec = pow(max(dot(viewDir, reflectDir), 0.0), 32);
    vec3 specular = specularStrength * spec * lightColor;

    // Combine results
    vec3 result = (ambient + diffuse + specular);

    // Fetch the texture color
    vec3 textureColor = texture(textureSampler, TexCoord).rgb;

    // Check if the fragment is part of an orbit line
    if (isOrbitLine) {
        // Use orbit color for orbit lines
        color = vec4(orbitColor, orbitAlpha); // Set the alpha value for opacity
    } else {
        // Apply emission effect
        vec3 emittedLight = emissionColor * emissionStrength; // Compute the emitted light
        color = vec4(result * textureColor + emittedLight, 1.0); // Combine with the existing color
    }
}