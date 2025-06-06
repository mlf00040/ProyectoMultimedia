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
#include <glui/glui.h>
#include <random>
#include <animacion.h>



struct DatosJuego{

    int numEnemigosPorOleada = 5;
    //flag para evitar que se haga mas de un frame
    bool oleadaGenerada = false;
    bool oleadaAumentada = false;

    int vidaEnemigos = 5;
    bool vidaAumentada = false;

    float tiempoAcumulado= 0.0f;
    int minutos=0;
    float segundos= 0.0f;
    std::string contador ="";

    glm::vec2 playerPos = {100,100}; //todo lo suyo seria ponerlo aleatorio en el mapa
    float direccionGiro = -90.0f;
    float tamanioNave = 64.0f; // es el tamanio o hitbox de la nave
    int kills=0;
    int vidaJugador = 3;

    std::vector<Balas> VBalas;
    std::vector<Enemigo> VEnemigos;
    std::vector<animacion> VAnimacion;

    float volumenGeneral = 1;
    float volumenMusica = 0.05;
    float volumenFX = 0.2;

    //Parametros auxiliares

    bool botonAPresionadoAnteriormente = false; // para evitar que se dispare una bala por frame cuando se utiliza mando.

    //Parametros para interfaces
    bool isGame=false;

    bool isMenuPausa = false;

    bool isGameOver = false;

}datosJuego;


#pragma region cosas de semillas y random
std::random_device semilla; //genera una semilla aleatorio
std::mt19937 xd(semilla.entropy() ? semilla() : std::time(nullptr)); // si la aleatoria no tiene suficiente entropia genera una utilizando la hora actual.
std::uniform_int_distribution<> genPos(0, 1000);   //random para sacar un numero del 1 al 1000 (usado para la generacion de enemigos)
std::uniform_int_distribution<> genMap(0, 4096);    //random de 1 a 4096 para establecer al jugador en una posicion aleatoria del mapa
std::uniform_int_distribution<> genSigno(0, 1);
#pragma endregion

#pragma region inicializacion de texturas

gl2d::Renderer2D renderer;
gl2d::Texture texturaNavePrincipal;
constexpr int CAPASFONDO = 4;
gl2d::Texture texturaFondo[CAPASFONDO];

gl2d::Texture texturaBotonPrueba;

gl2d::Texture bala1;
gl2d::TextureAtlasPadding atlasBalas;

gl2d::Texture enemigo1;
gl2d::TextureAtlasPadding atlasEnemigos;

gl2d::TextureAtlasPadding atlasExplosion;
gl2d::Texture explosion;

gl2d::Texture texturaVidaJugador;
gl2d::TextureAtlasPadding atlasVidaJugador;

RenderizadoCasillas generadorCasillas[CAPASFONDO];

Sound sonidoDisparo;
Sound musicaFondo;
Sound musicaMenu;
Sound musicaGameOver;

glui::RendererUi UIrenderer;

gl2d::Font fuenteMenu;

gl2d::Font fuenteGameOver;



#pragma endregion


void reiniciarJuego(){
    //reinicializa los datos del juego
    datosJuego.isGame=false;
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

    explosion.loadFromFileWithPixelPadding(RESOURCES_PATH"animaciones/explosionAsset.png",32,true);
    atlasExplosion = gl2d::TextureAtlasPadding(3,2,explosion.GetSize().x,explosion.GetSize().y);

    texturaVidaJugador.loadFromFileWithPixelPadding(RESOURCES_PATH"animaciones/vidaNaveSpriteSheet.png",16,true);
    atlasVidaJugador = gl2d::TextureAtlasPadding(2,1,texturaVidaJugador.GetSize().x,texturaVidaJugador.GetSize().y);

    texturaBotonPrueba.loadFromFile(RESOURCES_PATH"fuentes/PruebaBoton.png",true);

    generadorCasillas[0].fondo = texturaFondo[0];
    generadorCasillas[1].fondo = texturaFondo[1];
    generadorCasillas[2].fondo = texturaFondo[2];
    generadorCasillas[3].fondo = texturaFondo[3];

    generadorCasillas[0].fuerzaDistorsion = 0;
    generadorCasillas[1].fuerzaDistorsion = 0.25;
    generadorCasillas[2].fuerzaDistorsion = 0.5;
    generadorCasillas[3].fuerzaDistorsion = 0.75;

    fuenteMenu.createFromFile(RESOURCES_PATH"roboto_black.ttf");

    fuenteGameOver.createFromFile(RESOURCES_PATH"fuentes/OptimusPrincepsSemiBold.ttf");

#pragma endregion

#pragma region sonido

    sonidoDisparo = LoadSound(RESOURCES_PATH"sonidos/Disparo1.wav");
    SetSoundVolume(sonidoDisparo,datosJuego.volumenFX*datosJuego.volumenGeneral);

    musicaFondo = LoadSound(RESOURCES_PATH"sonidos/MusicaDelJuegoProvisional.wav");
    SetSoundVolume(musicaFondo,datosJuego.volumenMusica*datosJuego.volumenGeneral);
    //todo cambiar el formato mp3 a .raw
    musicaMenu = LoadSound(RESOURCES_PATH"sonidos/MusicaMenu.mp3");
    SetSoundVolume(musicaMenu,datosJuego.volumenMusica*datosJuego.volumenGeneral);

    musicaGameOver = LoadSound(RESOURCES_PATH"sonidos/UndertaleOSTDetermination.mp3");
    SetSoundVolume(musicaGameOver,datosJuego.volumenMusica*datosJuego.volumenGeneral);

#pragma endregion

	return true;
}

void gamePlay(float deltaTime,int w,int h){

#pragma region Gestion de tiempos

    datosJuego.tiempoAcumulado +=deltaTime;

    if(datosJuego.tiempoAcumulado >= 1.0f){
        datosJuego.segundos++;
        datosJuego.tiempoAcumulado -= 1.0f;

        if(datosJuego.segundos >= 60){
            datosJuego.minutos++;
            datosJuego.segundos -= 60;
        }
    }

    datosJuego.contador= std::to_string(datosJuego.minutos)+" : "+std::to_string(static_cast<int>(datosJuego.segundos));



#pragma endregion

#pragma region movimiento y teclas

    if(platform::isButtonHeld(platform::Button::Escape) ){
        datosJuego.isMenuPausa=!datosJuego.isMenuPausa;
    }

    glm::vec2 movimiento = {};

    //movimiento de posicion con teclado
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

    //Movimiento con mando
    float deadZone = 0.2f;
    //comprobar si el mando esta conectado
    if (glfwJoystickPresent(GLFW_JOYSTICK_1) && glfwJoystickIsGamepad(GLFW_JOYSTICK_1)) {

        GLFWgamepadstate state;

        if (glfwGetGamepadState(GLFW_JOYSTICK_1, &state)) {

            float axisX = state.axes[GLFW_GAMEPAD_AXIS_LEFT_X];
            float axisY = state.axes[GLFW_GAMEPAD_AXIS_LEFT_Y];

            if (std::abs(axisX) > deadZone || std::abs(axisY) > deadZone) {
                //si se pasa la zona muerta se mueve el personaje
                movimiento.x += axisX;
                movimiento.y += axisY;
            }

        }
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

    //bucle para que carge las distintas capas del fondo y asi dar efecto de movimiento
    for(int i=0; i< CAPASFONDO;i++){
        generadorCasillas[i].render(renderer);
    }

#pragma endregion

#pragma region manejo de enemigos

    //cada 30 segundos se aumenta en 1 la cantidad de spawneo
    if((static_cast<int>(datosJuego.segundos)%30)==0 && !datosJuego.oleadaAumentada && (static_cast<int>(datosJuego.segundos)!=0) ){

        datosJuego.oleadaAumentada= true;
        datosJuego.numEnemigosPorOleada++;

    }else if((static_cast<int>(datosJuego.segundos)%30)!=0){

        datosJuego.oleadaAumentada=false;

    }

    //cada minuto se le suma 5 mas a la vida de los enemigos
    if((static_cast<int>(datosJuego.segundos)%59)==0 && !datosJuego.vidaAumentada && (static_cast<int>(datosJuego.segundos)!=0)){

        datosJuego.vidaAumentada= true;
        datosJuego.vidaEnemigos += 2;

    }else if((static_cast<int>(datosJuego.segundos)%59)!=0){

        datosJuego.vidaAumentada=false;

    }

    //spawn de los enemigos cada 5 segundos
    if((static_cast<int>(datosJuego.segundos)%5)==0 && !datosJuego.oleadaGenerada){
        datosJuego.oleadaGenerada=true; //flag para evitar que se generen en mas de un frame
        for(int i=0;i<datosJuego.numEnemigosPorOleada;i++){
            bool posicionValida = false;

            glm::vec2 nuevaPosicion = {0, 0};
            const float radioMinimo = 600.0f; // Radio mínimo alrededor del jugador

            while (!posicionValida) {

                int posXrand = genPos(xd);
                int posYrand = genPos(xd);
                int Xsigno = genSigno(xd);
                int Ysigno = genSigno(xd);

                glm::vec2 posSumada = {0, 0};

                if (Xsigno == 1) {
                    posSumada.x += posXrand;
                } else {
                    posSumada.x -= posXrand;
                }
                if (Ysigno == 1) {
                    posSumada.y += posYrand;
                } else {
                    posSumada.y -= posYrand;
                }

                nuevaPosicion = {datosJuego.playerPos.x + posSumada.x, datosJuego.playerPos.y + posSumada.y};

                // Calcular la distancia entre el jugador y la nueva posición
                float distancia = glm::distance(datosJuego.playerPos, nuevaPosicion);

                // Verificar si la distancia es mayor que el radio mínimo
                if (distancia > radioMinimo) {
                    posicionValida = true;
                }
            }

            spawnEnemigo(datosJuego.VEnemigos, datosJuego.vidaEnemigos, 1, 0, 350, 64, nuevaPosicion.x, nuevaPosicion.y);

        }
    }else if ((static_cast<int>(datosJuego.segundos)%5)!=0){
        datosJuego.oleadaGenerada=false;
    }

    //actualiza el movimiento de los enemigos
    for(auto &e : datosJuego.VEnemigos){
        e.movimiento(deltaTime,datosJuego.playerPos);
    }

    //comprobar si un enemigo ha impactado al nave principal
    for(int i=0;i<datosJuego.VEnemigos.size();i++){

        if(impacto(datosJuego.VEnemigos[i].getPosicion(),datosJuego.playerPos,datosJuego.tamanioNave,datosJuego.VEnemigos[i].getTamanio())){
            //actualizar la vida del jugador
            if(datosJuego.vidaJugador>0){
                datosJuego.vidaJugador--;
            }

            //std::cout << "enemigo a impactado jugador"<<std::endl;

            //crear animacion de explosion en el lugar donde ha muerto el enemigo
            datosJuego.VAnimacion.emplace_back((datosJuego.VEnemigos[i].getPosicion()-glm::vec2{datosJuego.VEnemigos[i].getTamanio()/2,datosJuego.VEnemigos[i].getTamanio()/2})
                    ,explosion,atlasExplosion,glm::vec2{datosJuego.VEnemigos[i].getTamanio(),datosJuego.VEnemigos[i].getTamanio()}
                    ,0.125f,6);

            //quitamos al enemigo
            datosJuego.VEnemigos.erase(datosJuego.VEnemigos.begin()+i);
            i--;

            //reproducir audio de muerte.
        }


    }

#pragma endregion

#pragma region renderizado enemigos
    //renderizar enemigos
    for(auto &e : datosJuego.VEnemigos){
        e.render(renderer,enemigo1,atlasEnemigos,glm::vec2(1,0),datosJuego.playerPos);
    }

#pragma endregion

#pragma region manejar balas

//habra que adaptar esto a una utilizacion por tiempo y parametro

    //disparo con mando.
    glfwPollEvents();
    if (glfwJoystickPresent(GLFW_JOYSTICK_1) && glfwJoystickIsGamepad(GLFW_JOYSTICK_1)) {
        GLFWgamepadstate state;


        if (glfwGetGamepadState(GLFW_JOYSTICK_1, &state)) {
            // Obtener el estado actual del botón A
            bool botonAPresionadoAhora = state.buttons[GLFW_GAMEPAD_BUTTON_B];

            // Detección de flanco: detectar solo cuando el botón pasa de no presionado a presionado
            if (!datosJuego.botonAPresionadoAnteriormente && botonAPresionadoAhora) {

                if (!IsSoundPlaying(sonidoDisparo)) {
                    PlaySound(sonidoDisparo);
                }
                Balas b;
                //para medio centrar las balas
                glm::vec2 centroJugador = {datosJuego.playerPos.x ,
                                           datosJuego.playerPos.y}; // asi salen ya directamente del centro del jugador

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

            // Actualizar el estado anterior del botón A
            datosJuego.botonAPresionadoAnteriormente = botonAPresionadoAhora;
        }



    }

    //disparo con el raton
    if(platform::isRMousePressed()){
        //si no esta sonando el sonido de la bala lo reproducimos. todo seria mas eficaz con un vector de 3 o 4 sonidos porque se dispara mas rapido que lo que suena el sonido
        if(!IsSoundPlaying(sonidoDisparo)){
            PlaySound(sonidoDisparo);
        }

        Balas b;

        //para  centrar las balas
        glm::vec2 centroJugador={datosJuego.playerPos.x,
                                 datosJuego.playerPos.y}; // asi salen ya directamente del centro del jugador

        b.setPosition(centroJugador);

        //como las balas van autodirigidas se calcula el enemigo mas cercano
        glm::vec2 posEnemigoMasCercano = calculaPosEnemigoMasCercano(datosJuego.VEnemigos, centroJugador);
        glm::vec2 direccion = posEnemigoMasCercano - centroJugador;
        if (glm::length(direccion) > 0) {
            direccion = glm::normalize(direccion);
        }
        b.setDireccion(direccion);

        //se establece el danio de la bala
        b.setDanio(5);

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

        //Comprobar si la bala ha impactado en un enemigo
        for(int e=0;e<datosJuego.VEnemigos.size();e++){

            if(impacto(datosJuego.VBalas[i].getPosition(),datosJuego.VEnemigos[e].getPosicion(),datosJuego.VEnemigos[e].getTamanio(),datosJuego.VBalas[i].getTamanio())){
                //actualizar la vida del enemigo
                datosJuego.VEnemigos[e].setVida(datosJuego.VEnemigos[e].getVida()-datosJuego.VBalas[i].getDanio());
                //quitamos la bala
                datosJuego.VBalas.erase(datosJuego.VBalas.begin()+i);
                i--;
                if( datosJuego.VEnemigos[e].getVida() <= 0){

                    //inicializa la animacion.

                    //la pos del enemigo apunta al centro del enemigo, por tanto hay que restarle el tamaño del enemigo para que de la pos de la esquina superior izquierda
                    datosJuego.VAnimacion.emplace_back((datosJuego.VEnemigos[e].getPosicion()-glm::vec2{datosJuego.VEnemigos[e].getTamanio()/2,datosJuego.VEnemigos[e].getTamanio()/2})
                                                       ,explosion,atlasExplosion,glm::vec2{datosJuego.VEnemigos[e].getTamanio(),datosJuego.VEnemigos[e].getTamanio()}
                                                       ,0.125f,6);

                    //borra al enemigo
                    datosJuego.VEnemigos.erase(datosJuego.VEnemigos.begin()+e);
                    e--;
                    //aumenta en 1 las kills
                    datosJuego.kills++;

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

#pragma region manejo vida jugador
    if(datosJuego.vidaJugador==0){

        datosJuego.isGame=false;
        datosJuego.isGameOver=true;
    }
#pragma endregion

    //movimiento de la camara al seguir al jugador. (300 es la velocidad de la camara y asi da efecto de incercia)
    renderer.currentCamera.follow(datosJuego.playerPos,deltaTime*300,10,200,w,h);

    //renderiza a la nave jugador
    renderer.renderRectangle({datosJuego.playerPos- glm::vec2(datosJuego.tamanioNave/2,datosJuego.tamanioNave/2),  64, 64}, texturaNavePrincipal, Colors_White,{},datosJuego.direccionGiro + 90.0f);

#pragma region renderizadoAnimaciones

    for(int i=0;i<datosJuego.VAnimacion.size();i++){
        //actualiza el estado de la animacion
        datosJuego.VAnimacion[i].actualizar(deltaTime);

        if (datosJuego.VAnimacion[i].estaActiva()){
            datosJuego.VAnimacion[i].renderizar(renderer);
        }else{
            datosJuego.VAnimacion.erase(datosJuego.VAnimacion.begin()+i);
            i--;
        }
    }

#pragma endregion

#pragma region elementos UI

    //BLoque de la vida del jugador
    renderer.pushCamera();
    //Bloque para la vida del jugador arriba izquierda
    glui::Frame vidaFrame({0, 0, w, h});

    glui::Box vidaBox = glui::Box().xLeftPerc(0.05).yTopPerc(0.005)
            .xDimensionPercentage(0.125).yDimensionPercentage(0.125);

    //Renderizar corazones según la vida restante
    for (int i = 0; i < datosJuego.vidaJugador; i++) {
        glm::vec4 rect = {i * vidaBox.dimensions.x,vidaBox.dimensions.y, vidaBox.dimensions.z/2, vidaBox.dimensions.w/2}; // Posición y tamaño de cada corazón
        renderer.renderRectangle(rect, texturaVidaJugador, Colors_White, {},0,atlasVidaJugador.get(0,0,0));
    }
    renderer.popCamera();


    //Bloque del contador
    renderer.pushCamera();

    glui::Frame cont({0,0,w,h});
    glui::Box contadorBox = glui::Box().xLeftPerc(0.375).xDimensionPercentage(0.25).yAspectRatio(2.0f/8.0f);
    renderer.render9Patch2(contadorBox, Colors_White,
                           {}, 0, texturaBotonPrueba, GL2D_DefaultTextureCoords, {0.2,0.8,0.8,0.2});
    //funcion para renderizar el texto dentro de la textura del fondo.
    //la posicion que hay que pasarle al texto es la del centro del rectangulo porque esta activado lo de centrar
    renderer.renderText({contadorBox.dimensions.x+contadorBox.dimensions.z/2,contadorBox.dimensions.y+contadorBox.dimensions.w/2-(contadorBox.dimensions.w/12)},
                        datosJuego.contador.c_str(),fuenteMenu,Colors_Gray,renderer.determineTextRescaleFit(datosJuego.contador.c_str(),fuenteMenu,contadorBox)
                        ,3,3,1,Colors_Transparent);


    //Bloque del contador de enemigos muertos abajo izquierda

    glui::Frame kil({0,0,w,h});
    glui::Box killsBox = glui::Box().xLeftPerc(0.01).xDimensionPercentage(0.075).yTopPerc(0.95).yAspectRatio(2.0f/8.0f);
    //funcion para renderizar el texto dentro de la textura del fondo.
    std::string contKills = "Kills: "+std::to_string(datosJuego.kills);
    //la posicion que hay que pasarle al texto es la del centro del rectangulo, en este caso como es 0 es la esquina inferior izquierda.
    renderer.renderText({killsBox.dimensions.x,killsBox.dimensions.y+killsBox.dimensions.w},
                        contKills.c_str(),fuenteMenu,Colors_White,
                        renderer.determineTextRescaleFit(contKills,fuenteMenu,killsBox),
                        3,3,0,Colors_Transparent);

    renderer.popCamera();

#pragma endregion


#pragma region Debug dentro del juego
/*
    ImGui::Begin("debug");
    ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(1.0f, 0.0f, 0.0f, 1.0f));
    ImGui::Text("numero balas:  %d",(int)datosJuego.VBalas.size());
    ImGui::Text("numero enemigos: %d",(int)datosJuego.VEnemigos.size());
    ImGui::Text("n enemigos oleada: %d",(int)datosJuego.numEnemigosPorOleada);
    ImGui::Text("Vida enemigos: %d",(int)datosJuego.vidaEnemigos);

    ImGui::PopStyleColor();

    if(datosJuego.VEnemigos.size()>=1){
        ImGui::Text("vida Enemigo: %d",(int)datosJuego.VEnemigos[0].getVida());
    }

    if(ImGui::Button("Spawn enemigo")){

        bool posicionValida = false;
        glm::vec2 nuevaPosicion = {0, 0};
        const float radioMinimo = 600.0f; // Radio mínimo alrededor del jugador

        while (!posicionValida) {

            int posXrand = genPos(xd);
            int posYrand = genPos(xd);
            int Xsigno = genSigno(xd);
            int Ysigno = genSigno(xd);

            glm::vec2 posSumada = {0, 0};

            if (Xsigno == 1) {
                posSumada.x += posXrand;
            } else {
                posSumada.x -= posXrand;
            }
            if (Ysigno == 1) {
                posSumada.y += posYrand;
            } else {
                posSumada.y -= posYrand;
            }

            nuevaPosicion = {datosJuego.playerPos.x + posSumada.x, datosJuego.playerPos.y + posSumada.y};

            // Calcular la distancia entre el jugador y la nueva posición
            float distancia = glm::distance(datosJuego.playerPos, nuevaPosicion);

            // Verificar si la distancia es mayor que el radio mínimo
            if (distancia > radioMinimo) {
                posicionValida = true;
            }
        }

        spawnEnemigo(datosJuego.VEnemigos, 5, 1, 0, 350, 64, nuevaPosicion.x, nuevaPosicion.y);

    }

    if(ImGui::Button("reiniciar juego")){
        reiniciarJuego();
    }

    ImGui::End();
#pragma endregion

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
    */
#pragma endregion

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
    if(datosJuego.isMenuPausa){

    #pragma region Interfaz ajustes


        renderer.pushCamera();
        UIrenderer.Begin(4);

        glui::Frame menuPausa({0,0,w,h});

        renderer.renderRectangle({0,0,w,h}, texturaFondo[0], Colors_White,{},0);
        renderer.renderRectangle({0,0,w,h}, texturaFondo[1], Colors_White,{},0);
        renderer.renderRectangle({0,0,w,h}, texturaFondo[2], Colors_White,{},0);


        UIrenderer.sliderFloat("Volumen General",&datosJuego.volumenGeneral,0,1);
        UIrenderer.sliderFloat("Volumen Musica ",&datosJuego.volumenMusica,0,1);
        UIrenderer.sliderFloat("Volumen FX",&datosJuego.volumenFX,0,1);

        SetSoundVolume(sonidoDisparo,datosJuego.volumenFX*datosJuego.volumenGeneral);
        SetSoundVolume(musicaFondo,datosJuego.volumenMusica*datosJuego.volumenGeneral);
        SetSoundVolume(musicaGameOver,datosJuego.volumenMusica*datosJuego.volumenGeneral);
        SetSoundVolume(musicaMenu,datosJuego.volumenMusica*datosJuego.volumenGeneral);

        if(UIrenderer.Button("Atras",Colors_White,texturaBotonPrueba)){

            datosJuego.isMenuPausa=false;
        }

        UIrenderer.End();

        UIrenderer.renderFrame(renderer,fuenteMenu,platform::getRelMousePosition(),platform::isLMousePressed(),platform::isLMouseHeld(),
                               platform::isLMouseReleased(),platform::isButtonReleased(platform::Button::Escape),platform::getTypedInput(),deltaTime);


        renderer.popCamera();


    #pragma endregion

    }else if(datosJuego.isGame){

        StopSound(musicaMenu);
        StopSound(musicaGameOver);

        if(!IsSoundPlaying(musicaFondo)){

            PlaySound(musicaFondo);
        }

        //llamamos a la funcion que ejecuta el juego.
        gamePlay(deltaTime,w,h);

    }else if (datosJuego.isGameOver){

    #pragma region Pantalla Game Over

        StopSound(musicaFondo);

        if(!IsSoundPlaying(musicaGameOver)){

            PlaySound(musicaGameOver);
        }

        renderer.pushCamera();

        glui::Frame finJuegoFrame({0,0,w,h});

        renderer.renderRectangle({0,0,w,h},Colors_Black);

        UIrenderer.Begin(5);

        UIrenderer.Text("Game Over",Colors_Red);

        UIrenderer.Text(("Kills: "+ std::to_string(datosJuego.kills)+"              "+"Tiempo: "+datosJuego.contador.c_str()),Colors_White);


        if(UIrenderer.Button("Restart",Colors_White,texturaBotonPrueba)){

            datosJuego.isGameOver=false;

            reiniciarJuego();

            datosJuego.isGame=true;
        }

        if(UIrenderer.Button("Menu",Colors_White,texturaBotonPrueba)){

            datosJuego.isGameOver=false;
            datosJuego={};
        }

        UIrenderer.End();

        UIrenderer.renderFrame(renderer,fuenteGameOver,platform::getRelMousePosition(),platform::isLMousePressed(),platform::isLMouseHeld(),
                               platform::isLMouseReleased(),platform::isButtonReleased(platform::Button::Escape),platform::getTypedInput(),deltaTime);


        renderer.popCamera();

    #pragma endregion


    }else{

    #pragma region interfaz_inicio
        //si la musica del menu no se esta reproduciendo la ponemos

        StopSound(musicaGameOver);
        StopSound(musicaFondo);

        if(!IsSoundPlaying(musicaMenu)){
            PlaySound(musicaMenu);
        }

        renderer.pushCamera();

        glui::Frame menu({0,0,w,h});

        renderer.renderRectangle({0,0,w,h}, texturaFondo[0], Colors_White,{},0);
        renderer.renderRectangle({0,0,w,h}, texturaFondo[1], Colors_White,{},0);
        renderer.renderRectangle({0,0,w,h}, texturaFondo[2], Colors_White,{},0);


        renderer.renderRectangle({(w/8),(h/4),(w/4),(h/2)}, texturaNavePrincipal, Colors_White,{},0);

        //Lado izquierdo de la interfaz de Inicio (carga los botones)
        UIrenderer.Begin(12);

        UIrenderer.newColum(1);

        //boton para jugar
        if(UIrenderer.Button("Jugar",Colors_White,texturaBotonPrueba)){

            datosJuego.isGame=true;
        }

        //boton para el menu de opciones, todo en un archivo aparte que sean solo los constructores de las interfazces
        if(UIrenderer.Button("Opciones",Colors_White,texturaBotonPrueba)){

            datosJuego.isMenuPausa=true;
        }

        //boton para salir del juego
        if(UIrenderer.Button("Salir",Colors_White,texturaBotonPrueba)){
            return 0;
        }

        UIrenderer.End();

        //renderiza toda la columna
        UIrenderer.renderFrame(renderer,fuenteMenu,platform::getRelMousePosition(),platform::isLMousePressed(),platform::isLMouseHeld(),
                               platform::isLMouseReleased(),platform::isButtonReleased(platform::Button::Escape),platform::getTypedInput(),deltaTime);

        renderer.popCamera();


    #pragma endregion

    }
    renderer.flush();

    return true;

}

//This function might not be be called if the program is forced closed
void closeGame()
{



}

