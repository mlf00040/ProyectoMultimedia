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
#include <RenderizadoCasillasFondo.h>



struct DatosJuego{

    glm::vec2 playerPos = {100,100};
    float direccionGiro = -90.0f;

}datosJuego;

#pragma region inicializacion de texturas
gl2d::Renderer2D renderer;
gl2d::Texture texturaNavePrincipal;
constexpr int CAPASFONDO = 4;
gl2d::Texture texturaFondo[CAPASFONDO];

RenderizadoCasillas generadorCasillas[CAPASFONDO];
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
    if (!std::filesystem::exists(RESOURCES_PATH"space/FondoTrozo1.png")) {
        std::cerr << "Error: No se encontró un componente del fondo "<< std::endl;
    } else {
        texturaFondo[0].loadFromFile(RESOURCES_PATH"space/FondoTrozo1.png", true);
    }

    if (!std::filesystem::exists(RESOURCES_PATH"space/FondoTrozo2.png")) {
        std::cerr << "Error: No se encontró un componente del fondo "<< std::endl;
    } else {
        texturaFondo[1].loadFromFile(RESOURCES_PATH"space/FondoTrozo2.png", true);
    }

    if (!std::filesystem::exists(RESOURCES_PATH"space/FondoTrozo3.png")) {
        std::cerr << "Error: No se encontró un componente del fondo "<< std::endl;
    } else {
        texturaFondo[2].loadFromFile(RESOURCES_PATH"space/FondoTrozo3.png", true);
    }

    if (!std::filesystem::exists(RESOURCES_PATH"space/FondoTrozo4.png")) {
        std::cerr << "Error: No se encontró un componente del fondo "<< std::endl;
    } else {
        texturaFondo[3].loadFromFile(RESOURCES_PATH"space/FondoTrozo4.png", true);
    }


    generadorCasillas[0].fondo = texturaFondo[0];
    generadorCasillas[1].fondo = texturaFondo[1];
    generadorCasillas[2].fondo = texturaFondo[2];
    generadorCasillas[3].fondo = texturaFondo[3];

    generadorCasillas[0].fuerzaDistorsion = 0;
    generadorCasillas[1].fuerzaDistorsion = 0.25;
    generadorCasillas[2].fuerzaDistorsion = 0.5;
    generadorCasillas[3].fuerzaDistorsion = 0.75;


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

#pragma region Texturas del fondo

    for(int i=0; i< CAPASFONDO;i++){
        generadorCasillas[i].render(renderer);
    }

#pragma endregion

    float tamanioNave = 100.0f;

    renderer.currentCamera.follow(datosJuego.playerPos,deltaTime*300,10,200,w,h);

    renderer.renderRectangle({datosJuego.playerPos- glm::vec2(tamanioNave/2,tamanioNave/2),  100, 100}, texturaNavePrincipal, Colors_White,{},datosJuego.direccionGiro + 90.0f);



	renderer.flush();


	//ImGui::ShowDemoWindow();


	return true;
#pragma endregion

}

//This function might not be be called if the program is forced closed
void closeGame()
{



}
