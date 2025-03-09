//
// Created by Suspr on 09/03/2025.
//

#ifndef SPACEGAME_BALAS_H
#define SPACEGAME_BALAS_H

#include <gl2d/gl2d.h>


struct Balas{

    glm::vec2 position = {};
    glm::vec2 direccion = {};

    void render(gl2d::Renderer2D &renderer,
                gl2d::Texture texturaBala,
    gl2d::TextureAtlasPadding atlasBalas);
};



#endif //SPACEGAME_BALAS_H
