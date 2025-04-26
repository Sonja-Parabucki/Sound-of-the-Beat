#include "setup.h"


unsigned int compileShader(GLenum type, const char* source) {
    std::string content = "";
    std::ifstream file(source);
    std::stringstream ss;
    if (file.is_open()) {
        ss << file.rdbuf();
        file.close();
        std::cout << "Successfully read file: \"" << source << "\"" << std::endl;
    }
    else {
        ss << "";
        std::cout << "Error reading file: \"" << source << "\"" << std::endl;
    }
    std::string temp = ss.str();
    const char* sourceCode = temp.c_str();
    int shader = glCreateShader(type);

    int success;
    char infoLog[512];
    glShaderSource(shader, 1, &sourceCode, NULL);
    glCompileShader(shader);

    glGetShaderiv(shader, GL_COMPILE_STATUS, &success);
    if (success == GL_FALSE) {
        glGetShaderInfoLog(shader, 512, NULL, infoLog);
        if (type == GL_VERTEX_SHADER)
            printf("VERTEX");
        else if (type == GL_FRAGMENT_SHADER)
            printf("FRAGMENT");
        printf(" shader error: \n");
        printf(infoLog);
    }
    return shader;
}


unsigned int createShader(const char* vsSource, const char* fsSource) {
    unsigned int program = glCreateProgram();
    unsigned int vertexShader = compileShader(GL_VERTEX_SHADER, vsSource);
    unsigned int fragmentShader = compileShader(GL_FRAGMENT_SHADER, fsSource);
    glAttachShader(program, vertexShader);
    glAttachShader(program, fragmentShader);
    glLinkProgram(program);
    glValidateProgram(program);

    int success;
    char infoLog[512];
    glGetProgramiv(program, GL_VALIDATE_STATUS, &success);
    if (success == GL_FALSE) {
        glGetShaderInfoLog(program, 512, NULL, infoLog);
        std::cout << "Error creating unified shader: \n";
        std::cout << infoLog << std::endl;
    }
    glDetachShader(program, vertexShader);
    glDeleteShader(vertexShader);
    glDetachShader(program, fragmentShader);
    glDeleteShader(fragmentShader);

    return program;
}



unsigned int loadImageToTexture(const char* filePath) {
    int TextureWidth, TextureHeight, TextureChannels;
    stbi_set_flip_vertically_on_load(1);

    std::string defaultPath = "resources/img/backup.png";
    unsigned char* ImageData = stbi_load(filePath, &TextureWidth, &TextureHeight, &TextureChannels, 0);
    if (ImageData == NULL) {
        std::cout << "Failed to load image " << filePath << std::endl;
        ImageData = stbi_load(defaultPath.c_str(), &TextureWidth, &TextureHeight, &TextureChannels, 0);
    }
    if (ImageData == NULL) {
        std::cout << "Error loading default texture: " << defaultPath << std::endl;
        stbi_image_free(ImageData);
        return 0;
    }

    GLint InternalFormat = -1;
    switch (TextureChannels) {
    case 1: InternalFormat = GL_RED; break;
    case 2: InternalFormat = GL_RG; break;
    case 3: InternalFormat = GL_RGB; break;
    case 4: InternalFormat = GL_RGBA; break;
    default: InternalFormat = GL_RGB; break;
    }

    unsigned int Texture;
    glGenTextures(1, &Texture);
    glBindTexture(GL_TEXTURE_2D, Texture);
    glTexImage2D(GL_TEXTURE_2D, 0, InternalFormat, TextureWidth, TextureHeight, 0, InternalFormat, GL_UNSIGNED_BYTE, ImageData);
        
    glGenerateMipmap(GL_TEXTURE_2D);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        
    glBindTexture(GL_TEXTURE_2D, 0);

    stbi_image_free(ImageData);
    return Texture;
}

unsigned int windowWidth() {
    return 1920;
}


unsigned int windowHeight() {
    return 1080;
}


void initVABO(const float* vertices, size_t verticesLength, unsigned int stride, unsigned int* VAO, unsigned int* VBO, bool staticDraw) {
    glGenVertexArrays(1, VAO);
    glBindVertexArray(*VAO);

    glGenBuffers(1, VBO);
    glBindBuffer(GL_ARRAY_BUFFER, *VBO);
    if (staticDraw)
        glBufferData(GL_ARRAY_BUFFER, verticesLength * sizeof(float), vertices, GL_STATIC_DRAW);
    else
        glBufferData(GL_ARRAY_BUFFER, verticesLength * sizeof(float), vertices, GL_DYNAMIC_DRAW);

    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, stride, (void*)0); //x, y
    glEnableVertexAttribArray(0);
    if (stride == 5 * sizeof(float)) {
        glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, stride, (void*)(3 * sizeof(float))); //texture (s, t)
        glEnableVertexAttribArray(1);
    }

    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);
}


void limitFPS(double renderStart) {
    double renderTime = glfwGetTime() - renderStart;
    if (renderTime < FRAME_TIME)
        std::this_thread::sleep_for(std::chrono::duration<double>(FRAME_TIME - renderTime));
}

void setColor(unsigned int shader, char color) {
    switch (color) {
    case 'r': {
        glUniform3f(glGetUniformLocation(shader, "material.ambient"), 0.01f, 0.01f, 0.01f);
        glUniform3f(glGetUniformLocation(shader, "material.diffuse"), 0.2f, 0.f, 0.f);
        glUniform3f(glGetUniformLocation(shader, "material.specular"), 0.7f, 0.6f, 0.6f);
        break;
    }
    case 'b': {
        glUniform3f(glGetUniformLocation(shader, "material.ambient"), 0.01f, 0.01f, 0.01f);
        glUniform3f(glGetUniformLocation(shader, "material.diffuse"), 0.14f, 0.34f, 0.0f);
        glUniform3f(glGetUniformLocation(shader, "material.specular"), 0.5f, 0.5f, 0.5f);
        break;
    }
    case 'g': {
        glUniform3f(glGetUniformLocation(shader, "material.ambient"), 0.0f, 0.0f, 0.0f);
        glUniform3f(glGetUniformLocation(shader, "material.diffuse"), 0.1f, 0.1f, 0.1f);
        glUniform3f(glGetUniformLocation(shader, "material.specular"), 0.33f, 0.33f, 0.35f);
        break;
    }
    default: {
        glUniform3f(glGetUniformLocation(shader, "material.ambient"), 0.01f, 0.01f, 0.01f);
        glUniform3f(glGetUniformLocation(shader, "material.diffuse"), 0.34f, 0.35f, 0.15f);
        glUniform3f(glGetUniformLocation(shader, "material.specular"), 0.2f, 0.2f, 0.2f);
    }
    }
}