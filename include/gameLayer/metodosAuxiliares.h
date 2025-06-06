//
// Created by Suspr on 17/03/2025.
//

#ifndef SPACEGAME_METODOSAUXILIARES_H
#define SPACEGAME_METODOSAUXILIARES_H

#include <gl2d/gl2d.h>
#include <balas.h>
#include <enemigos.h>
#include <vector>
#include <glui/glui.h>

/**
 * @brief Metodo para calcular el enemigo mas cercano de la manera mas eficiente posible
 * @param vEnemigos
 * @param posJugador
 * @return
 */
glm::vec2 calculaPosEnemigoMasCercano(const std::vector<Enemigo> &vEnemigos, const glm::vec2 &posJugador);

bool impacto(const glm::vec2 &posOtro, const glm::vec2 &posNave, const float tamanioNave, const float tamanioOtro);


void spawnEnemigo(std::vector<Enemigo> &vEnemigos,int vida, int tipox, int tipoy, int velocidad, int tamanio,
                 int nuevaPosX, int nuevaPosY);



#endif //SPACEGAME_METODOSAUXILIARES_H
