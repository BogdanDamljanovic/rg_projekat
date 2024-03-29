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

unsigned int loadTexture(char const * path);

// settings
const unsigned int SCR_WIDTH = 800;
const unsigned int SCR_HEIGHT = 600;

// camera
float lastX = SCR_WIDTH / 2.0f;
float lastY = SCR_HEIGHT / 2.0f;
bool firstMouse = true;

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
    glm::vec3 ponyPosition = glm::vec3(-10, -0.3, -83);
    glm::vec3 dodgePosition = glm::vec3(25, 15, -760);
    glm::vec3 garagePosition = glm::vec3(-140, 0, -30);
    glm::vec3 dinerPosition = glm::vec3(-80, 0, -750);
    glm::vec3 roadPosition = glm::vec3(370, 0, -50);
    glm::vec3 roadPosition1 = glm::vec3(370, 0, -659);
    glm::vec3 roadPosition2 = glm::vec3(370, 0, -1268);
    glm::vec3 roadPosition4 = glm::vec3(370, 0, -1877);
    glm::vec3 roadPosition3 = glm::vec3(370, 0, 559);

    float dinerScale = 7;
    float ponyScale = 0.2;
    float dodgeScale = 35;
    float garageScale = 1.05;
    float roadScale = 40;

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

    // build and compile shaders
    // -------------------------
    Shader ourShader("resources/shaders/2.model_lighting.vs", "resources/shaders/2.model_lighting.fs");
    Shader skyboxShader("resources/shaders/skybox.vs", "resources/shaders/skybox.fs");
    Shader planeShader("resources/shaders/planeShader.vs", "resources/shaders/planeShader.fs");

    // load models
    // -----------

    Model garage("resources/objects/garage/garage.obj");
    garage.SetShaderTextureNamePrefix("material.");

    Model diner("resources/objects/gas_station/DioramaDiner.obj");
    diner.SetShaderTextureNamePrefix("material.");

    Model pony("resources/objects/pony_car/Pony_cartoon.obj");
    pony.SetShaderTextureNamePrefix("material.");

    Model dodge("resources/objects/dodge/dodge.obj");
    dodge.SetShaderTextureNamePrefix("material.");

    Model road("resources/objects/road/road.obj");
    road.SetShaderTextureNamePrefix("material.");

    Model road1("resources/objects/road/road.obj");
    road1.SetShaderTextureNamePrefix("material.");

    Model road2("resources/objects/road/road.obj");
    road2.SetShaderTextureNamePrefix("material.");

    Model road3("resources/objects/road/road.obj");
    road3.SetShaderTextureNamePrefix("material.");

    Model road4("resources/objects/road/road.obj");
    road4.SetShaderTextureNamePrefix("material.");

    PointLight& pointLight1 = programState->pointLight[0];
    pointLight1.position = glm::vec3(50, 35, -109);
    pointLight1.ambient = glm::vec3(10.0, 10.0, 10.0);
    pointLight1.diffuse = glm::vec3(0.9, 0.9, 0.9);
    pointLight1.specular = glm::vec3(1.0, 1.0, 1.0);

    pointLight1.constant = 17.0f;
    pointLight1.linear = 0.09f;
    pointLight1.quadratic = 0.032f;

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

    unsigned int groundTexture = loadTexture("resources/textures/zemlja4.jpg");
    planeShader.use();
    planeShader.setInt("texture1", 0);

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
    //skybox
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


        planeShader.setVec3("dirLight.direction", dirLight.direction);
        planeShader.setVec3("dirLight.ambient", dirLight.ambient);
        planeShader.setVec3("dirLight.diffuse", dirLight.diffuse);
        planeShader.setVec3("dirLight.specular", dirLight.specular);
        planeShader.setFloat("shininess", 1.0f);
        // planeShader.setBool("noc", noc);


        glDrawArrays(GL_TRIANGLES, 0, 6);
        glBindVertexArray(0);


        // don't forget to enable shader before setting uniforms
        ourShader.use();

        // pointLight1
        ourShader.setVec3("pointLight1.position", pointLight1.position);
        ourShader.setVec3("pointLight1.ambient", pointLight1.ambient);
        ourShader.setVec3("pointLight1.diffuse", pointLight1.diffuse);
        ourShader.setVec3("pointLight1.specular", pointLight1.specular);
        ourShader.setFloat("pointLight1.constant", pointLight1.constant);
        ourShader.setFloat("pointLight1.linear", pointLight1.linear);
        ourShader.setFloat("pointLight1.quadratic", pointLight1.quadratic);
        // pointLight2
        ourShader.setVec3("pointLight2.position", pointLight2.position);
        ourShader.setVec3("pointLight2.ambient", pointLight2.ambient);
        ourShader.setVec3("pointLight2.diffuse", pointLight2.diffuse);
        ourShader.setVec3("pointLight2.specular", pointLight2.specular);
        ourShader.setFloat("pointLight2.constant", pointLight2.constant);
        ourShader.setFloat("pointLight2.linear", pointLight2.linear);
        ourShader.setFloat("pointLight2.quadratic", pointLight2.quadratic);
        // pointLight3
        ourShader.setVec3("pointLight3.position", pointLight3.position);
        ourShader.setVec3("pointLight3.ambient", pointLight3.ambient);
        ourShader.setVec3("pointLight3.diffuse", pointLight3.diffuse);
        ourShader.setVec3("pointLight3.specular", pointLight3.specular);
        ourShader.setFloat("pointLight3.constant", pointLight3.constant);
        ourShader.setFloat("pointLight3.linear", pointLight3.linear);
        ourShader.setFloat("pointLight3.quadratic", pointLight3.quadratic);

        // directional light
        ourShader.setVec3("dirLight.direction", dirLight.direction);
        ourShader.setVec3("dirLight.ambient", dirLight.ambient);
        ourShader.setVec3("dirLight.diffuse", dirLight.diffuse);
        ourShader.setVec3("dirLight.specular", dirLight.specular);
        ourShader.setFloat("shininess", 1.0f);

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

        //dodge
        model = glm::mat4(1.0f);
        model = glm::translate(model, programState->dodgePosition);
        model = glm::scale(model, glm::vec3(programState->dodgeScale));
        model = glm::rotate(model, glm::radians(-90.0f), glm::vec3 (0.0, 1.0f, 0.0f));
        ourShader.setMat4("model", model);
        dodge.Draw(ourShader);

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
        static float f = 0.0f;
        ImGui::Begin("Hello window");
        ImGui::Text("Hello text");
        ImGui::SliderFloat("Float slider", &f, 0.0, 1.0);
        ImGui::ColorEdit3("Background color", (float *) &programState->clearColor);
        ImGui::DragFloat3("mammoth1 position", (float*)&programState->ponyPosition);
        ImGui::DragFloat("mammoth1 scale", &programState->ponyScale, 0.05, 0.1, 4.0);


        ImGui::DragFloat("pointLight.constant", &programState->pointLight[0].constant, 0.05, 0.0, 1.0);
        ImGui::DragFloat("pointLight.linear", &programState->pointLight[0].linear, 0.05, 0.0, 1.0);
        ImGui::DragFloat("pointLight.quadratic", &programState->pointLight[0].quadratic, 0.05, 0.0, 1.0);
        ImGui::End();
    }

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

unsigned int loadTexture(char const * path)
{
    unsigned int textureID;
    glGenTextures(1, &textureID);

    int width, height, nrComponents;
    unsigned char *data = stbi_load(path, &width, &height, &nrComponents, 0);
    if (data)
    {
        GLenum format = GL_RED;
        if (nrComponents == 1)
            format = GL_RED;
        else if (nrComponents == 3)
            format = GL_RGB;
        else if (nrComponents == 4)
            format = GL_RGBA;

        glBindTexture(GL_TEXTURE_2D, textureID);
        glTexImage2D(GL_TEXTURE_2D, 0, (GLint)format, width, height, 0, format, GL_UNSIGNED_BYTE, data);
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
