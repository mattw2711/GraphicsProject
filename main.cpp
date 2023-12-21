#include <GL/glew.h>
#include <GLFW/glfw3.h>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <algorithm>

#include "Shader.h"
#include "Camera.h"
#include "Model.h"
#include <iostream>

void framebuffer_size_callback(GLFWwindow* window, int width, int height);
void mouse_callback(GLFWwindow* window, double xpos, double ypos);
void scroll_callback(GLFWwindow* window, double xoffset, double yoffset);
void processInput(GLFWwindow *window);

// settings
const unsigned int SCR_WIDTH = 1600;
const unsigned int SCR_HEIGHT = 1200;

// camera
Camera camera(glm::vec3(0.0f, 0.0f, 10.0f));
float lastX = SCR_WIDTH / 2.0f;
float lastY = SCR_HEIGHT / 2.0f;
bool firstMouse = true;

// timing
float deltaTime = 10.0f;
float lastFrame = 0.0f;

int main()
{
    // glfw: initialize and configure
    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

#ifdef __APPLE__
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
#endif

    // glfw window creation
    GLFWwindow* window = glfwCreateWindow(SCR_WIDTH, SCR_HEIGHT, "Graphics Lab", NULL, NULL);
    if (window == NULL)
    {
        std::cout << "Failed to create GLFW window" << std::endl;
        glfwTerminate();
        return -1;
    }
    glfwMakeContextCurrent(window);
    glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);
    glfwSetCursorPosCallback(window, mouse_callback);
    glfwSetScrollCallback(window, scroll_callback);

    // tell GLFW to capture our mouse
    glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);

    // start GLEW extension handler
    glewExperimental = GL_TRUE;
    glewInit();

    // configure global opengl state
    glEnable(GL_DEPTH_TEST);

    // build and compile shaders
    Shader objectShader("res/shaders/vertex.shader", "res/shaders/fragment.shader");

    // load models
    Model base("res/background/background.obj");
    Model bus("res/Bus/Bus.obj");
    Model bus27("res/Bus27/Bus27.obj");
    Model bus122("res/Bus122/Bus122.obj");
    Model dragon("res/dragon/dragon.obj");
    Model spire("res/spire/spire.obj");
    Model fire("res/fire/fire.obj");
    Model luas("res/luas/luas.obj");

    // positions of the point lights
    glm::vec3 pointLightPositions[] = {
        glm::vec3(7.0f, 20.0f, -83.0f), //volcano
        glm::vec3(-5.0f, -1.2f, 0.0f), //bus1
        glm::vec3(30.0f, -1.0f, -50.0f), //bus2
        glm::vec3(20.0f, 0.0f, -15.0f), //spire
        glm::vec3(-30.0f, -1.0f, -20.0f), //bus3
        glm::vec3(45.0f, 0.0f, -40.0f), //luas1
        glm::vec3(-30.0f, 0.0f, -5.0f) //luas2
    };

    
    int numFireballs = 0;

    glm::vec3 fireballPositions[] = {
        glm::vec3(4.0f, -11.0f, -24.0f), //fireball 1
        glm::vec3(-12.0f, -20.0f, -34.0f), //fireball 2
        glm::vec3(-40.0f, -40.0f, -7.0f), //fireball 3
        glm::vec3(57.0f, -70.0f, -15.0f), //fireball 4
        glm::vec3(30.0f, -60.0f, -60.0f), //fireball 5
        glm::vec3(18.0f, -4.0f, -6.0f), //fireball 5
    };

    int numPointLights = sizeof(pointLightPositions)/sizeof(pointLightPositions[0]);
    numFireballs = sizeof(fireballPositions)/sizeof(fireballPositions[0]);

    float fireballDirections[numFireballs];
    std::fill_n(fireballDirections, numFireballs, 0.0f);

    float fireballSizes[] = {
        0.3f,
        0.7f,
        1.0f,
        0.5f,
        1.5f,
    };

    float fireballSpeeds[] = {
        0.08f,
        0.09f,
        0.04f,
        0.06f,
        0.03f,
    };

    // shader configuration
    objectShader.use();
    objectShader.setInt("material.diffuse", 0);
    objectShader.setInt("material.specular", 1);

    // render loop
    while (!glfwWindowShouldClose(window)) {
        // per-frame time logic
        float currentFrame = static_cast<float>(glfwGetTime());
        deltaTime = currentFrame - lastFrame;
        lastFrame = currentFrame;

        // Update fireball positions
        for (int j = 0; j < numFireballs; j++) {
            float speed = fireballSpeeds[j];
            if(fireballPositions[j].y > 5.0f && fireballDirections[j] == 0.0f)
                fireballDirections[j] = 180.0f;
            else if(fireballPositions[j].y < -10.0f && fireballDirections[j] == 180.0f)
                fireballDirections[j] = 0.0f;

            if(fireballDirections[j] == 180.0f)
                fireballPositions[j].y += (-speed);
            else
                fireballPositions[j].y += speed;
        }

        // input
        processInput(window);

        // render
        glClearColor(0.05f, 0.05f, 0.05f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        // don't forget to enable shader before setting uniforms
        objectShader.use();
        objectShader.setVec3("viewPos", camera.Position);
        objectShader.setFloat("material.shininess", 32.0f);

        // directional light
        objectShader.setVec3("dirLight.direction", 0.0f, 30.0f, -40.0f);
        objectShader.setVec3("dirLight.ambient", 0.5f, 0.5f, 0.5f);
        objectShader.setVec3("dirLight.diffuse", 0.4f, 0.4f, 0.4f);
        objectShader.setVec3("dirLight.specular", 0.05f, 0.05f, 0.05f);
        // point light 0 - volcano
        objectShader.setVec3("pointLights[0].position", pointLightPositions[0]);
        objectShader.setVec3("pointLights[0].ambient", 10.0f, 15.0f, 10.0f);
        objectShader.setVec3("pointLights[0].diffuse", 2.0f, 5.0f, 2.0f);
        objectShader.setVec3("pointLights[0].specular", 1.0f, 3.0f, 1.0f);
        objectShader.setFloat("pointLights[0].constant", 0.03f);
        objectShader.setFloat("pointLights[0].linear", 0.2f);
        objectShader.setFloat("pointLights[0].quadratic", 0.2f);
        // point light 1
        objectShader.setVec3("pointLights[1].position", pointLightPositions[1]);
        objectShader.setVec3("pointLights[1].ambient", 0.5f, 0.5f, 0.5f);
        objectShader.setVec3("pointLights[1].diffuse", 0.8f, 0.8f, 0.8f);
        objectShader.setVec3("pointLights[1].specular", 1.0f, 1.0f, 1.0f);
        objectShader.setFloat("pointLights[1].constant", 1.0f);
        objectShader.setFloat("pointLights[1].linear", 0.09f);
        objectShader.setFloat("pointLights[1].quadratic", 0.032f);
        // point light 2
        objectShader.setVec3("pointLights[2].position", pointLightPositions[2]);
        objectShader.setVec3("pointLights[2].ambient", 1.0f, 1.0f, 1.0f);
        objectShader.setVec3("pointLights[2].diffuse", 0.8f, 0.8f, 0.8f);
        objectShader.setVec3("pointLights[2].specular", 1.0f, 1.0f, 1.0f);
        objectShader.setFloat("pointLights[2].constant", 1.0f);
        objectShader.setFloat("pointLights[2].linear", 0.09f);
        objectShader.setFloat("pointLights[2].quadratic", 0.032f);
        // point light 3
        objectShader.setVec3("pointLights[3].position", pointLightPositions[3]);
        objectShader.setVec3("pointLights[3].ambient", 0.5f, 0.5f, 1.0f);
        objectShader.setVec3("pointLights[3].diffuse", 0.8f, 0.8f, 1.0f);
        objectShader.setVec3("pointLights[3].specular", 1.0f, 1.0f, 1.5f);
        objectShader.setFloat("pointLights[3].constant", 1.0f);
        objectShader.setFloat("pointLights[3].linear", 0.09f);
        objectShader.setFloat("pointLights[3].quadratic", 0.032f);
        // point light 4
        objectShader.setVec3("pointLights[4].position", pointLightPositions[4]);
        objectShader.setVec3("pointLights[4].ambient", 0.5f, 0.5f, 0.5f);
        objectShader.setVec3("pointLights[4].diffuse", 0.8f, 0.8f, 0.8f);
        objectShader.setVec3("pointLights[4].specular", 1.0f, 1.0f, 1.0f);
        objectShader.setFloat("pointLights[4].constant", 1.0f);
        objectShader.setFloat("pointLights[4].linear", 0.09f);
        objectShader.setFloat("pointLights[4].quadratic", 0.032f);
        // point light 5
        objectShader.setVec3("pointLights[5].position", pointLightPositions[5]);
        objectShader.setVec3("pointLights[5].ambient", 0.9f, 0.9f, 0.9f);
        objectShader.setVec3("pointLights[5].diffuse", 0.8f, 0.8f, 0.8f);
        objectShader.setVec3("pointLights[5].specular", 1.0f, 1.0f, 1.0f);
        objectShader.setFloat("pointLights[5].constant", 1.0f);
        objectShader.setFloat("pointLights[5].linear", 0.09f);
        objectShader.setFloat("pointLights[5].quadratic", 0.032f);
        // point light 5
        objectShader.setVec3("pointLights[6].position", pointLightPositions[6]);
        objectShader.setVec3("pointLights[6].ambient", 0.7f, 0.8f, 0.8f);
        objectShader.setVec3("pointLights[6].diffuse", 0.8f, 0.8f, 0.8f);
        objectShader.setVec3("pointLights[6].specular", 1.0f, 1.0f, 1.0f);
        objectShader.setFloat("pointLights[6].constant", 1.0f);
        objectShader.setFloat("pointLights[6].linear", 0.09f);
        objectShader.setFloat("pointLights[6].quadratic", 0.032f);

        // view/projection transformations
        glm::mat4 projection = glm::perspective(glm::radians(camera.Zoom), (float)SCR_WIDTH / (float)SCR_HEIGHT, 0.1f, 100.0f);
        glm::mat4 view = camera.GetViewMatrix();
        objectShader.setMat4("projection", projection);
        objectShader.setMat4("view", view);  

        // render the base
        glm::mat4 model = glm::mat4(1.0f);
        model = glm::translate(model, glm::vec3(0.0f, -2.0f, -85.0f));
        model = glm::scale( model, glm::vec3( 3.0f, 3.0f, 3.0f ) );
        model = glm::rotate(model, glm::radians(270.0f), glm::vec3(0.0f, 1.0f ,0.0f));
        objectShader.setMat4("model", model);
        base.Draw(objectShader);

        // render the bus1
        model = glm::mat4(1.0f);
        model = glm::translate(model, glm::vec3(-5.0f, -2.2f, 0.0f));
        model = glm::scale( model, glm::vec3( 1.2f, 1.2f, 1.2f ) );
        model = glm::rotate(model, glm::radians(270.0f), glm::vec3(0.0f, 1.0f ,0.0f));
        model = glm::rotate(model, glm::radians(35.0f), glm::vec3(1.0f, 0.0f ,0.0f));
        model = glm::rotate(model, glm::radians(-40.0f), glm::vec3(1.0f, 0.0f ,1.0f));
        objectShader.setMat4("model", model);
        bus.Draw(objectShader);

        // render the bus2
        model = glm::mat4(1.0f);
        model = glm::translate(model, glm::vec3(30.0f, -1.5f, -50.0f));
        model = glm::scale( model, glm::vec3( 1.2f, 1.2f, 1.2f ) );
        model = glm::rotate(model, glm::radians(50.0f), glm::vec3(0.0f, 1.0f ,0.0f));
        model = glm::rotate(model, glm::radians(135.0f), glm::vec3(1.0f, 0.0f ,0.0f));
        model = glm::rotate(model, glm::radians(-70.0f), glm::vec3(1.0f, 0.0f ,1.0f));
        objectShader.setMat4("model", model);
        bus27.Draw(objectShader);

        // render the bus3
        model = glm::mat4(1.0f);
        model = glm::translate(model, glm::vec3(-30.0f, -1.5f, -20.0f));
        model = glm::scale( model, glm::vec3( 1.2f, 1.2f, 1.2f ) );
        model = glm::rotate(model, glm::radians(150.0f), glm::vec3(0.0f, 1.0f ,0.0f));
        model = glm::rotate(model, glm::radians(20.0f), glm::vec3(1.0f, 0.0f ,0.0f));
        model = glm::rotate(model, glm::radians(-150.0f), glm::vec3(1.0f, 0.0f ,1.0f));
        objectShader.setMat4("model", model);
        bus122.Draw(objectShader);

        // render the luas1
        model = glm::mat4(1.0f);
        model = glm::translate(model, glm::vec3(45.0f, -2.5f, -40.0f));
        model = glm::scale( model, glm::vec3( 3.0f, 3.0f, 3.0f ) );
        model = glm::rotate(model, glm::radians(50.0f), glm::vec3(1.0f, 0.0f, 0.0f));
        model = glm::rotate(model, glm::radians(190.0f), glm::vec3(0.0f, 0.1f, 0.0f));
        model = glm::rotate(model, glm::radians(0.0f), glm::vec3(0.0f, 0.0f, 1.0f));
        objectShader.setMat4("model", model);
        luas.Draw(objectShader);

        // render the luas2
        model = glm::mat4(1.0f);
        model = glm::translate(model, glm::vec3(-30.0f, -2.5f, -5.0f));
        model = glm::scale( model, glm::vec3( 3.0f, 3.0f, 3.0f ) );
        model = glm::rotate(model, glm::radians(30.0f), glm::vec3(1.0f, 0.0f, 0.0f));
        model = glm::rotate(model, glm::radians(130.0f), glm::vec3(0.0f, 0.1f, 0.0f));
        model = glm::rotate(model, glm::radians(-10.0f), glm::vec3(0.0f, 0.0f, 1.0f));
        objectShader.setMat4("model", model);
        luas.Draw(objectShader);

        // render the spire
        model = glm::mat4(1.0f);
        model = glm::translate(model, glm::vec3(20.0f, -12.5f, -20.0f));
        model = glm::scale( model, glm::vec3( 10.0f, 10.0f, 10.0f ) );
        model = glm::rotate(model, glm::radians(266.0f), glm::vec3(0.0f, 1.0f, 0.0f));
        objectShader.setMat4("model", model);
        spire.Draw(objectShader);

        
        for(int i = numPointLights, j = 0; j < numFireballs; i++, j++) {
            // draw the fire light
            objectShader.setVec3("pointLights[" + std::to_string(i) + "].position", fireballPositions[j]);
            objectShader.setVec3("pointLights[" + std::to_string(i) + "].ambient", fireballSizes[j] * .3f, fireballSizes[j] * .3f, fireballSizes[j] * .3f);
            objectShader.setVec3("pointLights[" + std::to_string(i) + "].diffuse", 0.8f, 0.8f, 0.8f);
            objectShader.setVec3("pointLights[" + std::to_string(i) + "].specular", 0.5f, 0.5f, 0.5f);
            objectShader.setFloat("pointLights[" + std::to_string(i) + "].constant", 0.03f);
            objectShader.setFloat("pointLights[" + std::to_string(i) + "].linear", 0.2f);
            objectShader.setFloat("pointLights[" + std::to_string(i) + "].quadratic", 0.2f);
            objectShader.setMat4("model", model);
            glDrawArrays(GL_TRIANGLES, 0, 36);
            
            // render the fire objects
            model = glm::mat4(1.0f);
            model = glm::translate(model, fireballPositions[j]);
            model = glm::scale( model, fireballSizes[j] * glm::vec3( 1.0f, 1.0f, 1.0f ));
            model = glm::rotate(model, glm::radians(fireballDirections[j]), glm::vec3(1.0f, 0.0f, 0.0f));
            objectShader.setMat4("model", model);
            fire.Draw(objectShader);
        }

        // Draw objects lights
        for (unsigned int i = 1; i < numPointLights; i++)
        {
            model = glm::mat4(1.0f);
            model = glm::translate(model, pointLightPositions[i]);
            model = glm::scale(model, glm::vec3(0.2f)); // Make it a smaller cube
            objectShader.setMat4("model", model);
            glDrawArrays(GL_TRIANGLES, 0, 36);
        }
        
        objectShader.setMat4("projection", projection);
        objectShader.setMat4("view", view);
        model = glm::mat4(1.0f);
        model = glm::translate(model, pointLightPositions[0]);
        model = glm::scale(model, glm::vec3(1.0f)); // Make it a smaller cube
        objectShader.setMat4("model", model);
        glDrawArrays(GL_TRIANGLES, 0, 36);

        // glfw: swap buffers and poll IO events (keys pressed/released, mouse moved etc.)
        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    // glfw: terminate, clearing all previously allocated GLFW resources.
    glfwTerminate();
    return 0;
}

// process all input: query GLFW whether relevant keys are pressed/released this frame and react accordingly
void processInput(GLFWwindow *window)
{
    if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
        glfwSetWindowShouldClose(window, true);

    if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
        camera.ProcessKeyboard(FORWARD, deltaTime);
    if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
        camera.ProcessKeyboard(BACKWARD, deltaTime);
    if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
        camera.ProcessKeyboard(LEFT, deltaTime);
    if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
        camera.ProcessKeyboard(RIGHT, deltaTime);
}

// glfw: whenever the window size changed (by OS or user resize) this callback function executes
void framebuffer_size_callback(GLFWwindow* window, int width, int height)
{
    glViewport(0, 0, width, height);
}

// glfw: whenever the mouse moves, this callback is called
void mouse_callback(GLFWwindow* window, double xposIn, double yposIn)
{
    float xpos = static_cast<float>(xposIn);
    float ypos = static_cast<float>(yposIn);

    if (firstMouse)
    {
        lastX = xpos;
        lastY = ypos;
        firstMouse = false;
    }

    float xoffset = xpos - lastX;
    float yoffset = lastY - ypos;

    lastX = xpos;
    lastY = ypos;

    camera.ProcessMouseMovement(xoffset, yoffset);
}

// glfw: whenever the mouse scroll wheel scrolls, this callback is called
void scroll_callback(GLFWwindow* window, double xoffset, double yoffset)
{
    camera.ProcessMouseScroll(static_cast<float>(yoffset));
}