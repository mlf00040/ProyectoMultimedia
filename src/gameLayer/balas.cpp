//
// Created by Suspr on 09/03/2025.
//

#include <balas.h>

void Balas::render(gl2d::Renderer2D &renderer,
                   gl2d::Texture texturaBala,
                   gl2d::TextureAtlasPadding atlasBalas){

    glm::vec4 rect = {position.x-tamanio/2, position.y-tamanio/2, tamanio, tamanio}; // Tamaño de la bala
    renderer.renderRectangle(rect, texturaBala, Colors_White,{},glm::degrees(atan2(-direccion.y, direccion.x))-90.0f,atlasBalas.get(1,0));

}

void Balas::movimiento(float deltaTime) {

    position += direccion * deltaTime * velocidad;

}

const glm::vec2 &Balas::getPosition() const {
    return position;
}

void Balas::setPosition(const glm::vec2 &position) {
    Balas::position = position;
}

const glm::vec2 &Balas::getDireccion() const {
    return direccion;
}

void Balas::setDireccion(const glm::vec2 &direccion) {
    Balas::direccion = direccion;
}

float Balas::getDanio() const {
    return danio;
}

void Balas::setDanio(float danio) {
    Balas::danio = danio;
}

float Balas::getVelocidad() const {
    return velocidad;
}

void Balas::setVelocidad(float velocidad) {
    Balas::velocidad = velocidad;
}

float Balas::getTamanio() const {
    return tamanio;
}

void Balas::setTamanio(float tamanio) {
    Balas::tamanio = tamanio;
}
