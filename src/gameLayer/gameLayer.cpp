#define GLM_ENABLE_EXPERIMENTAL
#include "gameLayer.h"
#include <glad/glad.h>
#include <glm/glm.hpp>
#include <glm/gtx/transform.hpp>
#include "platformInput.h"
#include "imgui.h"
#include <iostream>
#include <sstream>
#include "imfilebrowser.h"
#include <gl2d/gl2d.h>
#include <platformTools.h>
#include <filesystem>



struct DatosJuego{

    glm::vec2 playerPos = {100,100};

}datosJuego;

gl2d::Renderer2D renderer;
gl2d::Texture texturaNavePrincipal;


bool initGame()
{
	//initializing stuff for the renderer
	gl2d::init();
	renderer.create();

    if (!std::filesystem::exists(RESOURCES_PATH"spaceShip/ships/green.png")) {
        std::cerr << "Error: No se encontró la imagen de la nave"<< std::endl;
    } else {
        texturaNavePrincipal.loadFromFile(RESOURCES_PATH"spaceShip/ships/green.png", true);
    }
	
	return true;
}



bool gameLogic(float deltaTime)
{
#pragma region init stuff
	int w = 0; int h = 0;
	w = platform::getFrameBufferSizeX(); //window w
	h = platform::getFrameBufferSizeY(); //window h
	
	glViewport(0, 0, w, h);
	glClear(GL_COLOR_BUFFER_BIT); //clear screen

	renderer.updateWindowMetrics(w, h);
#pragma endregion


    renderer.renderRectangle({datosJuego.playerPos , 100, 100}, texturaNavePrincipal);



	renderer.flush();


	//ImGui::ShowDemoWindow();


	return true;
#pragma endregion

}

//This function might not be be called if the program is forced closed
void closeGame()
{



}
