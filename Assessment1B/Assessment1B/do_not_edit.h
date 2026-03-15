#pragma once

#include <iostream>
#include <map>
#include <vector>
#include <algorithm>
#include <stdio.h>
#include <windows.h>
#include <wingdi.h>


#define TINYOBJLOADER_IMPLEMENTATION
#include "tiny_obj_loader.h"

using namespace std;
using namespace glm;

class vertex
{
public:
	glm::vec3 pos;
	glm::vec3 col;
	glm::vec3 nor;
};

class triangle
{
public:
	vertex v1, v2, v3;
	bool reflect;
	int primID;
};

std::vector<triangle> tris;

#define PIXEL_W 1080
#define PIXEL_H 1080
float pixelBuffer[PIXEL_W * PIXEL_H * 3];


glm::vec3 bkgd = glm::vec3(.6f, .6f, .6f);
glm::vec3 light_pos(4.f, 6.f, 4.f);

int max_recursion_depth = 1;

typedef glm::vec3(*closest_hit)(triangle* tri, int depth, glm::vec3 p, glm::vec3 dir);
void trace(glm::vec3 o, glm::vec3 dir, float& t, glm::vec3& io_col, int depth, closest_hit p_hit);


int obj_parse(const char* filename, std::vector<triangle>* io_tris, float scale)
{
	tinyobj::attrib_t attrib;
	std::vector<tinyobj::shape_t> shapes;
	std::vector<tinyobj::material_t> materials;
	std::string warn, err;

	if (!tinyobj::LoadObj(&attrib, &shapes, &materials, &warn, &err, filename))
	{
		throw std::runtime_error(warn + err);
	}

	std::vector<vertex> vertices;
	std::vector<uint32_t> indices;

	float max_vert = 0.f;
	for (const auto& v : attrib.vertices)
	{
		if (v > max_vert)
			max_vert = 1;
	}

	for (const auto& shape : shapes)
	{
		for (const auto& index : shape.mesh.indices)
		{
			vertex vert{};

			vert.pos =
			{
				(attrib.vertices[3 * index.vertex_index + 0] / max_vert) * scale,
				(attrib.vertices[3 * index.vertex_index + 1] / max_vert) * scale,
				(attrib.vertices[3 * index.vertex_index + 2] / max_vert) * scale
			};

			vert.nor =
			{
				attrib.normals[3 * index.normal_index + 0],
				attrib.normals[3 * index.normal_index + 1],
				attrib.normals[3 * index.normal_index + 2]
			};


			vert.col = { 1, 1, 1 };

			vertices.push_back(vert);
			indices.push_back(indices.size());
		}
	}


	for (int i = 0; i < indices.size() / 3; i++)
	{
		triangle tri;
		tri.v1 = vertices[indices[(i * 3) + 0]];
		tri.v2 = vertices[indices[(i * 3) + 1]];
		tri.v3 = vertices[indices[(i * 3) + 2]];

		tri.reflect = false;
		tri.primID = i;


		if (strcmp(filename, "objs/cornell2/cornell-box.obj") == 0)
		{
			if (tri.primID == 6 || tri.primID == 7)
				tri.v1.col = vec3(1, 0, 0);
			if (tri.primID == 2 || tri.primID == 3)
				tri.v1.col = vec3(0, 1, 0);
			if (tri.primID == 10 || tri.primID == 11)
				tri.v1.col = vec3(0, 0, 1);

			if (tri.primID == 8 || tri.primID == 9)
				tri.reflect = true;
		}

		if (strcmp(filename, "objs/quad/quad.obj") == 0)
		{
			tri.v1.col = vec3(0, 0, 1);
		}

		bool randomise_colour = false;
		if (randomise_colour)
		{
			float r = 0, g = 0, b = 0;
			while (r <= 0.2)
				r = static_cast <float> (rand()) / static_cast <float> (RAND_MAX);
			while (g <= 0.2)
				g = static_cast <float> (rand()) / static_cast <float> (RAND_MAX);
			while (b <= 0.2)
				b = static_cast <float> (rand()) / static_cast <float> (RAND_MAX);
			tri.v1.col = { r, g, b };
			tri.v2.col = { r, g, b };
			tri.v3.col = { r, g, b };
		}

		io_tris->push_back(tri);
	}

	printf("successfully parsed %s and read %d triangles \n", filename, io_tris->size());

}

int savebitmap(const char* filename, float* pixelBuffer, int w, int h)
{
	char* charBuffer = (char*)malloc(sizeof(unsigned char) * w * h * 3);

	for (int y = 0; y < h; y++)
	{
		for (int x = 0; x < w; x++)
		{
			charBuffer[(y * w * 3) + (x * 3) + 0] = pixelBuffer[(((h - 1) - y) * w * 3) + (x * 3) + 2];
			charBuffer[(y * w * 3) + (x * 3) + 1] = pixelBuffer[(((h - 1) - y) * w * 3) + (x * 3) + 1];
			charBuffer[(y * w * 3) + (x * 3) + 2] = pixelBuffer[(((h - 1) - y) * w * 3) + (x * 3) + 0];
		}
	}

	BITMAPINFOHEADER infoHdr;
	infoHdr.biSize = 40;
	infoHdr.biWidth = w;
	infoHdr.biHeight = h;
	infoHdr.biPlanes = 1;
	infoHdr.biBitCount = 24;
	infoHdr.biCompression = 0;
	infoHdr.biSizeImage = sizeof(unsigned char) * w * h * 3;
	infoHdr.biXPelsPerMeter = 0;
	infoHdr.biYPelsPerMeter = 0;
	infoHdr.biClrUsed = 0;
	infoHdr.biClrImportant = 0;

	BITMAPFILEHEADER fileHdr;
	fileHdr.bfType = 19778;
	fileHdr.bfSize = sizeof(BITMAPFILEHEADER) + sizeof(BITMAPINFOHEADER) + (sizeof(unsigned char) * w * h * 3);
	fileHdr.bfReserved1 = 0;
	fileHdr.bfReserved2 = 0;
	fileHdr.bfOffBits = sizeof(BITMAPFILEHEADER) + sizeof(BITMAPINFOHEADER);

	FILE* bitmapFile;

	errno_t err = fopen_s(&bitmapFile, filename, "wb");
	if (err != 0 || bitmapFile == NULL)
	{
		printf("savebitmap - open failed for %s\n", filename);
		return NULL;
	}

	fwrite(&fileHdr, sizeof(BITMAPFILEHEADER), 1, bitmapFile);

	fwrite(&infoHdr, sizeof(BITMAPINFOHEADER), 1, bitmapFile);

	fseek(bitmapFile, fileHdr.bfOffBits, SEEK_SET);

	int nBytes = infoHdr.biWidth * infoHdr.biHeight * 3;
	fwrite(charBuffer, sizeof(unsigned char), nBytes, bitmapFile);

	fclose(bitmapFile);

	free(charBuffer);

	printf("savebitmap - saved %s w=%d h=%d bits=%d\n", filename, infoHdr.biWidth, infoHdr.biHeight, infoHdr.biBitCount);

	return 0;
}

void CounterEndAndPrint(LARGE_INTEGER StartingTime, LARGE_INTEGER* EndingTime, LARGE_INTEGER Frequency)
{
	QueryPerformanceCounter(EndingTime);

	LARGE_INTEGER ElapsedMicroseconds;
	ElapsedMicroseconds.QuadPart = (*EndingTime).QuadPart - StartingTime.QuadPart;
	ElapsedMicroseconds.QuadPart *= 1000000;
	ElapsedMicroseconds.QuadPart /= Frequency.QuadPart;
	std::cout << (float)ElapsedMicroseconds.QuadPart / (float)1000000 << std::endl;
}

float linear_to_gamma(float linear_component)
{
	if (linear_component > 0)
	{
		return std::sqrt(linear_component);
	}

	return 0;
}


void writeCol(vec3 col, int pixel_x, int pixel_y)
{
	auto r = linear_to_gamma(col.x);
	auto g = linear_to_gamma(col.y);
	auto b = linear_to_gamma(col.z);


	float rc = std::clamp(r, .0f, 1.f);
	float gc = std::clamp(g, .0f, 1.f);
	float bc = std::clamp(b, .0f, 1.f);

	float pixel_r = rc * 255.f;
	float pixel_g = gc * 255.f;
	float pixel_b = bc * 255.f;

	pixelBuffer[(pixel_y * PIXEL_W * 3) + (pixel_x * 3) + 0] = pixel_r;
	pixelBuffer[(pixel_y * PIXEL_W * 3) + (pixel_x * 3) + 1] = pixel_g;
	pixelBuffer[(pixel_y * PIXEL_W * 3) + (pixel_x * 3) + 2] = pixel_b;

}
