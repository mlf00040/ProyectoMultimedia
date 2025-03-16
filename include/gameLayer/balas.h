//
// Created by Suspr on 09/03/2025.
//

#ifndef SPACEGAME_BALAS_H
#define SPACEGAME_BALAS_H

#include <gl2d/gl2d.h>


class Balas{

    glm::vec2 position = {};
    glm::vec2 direccion = {};

    float danio=0;

public:

    void render(gl2d::Renderer2D &renderer,
                gl2d::Texture texturaBala,
                gl2d::TextureAtlasPadding atlasBalas);

    void movimiento(float deltaTime);

    const glm::vec2 &getPosition() const;

    void setPosition(const glm::vec2 &position);

    const glm::vec2 &getDireccion() const;

    void setDireccion(const glm::vec2 &direccion);
};

#endif //SPACEGAME_BALAS_H
