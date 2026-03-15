#pragma once

#include <iostream>
#include "stb_image.h"

GLuint setup_texture(const char* filename)
{
    glEnable(GL_TEXTURE_2D);
    glEnable(GL_BLEND);

    GLuint texObject;
    glGenTextures(1, &texObject);
    glBindTexture(GL_TEXTURE_2D, texObject);

    // Texture wrapping and filtering
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);

    int width, height, nChannels;

    // Flip image vertically so it matches OpenGL UV origin
    stbi_set_flip_vertically_on_load(true);

    unsigned char* data = stbi_load(filename, &width, &height, &nChannels, 0);

    if (data)
    {
        GLenum format = (nChannels == 4) ? GL_RGBA : GL_RGB;

        glTexImage2D(GL_TEXTURE_2D, 0, format,width, height, 0, format, GL_UNSIGNED_BYTE, data);
        glGenerateMipmap(GL_TEXTURE_2D);
    }
    else
    {
        std::cout << "Failed to load texture: " << filename << std::endl;
    }

    stbi_image_free(data);

    return texObject;
}

GLuint setup_mipmaps(const char* filename[], int n)
{
    glEnable(GL_TEXTURE_2D);
    glEnable(GL_BLEND);

    GLuint texObject;
    glGenTextures(1, &texObject);
    glBindTexture(GL_TEXTURE_2D, texObject);

    // Texture wrapping and filtering - use trilinear filtering for mipmaps
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);

    // Arrays to hold data for each mipmap level
    int w[16], h[16], chan[16];
    unsigned char* pxls[16];

    // Flip image vertically so it matches OpenGL UV origin
    stbi_set_flip_vertically_on_load(true);

    // Loop through each mipmap level
    for (int c = 0; c < n; c++)
    {
        pxls[c] = stbi_load(filename[c], &w[c], &h[c], &chan[c], 0);

        if (pxls[c])
        {
            GLenum format = (chan[c] == 4) ? GL_RGBA : GL_RGB;

            glTexImage2D(GL_TEXTURE_2D,
                c,              // mipmap level
                format,
                w[c],
                h[c],
                0,
                format,
                GL_UNSIGNED_BYTE,
                pxls[c]);
        }
        else
        {
            std::cout << "Failed to load texture: " << filename[c] << std::endl;
        }

        stbi_image_free(pxls[c]);
    }

    return texObject;
}