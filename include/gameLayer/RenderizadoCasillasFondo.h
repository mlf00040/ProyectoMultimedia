//
// Created by Suspr on 06/03/2025.
//
#ifndef SPACEGAME_RENDERIZADOCASILLASFONDO_H
#define SPACEGAME_RENDERIZADOCASILLASFONDO_H


#include <gl2d/gl2d.h>

struct RenderizadoCasillas{

    float tamanioFondo = 4096;
    gl2d::Texture fondo;
    float fuerzaDistorsion = 1;

    void render (gl2d::Renderer2D &renderer);
};


#endif //SPACEGAME_RENDERIZADOCASILLASFONDO_H
