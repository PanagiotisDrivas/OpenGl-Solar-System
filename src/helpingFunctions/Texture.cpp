#include "Texture.h"

#include <iostream>

// initialize with image path and type
Texture::Texture(std::string dir, std::string path, aiTextureType type)
:dir(dir), path(path), type(type){
    generate();
}

// generate texture id
void Texture::generate() {
    
    glGenTextures(1, &id);
    glBindTexture(GL_TEXTURE_2D, id); //all upcoming GL_TEXTURE_2D operations now have effect on this texture object
}

//load image, create texture and generate mipmaps
void Texture::load(bool flip) {
    stbi_set_flip_vertically_on_load(flip);

    int width, height, nChannels;

    unsigned char* data = stbi_load((dir+"/"+path).c_str(), &width, &height, &nChannels, 0);

    GLenum colorMode = GL_RGB;
    switch (nChannels) {
    case 1:
        colorMode = GL_RED;
        break;
    case 4:
        colorMode = GL_RGBA;
        break;
    };
    
    if (data) {
        
        glBindTexture(GL_TEXTURE_2D, id);
        glTexImage2D(GL_TEXTURE_2D, 0, colorMode, width, height, 0, colorMode, GL_UNSIGNED_BYTE, data);
        glGenerateMipmap(GL_TEXTURE_2D);

        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    }else{
        std::cout << "Image not loaded at " << path << std::endl;
    }

    stbi_image_free(data);
}

// bind texture id(This function should be called before rendering to ensure that the correct texture is used.)
void Texture::bind() {
    glBindTexture(GL_TEXTURE_2D, id);
    //glActiveTexture(GL_TEXTURE0 + id);
}