#include "mkpch.h"
#include "opengl_backend.h"




#include <glad/glad.h>
#include <GLFW/glfw3.h>



//things i dont know what the perpose of it
void APIENTRY gl_debug_output(GLenum source, GLenum type, unsigned int id, GLenum severity, GLsizei /*length*/, const char* message, const void* /*userParam*/) {
    if (id == 131169 || id == 131185 || id == 131218 || id == 131204) return; // ignore these non-significant error codes
    std::cout << "---------------\n";
    std::cout << "Debug message (" << id << "): " << message << "\n";
    switch (source) {
    case GL_DEBUG_SOURCE_API:             std::cout << "Source: API"; break;
    case GL_DEBUG_SOURCE_WINDOW_SYSTEM:   std::cout << "Source: Window System"; break;
    case GL_DEBUG_SOURCE_SHADER_COMPILER: std::cout << "Source: Shader Compiler"; break;
    case GL_DEBUG_SOURCE_THIRD_PARTY:     std::cout << "Source: Third Party"; break;
    case GL_DEBUG_SOURCE_APPLICATION:     std::cout << "Source: Application"; break;
    case GL_DEBUG_SOURCE_OTHER:           std::cout << "Source: Other"; break;
    }
    std::cout << "\n";
    switch (type) {
    case GL_DEBUG_TYPE_ERROR:               std::cout << "Type: Error"; break;
    case GL_DEBUG_TYPE_DEPRECATED_BEHAVIOR: std::cout << "Type: Deprecated Behaviour"; break;
    case GL_DEBUG_TYPE_UNDEFINED_BEHAVIOR:  std::cout << "Type: Undefined Behaviour"; break;
    case GL_DEBUG_TYPE_PORTABILITY:         std::cout << "Type: Portability"; break;
    case GL_DEBUG_TYPE_PERFORMANCE:         std::cout << "Type: Performance"; break;
    case GL_DEBUG_TYPE_MARKER:              std::cout << "Type: Marker"; break;
    case GL_DEBUG_TYPE_PUSH_GROUP:          std::cout << "Type: Push Group"; break;
    case GL_DEBUG_TYPE_POP_GROUP:           std::cout << "Type: Pop Group"; break;
    case GL_DEBUG_TYPE_OTHER:               std::cout << "Type: Other"; break;
    }
    std::cout << "\n";
    switch (severity) {
    case GL_DEBUG_SEVERITY_HIGH:         std::cout << "Severity: high"; break;
    case GL_DEBUG_SEVERITY_MEDIUM:       std::cout << "Severity: medium"; break;
    case GL_DEBUG_SEVERITY_LOW:          std::cout << "Severity: low"; break;
    case GL_DEBUG_SEVERITY_NOTIFICATION: std::cout << "Severity: notification"; break;
    }    std::cout << "\n\n\n";
}

namespace opengl_backend
{

    GLuint _vertexDataVAO = 0;
    GLuint _vertexDataVBO = 0;
    GLuint _vertexDataEBO = 0;

    GLuint _weightedVertexDataVAO = 0;
    GLuint _weightedVertexDataVBO = 0;
    GLuint _weightedVertexDataEBO = 0;


    void init_minimum()
    {
        if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
            
            MK_CORE_CRITICAL("Failed to initialize GLAD");
            return;
        }
        GLint major, minor;
        glGetIntegerv(GL_MAJOR_VERSION, &major);
        glGetIntegerv(GL_MINOR_VERSION, &minor);
        const GLubyte* renderer = glGetString(GL_RENDERER);
        const GLubyte* vendor = glGetString(GL_VENDOR);
        MK_CORE_TRACE("GPU: {0}", reinterpret_cast<const char*>(renderer));
        MK_CORE_TRACE("Vendor: {}", reinterpret_cast<const char*>(vendor));
        MK_CORE_TRACE("GL version: {0}.{1}", major, minor);
        //std::cout << "\nGPU: " << renderer << "\n";
        //std::cout << "GL version: " << major << "." << minor << "\n\n";

        int flags;
        glGetIntegerv(GL_CONTEXT_FLAGS, &flags);
        if (flags & GL_CONTEXT_FLAG_DEBUG_BIT) {
            //std::cout << "Debug GL context enabled\n";
            glEnable(GL_DEBUG_OUTPUT);
            glEnable(GL_DEBUG_OUTPUT_SYNCHRONOUS); // makes sure errors are displayed synchronously
            glDebugMessageCallback(gl_debug_output, nullptr);
        }
        else {
            MK_CORE_ERROR("Debug GL context not available");
            //std::cout << "Debug GL context not available\n";
        }

        // Clear screen to black
        glClear(GL_COLOR_BUFFER_BIT);
    }
    void upload_vertex_data(std::vector<Vertex>& vertices, std::vector<unsigned int>& indices)
    {
        if (_vertexDataVAO != 0) {
            glDeleteVertexArrays(1, &_vertexDataVAO);
            glDeleteBuffers(1, &_vertexDataVBO);
            glDeleteBuffers(1, &_vertexDataEBO);
        }

        glGenVertexArrays(1, &_vertexDataVAO);
        glGenBuffers(1, &_vertexDataVBO);
        glGenBuffers(1, &_vertexDataEBO);

        glBindVertexArray(_vertexDataVAO);
        glBindBuffer(GL_ARRAY_BUFFER, _vertexDataVBO);
        glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(Vertex), &vertices[0], GL_STATIC_DRAW);
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, _vertexDataEBO);
        glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(uint32_t), &indices[0], GL_STATIC_DRAW);

        glEnableVertexAttribArray(0);
        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)0);
        glEnableVertexAttribArray(1);
        glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, normal));
        glEnableVertexAttribArray(2);
        glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, uv));
        glEnableVertexAttribArray(3);
        glVertexAttribPointer(3, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, tangent));

        glEnableVertexAttribArray(0);
        glBindBuffer(GL_ARRAY_BUFFER, 0);
        glBindVertexArray(0);
    }
    void upload_weighted_vertex_data(std::vector<WeightedVertex>& vertices, std::vector<uint32_t>& indices)
    {
        if (vertices.empty() || indices.empty()) {
            return;
        }

        if (_weightedVertexDataVAO != 0) {
            glDeleteVertexArrays(1, &_weightedVertexDataVAO);
            glDeleteBuffers(1, &_weightedVertexDataVBO);
            glDeleteBuffers(1, &_weightedVertexDataEBO);
        }

        glGenVertexArrays(1, &_weightedVertexDataVAO);
        glGenBuffers(1, &_weightedVertexDataVBO);
        glGenBuffers(1, &_weightedVertexDataEBO);

        glBindVertexArray(_weightedVertexDataVAO);
        glBindBuffer(GL_ARRAY_BUFFER, _weightedVertexDataVBO);
        glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(WeightedVertex), &vertices[0], GL_STATIC_DRAW);
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, _weightedVertexDataEBO);
        glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(uint32_t), &indices[0], GL_STATIC_DRAW);

        glEnableVertexAttribArray(0);
        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(WeightedVertex), (void*)0);
        glEnableVertexAttribArray(1);
        glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(WeightedVertex), (void*)offsetof(WeightedVertex, normal));
        glEnableVertexAttribArray(2);
        glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(WeightedVertex), (void*)offsetof(WeightedVertex, uv));
        glEnableVertexAttribArray(3);
        glVertexAttribPointer(3, 3, GL_FLOAT, GL_FALSE, sizeof(WeightedVertex), (void*)offsetof(WeightedVertex, tangent));
        glEnableVertexAttribArray(4);
        glVertexAttribPointer(4, 4, GL_FLOAT, GL_INT, sizeof(WeightedVertex), (void*)offsetof(WeightedVertex, boneID));
        glEnableVertexAttribArray(5);
        glVertexAttribPointer(5, 4, GL_FLOAT, GL_FALSE, sizeof(WeightedVertex), (void*)offsetof(WeightedVertex, weight));

        glEnableVertexAttribArray(0);
        glBindBuffer(GL_ARRAY_BUFFER, 0);
        glBindVertexArray(0);
    }
    GLuint get_vertex_dataVAO()
    {
        return _vertexDataVAO;
    }
    GLuint get_vertex_dataVBO()
    {
        return _vertexDataVBO;
    }
    GLuint get_vertex_dataEBO()
    {
        return _vertexDataEBO;
    }
}
