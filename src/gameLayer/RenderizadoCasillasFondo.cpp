//
// Created by Suspr on 06/03/2025.
//

#include <RenderizadoCasillasFondo.h>

void RenderizadoCasillas::render(gl2d::Renderer2D &renderer) {

    auto viewRect = renderer.getViewRect(); //te detecta el pixel superior izquierdo del viewport

    for( int y = -1; y<2 ; y++ ){
        for( int x = -1; x<2 ; x++ ){

            int posX = x + int(viewRect.x/tamanioFondo);
            int posY = y + int(viewRect.y/tamanioFondo);

            renderer.renderRectangle(glm::vec4{posX,posY,1,1}*tamanioFondo,fondo);
        }
    }






}