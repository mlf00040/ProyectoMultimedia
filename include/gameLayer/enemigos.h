//
// Created by Suspr on 16/03/2025.
//

#ifndef SPACEGAME_ENEMIGOS_H
#define SPACEGAME_ENEMIGOS_H

#include <gl2d/gl2d.h>


class Enemigo{

    glm::vec2 posicion = {};
    glm::vec2 direccion = {};
    glm::ivec2 tipo = {};

    float vida=0;
    float velocidad;
    float tamanio=64;

public:

    void render(gl2d::Renderer2D &renderer,
               gl2d::Texture texturaEnemigo,
               gl2d::TextureAtlasPadding atlasEnemigos,
               glm::vec2 tipo,
                glm::vec2 posJugador);

    void movimiento(float deltaTime, glm::vec2 posicionJugador);

    const glm::vec2 &getPosicion() const;

    void setPosicion(const glm::vec2 &posicion);

    const glm::vec2 &getDireccion() const;

    void setDireccion(const glm::vec2 &direccion);

    const glm::ivec2 &getTipo() const;

    void setTipo(const glm::ivec2 &tipo);

    float getVida() const;

    void setVida(float vida);

    float getVelocidad() const;

    void setVelocidad(float velocidad);

    float getTamanio() const;

    void setTamanio(float tamanio);
};

#endif //SPACEGAME_ENEMIGOS_H
