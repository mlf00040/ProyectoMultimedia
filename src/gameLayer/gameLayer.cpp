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
    float direccionGiro = -90.0f;

}datosJuego;

#pragma region inicializacion de texturas
gl2d::Renderer2D renderer;
gl2d::Texture texturaNavePrincipal;
gl2d::Texture texturaFondo;
#pragma endregion

bool initGame()
{
	//initializing stuff for the renderer
	gl2d::init();
	renderer.create();

    if (!std::filesystem::exists(RESOURCES_PATH"spaceShip/ships/green.png")) {
        std::cerr << "Error: No se encontró la imagen de la nave Jugador "<< std::endl;
    } else {
        texturaNavePrincipal.loadFromFile(RESOURCES_PATH"spaceShip/ships/green.png", true);
    }
    if (!std::filesystem::exists(RESOURCES_PATH"space/Fondo.png")) {
        std::cerr << "Error: No se encontró la imagen de la nave Jugador "<< std::endl;
    } else {
        texturaFondo.loadFromFile(RESOURCES_PATH"space/Fondo.png", true);
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

#pragma region Texturas del fondo
    renderer.renderRectangle({0,0 , 1800, 1800}, texturaFondo);

#pragma endregion
#pragma region movimiento
    glm::vec2 movimiento = {};

    //movimiento de posicion
    if(platform::isButtonHeld(platform::Button::Left) || platform::isButtonHeld(platform::Button::A)){
        movimiento.x -=1 ;
    }
    if(platform::isButtonHeld(platform::Button::Right) || platform::isButtonHeld(platform::Button::D)){
        movimiento.x += 1;
    }
    if(platform::isButtonHeld(platform::Button::Up) || platform::isButtonHeld(platform::Button::W)){
        movimiento.y -= 1;
    }
    if(platform::isButtonHeld(platform::Button::Down) || platform::isButtonHeld(platform::Button::S)){
        movimiento.y += 1;
    }

    //normalizacion del vector para que la diagonal vaya a la misma velocidad
    if(movimiento.x !=0 || movimiento.y !=0){

        movimiento = glm::normalize(movimiento);
        movimiento *= deltaTime * 400;
        datosJuego.playerPos += movimiento;

        //Rotacion del personaje
        float direccionRad = atan2(movimiento.y, -movimiento.x);
        datosJuego.direccionGiro = glm::degrees(direccionRad);

    }
#pragma endregion

    renderer.renderRectangle({datosJuego.playerPos , 100, 100}, texturaNavePrincipal, Colors_White,{},datosJuego.direccionGiro + 90.0f);



	renderer.flush();


	//ImGui::ShowDemoWindow();


	return true;
#pragma endregion

}

//This function might not be be called if the program is forced closed
void closeGame()
{



}
