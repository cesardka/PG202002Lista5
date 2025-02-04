#include "SceneManager.h"

//static controllers for mouse and keyboard
static bool keys[1024];
static bool resized;
static GLuint width, height;

SceneManager::SceneManager()
{
}

SceneManager::~SceneManager()
{
}

void SceneManager::initialize(GLuint w, GLuint h)
{
	width = w;
	height = h;
	
	// GLFW - GLEW - OPENGL general setup -- TODO: config file
	initializeGraphics();

}

void SceneManager::initializeGraphics()
{
	// Init GLFW
	glfwInit();

	// Create a GLFWwindow object that we can use for GLFW's functions
	window = glfwCreateWindow(width, height, "Hello Filters", nullptr, nullptr);
	glfwMakeContextCurrent(window);

	// Set the required callback functions
	glfwSetKeyCallback(window, key_callback);

	//Setando a callback de redimensionamento da janela
	glfwSetWindowSizeCallback(window, resize);
	
	// glad: load all OpenGL function pointers
	// ---------------------------------------
	if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
	{
		std::cout << "Failed to initialize GLAD" << std::endl;

	}

	// Adicionando os shaders
	addShader("../shaders/transformations.vs", "../shaders/transformations.frag");
	addShader("../shaders/FiltroGrayscale.vs", "../shaders/FiltroGrayscale.frag");
	addShader("../shaders/FiltroColorizacao.vs", "../shaders/FiltroColorizacao.frag");
	addShader("../shaders/FiltroBinarizacao.vs", "../shaders/FiltroBinarizacao.frag");
	addShader("../shaders/FiltroInversao.vs", "../shaders/FiltroInversao.frag");
	addShader("../shaders/FiltroLivre.vs", "../shaders/FiltroLivre.frag");
	addShader("../shaders/FiltroVignete.vs", "../shaders/FiltroVignete.frag");

	//setup the scene -- LEMBRANDO QUE A DESCRI��O DE UMA CENA PODE VIR DE ARQUIVO(S) DE 
	// CONFIGURA��O
	setupScene();

	resized = true; //para entrar no setup da c�mera na 1a vez

}

void SceneManager::addShader(string vFilename, string fFilename)
{
	Shader *shader = new Shader (vFilename.c_str(), fFilename.c_str());
	shaders.push_back(shader);
}


void SceneManager::key_callback(GLFWwindow * window, int key, int scancode, int action, int mode)
{
	if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS)
		glfwSetWindowShouldClose(window, GL_TRUE);
	if (key >= 0 && key < 1024)
	{
		if (action == GLFW_PRESS)
			keys[key] = true;
		else if (action == GLFW_RELEASE)
			keys[key] = false;
	}
}

void SceneManager::resize(GLFWwindow * window, int w, int h)
{
	width = w;
	height = h;
	resized = true;

	// Define the viewport dimensions
	glViewport(0, 0, width, height);
}


void SceneManager::update()
{
	if (keys[GLFW_KEY_ESCAPE])
		glfwSetWindowShouldClose(window, GL_TRUE);

	//alter o angulo do primeiro objeto
	//objects[0]->setAngle((float)glfwGetTime());
}

void SceneManager::render()
{
	// Clear the colorbuffer
	glClearColor(0.5f, 0.5f, 0.5f, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);


	if (resized) //se houve redimensionamento na janela, redefine a projection matrix
	{
		setupCamera2D();
		resized = false;
	}

	//atualiza e desenha os gameObjects

	for (int i = 0; i < objects.size(); i++)
	{ 
		objects[i]->update();
		// Aplicar a segunda textura de polaroid (shaders[4]) ou vignette (shaders[5])
		if (i == 4 || i == 5)
		{
			glActiveTexture(GL_TEXTURE1);
			if (i == 5)
			{
				glUniform1i(glGetUniformLocation(shaders[i]->Program, "ourTexture2"), 1);
				glBindTexture(GL_TEXTURE_2D, textureMaskIDVig);
			}
			else
			{
				glUniform1i(glGetUniformLocation(shaders[i]->Program, "ourTexture3"), 1);
				glBindTexture(GL_TEXTURE_2D, textureMaskIDPol);
			}
		}
		objects[i]->draw();
	}
	

}

void SceneManager::run()
{
	//GAME LOOP
	while (!glfwWindowShouldClose(window))
	{
		
		// Check if any events have been activiated (key pressed, mouse moved etc.) and call corresponding response functions
		glfwPollEvents();

		//Update method(s)
		update();

		//Render scene
		render();
		
		// Swap the screen buffers
		glfwSwapBuffers(window);
	}
}

void SceneManager::finish()
{
	// Terminate GLFW, clearing any resources allocated by GLFW.
	glfwTerminate();
}


void SceneManager::setupScene()
{
	//Cria��o dos gameObjects iniciais -- pode-se fazer m�todos de cria��o posteriormente
	//M�nimo: posicao e escala e ponteiro para o shader

	//Exerc�cio 2) Mostrando 7 filtros lado a lado, em cima e embaixo...
	GameObject* obj = new GameObject;
	obj->setPosition(glm::vec3(200.0f, 100.0f, 0.0));
	obj->setDimension(glm::vec3(200.0f, 200.0f, 1.0f)); //note que depois podemos reescalar conforme tamanho da sprite
	obj->setShader(shaders[0]); // exerc�cio 1 -- Exibir canais
	objects.push_back(obj); //adiciona o primeiro obj
	shaders[0]->setInt("code", 1);
	
	//Exerc�cio 1b) Grayscale com m�dia ponderada
	obj = new GameObject;
	obj->setPosition(glm::vec3(400.0f, 100.0f, 0.0));
	obj->setDimension(glm::vec3(200.0f, 200.0f, 1.0f));
	obj->setShader(shaders[1]);
	objects.push_back(obj); //adiciona o segundo 

	//Exerc�cio 1c) Colorizando em vermelho
	obj = new GameObject;
	obj->setPosition(glm::vec3(600.0f, 100.0f, 0.0));
	obj->setDimension(glm::vec3(200.0f, 200.0f, 1.0f));
	obj->setShader(shaders[2]);
	objects.push_back(obj); //adiciona o terceiro obj
	shaders[2]->setVec3("corColorizadora", 1.0, 0.0, 0.0);

	// Exercicio 1d) Invers�o de cores
	obj = new GameObject;
	obj->setPosition(glm::vec3(0.0f, 400.0f, 0.0));
	obj->setDimension(glm::vec3(200.0f, 200.0f, 1.0f));
	obj->setShader(shaders[3]);
	objects.push_back(obj); //adiciona o quarto obj

	// Exercicio 1e) Binariza��o de cores
	obj = new GameObject;
	obj->setPosition(glm::vec3(200.0f, 400.0f, 0.0));
	obj->setDimension(glm::vec3(200.0f, 200.0f, 1.0f));
	obj->setShader(shaders[4]);
	objects.push_back(obj); //adiciona o quinto obj
	shaders[4]->setInt("limiar", 120);

	// Exercicio 1f) Crie seu filtro! Efeito polaroid
	obj = new GameObject;
	obj->setPosition(glm::vec3(400.0f, 400.0f, 0.0));
	obj->setDimension(glm::vec3(200.0f, 200.0f, 1.0f));
	obj->setShader(shaders[5]);
	objects.push_back(obj); //adiciona o sexto obj

	// Exercicio 1g) Efeito vignete
	obj = new GameObject;
	obj->setPosition(glm::vec3(600.0f, 400.0f, 0.0));
	obj->setDimension(glm::vec3(200.0f, 200.0f, 1.0f));
	obj->setShader(shaders[6]);
	objects.push_back(obj); //adiciona o setimo obj

	//Carregamento das texturas (pode ser feito intercalado na cria��o)
	//Futuramente, utilizar classe de materiais e armazenar dimensoes, etc
	unsigned int texID = loadTexture("../textures/lena.png");
	for (int i = 0; i < objects.size(); i++)
	{
		objects[i]->setTexture(texID);
	}

	textureMaskIDVig = loadTexture("../textures/Vignette.png");
	textureMaskIDPol = loadTexture("../textures/polaroid.png");

	//Habilita transpar�ncia
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

}

void SceneManager::setupCamera2D() //TO DO: parametrizar aqui
{
	//corrigindo o aspecto
	float ratio = 1.0;
	float xMin = 0.0, xMax = 800.0, yMin = 0.0, yMax = 600.0, zNear = -1.0, zFar = 1.0;

	projection = glm::ortho(xMin * ratio, xMax * ratio, yMin, yMax, zNear, zFar);

	// Get their uniform location
	for (int i = 0; i < shaders.size(); i++)
	{
		shaders[i]->Use();
		GLint projLoc = glGetUniformLocation(shaders[i]->Program, "projection");
		glUniformMatrix4fv(projLoc, 1, GL_FALSE, glm::value_ptr(projection));
	}
}

unsigned int SceneManager::loadTexture(string filename)
{
	unsigned int texture;

	// load and create a texture 
	glGenTextures(1, &texture);
	glBindTexture(GL_TEXTURE_2D, texture); // all upcoming GL_TEXTURE_2D operations now have effect on this texture object
										   // set the texture wrapping parameters
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);	// set texture wrapping to GL_REPEAT (default wrapping method)
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	// set texture filtering parameters
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

	// load image, create texture and generate mipmaps
	int width, height, nrChannels;
	//unsigned char *data = SOIL_load_image("../textures/wall.jpg", &width, &height, 0, SOIL_LOAD_RGB);
	unsigned char *data = stbi_load(filename.c_str(), &width, &height, &nrChannels, 0);
	
	if (data)
	{
		if (nrChannels == 3) //jpg, bmp
		{
			glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, data);
		}
		else //png
		{
			glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, data);
		}
		glGenerateMipmap(GL_TEXTURE_2D);
	}
	else
	{
		std::cout << "Failed to load texture" << std::endl;
	}
	stbi_image_free(data);

	glBindTexture(GL_TEXTURE_2D, 0); // Unbind texture when done, so we won't accidentily mess up our texture.

	glActiveTexture(GL_TEXTURE0);

	return texture;
}
