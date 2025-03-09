//
// Created by Suspr on 09/03/2025.
//

#include <balas.h>

void Balas::render(gl2d::Renderer2D &renderer,
                   gl2d::Texture texturaBala,
                   gl2d::TextureAtlasPadding atlasBalas){

    glm::vec4 rect = {position.x, position.y, 32, 32}; // Tamaño de la bala
    renderer.renderRectangle(rect, texturaBala, Colors_White,{},{},atlasBalas.get(0,0));

}
