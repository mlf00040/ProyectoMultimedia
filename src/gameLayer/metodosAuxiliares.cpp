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

    // si la distancia entre la bala y el centro de la nave enemiga es menor o igual que el radio de la nave significa que la bala a impactado
    return glm::distance(posBala,posNave- glm::vec2(tamanioNave / 2, tamanioNave / 2)) <= (tamanioNave/2);

}

bool impacto(const glm::vec2 &posOtro, const glm::vec2 &posNave, const float tamanioNave, const float tamanioOtro) {
    //Calcular la distancia entre los centros, para ello calculamos el centro de los objetos ya que por defecto no lo es
    float distancia = glm::distance(posNave + glm::vec2(tamanioNave / 2, tamanioNave / 2), posOtro + glm::vec2(tamanioOtro / 2, tamanioOtro / 2));

    //Calcular la suma de los radios (mitad de los tamaños y una reduccion de uno de los componentes para que encaje bien visualmente)
    float radioTotal = ((tamanioNave / 2)-(tamanioNave/10) + (tamanioOtro / 2));

    return distancia <= radioTotal;
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