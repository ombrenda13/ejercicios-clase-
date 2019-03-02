/*---------------------------------------------------------*/
/* ----------------   Pr�ctica 4 --------------------------*/
/*-----------------    2019-2   ---------------------------*/
/*------------- Computaci�n gr�fica e interacci�n humano computadora  ---------------*/
#include <glew.h>
#include <glfw3.h>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include <shader_m.h>

#include <iostream>

void resize(GLFWwindow* window, int width, int height);
void my_input(GLFWwindow *window);

// settings
// Window size
int SCR_WIDTH = 3800;
int SCR_HEIGHT = 7600;

GLFWmonitor *monitors;
GLuint VBO, VAO, EBO;

void myData(void);
void display(void);
void getResolution(void);

//For Keyboard
float	movX = 0.0f,
movY = 0.0f,
movZ = -5.0f;
float angX = 0.0f;
float angY = 0.0f;
void getResolution()
{
	const GLFWvidmode * mode = glfwGetVideoMode(glfwGetPrimaryMonitor());

	SCR_WIDTH = mode->width;
	SCR_HEIGHT = (mode->height) - 80;
}

void myData()
{
	GLfloat vertices[] = {
		//Position				//Color
		-0.5f, -0.5f, 0.5f,		1.0f, 0.0f, 0.0f,	//V0 - Frontal
		0.5f, -0.5f, 0.5f,		1.0f, 0.0f, 0.0f,	//V1
		0.5f, 0.5f, 0.5f,		1.0f, 0.0f, 0.0f,	//V5
		-0.5f, 0.5f, 0.5f,		1.0f, 0.0f, 0.0f,	//V4

		0.5f, -0.5f, -0.5f,		1.0f, 1.0f, 0.0f,	//V2 - Trasera
		-0.5f, -0.5f, -0.5f,	1.0f, 1.0f, 0.0f,	//V3
		-0.5f, 0.5f, -0.5f,		1.0f, 1.0f, 0.0f,	//V7
		0.5f, 0.5f, -0.5f,		1.0f, 1.0f, 0.0f,	//V6

		-0.5f, 0.5f, 0.5f,		0.0f, 0.0f, 1.0f,	//V4 - Izq
		-0.5f, 0.5f, -0.5f,		0.0f, 0.0f, 1.0f,	//V7
		-0.5f, -0.5f, -0.5f,	0.0f, 0.0f, 1.0f,	//V3
		-0.5f, -0.5f, 0.5f,		0.0f, 0.0f, 1.0f,	//V0

		0.5f, 0.5f, 0.5f,		0.0f, 1.0f, 0.0f,	//V5 - Der
		0.5f, -0.5f, 0.5f,		0.0f, 1.0f, 0.0f,	//V1
		0.5f, -0.5f, -0.5f,		0.0f, 1.0f, 0.0f,	//V2
		0.5f, 0.5f, -0.5f,		0.0f, 1.0f, 0.0f,	//V6

		-0.5f, 0.5f, 0.5f,		1.0f, 0.0f, 1.0f,	//V4 - Sup
		0.5f, 0.5f, 0.5f,		1.0f, 0.0f, 1.0f,	//V5
		0.5f, 0.5f, -0.5f,		1.0f, 0.0f, 1.0f,	//V6
		-0.5f, 0.5f, -0.5f,		1.0f, 0.0f, 1.0f,	//V7

		-0.5f, -0.5f, 0.5f,		1.0f, 1.0f, 1.0f,	//V0 - Inf
		-0.5f, -0.5f, -0.5f,	1.0f, 1.0f, 1.0f,	//V3
		0.5f, -0.5f, -0.5f,		1.0f, 1.0f, 1.0f,	//V2
		0.5f, -0.5f, 0.5f,		1.0f, 1.0f, 1.0f,	//V1
	};

	unsigned int indices[] =	//I am not using index for this session
	{
		0
	};

	glGenVertexArrays(1, &VAO);
	glBindVertexArray(VAO);

	glGenBuffers(1, &VBO);

	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

	// position attribute
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)0);
	glEnableVertexAttribArray(0);
	// color attribute
	//glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)(3 * sizeof(float)));
	//glEnableVertexAttribArray(1);

	//Para trabajar con indices (Element Buffer Object)
	glGenBuffers(1, &EBO);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);

	glBindBuffer(GL_ARRAY_BUFFER, 0);

	glBindVertexArray(0);

}

void display(void)
{
	//Shader myShader("shaders/shader.vs", "shaders/shader.fs");
	Shader projectionShader("shaders/shader_projection.vs", "shaders/shader_projection.fs");

	projectionShader.use();

	// create transformations and Projection
	glm::mat4 model = glm::mat4(1.0f);		// initialize Matrix, Use this matrix for individual models
	glm::mat4 view = glm::mat4(1.0f);		//Use this matrix for ALL models
	glm::mat4 projection = glm::mat4(1.0f);	//This matrix is for Projection
	glm::mat4 modeltemp = model;


	//Use "projection" in order to change how we see the information
	projection = glm::perspective(glm::radians(45.0f), (float)SCR_WIDTH / (float)SCR_HEIGHT, 0.1f, 100.0f);
	//projection = glm::ortho(-5.0f, 5.0f, -3.0f, 3.0f, 0.1f, 10.0f);

	//Use "view" in order to affect all models
	view = glm::translate(view, glm::vec3(movX, movY, movZ));
	view = glm::rotate(view, glm::radians(angX), glm::vec3(0, 1, 0));//grado de rotacion,x,y,z
	view = glm::rotate(view, glm::radians(angY), glm::vec3(1, 0, 0));
	// pass them to the shaders
	projectionShader.setMat4("model", model);
	projectionShader.setMat4("view", view);
	// note: currently we set the projection matrix each frame, but since the projection matrix rarely changes it's often best practice to set it outside the main loop only once.
	projectionShader.setMat4("projection", projection);


	glBindVertexArray(VAO);

	projectionShader.setVec3("aColor", glm::vec3(1.0f, 0.0f, 0.0f));
	model = glm::scale(model, glm::vec3(1.5, 2.5, 1));// para hacer escalas al objeto x,y,z
	projectionShader.setMat4("model", model);
	glDrawArrays(GL_QUADS, 0, 24); //cabeza

	model = glm::translate(model, glm::vec3(0, -.5, 0));
	projectionShader.setVec3("aColor", glm::vec3(1.0f, 0.0f, 1.0f));
	model = glm::scale(model, glm::vec3(0.5, 0.5, 1));// para hacer escalas al objeto x,y,z
	projectionShader.setMat4("model", model);
	glDrawArrays(GL_QUADS, 0, 24); //cuello

	model = glm::translate(model, glm::vec3(0, -2, 0));
	projectionShader.setVec3("aColor", glm::vec3(0.0f, 0.0f, 1.0f));
	model = glm::scale(model, glm::vec3(4, 3, 1));// para hacer escalas al objeto x,y,z
	projectionShader.setMat4("model", model);
	glDrawArrays(GL_QUADS, 0, 24); //torso

	model = glm::translate(model, glm::vec3(-.7, .4, 0));
	model = glm::scale(model, glm::vec3(.5, .2, 1));// para hacer escalas al objeto x,y,z
	projectionShader.setVec3("aColor", glm::vec3(0.0f, 1.0f, 0.0f));
	projectionShader.setMat4("model", model);
	glDrawArrays(GL_QUADS, 0, 24); //hombro izquierdo

	model = glm::translate(model, glm::vec3(-0.25, -2, 0));
	model = glm::scale(model, glm::vec3(0.5, 3, 1));// para hacer escalas al objeto x,y,z
	projectionShader.setVec3("aColor", glm::vec3(1.0f, 0.0f, 2.0f));
	projectionShader.setMat4("model", model);
	glDrawArrays(GL_QUADS, 0, 24); //brazo izquierdo

	model = glm::translate(model, glm::vec3(6.3, .67, 0));
	projectionShader.setVec3("aColor", glm::vec3(0.0f, 1.0f, 0.0f));
	model = glm::scale(model, glm::vec3(2, .33, 1));// para hacer escalas al objeto x,y,z
	projectionShader.setMat4("model", model);

	glDrawArrays(GL_QUADS, 0, 24); //hombro derecho

	model = glm::translate(model, glm::vec3(.25, -2, 0));
	projectionShader.setVec3("aColor", glm::vec3(1.0f, 0.0f, 2.0f));
	model = glm::scale(model, glm::vec3(.5, 3, 1));// para hacer escalas al objeto x,y,z
	projectionShader.setMat4("model", model);
	glDrawArrays(GL_QUADS, 0, 24); //brazo derecho

	model = glm::translate(model, glm::vec3(-3.5, -1, 0));
	projectionShader.setVec3("aColor", glm::vec3(2.5f, 2.5f, 0.0f));
	model = glm::scale(model, glm::vec3(4, .66, 1));// para hacer escalas al objeto x,y,z
	projectionShader.setMat4("model", model);
	glDrawArrays(GL_QUADS, 0, 24); //cinturon

	model = glm::translate(model, glm::vec3(.35, -2, 0));
	projectionShader.setVec3("aColor", glm::vec3(1.0f, 1.0f, 1.0f));
	model = glm::scale(model, glm::vec3(.3, 3, 1));// para hacer escalas al objeto x,y,z
	projectionShader.setMat4("model", model);
	glDrawArrays(GL_QUADS, 0, 24); //pierna derecha 

	model = glm::translate(model, glm::vec3(-2.3, 0, 0));
	projectionShader.setVec3("aColor", glm::vec3(1.0f, 1.0f, 1.0f));
	model = glm::scale(model, glm::vec3(1, 1, 1));// para hacer escalas al objeto x,y,z
	projectionShader.setMat4("model", model);
	glDrawArrays(GL_QUADS, 0, 24); //pierna izquierda

model = glm::translate(model, glm::vec3(-.85, -.7, 0));
	model = glm::scale(model, glm::vec3(2.7,0.4,1));// para hacer escalas al objeto x,
	projectionShader.setVec3("aColor", glm::vec3(1.02f, 0.5f, 0.0f));
	projectionShader.setMat4("model", model);
	glDrawArrays(GL_QUADS, 0, 24); //pie izq

	model = glm::translate(model, glm::vec3(1.48, 0, 0));
	model = glm::scale(model, glm::vec3(1, 1, 1));// para hacer escalas al objeto x,
	projectionShader.setVec3("aColor", glm::vec3(1.02f, 0.5f, 0.0f));
	projectionShader.setMat4("model", model);
	glDrawArrays(GL_QUADS, 0, 24); //pierna izquierda


	//model = glm::translate(model, glm::vec3(3, 0, 0));
	//model = glm::scale(model,glm::vec3(2,2,5));
	//projectionShader.setVec3("aColor", glm::vec3(0.0f, 1.0f, 0.0f));
	//projectionShader.setMat4("model", model);

	//glDrawArrays(GL_QUADS,0,24);


	//model = glm::translate(model, glm::vec3(-2, 0, 0)); //se usa 2 para que la transformacion de 6 


//projectionShader.setVec3("aColor", glm::vec3(1.0f, 1.0f, 0.0f));
	//projectionShader.setMat4("model", model);

	//glDrawArrays(GL_QUADS, 0, 24);


	glBindVertexArray(0);

}

int main()
{
	// glfw: initialize and configure
	// ------------------------------
	glfwInit();
	/*glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);*/

#ifdef __APPLE__
	glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE); // uncomment this statement to fix compilation on OS X
#endif

	// glfw window creation
	// --------------------
	monitors = glfwGetPrimaryMonitor();
	getResolution();

	GLFWwindow* window = glfwCreateWindow(SCR_WIDTH, SCR_HEIGHT, "Practica 4", NULL, NULL);
	if (window == NULL)
	{
		std::cout << "Failed to create GLFW window" << std::endl;
		glfwTerminate();
		return -1;
	}
	glfwSetWindowPos(window, 0, 30);
	glfwMakeContextCurrent(window);
	glfwSetFramebufferSizeCallback(window, resize);

	glewInit();


	//Mis funciones
	//Datos a utilizar
	myData();
	glEnable(GL_DEPTH_TEST);

	// render loop
	// While the windows is not closed
	while (!glfwWindowShouldClose(window))
	{
		// input
		// -----
		my_input(window);

		// render
		// Backgound color
		glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		//Mi funci�n de dibujo
		display();

		// glfw: swap buffers and poll IO events (keys pressed/released, mouse moved etc.)
		// -------------------------------------------------------------------------------
		glfwSwapBuffers(window);
		glfwPollEvents();
	}

	// glfw: terminate, clearing all previously allocated GLFW resources.
	// ------------------------------------------------------------------
	glfwTerminate();
	return 0;
}

// process all input: query GLFW whether relevant keys are pressed/released this frame and react accordingly
// ---------------------------------------------------------------------------------------------------------
void my_input(GLFWwindow *window)
{
	if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)  //GLFW_RELEASE
		glfwSetWindowShouldClose(window, true);
	if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
		movX += 0.08f;
	if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
		movX -= 0.08f;
	if (glfwGetKey(window, GLFW_KEY_PAGE_UP) == GLFW_PRESS)
		movY += 0.08f;
	if (glfwGetKey(window, GLFW_KEY_PAGE_DOWN) == GLFW_PRESS)
		movY -= 0.08f;
	if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
		movZ -= 0.08f;
	if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
		movZ += 0.08f;
	if (glfwGetKey(window, GLFW_KEY_LEFT) == GLFW_PRESS)
		angX -= 1.5f;
	if (glfwGetKey(window, GLFW_KEY_RIGHT) == GLFW_PRESS)
		angX += 1.5f;
	if (glfwGetKey(window, GLFW_KEY_UP) == GLFW_PRESS)
		angY -= 1.5f;
	if (glfwGetKey(window, GLFW_KEY_DOWN) == GLFW_PRESS)
		angY += 1.5f;
}

// glfw: whenever the window size changed (by OS or user resize) this callback function executes
// ---------------------------------------------------------------------------------------------
void resize(GLFWwindow* window, int width, int height)
{
	// Set the Viewport to the size of the created window
	glViewport(0, 0, width, height);
}