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
#include <balas.h>
#include <enemigos.h>
#include <vector>
#include <metodosAuxiliares.h>
#include <raudio.h>



struct DatosJuego{

    glm::vec2 playerPos = {100,100};
    float direccionGiro = -90.0f;
    float tamanioNave = 64.0f;

    std::vector<Balas> VBalas;
    std::vector<Enemigo> VEnemigos;

    float volumenGeneral = 1;
    float volumenMusica = 0.05;
    float volumenFX = 0.2;

}datosJuego;

#pragma region inicializacion de texturas

gl2d::Renderer2D renderer;
gl2d::Texture texturaNavePrincipal;
constexpr int CAPASFONDO = 4;
gl2d::Texture texturaFondo[CAPASFONDO];

gl2d::Texture bala1;
gl2d::TextureAtlasPadding atlasBalas;

gl2d::Texture enemigo1;
gl2d::TextureAtlasPadding atlasEnemigos;

RenderizadoCasillas generadorCasillas[CAPASFONDO];

Sound sonidoDisparo;
Sound musicaFondo;

#pragma endregion

void reiniciarJuego(){
    datosJuego={};
    StopSound(musicaFondo);
}

bool initGame()
{
	//initializing stuff for the renderer
	gl2d::init();
	renderer.create();


#pragma region comprobacion de archivos y carga de texturas

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

    if (!std::filesystem::exists(RESOURCES_PATH"spaceShip/stitchedFiles/projectiles.png")) {
        std::cerr << "Error: No se encontró el archivo de proyectiles "<< std::endl;
    } else {
        bala1.loadFromFileWithPixelPadding
        (RESOURCES_PATH"spaceShip/stitchedFiles/projectiles.png",500, true);
        atlasBalas= gl2d::TextureAtlasPadding(3,2, bala1.GetSize().x,bala1.GetSize().y);
    }

    if (!std::filesystem::exists(RESOURCES_PATH"spaceShip/ships/newShips.png")) {
        std::cerr << "Error: No se encontró el archivo de enemigos "<< std::endl;
    } else {
        enemigo1.loadFromFileWithPixelPadding
                (RESOURCES_PATH"spaceShip/ships/newShips.png",16, true);
        atlasEnemigos= gl2d::TextureAtlasPadding(2,1, bala1.GetSize().x,bala1.GetSize().y);
    }

    generadorCasillas[0].fondo = texturaFondo[0];
    generadorCasillas[1].fondo = texturaFondo[1];
    generadorCasillas[2].fondo = texturaFondo[2];
    generadorCasillas[3].fondo = texturaFondo[3];

    generadorCasillas[0].fuerzaDistorsion = 0;
    generadorCasillas[1].fuerzaDistorsion = 0.25;
    generadorCasillas[2].fuerzaDistorsion = 0.5;
    generadorCasillas[3].fuerzaDistorsion = 0.75;

#pragma endregion

#pragma region sonido

    sonidoDisparo = LoadSound(RESOURCES_PATH"sonidos/Disparo1.wav");
    SetSoundVolume(sonidoDisparo,datosJuego.volumenFX*datosJuego.volumenGeneral);

    musicaFondo = LoadSound(RESOURCES_PATH"sonidos/MusicaDelJuegoProvisional.wav");
    SetSoundVolume(musicaFondo,datosJuego.volumenMusica*datosJuego.volumenGeneral);

#pragma endregion

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

    if(!IsSoundPlaying(musicaFondo)){
        PlaySound(musicaFondo);
    }
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
        movimiento *= deltaTime * 300;
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

#pragma region manejo de enemigos

    //actualiza el movimiento de los enemigos
    for(auto &e : datosJuego.VEnemigos){
        e.movimiento(deltaTime,datosJuego.playerPos);
    }

#pragma endregion

#pragma region renderizado enemigos
    //renderizar enemigos
    for(auto &e : datosJuego.VEnemigos){
        e.render(renderer,enemigo1,atlasEnemigos,glm::vec2(1,0),datosJuego.playerPos);
    }
    //impacto de balas


    //bajar la vida si golpea una bala

    //eliminar enemigo si se queda sin vida sino eliminar solo la bala




#pragma endregion

#pragma region manejar balas

//habra que adaptar esto a una utilizacion por tiempo y parametro
    //todo esto despues hay que ponerle un timer y quitar el raton.
    if(platform::isRMousePressed()){
        if(!IsSoundPlaying(sonidoDisparo)){
            PlaySound(sonidoDisparo);
        }
        Balas b;
        //para medio centrar las balas
        glm::vec2 centroJugador={datosJuego.playerPos.x-datosJuego.tamanioNave/2,
                  datosJuego.playerPos.y-datosJuego.tamanioNave/2};

        b.setPosition(centroJugador);

        glm::vec2 posEnemigoMasCercano = calculaPosEnemigoMasCercano(datosJuego.VEnemigos, centroJugador);
        glm::vec2 direccion = posEnemigoMasCercano - centroJugador;
        if (glm::length(direccion) > 0) {
            direccion = glm::normalize(direccion);
        }
        b.setDireccion(direccion);
        b.setDanio(5);
        //esto va a haber que cambiarlo


        datosJuego.VBalas.push_back(b);
    }

    //actualiza el movimiento de las balas
    for(auto &b : datosJuego.VBalas){
        b.movimiento(deltaTime);
    }

    for(int i=0;i<datosJuego.VBalas.size();i++){
        //borrar las balas fuera de la pantalla
        if(glm::distance(datosJuego.VBalas[i].getPosition(),datosJuego.playerPos)>1'000){
            datosJuego.VBalas.erase(datosJuego.VBalas.begin()+i);
            i--;
            continue;
        }

        //Comprobar si la bala ha impactado
        for(int e=0;e<datosJuego.VEnemigos.size();e++){

            if(impactoBala(datosJuego.VBalas[i].getPosition(),datosJuego.VEnemigos[e].getPosicion(),datosJuego.VEnemigos[e].getTamanio())){
                //actualizar la vida del enemigo
                datosJuego.VEnemigos[e].setVida(datosJuego.VEnemigos[e].getVida()-datosJuego.VBalas[i].getDanio());
                //quitamos la bala
                datosJuego.VBalas.erase(datosJuego.VBalas.begin()+i);
                i--;
                if( datosJuego.VEnemigos[e].getVida() <= 0){
                    //quitamos el enemigo (lo suyo seria llamar a una mini animacion de explosion y soltar chatarra espacial)
                    datosJuego.VEnemigos.erase(datosJuego.VEnemigos.begin()+e);
                    e--;

                    break;
                }
            }
        }

    }

#pragma endregion

#pragma region Renderizado de balas
    for(auto &b : datosJuego.VBalas){
        b.render(renderer,bala1,atlasBalas);
    }
#pragma endregion



    renderer.currentCamera.follow(datosJuego.playerPos,deltaTime*300,10,200,w,h); //300 es la velocidad de la camara

    renderer.renderRectangle({datosJuego.playerPos- glm::vec2(datosJuego.tamanioNave/2,datosJuego.tamanioNave/2),  64, 64}, texturaNavePrincipal, Colors_White,{},datosJuego.direccionGiro + 90.0f);



	renderer.flush();


	//ImGui::ShowDemoWindow();

#pragma region Debug dentro del juego

    ImGui::Begin("debug");
    ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(1.0f, 0.0f, 0.0f, 1.0f));
    ImGui::Text("numero balas:  %d",(int)datosJuego.VBalas.size());
    ImGui::Text("numero enemigos: %d",(int)datosJuego.VEnemigos.size());
    ImGui::PopStyleColor();

    if(datosJuego.VEnemigos.size()>=1){
        ImGui::Text("vida Enemigo: %d",(int)datosJuego.VEnemigos[0].getVida());
    }

    if(ImGui::Button("Spawn enemigo")){
        Enemigo e;
        e.setPosicion(datosJuego.playerPos);
        e.setVida(20);
        e.setTipo({1,0});
        e.setVelocidad(350);
        e.setTamanio(64);

        datosJuego.VEnemigos.push_back(e);
    }

    if(ImGui::Button("reiniciar juego")){
        reiniciarJuego();
    }

    ImGui::End();
#pragma region controladoresAudio

    //todo falta añadir que nose pueda pasar de un valor negativo
    ImGui::Begin("Audio Debug");

    ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(0.5f, 1.0f, 1.0f, 1.0f));
    if(ImGui::Button("Mutear juego")){
        datosJuego.volumenGeneral=0;
        SetSoundVolume(sonidoDisparo,datosJuego.volumenFX*datosJuego.volumenGeneral);
        SetSoundVolume(musicaFondo,datosJuego.volumenMusica*datosJuego.volumenGeneral);
    }
    if(ImGui::Button("Mutear Musica")){
        datosJuego.volumenMusica=0;
        SetSoundVolume(musicaFondo,datosJuego.volumenMusica*datosJuego.volumenGeneral);
    }
    if(ImGui::Button("Mutear Fx")){
        datosJuego.volumenFX=0;
        SetSoundVolume(sonidoDisparo,datosJuego.volumenFX*datosJuego.volumenGeneral);
    }
    ImGui::PopStyleColor();

    ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(0.0f, 1.0f, 0.0f, 1.0f));
    if(ImGui::Button("Subir Volumen juego")){
        datosJuego.volumenGeneral+=0.05;
        SetSoundVolume(sonidoDisparo,datosJuego.volumenFX*datosJuego.volumenGeneral);
        SetSoundVolume(musicaFondo,datosJuego.volumenMusica*datosJuego.volumenGeneral);
    }
    if(ImGui::Button("Subir Volumen Musica")){
        datosJuego.volumenMusica+=0.01;
        SetSoundVolume(musicaFondo,datosJuego.volumenMusica*datosJuego.volumenGeneral);
    }
    if(ImGui::Button("Subir Volumen Fx")){
        datosJuego.volumenFX+=0.05;
        SetSoundVolume(sonidoDisparo,datosJuego.volumenFX*datosJuego.volumenGeneral);
    }
    ImGui::PopStyleColor();

    ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(1.0f, 0.0f, 0.0f, 1.0f));
    if(ImGui::Button("Bajar Volumen juego")){
        datosJuego.volumenGeneral-=0.05;
        SetSoundVolume(sonidoDisparo,datosJuego.volumenFX*datosJuego.volumenGeneral);
        SetSoundVolume(musicaFondo,datosJuego.volumenMusica*datosJuego.volumenGeneral);
    }
    if(ImGui::Button("Bajar Volumen Musica")){
        datosJuego.volumenMusica-=0.01;
        SetSoundVolume(musicaFondo,datosJuego.volumenMusica*datosJuego.volumenGeneral);
    }
    if(ImGui::Button("Bajar Volumen Fx")){
        datosJuego.volumenFX-=0.05;
        SetSoundVolume(sonidoDisparo,datosJuego.volumenFX*datosJuego.volumenGeneral);
    }
    ImGui::PopStyleColor();
    ImGui::Text("Volumen General:  %f",(float)datosJuego.volumenGeneral);
    ImGui::Text("Volumen Musica: %f",(float)datosJuego.volumenMusica);
    ImGui::Text("Volumen FX: %f",(float)datosJuego.volumenFX);

    ImGui::End();
#pragma endregion


	return true;
#pragma endregion

}

//This function might not be be called if the program is forced closed
void closeGame()
{



}
