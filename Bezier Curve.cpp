#include <iostream>
#include <complex>
#include <vector>
#include <array>

#include <glad/glad.h>
#include <glfw3.h>

#include <glm/glm/glm.hpp>
#include <glm/glm/gtc/matrix_transform.hpp>
#include <glm/glm/gtc/type_ptr.hpp>

#include <random> // for std::mt19937
#include <ctime> // for std::time
#include <cmath>

#include "SHADER_H.h"
#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"


glm::vec3 cameraPos = glm::vec3(0.0f, 1.5f, 4.5f);
glm::vec3 cameraFront = glm::vec3(0.0f, 0.0, -1.0f);
glm::vec3 cameraUp = glm::vec3(0.0f, 1.0f, 0.0f);

//float lastX = 400, lastY = 300;
float yaw{ -90.0f }, pitch;
float lastX = 400, lastY = 300;
void mouse_callback(GLFWwindow* window, double xpos, double ypos);
bool firstMove = true;

float deltaTime = 0.0f; // Time between current frame and last frame
float lastFrame = 0.0f; // Time of last frame



void processInput(GLFWwindow* window);
void framebuffer_size_callback(GLFWwindow* window, int width, int height);



   std::vector<float> bezierFunction(const float* p0, const float* p1, const float* p2, float* p3,float t)
{
	std::vector<float> bezierPoints {p0[0],p0[1],p0[2]};
    bezierPoints.reserve(500);
	
	
	for (float i{ 1 }; i < t; ++i)
	{
		
		bezierPoints.push_back( pow(1 - i / t, 3) * p0[0] + 3.0f * pow(1 - i / t, 2) * i / t * p1[0] + 3.0f * (1 - i / t) * i / t * i / t * p2[0]*1.0f + 1.0f*i / t * i / t * i / t * p3[0]);
		bezierPoints.push_back(pow(1 - i / t, 3) * p0[1] + 3.0f * pow(1 - i / t, 2) * i / t * p1[1] + 3.0f * (1 - i / t) * i / t * i / t * p2[1]*1.0f + 1.0f*i / t * i / t * i / t * p3[1]);
		bezierPoints.push_back( pow(1 - i / t, 3) * p0[2] + 3.0f * pow(1 - i / t, 2) * i / t * p1[2] + 3.0f * (1 - i / t) * i / t * i / t * p2[2]*1.0f + 1.0f*i / t * i / t * i / t * p3[2]);
		
	}
	std::cout << bezierPoints.size();
	return bezierPoints;
}



int main()
{
	glfwInit(); // Initializes glfW library
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

	GLFWwindow* window = glfwCreateWindow(800, 600, "GLF Window", NULL, NULL);
	if (window == NULL)
	{
		std::cout << "Failed to create GLFW window\n";
		glfwTerminate();
		return -1;
	}

	glfwMakeContextCurrent(window); // Making this Window's context current for the calling thread


	// Glad manages function pointers to OpenGL functions --> Initializw GLAD
	if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
	{
		std::cout << "Failed to initialize GLAD" << std::endl;
		return -1;
	}

	//Telling OpenGL the size of the rendering window
	glViewport(0, 0, 800, 600);
	//Once the user resized window, viewPort should readjust itself---->
	glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);

	float p0[3]{ 1.0f,0.0f,0.0f };
	float p1[3]{ 3.0f,3.0f,0.0f };
	float p2[3]{ 6.0f,1.0f,0.0f };
	float p3[3]{ 4.0f,0.0f,0.0f };
	float t = 100;

	std::vector<float> coord = bezierFunction(p0, p1, p2, p3, t) ;
	//coord.reserve(500);
	float theCurve[500]{};

	int count{ 0 };

	for (int i{ 0 }; i < coord.size() / 3; ++i)
	{


		theCurve[count] = coord.at(count);
		theCurve[count + 1] = coord.at(count + 1);
		theCurve[count + 2] = coord.at(count + 2);
		count += 3;

	}



	
	
	unsigned int VAO, VBO;
	glGenVertexArrays(1, &VAO);
	glBindVertexArray(VAO);
	glGenBuffers(1, &VBO);
	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(theCurve), theCurve, GL_STATIC_DRAW);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
	glEnableVertexAttribArray(0);

	Shader bezierShader ("C:/OpenGL/shaders/bezier/vertex.txt", "C:/OpenGL/shaders/bezier/frag.txt");
	



	//Creating complete transformation
	glm::mat4 model = glm::mat4(1.0f);
	model = glm::translate(model, glm::vec3(0.0f, 0.0f, 0.0f));
	glm::mat4 view = glm::mat4(1.0f);
	view = glm::translate(view, glm::vec3(0.0f, 10.0f, 5.0f));
	glm::mat4 projection;
	projection = glm::perspective(glm::radians(45.0f), (800.0f / 600.0f), 0.1f, 100.0f);
	unsigned int modelLoc = glGetUniformLocation(bezierShader.ID, "model");
	unsigned int viewLoc = glGetUniformLocation(bezierShader.ID, "view");
	unsigned int projectionLoc = glGetUniformLocation(bezierShader.ID, "projection");

	unsigned int colorLoc = glGetUniformLocation(bezierShader.ID, "color");
	

	glEnable(GL_DEPTH_TEST);


	while (!glfwWindowShouldClose(window))
	{
		float currentFrame = glfwGetTime();
		deltaTime = currentFrame - lastFrame;
		lastFrame = currentFrame;
		

		glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
		glfwSetCursorPosCallback(window, mouse_callback);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);


		processInput(window);
		glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT);

		bezierShader.use();
		unsigned int modelLoc = glGetUniformLocation(bezierShader.ID, "model");
		unsigned int viewLoc = glGetUniformLocation(bezierShader.ID, "view");
		unsigned int projectionLoc = glGetUniformLocation(bezierShader.ID, "projection");
		view = glm::lookAt(cameraPos, cameraPos + cameraFront, cameraUp);
		//glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));
		glUniformMatrix4fv(viewLoc, 1, GL_FALSE, glm::value_ptr(view));
		glUniformMatrix4fv(projectionLoc, 1, GL_FALSE, glm::value_ptr(projection));

		// the Curve

		glBindVertexArray(VAO);
		for (int i{ 1 }; i < 360; ++i)
		{
			glm::mat4 model = glm::mat4(1.0f);
			//model = glm::translate(model, glm::vec3(0.0f, 0.0f, 0.0f));
			model = glm::rotate(model, float(glm::radians(1.0f*i)), glm::vec3(4.0, 0.0, 0.0));
			glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));
			glm::vec3 clr{ cos(i * 1.0f/255.0f*float(glfwGetTime())*1.0f), 1.0f*sin(i * 1.0f/255.0f*float(glfwGetTime())),sin(i/255.0f)*1.0f};
			glUniform3fv(colorLoc, 1,&clr[0]);

			glDrawArrays(GL_POINTS, 0, coord.size());
		}

		for (int i{ 1 }; i < 360; i += 10)
		{
			glm::mat4 model = glm::mat4(1.0f);
			model = glm::translate(model, glm::vec3(5.0f, 0.0f, 5.0f));
			model = glm::rotate(model, float(glm::radians(1.0f * i)), glm::vec3(4.0, 0.0, 0.0));
			glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));
			glDrawArrays(GL_LINE_LOOP, 0, coord.size());
		}




		glfwSwapBuffers(window);
		glfwPollEvents();



	}

	glfwTerminate();

	return 0;



}


void processInput(GLFWwindow* window)
{
	float cameraSpeed = 0.45f * deltaTime;

	if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
		glfwSetWindowShouldClose(window, true);
	if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
	{
		//if (cameraPos.y = 0.3f)
		cameraPos += cameraSpeed * cameraFront;
	}
	if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
	{
		//if (cameraPos.y = 0.3f)
		cameraPos -= cameraSpeed * cameraFront;
	}
	if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
		cameraPos += glm::cross(cameraFront, cameraUp) * cameraSpeed;
	if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
		cameraPos -= glm::cross(cameraFront, cameraUp) * cameraSpeed;


}

//Once the user resized window, viewPort should readjust itself---->
void framebuffer_size_callback(GLFWwindow* window, int width, int height)
{
	glViewport(0, 0, width, height);

}

void mouse_callback(GLFWwindow* window, double xpos, double ypos)
{

	if (firstMove)
	{
		lastX = xpos;
		lastY = ypos;
		firstMove = false;
	}

	float xoffset = xpos - lastX;
	float yoffest = lastY - ypos;

	lastX = xpos;
	lastY = ypos;
	const float sensitivity = 0.05f;
	xoffset *= sensitivity;
	yoffest *= sensitivity;

	yaw += xoffset;
	pitch += yoffest;

	if (pitch > 89.0f)
		pitch = 89.0f;
	if (pitch < -89.0f)
		pitch = -89.0f;

	glm::vec3 direction;
	direction.x = cos(glm::radians(yaw)) * cos(glm::radians(pitch));
	direction.y = sin(glm::radians(pitch));
	direction.z = sin(glm::radians(yaw));
	cameraFront = glm::normalize(direction);


}

