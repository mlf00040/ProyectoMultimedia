//
// Created by Suspr on 17/03/2025.
//

#include <limits>
#include <glm/glm.hpp>
#include <metodosAuxiliares.h>

glm::vec2 calculaPosEnemigoMasCercano(const std::vector<Enemigo> &vEnemigos, const glm::vec2 &posJugador) {

    //verificar si hay enemigos
    if (vEnemigos.empty()) {

        return glm::vec2(-1, -1);
    }

    float distanciaMinimaAlCuadrado = std::numeric_limits<float>::max();
    glm::vec2 posicionEnemigoMasCercano;

    for (const Enemigo &enemigo : vEnemigos) {
        const glm::vec2 &posEnemigo = enemigo.getPosicion();

        float disX = posEnemigo.x - posJugador.x;
        float disY = posEnemigo.y - posJugador.y;

        //se calcula la distancia al cuadrado porque es mas eficiente y no hay que usar el sqrt
        float distanciaAlCuadrado = disX * disX + disY * disY;

        if (distanciaAlCuadrado < distanciaMinimaAlCuadrado) {
            distanciaMinimaAlCuadrado = distanciaAlCuadrado;
            posicionEnemigoMasCercano = posEnemigo;
        }
    }
    return posicionEnemigoMasCercano;
}

bool impactoBala(const glm::vec2 &posBala,const glm::vec2 &posNave,const float tamanioNave){
    if(glm::distance(posBala,posNave-tamanioNave/2)<= tamanioNave/2){
        return true;
    }else{
        return false;
    }
}

void spawnEnemigo(std::vector<Enemigo> &vEnemigos,int vida, int tipox, int tipoy, int velocidad, int tamanio,
                  int nuevaPosX, int nuevaPosY){

    Enemigo e;
    e.setPosicion({nuevaPosX,nuevaPosY});
    e.setVida(vida);
    e.setTipo({tipox,tipoy});
    e.setVelocidad(velocidad);
    e.setTamanio(tamanio);

    vEnemigos.push_back(e);

}