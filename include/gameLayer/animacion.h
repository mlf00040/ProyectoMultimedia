//
// Created by Suspr on 27/04/2025.
//

#ifndef SPACEGAME_ANIMACION_H
#define SPACEGAME_ANIMACION_H

#include <gl2d/gl2d.h>
#include <iostream>

class animacion{
    glm::vec2 posicion;
    gl2d::Texture textura;
    gl2d::TextureAtlasPadding atlas;
    glm::vec2 tamanoSprite;
    float tiempoTranscurrido = 0.0f;
    float duracionFrame;              // Duración de cada frame (en segundos)
    int frameActual = 0;              // Frame actual de la animación
    int totalFrames;                  // Total de frames en el atlas
    bool activa = true;

public:
    animacion(glm::vec2 posicion, gl2d::Texture textura, gl2d::TextureAtlasPadding atlas,glm::vec2 tamanoSprite,
    float duracionFrame = 0.5f, int totalFrames = 6)
    : posicion(posicion), textura(textura), atlas(atlas),tamanoSprite(tamanoSprite), duracionFrame(duracionFrame),
            totalFrames(totalFrames){};

    void actualizar(float deltaTime);

    void renderizar(gl2d::Renderer2D &renderer) ;

    bool estaActiva() const;


};

#endif //SPACEGAME_ANIMACION_H
