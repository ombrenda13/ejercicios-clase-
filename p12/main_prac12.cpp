/*---------------------------------------------------------*/
/* ----------------   Pr�ctica 12 --------------------------*/
/*-----------------    2019-2   ---------------------------*/
/*------------- Alumno:  ORTIZ MART�NEZ BRENDA                    ---------------*/
/*INSTRUCCIONES:
	TECLA P: coche avanza
	TECLA O: coche se detiene
	TECLA I: coche retrocede
	MOUSE: c�mara
	TECLA D: movimiento de la vista hacia la derecha
	TECLA A: movimiento camara izquierda.
	TECLA S: alejar c�mara
	Tecla w: acercar camara


	tecla Y movimiento hacia adelante en posici�n Z
tecla H movimiento hacia atr�s en posici�n Z
tecla J movimiento hacia atr�s en posici�n X
tecla G movimiento hacia adelante en posici�n X
tecla X movimiento pierna izq hacia atr�s
tecla C movimiento pierna izq hacia adelante
tecla M movimiento pierna der hacia atr�s
tecla N movimiento pierna der hacia adelante
tecla T movimiento brazo der hacia atr�s
tecla R movimiento brazo der hacia adelante
tecla 1 movimiento brazo izq hacia atr�s
tecla 2 movimiento brazo izq hacia adelante
tecla 3 movimiento cabeza izq
tecla 4 movimiento cabeza der
tecla V girar cuerpo hac�a izq
tecla B girar cuerpo hac�a der
*/
//#define STB_IMAGE_IMPLEMENTATION
#include <glew.h>
#include <glfw3.h>
#include <stb_image.h>

#include "camera.h"
#include "Model.h"

// Other Libs
#include "SOIL2/SOIL2.h"

void resize(GLFWwindow* window, int width, int height);
void my_input(GLFWwindow *window, int key, int scancode, int action, int mode);
void mouse_callback(GLFWwindow *window, double xpos, double ypos);
void scroll_callback(GLFWwindow *window, double xoffset, double yoffset);

// settings
// Window size
int SCR_WIDTH = 3800;
int SCR_HEIGHT = 7600;

GLFWmonitor *monitors;
GLuint VBO, VAO, EBO;

//Camera
Camera camera(glm::vec3(0.0f, 0.0f, 13.0f));
double	lastX = 0.0f,
		lastY = 0.0f;
bool firstMouse = true;

//Timing
double	deltaTime = 0.0f,
		lastFrame = 0.0f;

//Lighting
glm::vec3 lightPosition(0.0f, 3.0f, 0.0f);
glm::vec3 lightDirection(0.0f, 0.0f, -3.0f);

void myData(void);
void display(Shader, Model, Model);
void getResolution(void);
void animate(void);
void LoadTextures(void);
unsigned int generateTextures(char*, bool);

//For Keyboard
float	movX = 0.0f,
		movY = 0.0f,
		movZ = -5.0f,
		rotX = 0.0f;

//Texture
unsigned int	t_smile;

//Keyframes
float	posX = 0.0f,
posY = 0.0f,
posZ = 0.0f,
rotRodIzq = 0.0f,
rotRodDer = 0.0f,
rotBraIzq = 0.0f,
rotBraDer = 0.0f,
giroCabeza = 0.0f,
		giroMonito = 0.0f;

#define MAX_FRAMES 20 //numero de frames
int i_max_steps = 190; // intervalos entre frames 
int i_curr_steps = 0;
typedef struct _frame
{
	//Variables para GUARDAR Key Frames
	float posX;		//Variable para PosicionX
	float posY;		//Variable para PosicionY
	float posZ;		//Variable para PosicionZ
	float incX;		//Variable para IncrementoX los incrementos ayudan a calcular el siguiente frame
	float incY;		//Variable para IncrementoY
	float incZ;		//Variable para IncrementoZ
	float rotRodIzq;
	float rotRodDer;
	float rotBraIzq;
	float rotBraDer;
	float rotInc;
	float rotInc2;
	float rotInc3;
	float rotInc4;
	float giroMonito;
	float giroMonitoInc;
	float giroCabeza;
	float giroCabezaInc;

}FRAME;

FRAME KeyFrame[MAX_FRAMES]; //se meten en un arreglo por que son muchos frames
int FrameIndex = 0;			//introducir datos
bool play = false; 
int playIndex = 0;

void saveFrame(void) //
{

	printf("frameindex %d\n", FrameIndex);

	KeyFrame[FrameIndex].posX = posX; //el estado en el que se encuetra el monito se guarda en la estructura
	KeyFrame[FrameIndex].posY = posY;
	KeyFrame[FrameIndex].posZ = posZ;

	KeyFrame[FrameIndex].rotRodIzq = rotRodIzq;
	KeyFrame[FrameIndex].rotRodDer= rotRodDer;

	KeyFrame[FrameIndex].rotBraIzq = rotBraIzq;
	KeyFrame[FrameIndex].rotBraDer = rotBraDer;

	KeyFrame[FrameIndex].giroCabeza = giroCabeza;

	KeyFrame[FrameIndex].giroMonito = giroMonito;

	FrameIndex++;
}

void resetElements(void) 
{
	posX = KeyFrame[0].posX;
	posY = KeyFrame[0].posY;
	posZ = KeyFrame[0].posZ;

	rotRodIzq = KeyFrame[0].rotRodIzq;
	rotRodDer = KeyFrame[0].rotRodDer;


	rotBraIzq = KeyFrame[0].rotBraIzq;
	rotBraDer = KeyFrame[0].rotBraDer;

	giroCabeza = KeyFrame[0].giroCabeza;

	giroMonito = KeyFrame[0].giroMonito;

}

void interpolation(void) //la interpolaci�n que ocurre entre un key frame y otro 
{         

	KeyFrame[playIndex].incX = (KeyFrame[playIndex + 1].posX - KeyFrame[playIndex].posX) / i_max_steps;
	KeyFrame[playIndex].incY = (KeyFrame[playIndex + 1].posY - KeyFrame[playIndex].posY) / i_max_steps;
	KeyFrame[playIndex].incZ = (KeyFrame[playIndex + 1].posZ - KeyFrame[playIndex].posZ) / i_max_steps;

	KeyFrame[playIndex].rotInc = (KeyFrame[playIndex + 1].rotRodIzq - KeyFrame[playIndex].rotRodIzq) / i_max_steps;
	KeyFrame[playIndex].rotInc2 = (KeyFrame[playIndex + 1].rotRodDer - KeyFrame[playIndex].rotRodDer) / i_max_steps;

	KeyFrame[playIndex].rotInc3 = (KeyFrame[playIndex + 1].rotBraIzq - KeyFrame[playIndex].rotBraIzq) / i_max_steps;
	KeyFrame[playIndex].rotInc4 = (KeyFrame[playIndex + 1].rotBraDer - KeyFrame[playIndex].rotBraDer) / i_max_steps;
	
	KeyFrame[playIndex].giroMonitoInc = (KeyFrame[playIndex + 1].giroMonito - KeyFrame[playIndex].giroMonito) / i_max_steps;
	KeyFrame[playIndex].giroCabezaInc = (KeyFrame[playIndex + 1].giroCabeza - KeyFrame[playIndex].giroCabeza) / i_max_steps;

}


unsigned int generateTextures(const char* filename, bool alfa)
{
	unsigned int textureID;
	glGenTextures(1, &textureID);
	glBindTexture(GL_TEXTURE_2D, textureID);
	// set the texture wrapping parameters
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	// set texture filtering parameters
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	// load image, create texture and generate mipmaps
	int width, height, nrChannels;
	stbi_set_flip_vertically_on_load(true); // tell stb_image.h to flip loaded texture's on the y-axis.
	
	unsigned char *data = stbi_load(filename, &width, &height, &nrChannels, 0);
	if (data)
	{
		if (alfa)
			glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, data);
		else
			glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, data);
		glGenerateMipmap(GL_TEXTURE_2D);
		return textureID;
	}
	else
	{
		std::cout << "Failed to load texture" << std::endl;
		return 100;
	}

	stbi_image_free(data);
}

void getResolution()
{
	const GLFWvidmode * mode = glfwGetVideoMode(glfwGetPrimaryMonitor());

	SCR_WIDTH = mode->width;
	SCR_HEIGHT = (mode->height) - 80;

	lastX = SCR_WIDTH / 2.0f;
	lastY = SCR_HEIGHT / 2.0f;

}

void LoadTextures()
{

	
}

void myData()
{	
	float vertices[] = {
		// positions          // normals           // texture coords
		-0.5f, -0.5f, -0.5f,  0.0f,  0.0f, -1.0f,  0.0f,  0.0f,
		 0.5f, -0.5f, -0.5f,  0.0f,  0.0f, -1.0f,  1.0f,  0.0f,
		 0.5f,  0.5f, -0.5f,  0.0f,  0.0f, -1.0f,  1.0f,  1.0f,
		-0.5f,  0.5f, -0.5f,  0.0f,  0.0f, -1.0f,  0.0f,  1.0f,

		-0.5f, -0.5f,  0.5f,  0.0f,  0.0f,  1.0f,  0.0f,  0.0f,
		 0.5f, -0.5f,  0.5f,  0.0f,  0.0f,  1.0f,  1.0f,  0.0f,
		 0.5f,  0.5f,  0.5f,  0.0f,  0.0f,  1.0f,  1.0f,  1.0f,
		-0.5f,  0.5f,  0.5f,  0.0f,  0.0f,  1.0f,  0.0f,  1.0f,

		-0.5f,  0.5f,  0.5f, -1.0f,  0.0f,  0.0f,  1.0f,  0.0f,
		-0.5f,  0.5f, -0.5f, -1.0f,  0.0f,  0.0f,  1.0f,  1.0f,
		-0.5f, -0.5f, -0.5f, -1.0f,  0.0f,  0.0f,  0.0f,  1.0f,
		-0.5f, -0.5f,  0.5f, -1.0f,  0.0f,  0.0f,  0.0f,  0.0f,

		 0.5f,  0.5f,  0.5f,  1.0f,  0.0f,  0.0f,  1.0f,  0.0f,
		 0.5f,  0.5f, -0.5f,  1.0f,  0.0f,  0.0f,  1.0f,  1.0f,
		 0.5f, -0.5f, -0.5f,  1.0f,  0.0f,  0.0f,  0.0f,  1.0f,
		 0.5f, -0.5f,  0.5f,  1.0f,  0.0f,  0.0f,  0.0f,  0.0f,

		-0.5f, -0.5f, -0.5f,  0.0f, -1.0f,  0.0f,  0.0f,  1.0f,
		 0.5f, -0.5f, -0.5f,  0.0f, -1.0f,  0.0f,  1.0f,  1.0f,
		 0.5f, -0.5f,  0.5f,  0.0f, -1.0f,  0.0f,  1.0f,  0.0f,
		-0.5f, -0.5f,  0.5f,  0.0f, -1.0f,  0.0f,  0.0f,  0.0f,

		-0.5f,  0.5f, -0.5f,  0.0f,  1.0f,  0.0f,  0.0f,  1.0f,
		 0.5f,  0.5f, -0.5f,  0.0f,  1.0f,  0.0f,  1.0f,  1.0f,
		 0.5f,  0.5f,  0.5f,  0.0f,  1.0f,  0.0f,  1.0f,  0.0f,
		-0.5f,  0.5f,  0.5f,  0.0f,  1.0f,  0.0f,  0.0f,  0.0f,

	};
	unsigned int indices[] = {
		0, 1, 3, // first triangle
		1, 2, 3  // second triangle
	};
	
	glGenVertexArrays(1, &VAO);
	glGenBuffers(1, &VBO);
	glGenBuffers(1, &EBO);

	glBindVertexArray(VAO);

	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);

	// position attribute
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)0);
	glEnableVertexAttribArray(0);
	// normal attribute
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(3 * sizeof(float)));
	glEnableVertexAttribArray(1);
	// texture coord attribute
	glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(6 * sizeof(float)));
	glEnableVertexAttribArray(2);

}

void animate(void)
{
	if (play)
	{
		if (i_curr_steps >= i_max_steps) //end of animation between frames?
		{
			playIndex++;
			if (playIndex > FrameIndex - 2)	//end of total animation?
			{
				printf("termina anim\n");
				playIndex = 0;
				play = false;
			}
			else //Next frame interpolations
			{
				i_curr_steps = 0; //Reset counter
								  //Interpolation
				interpolation();
			}
		}
		else
		{
			//Draw animation
			posX += KeyFrame[playIndex].incX;
			posY += KeyFrame[playIndex].incY;
			posZ += KeyFrame[playIndex].incZ;

			rotRodIzq += KeyFrame[playIndex].rotInc;
			rotRodDer += KeyFrame[playIndex].rotInc2;

			rotBraIzq += KeyFrame[playIndex].rotInc3;
			rotBraDer += KeyFrame[playIndex].rotInc4;

			giroMonito += KeyFrame[playIndex].giroCabezaInc;

			giroMonito += KeyFrame[playIndex].giroMonitoInc;

			i_curr_steps++;
		}

	}
	
}

void display(Shader shader, Model botaDer, Model piernaDer, Model piernaIzq, Model torso,
	Model brazoDer, Model brazoIzq, Model cabeza, Model piso)
{
	shader.use();

	// create transformations and Projection
	glm::mat4 tmp = glm::mat4(1.0f);
	glm::mat4 model = glm::mat4(1.0f);		// initialize Matrix, Use this matrix for individual models
	glm::mat4 view = glm::mat4(1.0f);		//Use this matrix for ALL models
	glm::mat4 projection = glm::mat4(1.0f);	//This matrix is for Projection

	//Use "projection" to include Camera
	projection = glm::perspective(glm::radians(camera.Zoom), (float)SCR_WIDTH / (float)SCR_HEIGHT, 0.1f, 100.0f);
	view = camera.GetViewMatrix();

	// pass them to the shaders
	shader.setMat4("model", model);
	shader.setMat4("view", view);
	// note: currently we set the projection matrix each frame, but since the projection matrix rarely changes it's often best practice to set it outside the main loop only once.
	shader.setMat4("projection", projection);

	model = glm::translate(glm::mat4(1.0f), glm::vec3(0.0f, -1.0f, -1.0f));
	model = glm::scale(model, glm::vec3(0.007f, 0.007f, 0.007f));
	shader.setMat4("model", model);
	piso.Draw(shader);

	//Personaje
	model = glm::mat4(1.0f);
	model = glm::translate(model, glm::vec3(0, 1, 0));
	model = glm::translate(model, glm::vec3(posX, posY, posZ));
	tmp = model = glm::rotate(model, glm::radians(giroMonito), glm::vec3(0.0f, 1.0f, 0.0));
	shader.setMat4("model", model);
	torso.Draw(shader);

	//Pierna Der
	model = glm::translate(tmp, glm::vec3(-0.5f, 0.0f, -0.1f));
	model = glm::rotate(model, glm::radians(0.0f), glm::vec3(0.0f, 1.0f, 0.0));
	model = glm::rotate(model, glm::radians(-rotRodIzq), glm::vec3(1.0f, 0.0f, 0.0f));
	shader.setMat4("model", model);
	piernaDer.Draw(shader);

	//Pie Der
	model = glm::translate(model, glm::vec3(0, -0.9f, -0.2f));
	shader.setMat4("model", model);
	botaDer.Draw(shader);

	//Pierna Izq
	model = glm::translate(tmp, glm::vec3(0.5f, 0.0f, -0.1f));
	model = glm::rotate(model, glm::radians(0.0f), glm::vec3(0.0f, 1.0f, 0.0));
	model = glm::rotate(model, glm::radians(-rotRodDer), glm::vec3(1.0f, 0.0f, 0.0f));
	shader.setMat4("model", model);
	piernaIzq.Draw(shader);

	//Pie Iz
	model = glm::translate(model, glm::vec3(0, -0.9f, -0.2f));
	shader.setMat4("model", model);
	botaDer.Draw(shader);	//Izq trase

	//Brazo derecho
	model = glm::translate(tmp, glm::vec3(0.0f, - 1.0f, 0.0f));
	model = glm::rotate(model, glm::radians(0.0f), glm::vec3(0.0f, 1.0f, 0.0f));
	model = glm::translate(model, glm::vec3(-0.75f, 2.5f, 0));

	model = glm::rotate(model, glm::radians(0.0f), glm::vec3(0.0f, 1.0f, 0.0));
	model = glm::rotate(model, glm::radians(-rotBraDer), glm::vec3(1.0f, 0.0f, 0.0f));
	shader.setMat4("model", model);
	brazoDer.Draw(shader);

	//Brazo izquierdo
	model = glm::translate(tmp, glm::vec3(0.0f, - 1.0f, 0.0f));
	model = glm::rotate(model, glm::radians(0.0f), glm::vec3(0.0f, 1.0f, 0.0f));
	model = glm::translate(model, glm::vec3(0.75f, 2.5f, 0));
	model = glm::rotate(model, glm::radians(-rotBraIzq), glm::vec3(1.0f, 0.0f, 0.0f));
	shader.setMat4("model", model);
	brazoIzq.Draw(shader);

	//Cabeza
	model = glm::translate(tmp, glm::vec3(0.0f, - 1.0f, 0.0f));
	model = glm::translate(model, glm::vec3(0.0f, 2.5f, 0));
	model = glm::rotate(model, glm::radians(0.0f), glm::vec3(0.0f, 1.0f, 0.0));
	model = glm::rotate(model, glm::radians(-giroCabeza), glm::vec3(0.0f, 1.0f, 0.0f));

	shader.setMat4("model", model);
	cabeza.Draw(shader);
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

    GLFWwindow* window = glfwCreateWindow(SCR_WIDTH, SCR_HEIGHT, "Practica 10", NULL, NULL);
    if (window == NULL)
    {
        std::cout << "Failed to create GLFW window" << std::endl;
        glfwTerminate();
        return -1;
    }
	glfwSetWindowPos(window, 0, 30);
    glfwMakeContextCurrent(window);
	glfwSetKeyCallback(window, my_input);
    glfwSetFramebufferSizeCallback(window, resize);
	glfwSetCursorPosCallback(window, mouse_callback);
	glfwSetScrollCallback(window, scroll_callback);

	//To Enable capture of our mouse
	glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);

	glewInit();


	//Mis funciones
	//Datos a utilizar
	LoadTextures();
	myData();
	glEnable(GL_DEPTH_TEST);
	
	Shader modelShader("Shaders/modelLoading.vs", "Shaders/modelLoading.fs");
	// Load models
	Model botaDer = ((char *)"Models/Personaje/bota.obj");
	Model piernaDer = ((char *)"Models/Personaje/piernader.obj");
	Model piernaIzq = ((char *)"Models/Personaje/piernader.obj");
	Model torso = ((char *)"Models/Personaje/torso.obj");
	Model brazoDer = ((char *)"Models/Personaje/brazoder.obj");
	Model brazoIzq = ((char *)"Models/Personaje/brazoizq.obj");
	Model cabeza = ((char *)"Models/Personaje/cabeza.obj");
	Model pisoModel = ((char *)"Models/piso/piso.obj");

	//Inicializaci�n de KeyFrames

	for (int i = 0; i < MAX_FRAMES; i++)
	{
		KeyFrame[i].posX = 0;
		KeyFrame[i].incX = 0;
		KeyFrame[i].incY = 0;
		KeyFrame[i].incZ = 0;
		KeyFrame[i].rotRodIzq = 0;
		KeyFrame[i].rotRodDer = 0;
		KeyFrame[i].rotBraIzq = 0;
		KeyFrame[i].rotBraDer = 0;
		KeyFrame[i].giroMonito = 0;

		KeyFrame[i].giroCabeza = 0;

		KeyFrame[i].rotInc = 0;
		KeyFrame[i].rotInc2 = 0;
		KeyFrame[i].rotInc3 = 0;
		KeyFrame[i].rotInc4 = 0;
		KeyFrame[i].giroCabezaInc= 0;
		KeyFrame[i].giroMonitoInc= 0;




	}

    
	glm::mat4 projection = glm::mat4(1.0f);	//This matrix is for Projection
	projection = glm::perspective(glm::radians(camera.Zoom), (float)SCR_WIDTH / (float)SCR_HEIGHT, 0.1f, 100.0f);
	// render loop
    // While the windows is not closed
    while (!glfwWindowShouldClose(window))
    {
		// per-frame time logic
		// --------------------
		double currentFrame = glfwGetTime();
		deltaTime = currentFrame - lastFrame;
		lastFrame = currentFrame;
        // input
        // -----
        //my_input(window);
		animate();

        // render
        // Backgound color
        glClearColor(1.0f, 1.0f, 1.0f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		//display(modelShader, ourModel, llantasModel);
		display(modelShader, botaDer, piernaDer, 
				piernaIzq, torso, brazoDer, brazoIzq,
				cabeza, pisoModel);

        // glfw: swap buffers and poll IO events (keys pressed/released, mouse moved etc.)
        // -------------------------------------------------------------------------------
        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    // glfw: terminate, clearing all previously allocated GLFW resources.
    // ------------------------------------------------------------------
	glDeleteVertexArrays(1, &VAO);
	glDeleteBuffers(1, &VBO);

    glfwTerminate();
    return 0;
}

// process all input: query GLFW whether relevant keys are pressed/released this frame and react accordingly
// ---------------------------------------------------------------------------------------------------------
void my_input(GLFWwindow *window, int key, int scancode, int action, int mode)
{
	if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
		glfwSetWindowShouldClose(window, true);
	if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
		camera.ProcessKeyboard(FORWARD, (float)deltaTime);
	if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
		camera.ProcessKeyboard(BACKWARD, (float)deltaTime);
	if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
		camera.ProcessKeyboard(LEFT, (float)deltaTime);
	if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
		camera.ProcessKeyboard(RIGHT, (float)deltaTime);
	//To Configure Model
	if (glfwGetKey(window, GLFW_KEY_Y) == GLFW_PRESS)
		posZ++;
	if (glfwGetKey(window, GLFW_KEY_H) == GLFW_PRESS)
		posZ--;
	if (glfwGetKey(window, GLFW_KEY_G) == GLFW_PRESS)
		posX--;
	if (glfwGetKey(window, GLFW_KEY_J) == GLFW_PRESS)
		posX++;
	if (glfwGetKey(window, GLFW_KEY_X) == GLFW_PRESS)
		rotRodIzq--;
	if (glfwGetKey(window, GLFW_KEY_C) == GLFW_PRESS)
		rotRodIzq++;
	if (glfwGetKey(window, GLFW_KEY_M) == GLFW_PRESS)
		rotRodDer--;
	if (glfwGetKey(window, GLFW_KEY_N) == GLFW_PRESS)
		rotRodDer++;
	if (glfwGetKey(window, GLFW_KEY_T) == GLFW_PRESS)
		rotBraDer--;
	if (glfwGetKey(window, GLFW_KEY_R) == GLFW_PRESS)
		rotBraDer++;
	if (glfwGetKey(window, GLFW_KEY_1) == GLFW_PRESS)
		rotBraIzq--;
	if (glfwGetKey(window, GLFW_KEY_2) == GLFW_PRESS)
		rotBraIzq++;
	if (glfwGetKey(window, GLFW_KEY_3) == GLFW_PRESS)
		giroCabeza--;
	if (glfwGetKey(window, GLFW_KEY_4) == GLFW_PRESS)
		giroCabeza++;
	if (glfwGetKey(window, GLFW_KEY_V) == GLFW_PRESS)
		giroMonito--;
	if (glfwGetKey(window, GLFW_KEY_B) == GLFW_PRESS)
		giroMonito++;

	
	
	//To play KeyFrame animation 
	if (glfwGetKey(window, GLFW_KEY_P) == GLFW_PRESS)
	{
		if (play == false && (FrameIndex > 1))
		{
			resetElements();
			//First Interpolation				
			interpolation();

			play = true;
			playIndex = 0;
			i_curr_steps = 0;
		}
		else
		{
			play = false;
		}
	}

	//To Save a KeyFrame
	if (glfwGetKey(window, GLFW_KEY_L) == GLFW_PRESS)
	{
		if (FrameIndex < MAX_FRAMES)
		{
			saveFrame();
		}
	}
}

// glfw: whenever the window size changed (by OS or user resize) this callback function executes
// ---------------------------------------------------------------------------------------------
void resize(GLFWwindow* window, int width, int height)
{
    // Set the Viewport to the size of the created window
    glViewport(0, 0, width, height);
}

// glfw: whenever the mouse moves, this callback is called
// -------------------------------------------------------
void mouse_callback(GLFWwindow* window, double xpos, double ypos)
{
	if (firstMouse)
	{
		lastX = xpos;
		lastY = ypos;
		firstMouse = false;
	}

	double xoffset = xpos - lastX;
	double yoffset = lastY - ypos; // reversed since y-coordinates go from bottom to top

	lastX = xpos;
	lastY = ypos;

	camera.ProcessMouseMovement(xoffset, yoffset);
}

// glfw: whenever the mouse scroll wheel scrolls, this callback is called
// ----------------------------------------------------------------------
void scroll_callback(GLFWwindow* window, double xoffset, double yoffset)
{
	camera.ProcessMouseScroll(yoffset);
}