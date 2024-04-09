#include "imgui.h"
#include "imgui_impl_glfw.h"
#include "imgui_impl_opengl3.h"

#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>


#include <learnopengl/filesystem.h>
#include <learnopengl/shader.h>
#include <learnopengl/camera.h>
#include <learnopengl/model.h>

#include <iostream>

unsigned int loadCubemap(vector<std::string> faces);

void framebuffer_size_callback(GLFWwindow *window, int width, int height);

void mouse_callback(GLFWwindow *window, double xpos, double ypos);

void scroll_callback(GLFWwindow *window, double xoffset, double yoffset);

void processInput(GLFWwindow *window);

void key_callback(GLFWwindow *window, int key, int scancode, int action, int mods);

unsigned int loadTexture(const char *path, bool gammaCorrection);

void renderQuad();

// settings
const unsigned int SCR_WIDTH = 800;
const unsigned int SCR_HEIGHT = 600;

// camera
float lastX = SCR_WIDTH / 2.0f;
float lastY = SCR_HEIGHT / 2.0f;
bool firstMouse = true;

float heightScale = 0.1;
bool noc = false;

// timing
float deltaTime = 0.0f;
float lastFrame = 0.0f;

struct PointLight {
    glm::vec3 position;
    glm::vec3 ambient;
    glm::vec3 diffuse;
    glm::vec3 specular;

    float constant;
    float linear;
    float quadratic;

};

struct DirLight {
    glm::vec3 direction;

    glm::vec3 ambient;
    glm::vec3 diffuse;
    glm::vec3 specular;
};

struct ProgramState {
    glm::vec3 clearColor = glm::vec3(0);
    bool ImGuiEnabled = false;
    Camera camera;
    bool CameraMouseMovementUpdateEnabled = true;
    glm::vec3 lampPosition = glm::vec3(562, 10, 3570);
    glm::vec3 ponyPosition = glm::vec3(-10, -0.3, -83);
    glm::vec3 slupanPosition = glm::vec3(-260, -10, 150);
    glm::vec3 dodgePosition = glm::vec3(450, 0, 3685);
    glm::vec3 garagePosition = glm::vec3(-140, 0, -30);
    glm::vec3 dinerPosition = glm::vec3(-80, 0, -750);
    glm::vec3 roadPosition = glm::vec3(370, 0, -50);
    glm::vec3 roadPosition1 = glm::vec3(370, 0, -659);
    glm::vec3 roadPosition2 = glm::vec3(370, 0, -1268);
    glm::vec3 roadPosition4 = glm::vec3(370, 0, -1877);
    glm::vec3 roadPosition3 = glm::vec3(370, 0, 559);
    glm::vec3 road_without_side_Position = glm::vec3(80, -3, 350);
    glm::vec3 road1_without_side_Position = glm::vec3(100, -3, -1620);
    glm::vec3 roadPosition5 = glm::vec3(370, 0, -2486);
    glm::vec3 roadPosition6 = glm::vec3(370, 0, 1168);
    glm::vec3 roadPosition7 = glm::vec3(370, 0, 2616);
    glm::vec3 roadPosition8 = glm::vec3(370, 0, -3781);
    glm::vec3 roadPosition9 = glm::vec3(370, 0, -1178);

    float dinerScale = 7;
    float ponyScale = 0.2;
    float lampScale = 55;
    float dodgeScale = 35;
    float slupanScale = 7;
    float garageScale = 1.05;
    float roadScale = 40;
    float road_without_side = 57;
    float road1_without_side = 65;

    PointLight pointLight[5];
    DirLight dirLight;

    ProgramState()
            : camera(glm::vec3(0.0f, 0.0f, 3.0f)) {}

    void SaveToFile(std::string filename);

    void LoadFromFile(std::string filename);
};

void ProgramState::SaveToFile(std::string filename) {
    std::ofstream out(filename);
    out << clearColor.r << '\n'
        << clearColor.g << '\n'
        << clearColor.b << '\n'
        << ImGuiEnabled << '\n'
        << camera.Position.x << '\n'
        << camera.Position.y << '\n'
        << camera.Position.z << '\n'
        << camera.Front.x << '\n'
        << camera.Front.y << '\n'
        << camera.Front.z << '\n';
}

void ProgramState::LoadFromFile(std::string filename) {
    std::ifstream in(filename);
    if (in) {
        in >> clearColor.r
           >> clearColor.g
           >> clearColor.b
           >> ImGuiEnabled
           >> camera.Position.x
           >> camera.Position.y
           >> camera.Position.z
           >> camera.Front.x
           >> camera.Front.y
           >> camera.Front.z;
    }
}

ProgramState *programState;

void DrawImGui(ProgramState *programState);

int main() {
    // glfw: initialize and configure
    // ------------------------------
    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

#ifdef __APPLE__
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
#endif

    // glfw window creation
    // --------------------
    GLFWwindow *window = glfwCreateWindow(SCR_WIDTH, SCR_HEIGHT, "grafika", NULL, NULL);
    if (window == NULL) {
        std::cout << "Failed to create GLFW window" << std::endl;
        glfwTerminate();
        return -1;
    }
    glfwMakeContextCurrent(window);
    glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);
    glfwSetCursorPosCallback(window, mouse_callback);
    glfwSetScrollCallback(window, scroll_callback);
    glfwSetKeyCallback(window, key_callback);
    // tell GLFW to capture our mouse
    glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);

    // glad: load all OpenGL function pointers
    // ---------------------------------------
    if (!gladLoadGLLoader((GLADloadproc) glfwGetProcAddress)) {
        std::cout << "Failed to initialize GLAD" << std::endl;
        return -1;
    }

    // tell stb_image.h to flip loaded texture's on the y-axis (before loading model).
    stbi_set_flip_vertically_on_load(false);

    programState = new ProgramState;
    programState->LoadFromFile("resources/program_state.txt");
    if (programState->ImGuiEnabled) {
        glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
    }
    // Init Imgui
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGuiIO &io = ImGui::GetIO();
    (void) io;


    ImGui_ImplGlfw_InitForOpenGL(window, true);
    ImGui_ImplOpenGL3_Init("#version 330 core");

    // configure global opengl state
    // -----------------------------
    glEnable(GL_DEPTH_TEST);

//    glEnable(GL_CULL_FACE);
//    glCullFace(GL_BACK);


    // build and compile shaders
    // -------------------------
    Shader ourShader("resources/shaders/2.model_lighting.vs", "resources/shaders/2.model_lighting.fs");
    Shader skyboxShader("resources/shaders/skybox.vs", "resources/shaders/skybox.fs");
    Shader planeShader("resources/shaders/planeShader.vs", "resources/shaders/planeShader.fs");
    Shader blendingShader("resources/shaders/blending.vs", "resources/shaders/blending.fs");
    Shader parallaxShader("resources/shaders/parallax_mapping.vs", "resources/shaders/parallax_mapping.fs");

    // load models
    // -----------

    Model garage("resources/objects/garage/garage.obj");
    garage.SetShaderTextureNamePrefix("material.");

    Model diner("resources/objects/diner/DioramaDiner.obj");
    diner.SetShaderTextureNamePrefix("material.");

    Model pony("resources/objects/pony_car/Pony_cartoon.obj");
    pony.SetShaderTextureNamePrefix("material.");

    Model dodge("resources/objects/dodge/dodge.obj");
    dodge.SetShaderTextureNamePrefix("material.");

    Model lamp("resources/objects/street_lamp/street_lamp_02.obj");
    lamp.SetShaderTextureNamePrefix("material.");

    Model slupan("resources/objects/crashed_car/car03.obj");
    slupan.SetShaderTextureNamePrefix("material.");

    Model road("resources/objects/road/road.obj");
    road.SetShaderTextureNamePrefix("material.");

    Model road1("resources/objects/road/road.obj");
    road1.SetShaderTextureNamePrefix("material.");

    Model road2("resources/objects/road1/road.obj");
    road2.SetShaderTextureNamePrefix("material.");

    Model road3("resources/objects/road1/road.obj");
    road3.SetShaderTextureNamePrefix("material.");

    Model road4("resources/objects/road1/road.obj");
    road4.SetShaderTextureNamePrefix("material.");

    Model road5("resources/objects/road/road.obj");
    road5.SetShaderTextureNamePrefix("material.");

    Model road6("resources/objects/road/road.obj");
    road6.SetShaderTextureNamePrefix("material.");

    Model road7("resources/objects/road/road.obj");
    road7.SetShaderTextureNamePrefix("material.");

    Model road8("resources/objects/road/road.obj");
    road8.SetShaderTextureNamePrefix("material.");

    Model road9("resources/objects/road2/road.obj");
    road9.SetShaderTextureNamePrefix("material.");

    Model road_without_side("resources/objects/road1/road.obj");
    road_without_side.SetShaderTextureNamePrefix("material.");

    Model road1_without_side("resources/objects/road1/road.obj");
    road1_without_side.SetShaderTextureNamePrefix("material.");

    PointLight& pointLight1 = programState->pointLight[0];
    pointLight1.position = glm::vec3(-10, 445, 50);
    pointLight1.ambient = glm::vec3(7.0, 7.0, 7.0);
    pointLight1.diffuse = glm::vec3(0.9, 0.9, 0.9);
    pointLight1.specular = glm::vec3(1.0, 1.0, 1.0);

    pointLight1.constant = 17.0f;
    pointLight1.linear = 0.09f;
    pointLight1.quadratic = 0.001f;

    PointLight& pointLight2 = programState->pointLight[1];
    pointLight2.position = glm::vec3(-9, 35, -109);
    pointLight2.ambient = glm::vec3(10.0, 10.0, 10.0);
    pointLight2.diffuse = glm::vec3(0.9, 0.9, 0.9);
    pointLight2.specular = glm::vec3(1.0, 1.0, 1.0);

    pointLight2.constant = 17.0f;
    pointLight2.linear = 0.09f;
    pointLight2.quadratic = 0.032f;

    PointLight& pointLight3 = programState->pointLight[2];
    pointLight3.position = glm::vec3(12, -43.9, -27);
    pointLight3.ambient = glm::vec3(10.0, 10.0, 10.0);
    pointLight3.diffuse = glm::vec3(0.9, 0.9, 0.9);
    pointLight3.specular = glm::vec3(1.0, 1.0, 1.0);

    pointLight3.constant = 17.0f;
    pointLight3.linear = 0.09f;
    pointLight3.quadratic = 0.032f;

    DirLight& dirLight = programState->dirLight;

    dirLight.direction = glm::vec3(-2.0f, -1.0f, -0.3f);
    dirLight.ambient = glm::vec3(0.35f, 0.35f, 0.35f);
    dirLight.diffuse = glm::vec3(1, 0.8, 0.1);
    dirLight.specular = glm::vec3(0.5f, 0.5f, 0.5f);

    float cubeVertices[] = {
            // back face
            -0.5f, -0.5f, -0.5f,  0.0f, 0.0f, // bottom-left
            0.5f, -0.5f, -0.5f,  1.0f, 0.0f, // bottom-right
            0.5f,  0.5f, -0.5f,  1.0f, 1.0f, // top-right
            0.5f,  0.5f, -0.5f,  1.0f, 1.0f, // top-right
            -0.5f,  0.5f, -0.5f,  0.0f, 1.0f, // top-left
            -0.5f, -0.5f, -0.5f,  0.0f, 0.0f, // bottom-left
            // front face
            -0.5f, -0.5f,  0.5f,  0.0f, 0.0f, // bottom-left
            0.5f,  0.5f,  0.5f,  1.0f, 1.0f, // top-right
            0.5f, -0.5f,  0.5f,  1.0f, 0.0f, // bottom-right
            0.5f,  0.5f,  0.5f,  1.0f, 1.0f, // top-right
            -0.5f, -0.5f,  0.5f,  0.0f, 0.0f, // bottom-left
            -0.5f,  0.5f,  0.5f,  0.0f, 1.0f, // top-left
            // left face
            -0.5f,  0.5f,  0.5f,  1.0f, 0.0f, // top-right
            -0.5f, -0.5f, -0.5f,  0.0f, 1.0f, // bottom-left
            -0.5f,  0.5f, -0.5f,  1.0f, 1.0f, // top-left
            -0.5f, -0.5f, -0.5f,  0.0f, 1.0f, // bottom-left
            -0.5f,  0.5f,  0.5f,  1.0f, 0.0f, // top-right
            -0.5f, -0.5f,  0.5f,  0.0f, 0.0f, // bottom-right
            // right face
            0.5f,  0.5f,  0.5f,  1.0f, 0.0f, // top-left
            0.5f,  0.5f, -0.5f,  1.0f, 1.0f, // top-right
            0.5f, -0.5f, -0.5f,  0.0f, 1.0f, // bottom-right
            0.5f, -0.5f, -0.5f,  0.0f, 1.0f, // bottom-right
            0.5f, -0.5f,  0.5f,  0.0f, 0.0f, // bottom-left
            0.5f,  0.5f,  0.5f,  1.0f, 0.0f, // top-left
            // bottom face
            -0.5f, -0.5f, -0.5f,  0.0f, 1.0f, // top-right
            0.5f, -0.5f,  0.5f,  1.0f, 0.0f, // bottom-left
            0.5f, -0.5f, -0.5f,  1.0f, 1.0f, // top-left
            0.5f, -0.5f,  0.5f,  1.0f, 0.0f, // bottom-left
            -0.5f, -0.5f, -0.5f,  0.0f, 1.0f, // top-right
            -0.5f, -0.5f,  0.5f,  0.0f, 0.0f, // bottom-right
            // top face
            -0.5f,  0.5f, -0.5f,  0.0f, 1.0f, // top-left
            0.5f,  0.5f, -0.5f,  1.0f, 1.0f, // top-right
            0.5f,  0.5f,  0.5f,  1.0f, 0.0f, // bottom-right
            0.5f,  0.5f,  0.5f,  1.0f, 0.0f, // bottom-right
            -0.5f,  0.5f,  0.5f,  0.0f, 0.0f, // bottom-left
            -0.5f,  0.5f, -0.5f,  0.0f, 1.0f  // top-left
    };
    glad_glFrontFace(GL_CW);

    //     cube VAO
    unsigned int cubeVAO, cubeVBO;
    glGenVertexArrays(1, &cubeVAO);
    glGenBuffers(1, &cubeVBO);
    glBindVertexArray(cubeVAO);
    glBindBuffer(GL_ARRAY_BUFFER, cubeVBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(cubeVertices), &cubeVertices, GL_STATIC_DRAW);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)(3 * sizeof(float)));

    unsigned int cardboardTexture = loadTexture(FileSystem::getPath("resources/textures/karton.jpg").c_str(), true);

    // Grass vertices
    float transparentVertices[] = {
            // positions         // texture Coords
            0.0f,  0.5f,  0.0f,  0.0f,  0.0f,
            0.0f, -0.5f,  0.0f,  0.0f,  1.0f,
            1.0f, -0.5f,  0.0f,  1.0f,  1.0f,

            0.0f,  0.5f,  0.0f,  0.0f,  0.0f,
            1.0f, -0.5f,  0.0f,  1.0f,  1.0f,
            1.0f,  0.5f,  0.0f,  1.0f,  0.0f
    };

    // transparent vegetation location
    vector<glm::vec3> pozicija_trave;
    vector<float> rotacija_trave;

    //Grass positions
    for(float i = 0; i < 28 ; i++){
        pozicija_trave.push_back(glm::vec3(90.0f, 40.0f, -500.0f-25.0f*i));
        rotacija_trave.push_back(rand()%90);
    }

    // transparent VAO
    unsigned int transparentVAO, transparentVBO;
    glGenVertexArrays(1, &transparentVAO);
    glGenBuffers(1, &transparentVBO);
    glBindVertexArray(transparentVAO);
    glBindBuffer(GL_ARRAY_BUFFER, transparentVBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(transparentVertices), transparentVertices, GL_STATIC_DRAW);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)(3 * sizeof(float)));
    glBindVertexArray(0);

    unsigned int grassTexture = loadTexture(FileSystem::getPath("resources/textures/trava.png").c_str(), true);
    blendingShader.use();
    blendingShader.setInt("texture1", 0);

    //plane
    float planeVertices[] = {
            //      vertex           texture        normal
            50.0f, 0.0f,  50.0f,  2.0f, 0.0f, 0.0f, 1.0f, 0.0f,
            -50.0f, 0.0f, -50.0f,  0.0f, 2.0f, 0.0f, 1.0f, 0.0f,
            -50.0f, 0.0f,  50.0f,  0.0f, 0.0f, 0.0f, 1.0f, 0.0f,


            50.0f, 0.0f,  50.0f,  2.0f, 0.0f, 0.0f, 1.0f, 0.0f,
            50.0f, 0.0f, -50.0f,  2.0f, 2.0f, 0.0f, 1.0f, 0.0f,
            -50.0f, 0.0f, -50.0f,  0.0f, 2.0f, 0.0f, 1.0f, 0.0f
    };

    unsigned int planeVAO, planeVBO;
    glGenVertexArrays(1, &planeVAO);
    glGenBuffers(1, &planeVBO);

    glBindVertexArray(planeVAO);

    glBindBuffer(GL_ARRAY_BUFFER, planeVBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(planeVertices), &planeVertices, GL_STATIC_DRAW);

    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)0);

    glEnableVertexAttribArray(1);
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(3 * sizeof(float)));

    glEnableVertexAttribArray(2);
    glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(5 * sizeof(float)));

    glBindVertexArray(0);

    unsigned int groundTexture = loadTexture(FileSystem::getPath("resources/textures/pesak.jpg").c_str(), true);

    planeShader.use();
    planeShader.setInt("texture1", 0);

    //sahta
    unsigned int diffuseMap = loadTexture(FileSystem::getPath("resources/textures/metal.jpg").c_str(), true);
    unsigned int normalMap  = loadTexture(FileSystem::getPath("resources/textures/metal_normal_map.png").c_str(), true);
    unsigned int heightMap  = loadTexture(FileSystem::getPath("resources/textures/metal_height_map.png").c_str(), true);

    parallaxShader.use();
    parallaxShader.setInt("diffuseMap", 0);
    parallaxShader.setInt("normalMap", 1);
    parallaxShader.setInt("depthMap", 2);

    //skybox
    float skyboxVertices[] = {
            // positions
            -1.0f,  1.0f, -1.0f,
            -1.0f, -1.0f, -1.0f,
            1.0f, -1.0f, -1.0f,
            1.0f, -1.0f, -1.0f,
            1.0f,  1.0f, -1.0f,
            -1.0f,  1.0f, -1.0f,

            -1.0f, -1.0f,  1.0f,
            -1.0f, -1.0f, -1.0f,
            -1.0f,  1.0f, -1.0f,
            -1.0f,  1.0f, -1.0f,
            -1.0f,  1.0f,  1.0f,
            -1.0f, -1.0f,  1.0f,

            1.0f, -1.0f, -1.0f,
            1.0f, -1.0f,  1.0f,
            1.0f,  1.0f,  1.0f,
            1.0f,  1.0f,  1.0f,
            1.0f,  1.0f, -1.0f,
            1.0f, -1.0f, -1.0f,

            -1.0f, -1.0f,  1.0f,
            -1.0f,  1.0f,  1.0f,
            1.0f,  1.0f,  1.0f,
            1.0f,  1.0f,  1.0f,
            1.0f, -1.0f,  1.0f,
            -1.0f, -1.0f,  1.0f,

            -1.0f,  1.0f, -1.0f,
            1.0f,  1.0f, -1.0f,
            1.0f,  1.0f,  1.0f,
            1.0f,  1.0f,  1.0f,
            -1.0f,  1.0f,  1.0f,
            -1.0f,  1.0f, -1.0f,

            -1.0f, -1.0f, -1.0f,
            -1.0f, -1.0f,  1.0f,
            1.0f, -1.0f, -1.0f,
            1.0f, -1.0f, -1.0f,
            -1.0f, -1.0f,  1.0f,
            1.0f, -1.0f,  1.0f
    };

    unsigned int skyboxVAO, skyboxVBO;
    glGenVertexArrays(1, &skyboxVAO);
    glGenBuffers(1, &skyboxVBO);
    glBindVertexArray(skyboxVAO);
    glBindBuffer(GL_ARRAY_BUFFER, skyboxVBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(skyboxVertices), &skyboxVertices, GL_STATIC_DRAW);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3*sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);

    vector<std::string> faces
            {
                    FileSystem::getPath("/resources/textures/skybox/right.jpg"),
                    FileSystem::getPath("/resources/textures/skybox/left.jpg"),
                    FileSystem::getPath("/resources/textures/skybox/top.jpg"),
                    FileSystem::getPath("/resources/textures/skybox/bottom.jpg"),
                    FileSystem::getPath("/resources/textures/skybox/front.jpg"),
                    FileSystem::getPath("/resources/textures/skybox/back.jpg")
            };


    unsigned int cubemapTexture = loadCubemap(faces);

    skyboxShader.use();
    skyboxShader.setInt("skybox",0);
    glm::vec3 pozicija_dodga = glm::vec3(450, 0, 3685);

    //brojaci za kretanje auta i svetla
    int brojac = 3685;
    float desni_far = 3608.47;
    float levi_far = 3607.11;
    float desni_far_2 = 3592.42;
    float levi_far_2 = 3591.98;

    // lamp locations
    vector<glm::vec3> pozicija_lampe;

    //lamp positions
    for(float i = 0; i < 10 ; i++){
        pozicija_lampe.push_back(glm::vec3(562, 10, 3570 - 900*i));
    }

    // light locations
    vector<glm::vec3> pozicija_svetla;

    //lamp positions
    for(float i = 0; i < 10 ; i++){
        pozicija_svetla.push_back(glm::vec3(556, 153, 3570 - 900*i));
    }

    vector<string> pointlightsPos({
        "pointLight1.position",
        "pointLight2.position",
        "pointLight3.position",
        "pointLight4.position",
        "pointLight5.position",
        "pointLight6.position",
        "pointLight7.position",
        "pointLight8.position",
        "pointLight9.position",
        "pointLight10.position",

    });
    vector<string> pointlightsAmb({
        "pointLight1.ambient",
        "pointLight2.ambient",
        "pointLight3.ambient",
        "pointLight4.ambient",
        "pointLight5.ambient",
        "pointLight6.ambient",
        "pointLight7.ambient",
        "pointLight8.ambient",
        "pointLight9.ambient",
        "pointLight10.ambient",

        });
    vector<string> pointlightsDiff({
        "pointLight1.diffuse",
        "pointLight2.diffuse",
        "pointLight3.diffuse",
        "pointLight4.diffuse",
        "pointLight5.diffuse",
        "pointLight6.diffuse",
        "pointLight7.diffuse",
        "pointLight8.diffuse",
        "pointLight9.diffuse",
        "pointLight10.diffuse",

        });
    vector<string> pointlightsSpec({
        "pointLight1.specular",
        "pointLight2.specular",
        "pointLight3.specular",
        "pointLight4.specular",
        "pointLight5.specular",
        "pointLight6.specular",
        "pointLight7.specular",
        "pointLight8.specular",
        "pointLight9.specular",
        "pointLight10.specular",

        });
    vector<string> pointlightsConst({
        "pointLight1.constant",
        "pointLight2.constant",
        "pointLight3.constant",
        "pointLight4.constant",
        "pointLight5.constant",
        "pointLight6.constant",
        "pointLight7.constant",
        "pointLight8.constant",
        "pointLight9.constant",
        "pointLight10.constant",

        });
    vector<string> pointlightsLin({
        "pointLight1.linear",
        "pointLight2.linear",
        "pointLight3.linear",
        "pointLight4.linear",
        "pointLight5.linear",
        "pointLight6.linear",
        "pointLight7.linear",
        "pointLight8.linear",
        "pointLight9.linear",
        "pointLight10.linear",

        });
    vector<string> pointlightsQuad({
        "pointLight1.quadratic",
        "pointLight2.quadratic",
        "pointLight3.quadratic",
        "pointLight4.quadratic",
        "pointLight5.quadratic",
        "pointLight6.quadratic",
        "pointLight7.quadratic",
        "pointLight8.quadratic",
        "pointLight9.quadratic",
        "pointLight10.quadratic",

        });



    // render loop
    // -----------
    while (!glfwWindowShouldClose(window)) {
        // per-frame time logic
        // --------------------
        float currentFrame = glfwGetTime();
        deltaTime = currentFrame - lastFrame;
        lastFrame = currentFrame;

        // input
        // -----
        processInput(window);
        glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);


        // render
        // ------
        glClearColor(programState->clearColor.r, programState->clearColor.g, programState->clearColor.b, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);


        planeShader.use();
        glm::mat4 model = glm::mat4(1.0f);
        glm::mat4 view = programState->camera.GetViewMatrix();
        glm::mat4 projection = glm::perspective(glm::radians(programState->camera.Zoom), (float)SCR_WIDTH / (float)SCR_HEIGHT, 0.1f, 10000.0f);
        planeShader.setMat4("view", view);
        planeShader.setMat4("projection", projection);

        glBindVertexArray(planeVAO);
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, groundTexture);
        model = glm::translate(model, glm::vec3(0.0f, -6.0f, 0.0f));
        model = glm::scale(model, glm::vec3 (100));
        planeShader.setMat4("model", model);

        planeShader.setBool("noc", noc);
        planeShader.setVec3("dirLight.direction", dirLight.direction);
        planeShader.setVec3("dirLight.ambientDay", dirLight.ambient);
        planeShader.setVec3("dirLight.diffuseDay", 0.1f, 0.1f,0.1f);
        planeShader.setVec3("dirLight.specularDay", dirLight.specular);
        planeShader.setVec3("dirLight.ambientNight", 0.0f, 0.0f,0.0f);
        planeShader.setVec3("dirLight.diffuseNight", 0.1f, 0.1f, 0.1f);
        planeShader.setVec3("dirLight.specularNight", 0.0f, 0.0f,0.0f);
        planeShader.setFloat("shininess", 32.0f);

        glDrawArrays(GL_TRIANGLES, 0, 6);
        glBindVertexArray(0);


        //trava
        blendingShader.use();
        glm::mat4 trava_model = glm::mat4(1.0f);
        blendingShader.setMat4("projection", projection);
        blendingShader.setMat4("view", view);
        glBindVertexArray(transparentVAO);
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, grassTexture);
        for (unsigned int i = 0; i < 28; i++)
        {
            trava_model = glm::mat4(1.0f);
            trava_model = glm::translate(trava_model, pozicija_trave[i]);
            trava_model = glm::scale(trava_model, glm::vec3(100.0f));
            trava_model = glm::rotate(trava_model ,glm::radians(rotacija_trave[i]), glm::vec3(0.0f ,1.0f, 0.0f));
            blendingShader.setMat4("model", trava_model);
            glDrawArrays(GL_TRIANGLES, 0, 6);
        }


        // kartonska kutija
        glEnable(GL_CULL_FACE);
        glCullFace(GL_BACK);

        glBindVertexArray(cubeVAO);
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, cardboardTexture);
        model = glm::mat4(1.0f);
        model = glm::translate(model, glm::vec3(-280.0f, 35.5f, -550.0f));
        model = glm::scale(model, glm::vec3(25.0f));
        blendingShader.setMat4("model", model);
        glDrawArrays(GL_TRIANGLES, 0, 36);

        glDisable(GL_CULL_FACE);

        //sahta
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, diffuseMap);
        glActiveTexture(GL_TEXTURE1);
        glBindTexture(GL_TEXTURE_2D, normalMap);
        glActiveTexture(GL_TEXTURE2);
        glBindTexture(GL_TEXTURE_2D, heightMap);

        parallaxShader.use();

        parallaxShader.setMat4("projection", projection);
        parallaxShader.setMat4("view", view);
        parallaxShader.setVec3("viewPos", programState->camera.Position);
        parallaxShader.setVec3("lightPos", glm::vec3(3.0f, 450.0f, 40.0f));
        parallaxShader.setFloat("heightScale", heightScale);

        glm::mat4 quad = glm::mat4(1.0f);
        quad = glm::translate(quad, glm::vec3(-395.0f, 20.0f, -1196.0f));
        quad = glm::rotate(quad, glm::radians(-90.0f), glm::vec3(1.0f, 0.0f, 0.0f));
        quad = glm::scale(quad, glm::vec3(40.0f, 40.0f, 40.0f));
        parallaxShader.setMat4("model", quad);
        renderQuad();

        //ourShader
        ourShader.use();
        ourShader.setBool("noc", noc);
        //spotlight1, svetlo kamere
        ourShader.setVec3("spotlight1.position", programState->camera.Position);
        ourShader.setVec3("spotlight1.direction", programState->camera.Front);
        ourShader.setFloat("spotlight1.cutOff", glm::cos(glm::radians(12.5f)));
        ourShader.setFloat("spotlight1.outerCutOff", glm::cos(glm::radians(17.5f)));
        ourShader.setVec3("spotlight1.ambient", glm::vec3(0.0f, 0.0f, 0.0f));
        ourShader.setVec3("spotlight1.diffuse", glm::vec3(1, 0.8, 0.1));
        ourShader.setVec3("spotlight1.specular", glm::vec3(0.5f, 0.5f, 0.5f));
        ourShader.setFloat("spotlight1.constant", 1.0f);
        ourShader.setFloat("spotlight1.linear", 0.0f);
        ourShader.setFloat("spotlight1.quadratic", 0.00001f);

        //spotlight2, desni far
        ourShader.setVec3("spotlight2.position", glm::vec3(34.29f, 36.21f, -53.63f));
        ourShader.setVec3("spotlight2.direction", glm::vec3(0.985f, -0.158f, -0.058f));
        ourShader.setFloat("spotlight2.cutOff", glm::cos(glm::radians(12.5f)));
        ourShader.setFloat("spotlight2.outerCutOff", glm::cos(glm::radians(17.5f)));
        ourShader.setVec3("spotlight2.ambient", glm::vec3(0.0f, 0.0f, 0.0f));
        ourShader.setVec3("spotlight2.diffuse", glm::vec3(1, 0.8, 0.1));
        ourShader.setVec3("spotlight2.specular", glm::vec3(0.5f, 0.5f, 0.5f));
        ourShader.setFloat("spotlight2.constant", 1.0f);
        ourShader.setFloat("spotlight2.linear", 0.0f);
        ourShader.setFloat("spotlight2.quadratic", 0.00001f);

        //spotlight3, desni far, svetlo ka faru
        ourShader.setVec3("spotlight3.position", glm::vec3(64.38f, 33.26f, -54.59f));
        ourShader.setVec3("spotlight3.direction", glm::vec3(-0.999f, 0.027f, 0.032f));
        ourShader.setFloat("spotlight3.cutOff", glm::cos(glm::radians(12.5f)));
        ourShader.setFloat("spotlight3.outerCutOff", glm::cos(glm::radians(17.5f)));
        ourShader.setVec3("spotlight3.ambient", glm::vec3(0.0f, 0.0f, 0.0f));
        ourShader.setVec3("spotlight3.diffuse", glm::vec3(1, 0.8, 0.1));
        ourShader.setVec3("spotlight3.specular", glm::vec3(0.5f, 0.5f, 0.5f));
        ourShader.setFloat("spotlight3.constant", 1.0f);
        ourShader.setFloat("spotlight3.linear", 0.0f);
        ourShader.setFloat("spotlight3.quadratic", 0.004f);

        //spotlight4, levi far
        ourShader.setVec3("spotlight4.position", glm::vec3(43.17f, 34.06f, -112.10f));
        ourShader.setVec3("spotlight4.direction", glm::vec3(0.989f, -0.142f, 0.048f));
        ourShader.setFloat("spotlight4.cutOff", glm::cos(glm::radians(12.5f)));
        ourShader.setFloat("spotlight4.outerCutOff", glm::cos(glm::radians(17.5f)));
        ourShader.setVec3("spotlight4.ambient", glm::vec3(0.0f, 0.0f, 0.0f));
        ourShader.setVec3("spotlight4.diffuse", glm::vec3(1, 0.8, 0.1));
        ourShader.setVec3("spotlight4.specular", glm::vec3(0.5f, 0.5f, 0.5f));
        ourShader.setFloat("spotlight4.constant", 1.0f);
        ourShader.setFloat("spotlight4.linear", 0.0f);
        ourShader.setFloat("spotlight4.quadratic", 0.00001f);

        //spotlight5, levi far, svetlo ka faru
        ourShader.setVec3("spotlight5.position", glm::vec3(66.82f, 37.20f, -113.89f));
        ourShader.setVec3("spotlight5.direction", glm::vec3(-0.964f, -0.224f, 0.135f));
        ourShader.setFloat("spotlight5.cutOff", glm::cos(glm::radians(12.5f)));
        ourShader.setFloat("spotlight5.outerCutOff", glm::cos(glm::radians(17.5f)));
        ourShader.setVec3("spotlight5.ambient", glm::vec3(0.0f, 0.0f, 0.0f));
        ourShader.setVec3("spotlight5.diffuse", glm::vec3(1, 0.8, 0.1));
        ourShader.setVec3("spotlight5.specular", glm::vec3(0.5f, 0.5f, 0.5f));
        ourShader.setFloat("spotlight5.constant", 1.0f);
        ourShader.setFloat("spotlight5.linear", 0.0f);
        ourShader.setFloat("spotlight5.quadratic", 0.004f);

        //spotlight6, desni far, dodge
        ourShader.setVec3("spotlight6.position", glm::vec3(474.04f, 23.76f, desni_far));
        ourShader.setVec3("spotlight6.direction", glm::vec3(-0.0001f, -0.103f, -0.995f));
        ourShader.setFloat("spotlight6.cutOff", glm::cos(glm::radians(12.5f)));
        ourShader.setFloat("spotlight6.outerCutOff", glm::cos(glm::radians(17.5f)));
        ourShader.setVec3("spotlight6.ambient", glm::vec3(0.0f, 0.0f, 0.0f));
        ourShader.setVec3("spotlight6.diffuse", glm::vec3(1, 0.8, 0.1));
        ourShader.setVec3("spotlight6.specular", glm::vec3(0.5f, 0.5f, 0.5f));
        ourShader.setFloat("spotlight6.constant", 1.0f);
        ourShader.setFloat("spotlight6.linear", 0.0f);
        ourShader.setFloat("spotlight6.quadratic", 0.00001f);
        desni_far = desni_far - 5;

        //spotlight7, desni far, svetlo ka faru, dodge
        ourShader.setVec3("spotlight7.position", glm::vec3(475.07f, 24.54f, desni_far_2));
        ourShader.setVec3("spotlight7.direction", glm::vec3(-0.009f, -0.052f, 0.999f));
        ourShader.setFloat("spotlight7.cutOff", glm::cos(glm::radians(12.5f)));
        ourShader.setFloat("spotlight7.outerCutOff", glm::cos(glm::radians(17.5f)));
        ourShader.setVec3("spotlight7.ambient", glm::vec3(0.0f, 0.0f, 0.0f));
        ourShader.setVec3("spotlight7.diffuse", glm::vec3(1, 0.8, 0.1));
        ourShader.setVec3("spotlight7.specular", glm::vec3(0.5f, 0.5f, 0.5f));
        ourShader.setFloat("spotlight7.constant", 1.0f);
        ourShader.setFloat("spotlight7.linear", 0.0f);
        ourShader.setFloat("spotlight7.quadratic", 0.004f);
        desni_far_2 = desni_far_2 - 5;

        //spotlight8, levi far, dodge
        ourShader.setVec3("spotlight8.position", glm::vec3(425.63f, 23.41f, levi_far));
        ourShader.setVec3("spotlight8.direction", glm::vec3(-0.012f, -0.104f, -0.995f));
        ourShader.setFloat("spotlight8.cutOff", glm::cos(glm::radians(12.5f)));
        ourShader.setFloat("spotlight8.outerCutOff", glm::cos(glm::radians(17.5f)));
        ourShader.setVec3("spotlight8.ambient", glm::vec3(0.0f, 0.0f, 0.0f));
        ourShader.setVec3("spotlight8.diffuse", glm::vec3(1, 0.8, 0.1));
        ourShader.setVec3("spotlight8.specular", glm::vec3(0.5f, 0.5f, 0.5f));
        ourShader.setFloat("spotlight8.constant", 1.0f);
        ourShader.setFloat("spotlight8.linear", 0.0f);
        ourShader.setFloat("spotlight8.quadratic", 0.00001f);
        levi_far = levi_far - 5;

        //spotlight9, levi far, svetlo ka faru, dodge
        ourShader.setVec3("spotlight9.position", glm::vec3(425.07f, 24.54f, levi_far_2));
        ourShader.setVec3("spotlight9.direction", glm::vec3(-0.009f, -0.052f, 0.999f));
        ourShader.setFloat("spotlight9.cutOff", glm::cos(glm::radians(12.5f)));
        ourShader.setFloat("spotlight9.outerCutOff", glm::cos(glm::radians(17.5f)));
        ourShader.setVec3("spotlight9.ambient", glm::vec3(0.0f, 0.0f, 0.0f));
        ourShader.setVec3("spotlight9.diffuse", glm::vec3(1, 0.8, 0.1));
        ourShader.setVec3("spotlight9.specular", glm::vec3(0.5f, 0.5f, 0.5f));
        ourShader.setFloat("spotlight9.constant", 1.0f);
        ourShader.setFloat("spotlight9.linear", 0.0f);
        ourShader.setFloat("spotlight9.quadratic", 0.004f);
        levi_far_2 = levi_far_2 - 5;

        for(unsigned int i = 0; i < 10; i++){
            ourShader.setVec3(pointlightsPos[i], pozicija_svetla[i]);
            ourShader.setVec3(pointlightsAmb[i], pointLight1.ambient);
            ourShader.setVec3(pointlightsDiff[i], pointLight1.diffuse);
            ourShader.setVec3(pointlightsSpec[i], pointLight1.specular);
            ourShader.setFloat(pointlightsConst[i], pointLight1.constant);
            ourShader.setFloat(pointlightsLin[i], pointLight1.linear);
            ourShader.setFloat(pointlightsQuad[i], pointLight1.quadratic);
        }

        // diner light1
        ourShader.setVec3("pointLight11.position", glm::vec3(-290, 223, -680));
        ourShader.setVec3("pointLight11.ambient", pointLight1.ambient);
        ourShader.setVec3("pointLight11.diffuse", pointLight1.diffuse);
        ourShader.setVec3("pointLight11.specular", pointLight1.specular);
        ourShader.setFloat("pointLight11.constant", pointLight1.constant);
        ourShader.setFloat("pointLight11.linear", pointLight1.linear);
        ourShader.setFloat("pointLight11.quadratic", 0.007);

        // diner light2
        ourShader.setVec3("pointLight12.position", glm::vec3(-290, 223, -770));
        ourShader.setVec3("pointLight12.ambient", pointLight2.ambient);
        ourShader.setVec3("pointLight12.diffuse", pointLight2.diffuse);
        ourShader.setVec3("pointLight12.specular", pointLight2.specular);
        ourShader.setFloat("pointLight12.constant", pointLight2.constant);
        ourShader.setFloat("pointLight12.linear", pointLight2.linear);
        ourShader.setFloat("pointLight12.quadratic", 0.007);

        // diner light dok ulaza
        ourShader.setVec3("pointLight13.position", glm::vec3(60, 100, -1193));
        ourShader.setVec3("pointLight13.ambient", pointLight3.ambient);
        ourShader.setVec3("pointLight13.diffuse", pointLight3.diffuse);
        ourShader.setVec3("pointLight13.specular", pointLight3.specular);
        ourShader.setFloat("pointLight13.constant", pointLight3.constant);
        ourShader.setFloat("pointLight13.linear", pointLight3.linear);
        ourShader.setFloat("pointLight13.quadratic", 0.001);

        // svetlo iznutra1
        ourShader.setVec3("pointLight14.position", glm::vec3(-347, 124, -702));
        ourShader.setVec3("pointLight14.ambient", pointLight3.ambient);
        ourShader.setVec3("pointLight14.diffuse", pointLight3.diffuse);
        ourShader.setVec3("pointLight14.specular", pointLight3.specular);
        ourShader.setFloat("pointLight14.constant", pointLight3.constant);
        ourShader.setFloat("pointLight14.linear", pointLight3.linear);
        ourShader.setFloat("pointLight14.quadratic", 0.01);

        // svetlo iznutra2
        ourShader.setVec3("pointLight15.position", glm::vec3(-350, 113, -921));
        ourShader.setVec3("pointLight15.ambient", pointLight3.ambient);
        ourShader.setVec3("pointLight15.diffuse", pointLight3.diffuse);
        ourShader.setVec3("pointLight15.specular", pointLight3.specular);
        ourShader.setFloat("pointLight15.constant", pointLight3.constant);
        ourShader.setFloat("pointLight15.linear", pointLight3.linear);
        ourShader.setFloat("pointLight15.quadratic", 0.01);

        // directional light
        ourShader.setVec3("dirLight.direction", dirLight.direction);
        ourShader.setVec3("dirLight.ambient", dirLight.ambient);
        ourShader.setVec3("dirLight.diffuse", dirLight.diffuse);
        ourShader.setVec3("dirLight.specular", dirLight.specular);
//        ourShader.setFloat("shininess", 1.0f);


        ourShader.setVec3("viewPosition", programState->camera.Position);
        ourShader.setFloat("material.shininess", 32.0f);

        // view/projection transformations
        projection = glm::perspective(glm::radians(programState->camera.Zoom),
                                                (float) SCR_WIDTH / (float) SCR_HEIGHT, 0.1f, 10000.0f);
        view = programState->camera.GetViewMatrix();
        ourShader.setMat4("projection", projection);
        ourShader.setMat4("view", view);


        // render the loaded model

        //garage
        model = glm::mat4(1.0f);
        model = glm::translate(model, programState->garagePosition);
        model = glm::scale(model, glm::vec3(programState->garageScale));
        model = glm::rotate(model, glm::radians(90.0f), glm::vec3 (0.0, 1.0f, 0.0f));
        ourShader.setMat4("model", model);
        garage.Draw(ourShader);

        //diner
        model = glm::mat4(1.0f);
        model = glm::translate(model, programState->dinerPosition);
        model = glm::scale(model, glm::vec3(programState->dinerScale));
        model = glm::rotate(model, glm::radians(90.0f), glm::vec3 (0.0, 1.0f, 0.0f));
        ourShader.setMat4("model", model);
        diner.Draw(ourShader);

        //pony
        model = glm::mat4(1.0f);
        model = glm::translate(model, programState->ponyPosition);
        model = glm::scale(model, glm::vec3(programState->ponyScale));
        model = glm::rotate(model, glm::radians(90.0f), glm::vec3 (0.0, 1.0f, 0.0f));
        ourShader.setMat4("model", model);
        pony.Draw(ourShader);

        if(brojac == -4730){
            brojac = 3685;
            desni_far = 3608.47;
            levi_far = 3607.11;
            desni_far_2 = 3592.42;
            levi_far_2 = 3591.98;

        }
        //dodge
        model = glm::mat4(1.0f);
        model = glm::translate(model, glm::vec3(450, 0, brojac));
        model = glm::scale(model, glm::vec3(programState->dodgeScale));
        model = glm::rotate(model, glm::radians(180.0f), glm::vec3 (0.0, 1.0f, 0.0f));
        ourShader.setMat4("model", model);
        dodge.Draw(ourShader);
        brojac = brojac - 5;

        //street lamp
        for(unsigned int i = 0; i < 10; i++) {
            model = glm::mat4(1.0f);
            model = glm::translate(model, pozicija_lampe[i]);
            model = glm::scale(model, glm::vec3(programState->lampScale));
//        model = glm::rotate(model, glm::radians(180.0f), glm::vec3 (0.0, 1.0f, 0.0f));
            ourShader.setMat4("model", model);
            lamp.Draw(ourShader);
        }

        //slupan
        model = glm::mat4(1.0f);
        model = glm::translate(model, programState->slupanPosition);
        model = glm::scale(model, glm::vec3(programState->slupanScale));
        model = glm::rotate(model, glm::radians(180.0f), glm::vec3 (0.0, 1.0f, 1.0f));
        model = glm::rotate(model, glm::radians(20.0f), glm::vec3 (0.0f, 0.0f, 1.0f));
        model = glm::rotate(model, glm::radians(5.0f), glm::vec3 (0.0f, 1.0f, 0.0f));
        ourShader.setMat4("model", model);
        slupan.Draw(ourShader);

        //road
        model = glm::mat4(1.0f);
        model = glm::translate(model, programState->roadPosition);
        model = glm::scale(model, glm::vec3(programState->roadScale));
        model = glm::rotate(model, glm::radians(0.0f), glm::vec3 (0.0, 1.0f, 0.0f));
        ourShader.setMat4("model", model);
        road.Draw(ourShader);

        //road1
        model = glm::mat4(1.0f);
        model = glm::translate(model, programState->roadPosition1);
        model = glm::scale(model, glm::vec3(programState->roadScale));
        model = glm::rotate(model, glm::radians(0.0f), glm::vec3 (0.0, 1.0f, 0.0f));
        ourShader.setMat4("model", model);
        road1.Draw(ourShader);

        //road2
        model = glm::mat4(1.0f);
        model = glm::translate(model, programState->roadPosition2);
        model = glm::scale(model, glm::vec3(programState->roadScale));
        model = glm::rotate(model, glm::radians(0.0f), glm::vec3 (0.0, 1.0f, 0.0f));
        ourShader.setMat4("model", model);
        road2.Draw(ourShader);

        //road3
        model = glm::mat4(1.0f);
        model = glm::translate(model, programState->roadPosition3);
        model = glm::scale(model, glm::vec3(programState->roadScale));
        model = glm::rotate(model, glm::radians(0.0f), glm::vec3 (0.0, 1.0f, 0.0f));
        ourShader.setMat4("model", model);
        road3.Draw(ourShader);

        //road4
        model = glm::mat4(1.0f);
        model = glm::translate(model, programState->roadPosition4);
        model = glm::scale(model, glm::vec3(programState->roadScale));
        model = glm::rotate(model, glm::radians(0.0f), glm::vec3 (0.0, 1.0f, 0.0f));
        ourShader.setMat4("model", model);
        road4.Draw(ourShader);

        //road5
        model = glm::mat4(1.0f);
        model = glm::translate(model, programState->roadPosition5);
        model = glm::scale(model, glm::vec3(programState->roadScale));
        model = glm::rotate(model, glm::radians(0.0f), glm::vec3 (0.0, 1.0f, 0.0f));
        ourShader.setMat4("model", model);
        road5.Draw(ourShader);

        //road6
        model = glm::mat4(1.0f);
        model = glm::translate(model, programState->roadPosition6);
        model = glm::scale(model, glm::vec3(programState->roadScale));
        model = glm::rotate(model, glm::radians(0.0f), glm::vec3 (0.0, 1.0f, 0.0f));
        ourShader.setMat4("model", model);
        road6.Draw(ourShader);

        //road7
        model = glm::mat4(1.0f);
        model = glm::translate(model, programState->roadPosition7);
        model = glm::scale(model, glm::vec3(40.0f, 40.0f, 150.0f));
        model = glm::rotate(model, glm::radians(0.0f), glm::vec3 (0.0, 1.0f, 0.0f));
        ourShader.setMat4("model", model);
        road7.Draw(ourShader);

        //road8
        model = glm::mat4(1.0f);
        model = glm::translate(model, programState->roadPosition8);
        model = glm::scale(model, glm::vec3(40.0f, 40.0f, 130.0f));
        model = glm::rotate(model, glm::radians(0.0f), glm::vec3 (0.0, 1.0f, 0.0f));
        ourShader.setMat4("model", model);
        road8.Draw(ourShader);

        //road9
        model = glm::mat4(1.0f);
        model = glm::translate(model, programState->roadPosition9);
        model = glm::scale(model, glm::vec3(40.0f, 40.0f, 28.0f));
        model = glm::rotate(model, glm::radians(0.0f), glm::vec3 (0.0, 1.0f, 0.0f));
        ourShader.setMat4("model", model);
        road9.Draw(ourShader);

        //bez bankine prvi
        model = glm::mat4(1.0f);
        model = glm::translate(model, programState->road_without_side_Position);
        model = glm::scale(model, glm::vec3(programState->road_without_side));
        model = glm::rotate(model, glm::radians(225.0f), glm::vec3 (0.0, 1.0f, 0.0f));
        ourShader.setMat4("model", model);
        road_without_side.Draw(ourShader);

        //bez bankine drugi
        model = glm::mat4(1.0f);
        model = glm::translate(model, programState->road1_without_side_Position);
        model = glm::rotate(model, glm::radians(150.0f), glm::vec3 (0.0, 1.0f, 0.0f));
        model = glm::scale(model, glm::vec3(45.0f, 60.0f, 70.0f));
        ourShader.setMat4("model", model);
        road1_without_side.Draw(ourShader);

        //skybox
        glDepthFunc(GL_LEQUAL);
        skyboxShader.use();
        view = glm::mat4(glm::mat3(programState->camera.GetViewMatrix()));
        skyboxShader.setMat4("view", view);
        skyboxShader.setMat4("projection", projection);

        //skybox cube
        glBindVertexArray(skyboxVAO);
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_CUBE_MAP, cubemapTexture);
        glDrawArrays(GL_TRIANGLES, 0, 36);
        glBindVertexArray(0);
        glDepthFunc(GL_LESS);

        if (programState->ImGuiEnabled)
            DrawImGui(programState);

        // glfw: swap buffers and poll IO events (keys pressed/released, mouse moved etc.)
        // -------------------------------------------------------------------------------
        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    programState->SaveToFile("resources/program_state.txt");
    delete programState;
    ImGui_ImplOpenGL3_Shutdown();
    ImGui_ImplGlfw_Shutdown();
    ImGui::DestroyContext();
    // glfw: terminate, clearing all previously allocated GLFW resources.
    // ------------------------------------------------------------------
    glDeleteVertexArrays(1, &skyboxVAO);
    glDeleteVertexArrays(1, &skyboxVBO);

    glfwTerminate();
    return 0;
}

// process all input: query GLFW whether relevant keys are pressed/released this frame and react accordingly
// ---------------------------------------------------------------------------------------------------------
void processInput(GLFWwindow *window) {
    if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
        glfwSetWindowShouldClose(window, true);
    if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
        programState->camera.ProcessKeyboard(FORWARD, 2);
    if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
        programState->camera.ProcessKeyboard(BACKWARD, 2);
    if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
        programState->camera.ProcessKeyboard(LEFT, 2);
    if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
        programState->camera.ProcessKeyboard(RIGHT, 2);
}

// glfw: whenever the window size changed (by OS or user resize) this callback function executes
// ---------------------------------------------------------------------------------------------
void framebuffer_size_callback(GLFWwindow *window, int width, int height) {
    // make sure the viewport matches the new window dimensions; note that width and
    // height will be significantly larger than specified on retina displays.
    glViewport(0, 0, width, height);
}

// glfw: whenever the mouse moves, this callback is called
// -------------------------------------------------------
void mouse_callback(GLFWwindow *window, double xpos, double ypos) {
    if (firstMouse) {
        lastX = xpos;
        lastY = ypos;
        firstMouse = false;
    }

    float xoffset = xpos - lastX;
    float yoffset = lastY - ypos; // reversed since y-coordinates go from bottom to top

    lastX = xpos;
    lastY = ypos;

    if (programState->CameraMouseMovementUpdateEnabled)
        programState->camera.ProcessMouseMovement(xoffset, yoffset);
}

// glfw: whenever the mouse scroll wheel scrolls, this callback is called
// ----------------------------------------------------------------------
void scroll_callback(GLFWwindow *window, double xoffset, double yoffset) {
    programState->camera.ProcessMouseScroll(yoffset);
}

void DrawImGui(ProgramState *programState) {
    ImGui_ImplOpenGL3_NewFrame();
    ImGui_ImplGlfw_NewFrame();
    ImGui::NewFrame();



    {
        ImGui::Begin("Camera info");
        const Camera& c = programState->camera;
        ImGui::Text("Camera position: (%f, %f, %f)", c.Position.x, c.Position.y, c.Position.z);
        ImGui::Text("(Yaw, Pitch): (%f, %f)", c.Yaw, c.Pitch);
        ImGui::Text("Camera front: (%f, %f, %f)", c.Front.x, c.Front.y, c.Front.z);
        ImGui::Checkbox("Camera mouse update", &programState->CameraMouseMovementUpdateEnabled);
        ImGui::End();
    }

    ImGui::Render();
    ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
}

void key_callback(GLFWwindow *window, int key, int scancode, int action, int mods) {
    if (key == GLFW_KEY_F1 && action == GLFW_PRESS) {
        programState->ImGuiEnabled = !programState->ImGuiEnabled;
        if (programState->ImGuiEnabled) {
            programState->CameraMouseMovementUpdateEnabled = false;
            glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
        } else {
            glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
        }
    }

    if (key == GLFW_KEY_N && action == GLFW_PRESS) {
        noc = !noc;
    }

}


unsigned int loadCubemap(vector<std::string> faces)
{
    unsigned int textureID;
    glGenTextures(1, &textureID);
    glBindTexture(GL_TEXTURE_CUBE_MAP, textureID);

    int width, height, nrChannels;
    for (unsigned int i = 0; i < faces.size(); i++)
    {
        unsigned char *data = stbi_load(faces[i].c_str(), &width, &height, &nrChannels, 0);
        if (data)
        {
            glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i,
                         0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, data
            );
            stbi_image_free(data);
        }
        else
        {
            std::cout << "Cubemap tex failed to load at path: " << faces[i] << std::endl;
            stbi_image_free(data);
        }
    }
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);

    return textureID;
}

unsigned int loadTexture(char const * path, bool gammaCorrection)
{
    unsigned int textureID;
    glGenTextures(1, &textureID);

    int width, height, nrComponents;
    unsigned char *data = stbi_load(path, &width, &height, &nrComponents, 0);
    if (data)
    {
        GLenum internalFormat;
        GLenum dataFormat;
        if (nrComponents == 1)
        {
            internalFormat = dataFormat = GL_RED;
        }
        else if (nrComponents == 3)
        {
            internalFormat = gammaCorrection ? GL_SRGB : GL_RGB;
            dataFormat = GL_RGB;
        }
        else if (nrComponents == 4)
        {
            internalFormat = gammaCorrection ? GL_SRGB_ALPHA : GL_RGBA;
            dataFormat = GL_RGBA;
        }

        glBindTexture(GL_TEXTURE_2D, textureID);
        glTexImage2D(GL_TEXTURE_2D, 0, internalFormat, width, height, 0, dataFormat, GL_UNSIGNED_BYTE, data);
        glGenerateMipmap(GL_TEXTURE_2D);

        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

        stbi_image_free(data);
    }
    else
    {
        std::cout << "Texture failed to load at path: " << path << std::endl;
        stbi_image_free(data);
    }

    return textureID;
}

unsigned int quadVAO = 0;
unsigned int quadVBO;
void renderQuad()
{
    if (quadVAO == 0)
    {
        // positions
        glm::vec3 pos1(-1.0f,  1.0f, 0.0f);
        glm::vec3 pos2(-1.0f, -1.0f, 0.0f);
        glm::vec3 pos3( 1.0f, -1.0f, 0.0f);
        glm::vec3 pos4( 1.0f,  1.0f, 0.0f);
        // texture coordinates
        glm::vec2 uv1(0.0f, 1.0f);
        glm::vec2 uv2(0.0f, 0.0f);
        glm::vec2 uv3(1.0f, 0.0f);
        glm::vec2 uv4(1.0f, 1.0f);
        // normal vector
        glm::vec3 nm(0.0f, 0.0f, 1.0f);

        // calculate tangent/bitangent vectors of both triangles
        glm::vec3 tangent1, bitangent1;
        glm::vec3 tangent2, bitangent2;
        // triangle 1
        // ----------
        glm::vec3 edge1 = pos2 - pos1;
        glm::vec3 edge2 = pos3 - pos1;
        glm::vec2 deltaUV1 = uv2 - uv1;
        glm::vec2 deltaUV2 = uv3 - uv1;

        float f = 1.0f / (deltaUV1.x * deltaUV2.y - deltaUV2.x * deltaUV1.y);

        tangent1.x = f * (deltaUV2.y * edge1.x - deltaUV1.y * edge2.x);
        tangent1.y = f * (deltaUV2.y * edge1.y - deltaUV1.y * edge2.y);
        tangent1.z = f * (deltaUV2.y * edge1.z - deltaUV1.y * edge2.z);
        tangent1 = glm::normalize(tangent1);

        bitangent1.x = f * (-deltaUV2.x * edge1.x + deltaUV1.x * edge2.x);
        bitangent1.y = f * (-deltaUV2.x * edge1.y + deltaUV1.x * edge2.y);
        bitangent1.z = f * (-deltaUV2.x * edge1.z + deltaUV1.x * edge2.z);
        bitangent1 = glm::normalize(bitangent1);

        // triangle 2
        // ----------
        edge1 = pos3 - pos1;
        edge2 = pos4 - pos1;
        deltaUV1 = uv3 - uv1;
        deltaUV2 = uv4 - uv1;

        f = 1.0f / (deltaUV1.x * deltaUV2.y - deltaUV2.x * deltaUV1.y);

        tangent2.x = f * (deltaUV2.y * edge1.x - deltaUV1.y * edge2.x);
        tangent2.y = f * (deltaUV2.y * edge1.y - deltaUV1.y * edge2.y);
        tangent2.z = f * (deltaUV2.y * edge1.z - deltaUV1.y * edge2.z);
        tangent2 = glm::normalize(tangent2);

        bitangent2.x = f * (-deltaUV2.x * edge1.x + deltaUV1.x * edge2.x);
        bitangent2.y = f * (-deltaUV2.x * edge1.y + deltaUV1.x * edge2.y);
        bitangent2.z = f * (-deltaUV2.x * edge1.z + deltaUV1.x * edge2.z);
        bitangent2 = glm::normalize(bitangent2);

        float quadVertices[] = {
                // positions            // normal         // texcoords  // tangent                          // bitangent
                pos1.x, pos1.y, pos1.z, nm.x, nm.y, nm.z, uv1.x, uv1.y, tangent1.x, tangent1.y, tangent1.z, bitangent1.x, bitangent1.y, bitangent1.z,
                pos2.x, pos2.y, pos2.z, nm.x, nm.y, nm.z, uv2.x, uv2.y, tangent1.x, tangent1.y, tangent1.z, bitangent1.x, bitangent1.y, bitangent1.z,
                pos3.x, pos3.y, pos3.z, nm.x, nm.y, nm.z, uv3.x, uv3.y, tangent1.x, tangent1.y, tangent1.z, bitangent1.x, bitangent1.y, bitangent1.z,

                pos1.x, pos1.y, pos1.z, nm.x, nm.y, nm.z, uv1.x, uv1.y, tangent2.x, tangent2.y, tangent2.z, bitangent2.x, bitangent2.y, bitangent2.z,
                pos3.x, pos3.y, pos3.z, nm.x, nm.y, nm.z, uv3.x, uv3.y, tangent2.x, tangent2.y, tangent2.z, bitangent2.x, bitangent2.y, bitangent2.z,
                pos4.x, pos4.y, pos4.z, nm.x, nm.y, nm.z, uv4.x, uv4.y, tangent2.x, tangent2.y, tangent2.z, bitangent2.x, bitangent2.y, bitangent2.z
        };
        // configure plane VAO
        glGenVertexArrays(1, &quadVAO);
        glGenBuffers(1, &quadVBO);
        glBindVertexArray(quadVAO);
        glBindBuffer(GL_ARRAY_BUFFER, quadVBO);
        glBufferData(GL_ARRAY_BUFFER, sizeof(quadVertices), &quadVertices, GL_STATIC_DRAW);
        glEnableVertexAttribArray(0);
        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 14 * sizeof(float), (void*)0);
        glEnableVertexAttribArray(1);
        glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 14 * sizeof(float), (void*)(3 * sizeof(float)));
        glEnableVertexAttribArray(2);
        glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 14 * sizeof(float), (void*)(6 * sizeof(float)));
        glEnableVertexAttribArray(3);
        glVertexAttribPointer(3, 3, GL_FLOAT, GL_FALSE, 14 * sizeof(float), (void*)(8 * sizeof(float)));
        glEnableVertexAttribArray(4);
        glVertexAttribPointer(4, 3, GL_FLOAT, GL_FALSE, 14 * sizeof(float), (void*)(11 * sizeof(float)));
    }
    glBindVertexArray(quadVAO);
    glDrawArrays(GL_TRIANGLES, 0, 6);
    glBindVertexArray(0);
}
