//
// Created by Suspr on 16/03/2025.
//

#include <enemigos.h>
#include <iostream>

void Enemigo::render(gl2d::Renderer2D &renderer, gl2d::Texture texturaEnemigo, gl2d::TextureAtlasPadding atlasEnemigos,glm::vec2 tipo,glm::vec2 posJugador) {

    glm::vec4 rect = {posicion.x-(64/2), posicion.y-(64/2), 64, 64}; // Tamaño de los enemigos
    glm::vec2 dir = posJugador - posicion;
    glm::vec2 dirNormalizado = glm::normalize(dir);
    renderer.renderRectangle(rect, texturaEnemigo, Colors_White,{},glm::degrees(atan2(-dirNormalizado.y, dirNormalizado.x))-90.0f,atlasEnemigos.get(tipo.x,tipo.y));
}

void Enemigo::movimiento(float deltaTime, glm::vec2 posicionJugador) {


    glm::vec2 direccionAlJugador = posicionJugador - posicion;
    if(glm::length(direccionAlJugador)==0){
        direccionAlJugador={1,0};
    }else{
        direccionAlJugador = glm::normalize(direccionAlJugador);
    }

    glm::vec2 nuevaDireccion = direccionAlJugador + direccion;
    if(glm::length(nuevaDireccion) == 0){
        nuevaDireccion = glm::vec2 (direccionAlJugador.y,-direccionAlJugador.x);
    }else{
        nuevaDireccion= deltaTime * 5.0f * direccionAlJugador + direccion;
    }
    nuevaDireccion = glm::normalize(nuevaDireccion);

    direccion = nuevaDireccion;

    posicion +=direccion *deltaTime*velocidad;

}

const glm::vec2 &Enemigo::getPosicion() const {
    return posicion;
}

void Enemigo::setPosicion(const glm::vec2 &posicion) {
    Enemigo::posicion = posicion;
}

const glm::vec2 &Enemigo::getDireccion() const {
    return direccion;
}

void Enemigo::setDireccion(const glm::vec2 &direccion) {
    Enemigo::direccion = direccion;
}

const glm::ivec2 &Enemigo::getTipo() const {
    return tipo;
}

void Enemigo::setTipo(const glm::ivec2 &tipo) {
    Enemigo::tipo = tipo;
}

float Enemigo::getVida() const {
    return vida;
}

void Enemigo::setVida(float vida) {
    Enemigo::vida = vida;
}

float Enemigo::getVelocidad() const {
    return velocidad;
}

void Enemigo::setVelocidad(float velocidad) {
    Enemigo::velocidad = velocidad;
}
