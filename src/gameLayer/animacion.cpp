//
// Created by Suspr on 27/04/2025.
//

#include <animacion.h>

//actualiza la animacion si se ha pasado el tiempo establecido
void animacion::actualizar(float deltaTime) {
    if(!activa) return;

    tiempoTranscurrido +=deltaTime;

    if(tiempoTranscurrido >= duracionFrame){

        tiempoTranscurrido-=duracionFrame;
        frameActual++;

        if (frameActual >= totalFrames){
            activa=false;
        }
    }
}

void animacion::actualizarManual() {
    frameActual++;

    if (frameActual >= totalFrames){
        activa=false;
    }
}

void animacion::renderizar(gl2d::Renderer2D &renderer)  {
    if (!activa) return;

    glm::vec4 rect = {posicion.x, posicion.y, tamanoSprite.x, tamanoSprite.y};

    //calcula el frame actual del atlas
    int fila = frameActual / atlas.xCount;
    int columna = frameActual % atlas.xCount;


    renderer.renderRectangle(rect, textura, Colors_White, {}, 0,atlas.get(columna,fila,0));
}

bool animacion::estaActiva() const {
    return activa;
}