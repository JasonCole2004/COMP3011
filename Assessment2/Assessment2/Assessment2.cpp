#include <GL/gl3w.h>
#include <GLFW/glfw3.h>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include <iostream>
#include <vector>
#include <cmath>
#include <string>

#include "error.h"
#include "file.h"
#include "shader.h"
#include "camera.h"
#define STB_IMAGE_IMPLEMENTATION
#include "texture.h"

#define TINYOBJLOADER_IMPLEMENTATION
#include "tiny_obj_loader.h"

SCamera camera;
float timeScale  = 1.0f;
float camYaw     = -90.0f;
float camPitch   =  -9.0f;
float lastMouseX = 960.0f;
float lastMouseY = 540.0f;
bool  firstMouse = true;

// Camera speed 
float camSpeed     = 0.015f;
float camSpeedFast = 0.05f;

void mouse_callback(GLFWwindow* window, double xpos, double ypos)
{
	// Skip the first frame to avoid a jump when the cursor is first captured
	if (firstMouse)
	{
		lastMouseX = (float)xpos;
		lastMouseY = (float)ypos;
		firstMouse = false;
	}

	float dx = ((float)xpos - lastMouseX) * 0.1f;
	float dy = (lastMouseY - (float)ypos) * 0.1f;
	lastMouseX = (float)xpos;
	lastMouseY = (float)ypos;

	camYaw   += dx;
	camPitch += dy;
	camPitch  = glm::clamp(camPitch, -89.0f, 89.0f);

	glm::vec3 front;
	front.x = cosf(glm::radians(camYaw)) * cosf(glm::radians(camPitch));
	front.y = sinf(glm::radians(camPitch));
	front.z = sinf(glm::radians(camYaw)) * cosf(glm::radians(camPitch));
	camera.Front = glm::normalize(front);
}

void framebuffer_size_callback(GLFWwindow* window, int w, int h)
{
	glViewport(0, 0, w, h);
}

void processKeyboard(GLFWwindow* window)
{
	if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
		glfwSetWindowShouldClose(window, true);

	// WASD fly camera, Shift to boost speed
	float speed = (glfwGetKey(window, GLFW_KEY_LEFT_SHIFT) == GLFW_PRESS) ? camSpeedFast : camSpeed;
	glm::vec3 right = glm::normalize(glm::cross(camera.Front, camera.Up));
	if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS) camera.Position += speed * camera.Front;
	if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS) camera.Position -= speed * camera.Front;
	if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS) camera.Position -= speed * right;
	if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS) camera.Position += speed * right;

	// Z/X slow down or speed up the simulation
	if (glfwGetKey(window, GLFW_KEY_Z) == GLFW_PRESS) timeScale = glm::max(0.0f, timeScale - 0.005f);
	if (glfwGetKey(window, GLFW_KEY_X) == GLFW_PRESS) timeScale += 0.005f;
}

std::vector<float> generateSphere(float radius, int stacks, int slices, float r, float g, float b)
{
	std::vector<float> verts;

	auto push = [&](float x, float y, float z, float u, float v) {
		verts.push_back(x); verts.push_back(y); verts.push_back(z);
		verts.push_back(r); verts.push_back(g); verts.push_back(b);
		float len = sqrtf(x * x + y * y + z * z);
		verts.push_back(x / len); verts.push_back(y / len); verts.push_back(z / len);
		verts.push_back(u); verts.push_back(v);
	};

	for (int i = 0; i < stacks; i++)
	{
		float lat0 = glm::pi<float>() * (-0.5f + (float)i       / stacks);
		float lat1 = glm::pi<float>() * (-0.5f + (float)(i + 1) / stacks);
		float v0 = (float)i       / stacks;
		float v1 = (float)(i + 1) / stacks;

		for (int j = 0; j < slices; j++)
		{
			float lon0 = 2.0f * glm::pi<float>() * (float)j       / slices;
			float lon1 = 2.0f * glm::pi<float>() * (float)(j + 1) / slices;
			float u0 = (float)j       / slices;
			float u1 = (float)(j + 1) / slices;

			float x00 = radius * cosf(lat0) * cosf(lon0), y00 = radius * sinf(lat0), z00 = radius * cosf(lat0) * sinf(lon0);
			float x10 = radius * cosf(lat1) * cosf(lon0), y10 = radius * sinf(lat1), z10 = radius * cosf(lat1) * sinf(lon0);
			float x01 = radius * cosf(lat0) * cosf(lon1), y01 = radius * sinf(lat0), z01 = radius * cosf(lat0) * sinf(lon1);
			float x11 = radius * cosf(lat1) * cosf(lon1), y11 = radius * sinf(lat1), z11 = radius * cosf(lat1) * sinf(lon1);

			push(x00, y00, z00, u0, v0); push(x10, y10, z10, u0, v1); push(x11, y11, z11, u1, v1);
			push(x00, y00, z00, u0, v0); push(x11, y11, z11, u1, v1); push(x01, y01, z01, u1, v0);
		}
	}

	return verts;
}

std::vector<float> generateRing(float innerR, float outerR, int slices, float r, float g, float b)
{
	std::vector<float> verts;

	auto push = [&](float x, float z, float u, float v) {
		verts.push_back(x);    verts.push_back(0.0f); verts.push_back(z);
		verts.push_back(r);    verts.push_back(g);    verts.push_back(b);
		verts.push_back(0.0f); verts.push_back(1.0f); verts.push_back(0.0f);
		verts.push_back(u);    verts.push_back(v);
	};

	for (int i = 0; i < slices; i++)
	{
		float a0 = 2.0f * glm::pi<float>() * (float)i       / slices;
		float a1 = 2.0f * glm::pi<float>() * (float)(i + 1) / slices;
		float u0 = (float)i / slices, u1 = (float)(i + 1) / slices;

		float ix0 = innerR * cosf(a0), iz0 = innerR * sinf(a0);
		float ix1 = innerR * cosf(a1), iz1 = innerR * sinf(a1);
		float ox0 = outerR * cosf(a0), oz0 = outerR * sinf(a0);
		float ox1 = outerR * cosf(a1), oz1 = outerR * sinf(a1);

		push(ix0, iz0, 0.0f, u0); push(ox0, oz0, 1.0f, u0); push(ox1, oz1, 1.0f, u1);
		push(ix0, iz0, 0.0f, u0); push(ox1, oz1, 1.0f, u1); push(ix1, iz1, 0.0f, u1);
	}

	return verts;
}

// Cubic Bezier used for the asteroid flight path through the solar system
glm::vec3 bezier(glm::vec3 p0, glm::vec3 p1, glm::vec3 p2, glm::vec3 p3, float t)
{
	float u = 1.0f - t;
	return u*u*u*p0 + 3.0f*u*u*t*p1 + 3.0f*u*t*t*p2 + t*t*t*p3;
}

struct PlanetInfo
{
	float orbitRadius;
	float orbitSpeed;
	float spinSpeed;
	float size;
	int   vertCount;
};

int main()
{
	glfwInit();
	glfwWindowHint(GLFW_SAMPLES, 4); 
	GLFWwindow* window = glfwCreateWindow(1920, 1080, "Solar System", NULL, NULL);
	glfwMakeContextCurrent(window);
	glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);
	glfwSetCursorPosCallback(window, mouse_callback);
	glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);

	gl3wInit();

	glEnable(GL_DEBUG_OUTPUT);
	glDebugMessageCallback(DebugCallback, 0);

	glEnable(GL_DEPTH_TEST);
	glEnable(GL_MULTISAMPLE);
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	unsigned int shaderProgram     = CompileShader("triangle.vert",  "triangle.frag");
	unsigned int shadowShader      = CompileShader("shadow.vert",    "shadow.frag");
	unsigned int flareShader       = CompileShader("flare.vert",     "flare.frag");
	unsigned int bloomBrightShader = CompileShader("quad.vert",      "bloom_bright.frag");
	unsigned int bloomBlurShader   = CompileShader("quad.vert",      "bloom_blur.frag");
	unsigned int bloomCompShader   = CompileShader("quad.vert",      "bloom_composite.frag");

	// VAO layout: [0] = sun, [1-8] = planets, [9] = moon
	const int NUM_PLANETS = 8;
	unsigned int VAO[10], VBO[10];
	glGenVertexArrays(10, VAO);
	glGenBuffers(10, VBO);

	// Vertex format: pos(3) + col(3) + normal(3) + UV(2) = 11 floats
	const GLsizei stride = 11 * sizeof(float);

	// Sun
	std::vector<float> sunVerts = generateSphere(1.0f, 24, 24, 1.0f, 1.0f, 1.0f);
	int sunVertCount = (int)sunVerts.size() / 11;
	glBindVertexArray(VAO[0]);
	glBindBuffer(GL_ARRAY_BUFFER, VBO[0]);
	glBufferData(GL_ARRAY_BUFFER, sunVerts.size() * sizeof(float), sunVerts.data(), GL_STATIC_DRAW);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, stride, (void*)0);
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, stride, (void*)(3 * sizeof(float)));
	glEnableVertexAttribArray(1);
	glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, stride, (void*)(6 * sizeof(float)));
	glEnableVertexAttribArray(2);
	glVertexAttribPointer(3, 2, GL_FLOAT, GL_FALSE, stride, (void*)(9 * sizeof(float)));
	glEnableVertexAttribArray(3);

	// Planet data: orbit radii and speeds are loosely based on real relative periods
	PlanetInfo planets[NUM_PLANETS] = {
		{ 2.5f,  0.60f,  0.008f, 0.08f, 0 }, // Mercury
		{ 3.8f,  0.234f, 0.004f, 0.18f, 0 }, // Venus
		{ 5.2f,  0.144f, 0.10f,  0.20f, 0 }, // Earth
		{ 7.0f,  0.076f, 0.097f, 0.12f, 0 }, // Mars
		{ 10.5f, 0.012f, 0.25f,  0.65f, 0 }, // Jupiter
		{ 14.0f, 0.005f, 0.22f,  0.55f, 0 }, // Saturn
		{ 17.5f, 0.0018f,0.14f,  0.30f, 0 }, // Uranus
		{ 21.0f, 0.0008f,0.15f,  0.28f, 0 }, // Neptune
	};
	for (int p = 0; p < NUM_PLANETS; p++)
	{
		// Vertex colour is unused: planets always render with a texture
		std::vector<float> verts = generateSphere(1.0f, 24, 24, 1.0f, 1.0f, 1.0f);
		planets[p].vertCount = (int)verts.size() / 11;

		glBindVertexArray(VAO[p + 1]);
		glBindBuffer(GL_ARRAY_BUFFER, VBO[p + 1]);
		glBufferData(GL_ARRAY_BUFFER, verts.size() * sizeof(float), verts.data(), GL_STATIC_DRAW);
		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, stride, (void*)0);
		glEnableVertexAttribArray(0);
		glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, stride, (void*)(3 * sizeof(float)));
		glEnableVertexAttribArray(1);
		glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, stride, (void*)(6 * sizeof(float)));
		glEnableVertexAttribArray(2);
		glVertexAttribPointer(3, 2, GL_FLOAT, GL_FALSE, stride, (void*)(9 * sizeof(float)));
		glEnableVertexAttribArray(3);
	}

	// Moon (orbits Earth = planet 2)
	std::vector<float> moonVerts = generateSphere(1.0f, 64, 64, 1.0f, 1.0f, 1.0f);
	int moonVertCount = (int)moonVerts.size() / 11;
	glBindVertexArray(VAO[9]);
	glBindBuffer(GL_ARRAY_BUFFER, VBO[9]);
	glBufferData(GL_ARRAY_BUFFER, moonVerts.size() * sizeof(float), moonVerts.data(), GL_STATIC_DRAW);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, stride, (void*)0);
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, stride, (void*)(3 * sizeof(float)));
	glEnableVertexAttribArray(1);
	glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, stride, (void*)(6 * sizeof(float)));
	glEnableVertexAttribArray(2);
	glVertexAttribPointer(3, 2, GL_FLOAT, GL_FALSE, stride, (void*)(9 * sizeof(float)));
	glEnableVertexAttribArray(3);

	// Saturn's rings
	std::vector<float> ringVerts = generateRing(1.3f, 2.4f, 128, 1.0f, 1.0f, 1.0f);
	int ringVertCount = (int)ringVerts.size() / 11;
	unsigned int ringVAO, ringVBO;
	glGenVertexArrays(1, &ringVAO);
	glGenBuffers(1, &ringVBO);
	glBindVertexArray(ringVAO);
	glBindBuffer(GL_ARRAY_BUFFER, ringVBO);
	glBufferData(GL_ARRAY_BUFFER, ringVerts.size() * sizeof(float), ringVerts.data(), GL_STATIC_DRAW);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, stride, (void*)0);
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, stride, (void*)(3 * sizeof(float)));
	glEnableVertexAttribArray(1);
	glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, stride, (void*)(6 * sizeof(float)));
	glEnableVertexAttribArray(2);
	glVertexAttribPointer(3, 2, GL_FLOAT, GL_FALSE, stride, (void*)(9 * sizeof(float)));
	glEnableVertexAttribArray(3);

	// Asteroid path: cubic Bezier that sligshots around the sun
	glm::vec3 bezP0 = glm::vec3( 25.0f,  3.0f,  0.0f);
	glm::vec3 bezP1 = glm::vec3(  4.0f,  1.0f, -10.0f);
	glm::vec3 bezP2 = glm::vec3(-10.0f, -1.0f,  -4.0f);
	glm::vec3 bezP3 = glm::vec3( -5.0f, -3.0f,  25.0f);

	// Pre-bake the curve as a line strip so it can optionally be drawn
	const int CURVE_SEGS = 64;
	std::vector<float> curveVerts;
	for (int i = 0; i <= CURVE_SEGS; i++)
	{
		glm::vec3 p = bezier(bezP0, bezP1, bezP2, bezP3, (float)i / CURVE_SEGS);
		curveVerts.push_back(p.x);  curveVerts.push_back(p.y);  curveVerts.push_back(p.z);
		curveVerts.push_back(0.8f); curveVerts.push_back(0.8f); curveVerts.push_back(0.4f); 
		curveVerts.push_back(0.0f); curveVerts.push_back(1.0f); curveVerts.push_back(0.0f); 
		curveVerts.push_back(0.0f); curveVerts.push_back(0.0f);                              
	}
	unsigned int curveVAO, curveVBO;
	glGenVertexArrays(1, &curveVAO);
	glGenBuffers(1, &curveVBO);
	glBindVertexArray(curveVAO);
	glBindBuffer(GL_ARRAY_BUFFER, curveVBO);
	glBufferData(GL_ARRAY_BUFFER, curveVerts.size() * sizeof(float), curveVerts.data(), GL_STATIC_DRAW);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, stride, (void*)0);
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, stride, (void*)(3 * sizeof(float)));
	glEnableVertexAttribArray(1);
	glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, stride, (void*)(6 * sizeof(float)));
	glEnableVertexAttribArray(2);
	glVertexAttribPointer(3, 2, GL_FLOAT, GL_FALSE, stride, (void*)(9 * sizeof(float)));
	glEnableVertexAttribArray(3);

	// Asteroid OBJ
	std::vector<float> asteroidVerts;
	int asteroidVertCount = 0;
	{
		tinyobj::ObjReaderConfig cfg;
		tinyobj::ObjReader reader;
		if (reader.ParseFromFile("asteroid.obj", cfg))
		{
			auto& attrib    = reader.GetAttrib();
			auto& shapes    = reader.GetShapes();
			auto& materials = reader.GetMaterials();

			for (size_t s = 0; s < shapes.size(); s++)
			{
				size_t index_offset = 0;
				for (size_t f = 0; f < shapes[s].mesh.num_face_vertices.size(); f++)
				{
					int fv     = shapes[s].mesh.num_face_vertices[f];
					int mat_id = shapes[s].mesh.material_ids[f];

					float r = 0.6f, g = 0.55f, b = 0.5f;
					if (mat_id >= 0 && mat_id < (int)materials.size())
					{
						r = materials[mat_id].diffuse[0];
						g = materials[mat_id].diffuse[1];
						b = materials[mat_id].diffuse[2];
					}

					for (int v = 0; v < fv; v++)
					{
						tinyobj::index_t idx = shapes[s].mesh.indices[index_offset + v];

						float vx = attrib.vertices[3 * idx.vertex_index + 0];
						float vy = attrib.vertices[3 * idx.vertex_index + 1];
						float vz = attrib.vertices[3 * idx.vertex_index + 2];

						float nx = 0, ny = 1, nz = 0;
						if (idx.normal_index >= 0) {
							nx = attrib.normals[3 * idx.normal_index + 0];
							ny = attrib.normals[3 * idx.normal_index + 1];
							nz = attrib.normals[3 * idx.normal_index + 2];
						}

						float tu = 0, tv = 0;
						if (idx.texcoord_index >= 0) {
							tu = attrib.texcoords[2 * idx.texcoord_index + 0];
							tv = attrib.texcoords[2 * idx.texcoord_index + 1];
						}

						asteroidVerts.push_back(vx); asteroidVerts.push_back(vy); asteroidVerts.push_back(vz);
						asteroidVerts.push_back(r);  asteroidVerts.push_back(g);  asteroidVerts.push_back(b);
						asteroidVerts.push_back(nx); asteroidVerts.push_back(ny); asteroidVerts.push_back(nz);
						asteroidVerts.push_back(tu); asteroidVerts.push_back(tv);
					}
					index_offset += fv;
				}
			}
			asteroidVertCount = (int)asteroidVerts.size() / 11;
		}
		else
		{
			std::cout << "Failed to load asteroid.obj: " << reader.Error() << std::endl;
		}
	}
	unsigned int asteroidVAO, asteroidVBO;
	glGenVertexArrays(1, &asteroidVAO);
	glGenBuffers(1, &asteroidVBO);
	glBindVertexArray(asteroidVAO);
	glBindBuffer(GL_ARRAY_BUFFER, asteroidVBO);
	glBufferData(GL_ARRAY_BUFFER, asteroidVerts.size() * sizeof(float), asteroidVerts.data(), GL_STATIC_DRAW);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, stride, (void*)0);
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, stride, (void*)(3 * sizeof(float)));
	glEnableVertexAttribArray(1);
	glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, stride, (void*)(6 * sizeof(float)));
	glEnableVertexAttribArray(2);
	glVertexAttribPointer(3, 2, GL_FLOAT, GL_FALSE, stride, (void*)(9 * sizeof(float)));
	glEnableVertexAttribArray(3);

	// Spaceship OBJ
	std::vector<float> shipVerts;
	int shipVertCount = 0;
	{
		tinyobj::ObjReaderConfig cfg;
		tinyobj::ObjReader reader;
		if (reader.ParseFromFile("spaceship.obj", cfg))
		{
			auto& attrib    = reader.GetAttrib();
			auto& shapes    = reader.GetShapes();
			auto& materials = reader.GetMaterials();

			for (size_t s = 0; s < shapes.size(); s++)
			{
				size_t index_offset = 0;
				for (size_t f = 0; f < shapes[s].mesh.num_face_vertices.size(); f++)
				{
					int fv     = shapes[s].mesh.num_face_vertices[f];
					int mat_id = shapes[s].mesh.material_ids[f];

					float r = 0.7f, g = 0.7f, b = 0.7f;
					if (mat_id >= 0 && mat_id < (int)materials.size())
					{
						r = materials[mat_id].diffuse[0];
						g = materials[mat_id].diffuse[1];
						b = materials[mat_id].diffuse[2];
					}

					for (int v = 0; v < fv; v++)
					{
						tinyobj::index_t idx = shapes[s].mesh.indices[index_offset + v];

						float vx = attrib.vertices[3 * idx.vertex_index + 0];
						float vy = attrib.vertices[3 * idx.vertex_index + 1];
						float vz = attrib.vertices[3 * idx.vertex_index + 2];

						float nx = 0, ny = 1, nz = 0;
						if (idx.normal_index >= 0) {
							nx = attrib.normals[3 * idx.normal_index + 0];
							ny = attrib.normals[3 * idx.normal_index + 1];
							nz = attrib.normals[3 * idx.normal_index + 2];
						}

						float tu = 0, tv = 0;
						if (idx.texcoord_index >= 0) {
							tu = attrib.texcoords[2 * idx.texcoord_index + 0];
							tv = attrib.texcoords[2 * idx.texcoord_index + 1];
						}

						shipVerts.push_back(vx); shipVerts.push_back(vy); shipVerts.push_back(vz);
						shipVerts.push_back(r);  shipVerts.push_back(g);  shipVerts.push_back(b);
						shipVerts.push_back(nx); shipVerts.push_back(ny); shipVerts.push_back(nz);
						shipVerts.push_back(tu); shipVerts.push_back(tv);
					}
					index_offset += fv;
				}
			}
			shipVertCount = (int)shipVerts.size() / 11;
		}
		else
		{
			std::cout << "Failed to load spaceship.obj: " << reader.Error() << std::endl;
		}
	}

	unsigned int shipVAO, shipVBO;
	glGenVertexArrays(1, &shipVAO);
	glGenBuffers(1, &shipVBO);
	glBindVertexArray(shipVAO);
	glBindBuffer(GL_ARRAY_BUFFER, shipVBO);
	glBufferData(GL_ARRAY_BUFFER, shipVerts.size() * sizeof(float), shipVerts.data(), GL_STATIC_DRAW);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, stride, (void*)0);
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, stride, (void*)(3 * sizeof(float)));
	glEnableVertexAttribArray(1);
	glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, stride, (void*)(6 * sizeof(float)));
	glEnableVertexAttribArray(2);
	glVertexAttribPointer(3, 2, GL_FLOAT, GL_FALSE, stride, (void*)(9 * sizeof(float)));
	glEnableVertexAttribArray(3);

	// Skysphere (stars background): rendered from inside, so scale X negative to un-mirror the texture
	std::vector<float> skyVerts = generateSphere(1.0f, 32, 32, 1.0f, 1.0f, 1.0f);
	int skyVertCount = (int)skyVerts.size() / 11;
	unsigned int skyVAO, skyVBO;
	glGenVertexArrays(1, &skyVAO);
	glGenBuffers(1, &skyVBO);
	glBindVertexArray(skyVAO);
	glBindBuffer(GL_ARRAY_BUFFER, skyVBO);
	glBufferData(GL_ARRAY_BUFFER, skyVerts.size() * sizeof(float), skyVerts.data(), GL_STATIC_DRAW);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, stride, (void*)0);
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, stride, (void*)(3 * sizeof(float)));
	glEnableVertexAttribArray(1);
	glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, stride, (void*)(6 * sizeof(float)));
	glEnableVertexAttribArray(2);
	glVertexAttribPointer(3, 2, GL_FLOAT, GL_FALSE, stride, (void*)(9 * sizeof(float)));
	glEnableVertexAttribArray(3);

	// Probe: satellite OBJ, controlled independently with arrow keys + Q/E
	std::vector<float> probeVerts;
	int probeVertCount = 0;
	{
		tinyobj::ObjReaderConfig cfg;
		tinyobj::ObjReader reader;
		if (reader.ParseFromFile("satellite.obj", cfg))
		{
			auto& attrib    = reader.GetAttrib();
			auto& shapes    = reader.GetShapes();
			auto& materials = reader.GetMaterials();

			for (size_t s = 0; s < shapes.size(); s++)
			{
				size_t index_offset = 0;
				for (size_t f = 0; f < shapes[s].mesh.num_face_vertices.size(); f++)
				{
					int fv     = shapes[s].mesh.num_face_vertices[f];
					int mat_id = shapes[s].mesh.material_ids[f];

					float r = 0.7f, g = 0.7f, b = 0.75f;
					if (mat_id >= 0 && mat_id < (int)materials.size())
					{
						r = materials[mat_id].diffuse[0];
						g = materials[mat_id].diffuse[1];
						b = materials[mat_id].diffuse[2];
					}

					// Collect positions first to compute a face normal if the OBJ has none
					glm::vec3 faceNormal(0.0f, 1.0f, 0.0f);
					if (fv >= 3)
					{
						tinyobj::index_t i0 = shapes[s].mesh.indices[index_offset + 0];
						tinyobj::index_t i1 = shapes[s].mesh.indices[index_offset + 1];
						tinyobj::index_t i2 = shapes[s].mesh.indices[index_offset + 2];
						glm::vec3 p0(attrib.vertices[3*i0.vertex_index], attrib.vertices[3*i0.vertex_index+1], attrib.vertices[3*i0.vertex_index+2]);
						glm::vec3 p1(attrib.vertices[3*i1.vertex_index], attrib.vertices[3*i1.vertex_index+1], attrib.vertices[3*i1.vertex_index+2]);
						glm::vec3 p2(attrib.vertices[3*i2.vertex_index], attrib.vertices[3*i2.vertex_index+1], attrib.vertices[3*i2.vertex_index+2]);
						glm::vec3 fn = glm::cross(p1 - p0, p2 - p0);
						float len = glm::length(fn);
						if (len > 0.0f) faceNormal = fn / len;
					}

					for (int v = 0; v < fv; v++)
					{
						tinyobj::index_t idx = shapes[s].mesh.indices[index_offset + v];

						float vx = attrib.vertices[3 * idx.vertex_index + 0];
						float vy = attrib.vertices[3 * idx.vertex_index + 1];
						float vz = attrib.vertices[3 * idx.vertex_index + 2];

						float nx = faceNormal.x, ny = faceNormal.y, nz = faceNormal.z;
						if (idx.normal_index >= 0) {
							nx = attrib.normals[3 * idx.normal_index + 0];
							ny = attrib.normals[3 * idx.normal_index + 1];
							nz = attrib.normals[3 * idx.normal_index + 2];
						}

						float tu = 0, tv = 0;
						if (idx.texcoord_index >= 0) {
							tu = attrib.texcoords[2 * idx.texcoord_index + 0];
							tv = attrib.texcoords[2 * idx.texcoord_index + 1];
						}

						probeVerts.push_back(vx); probeVerts.push_back(vy); probeVerts.push_back(vz);
						probeVerts.push_back(r);  probeVerts.push_back(g);  probeVerts.push_back(b);
						probeVerts.push_back(nx); probeVerts.push_back(ny); probeVerts.push_back(nz);
						probeVerts.push_back(tu); probeVerts.push_back(tv);
					}
					index_offset += fv;
				}
			}
			probeVertCount = (int)probeVerts.size() / 11;
		}
		else
		{
			std::cout << "Failed to load satellite.obj: " << reader.Error() << std::endl;
		}
	}
	unsigned int probeVAO, probeVBO;
	glGenVertexArrays(1, &probeVAO);
	glGenBuffers(1, &probeVBO);
	glBindVertexArray(probeVAO);
	glBindBuffer(GL_ARRAY_BUFFER, probeVBO);
	glBufferData(GL_ARRAY_BUFFER, probeVerts.size() * sizeof(float), probeVerts.data(), GL_STATIC_DRAW);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, stride, (void*)0);
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, stride, (void*)(3 * sizeof(float)));
	glEnableVertexAttribArray(1);
	glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, stride, (void*)(6 * sizeof(float)));
	glEnableVertexAttribArray(2);
	glVertexAttribPointer(3, 2, GL_FLOAT, GL_FALSE, stride, (void*)(9 * sizeof(float)));
	glEnableVertexAttribArray(3);

	// Full-screen quad used for lens flare sprites and all bloom post-process passes
	float quadVerts[] = {
		-1.0f,-1.0f,0.0f, 1,1,1, 0,0,1, 0.0f,0.0f,
		 1.0f,-1.0f,0.0f, 1,1,1, 0,0,1, 1.0f,0.0f,
		 1.0f, 1.0f,0.0f, 1,1,1, 0,0,1, 1.0f,1.0f,
		-1.0f,-1.0f,0.0f, 1,1,1, 0,0,1, 0.0f,0.0f,
		 1.0f, 1.0f,0.0f, 1,1,1, 0,0,1, 1.0f,1.0f,
		-1.0f, 1.0f,0.0f, 1,1,1, 0,0,1, 0.0f,1.0f,
	};
	unsigned int flareVAO, flareVBO;
	glGenVertexArrays(1, &flareVAO);
	glGenBuffers(1, &flareVBO);
	glBindVertexArray(flareVAO);
	glBindBuffer(GL_ARRAY_BUFFER, flareVBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(quadVerts), quadVerts, GL_STATIC_DRAW);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, stride, (void*)0);
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, stride, (void*)(3 * sizeof(float)));
	glEnableVertexAttribArray(1);
	glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, stride, (void*)(6 * sizeof(float)));
	glEnableVertexAttribArray(2);
	glVertexAttribPointer(3, 2, GL_FLOAT, GL_FALSE, stride, (void*)(9 * sizeof(float)));
	glEnableVertexAttribArray(3);

	glBindVertexArray(0);
	glBindBuffer(GL_ARRAY_BUFFER, 0);

	// Cube shadow map: one depth face per direction from the sun, covering the full solar system.
	// 1024 per face (6 faces) gives similar memory to a single 2048 map while covering all directions.
	const int SHADOW_SIZE = 4096;
	unsigned int shadowFBO, shadowCubeMap;
	glGenTextures(1, &shadowCubeMap);
	glBindTexture(GL_TEXTURE_CUBE_MAP, shadowCubeMap);
	for (int i = 0; i < 6; i++)
		glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0, GL_DEPTH_COMPONENT,
		             SHADOW_SIZE, SHADOW_SIZE, 0, GL_DEPTH_COMPONENT, GL_FLOAT, NULL);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
	glGenFramebuffers(1, &shadowFBO);
	glBindFramebuffer(GL_FRAMEBUFFER, shadowFBO);
	glDrawBuffer(GL_NONE);
	glReadBuffer(GL_NONE);
	glBindFramebuffer(GL_FRAMEBUFFER, 0);

	// Scene framebuffer: renders the full scene to a floating point texture so bloom can read it.
	// A second colour texture is attached alongside it; the sun writes its colour there
	// and everything else writes black, so bloom only ever glows around the sun
	unsigned int sceneFBO, sceneTex, emissiveTex, sceneDepthRBO;
	glGenFramebuffers(1, &sceneFBO);
	glGenTextures(1, &sceneTex);
	glBindTexture(GL_TEXTURE_2D, sceneTex);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB16F, 1920, 1080, 0, GL_RGB, GL_FLOAT, NULL);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glGenTextures(1, &emissiveTex);
	glBindTexture(GL_TEXTURE_2D, emissiveTex);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB16F, 1920, 1080, 0, GL_RGB, GL_FLOAT, NULL);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glGenRenderbuffers(1, &sceneDepthRBO);
	glBindRenderbuffer(GL_RENDERBUFFER, sceneDepthRBO);
	glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT24, 1920, 1080);
	glBindFramebuffer(GL_FRAMEBUFFER, sceneFBO);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, sceneTex,    0);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT1, GL_TEXTURE_2D, emissiveTex, 0);
	glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, sceneDepthRBO);
	unsigned int sceneDrawBufs[] = { GL_COLOR_ATTACHMENT0, GL_COLOR_ATTACHMENT1 };
	glDrawBuffers(2, sceneDrawBufs);
	glBindFramebuffer(GL_FRAMEBUFFER, 0);

	// Bloom ping-pong FBOs (quarter resolution for performance on lab computers)
	const int BW = 480, BH = 270;
	unsigned int bloomFBO[2], bloomTex[2];
	glGenFramebuffers(2, bloomFBO);
	glGenTextures(2, bloomTex);
	for (int i = 0; i < 2; i++)
	{
		glBindTexture(GL_TEXTURE_2D, bloomTex[i]);
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB16F, BW, BH, 0, GL_RGB, GL_FLOAT, NULL);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
		glBindFramebuffer(GL_FRAMEBUFFER, bloomFBO[i]);
		glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, bloomTex[i], 0);
	}
	glBindFramebuffer(GL_FRAMEBUFFER, 0);

	glUseProgram(shaderProgram);

	// Textures: [0]=sun, [1-8]=planets, [9]=moon
	GLuint textures[10];
	textures[0] = setup_texture("sun.jpg");
	const char* planetTexFiles[NUM_PLANETS] = {
		"mercury.jpg", "venus.jpg", "earth.jpg", "mars.jpg",
		"jupiter.jpg", "saturn.jpg", "uranus.jpg", "neptune.jpg"
	};
	for (int p = 0; p < NUM_PLANETS; p++)
		textures[p + 1] = setup_texture(planetTexFiles[p]);
	textures[9] = setup_texture("moon.jpg");
	GLuint ringTex       = setup_texture("saturn_rings.png");
	GLuint starTex       = setup_texture("stars.jpg");
	GLuint earthNormalTex = setup_texture("earth_normal_map.png");

	glActiveTexture(GL_TEXTURE0);
	glUniform1i(glGetUniformLocation(shaderProgram, "tex"),           0);
	glUniform1i(glGetUniformLocation(shaderProgram, "shadowCubeMap"), 1);
	glUniform1i(glGetUniformLocation(shaderProgram, "normalMap"),     2);

	InitCamera(camera);
	camera.Position = glm::vec3(0.0f, 5.0f, 30.0f);
	camera.Up       = glm::vec3(0.0f, 1.0f,  0.0f);
	{
		glm::vec3 f;
		f.x = cosf(glm::radians(camYaw)) * cosf(glm::radians(camPitch));
		f.y = sinf(glm::radians(camPitch));
		f.z = sinf(glm::radians(camYaw)) * cosf(glm::radians(camPitch));
		camera.Front = glm::normalize(f);
	}

	// Satellite probe state: controlled with arrow keys and Q/E
	float satAngle  = 0.0f;   
	float satRadius = 0.27f;  
	float satHeight = 0.0f;   

	// Ship HUD position: tuned visually, shouldn't need changing
	float shipPosX  =  0.38f;
	float shipPosY  = -0.25f;
	float shipPosZ  = -4.0f;
	float shipRotX  =   0.0f;
	float shipRotY  = 270.0f;
	float shipRotZ  =   0.0f;
	float shipScale =  0.06f;

	while (!glfwWindowShouldClose(window))
	{
		// t is scaled simulation time (affected by Z/X), realT is wall-clock time.
		// Spin speed uses realT so that planets spin at a consistent rate
		// regardless of the simulation speed slider.
		static float t        = 0.0f;
		static float lastTime = (float)glfwGetTime();
		float now   = (float)glfwGetTime();
		float realT = now;
		t += (now - lastTime) * timeScale;
		lastTime = now;

		// Earth center (orbit only, spin doesn't move the center)
		glm::vec3 earthCenter;
		{
			glm::mat4 m = glm::mat4(1.0f);
			m = glm::rotate(m, t * planets[2].orbitSpeed, glm::vec3(0.0f, 1.0f, 0.0f));
			m = glm::translate(m, glm::vec3(planets[2].orbitRadius, 0.0f, 0.0f));
			earthCenter = glm::vec3(m * glm::vec4(0.0f, 0.0f, 0.0f, 1.0f));
		}

		// Moon model: computed once, used in both passes
		glm::mat4 moonModel = glm::mat4(1.0f);
		moonModel = glm::translate(moonModel, earthCenter);
		moonModel = glm::rotate(moonModel, t * 1.4f, glm::vec3(0.0f, 1.0f, 0.0f));  // orbit
		moonModel = glm::translate(moonModel, glm::vec3(0.65f, 0.0f, 0.0f));
		moonModel = glm::rotate(moonModel, realT * 2.5f, glm::vec3(0.0f, 1.0f, 0.0f)); // spin
		moonModel = glm::scale(moonModel, glm::vec3(0.06f));

		// Asteroid position: ping-pongs along the Bezier curve
		float tRaw  = fmod(t * 0.05f, 2.0f);
		float asteroidT = (tRaw <= 1.0f) ? tRaw : 2.0f - tRaw;
		glm::vec3 asteroidPos = bezier(bezP0, bezP1, bezP2, bezP3, asteroidT);
		glm::mat4 asteroidModel = glm::mat4(1.0f);
		asteroidModel = glm::translate(asteroidModel, asteroidPos);
		asteroidModel = glm::scale(asteroidModel, glm::vec3(0.03f));

		// Satellite probe model: computed here so it is available for the shadow pass
		glm::vec3 probePos = earthCenter + glm::vec3(
			cosf(satAngle) * satRadius,
			satHeight,
			sinf(satAngle) * satRadius);
		glm::mat4 probeModel = glm::translate(glm::mat4(1.0f), probePos);
		probeModel = glm::scale(probeModel, glm::vec3(0.02f));

		// Shadow pass: render 6 depth faces for the cube shadow map.
		// Each face covers 90 degrees so together they cover all directions from the sun
		const float shadowFar = 30.0f; 
		glm::vec3   sunPos     = glm::vec3(0.0f);
		glm::mat4   shadowProj = glm::perspective(glm::radians(90.0f), 1.0f, 0.1f, shadowFar);
		glm::mat4 shadowViews[6] = {
			glm::lookAt(sunPos, sunPos + glm::vec3( 1, 0, 0), glm::vec3(0,-1, 0)),
			glm::lookAt(sunPos, sunPos + glm::vec3(-1, 0, 0), glm::vec3(0,-1, 0)),
			glm::lookAt(sunPos, sunPos + glm::vec3( 0, 1, 0), glm::vec3(0, 0, 1)),
			glm::lookAt(sunPos, sunPos + glm::vec3( 0,-1, 0), glm::vec3(0, 0,-1)),
			glm::lookAt(sunPos, sunPos + glm::vec3( 0, 0, 1), glm::vec3(0,-1, 0)),
			glm::lookAt(sunPos, sunPos + glm::vec3( 0, 0,-1), glm::vec3(0,-1, 0)),
		};

		glViewport(0, 0, SHADOW_SIZE, SHADOW_SIZE);
		glBindFramebuffer(GL_FRAMEBUFFER, shadowFBO);
		glUseProgram(shadowShader);
		glUniform3fv(glGetUniformLocation(shadowShader, "lightPos"),  1, glm::value_ptr(sunPos));
		glUniform1f (glGetUniformLocation(shadowShader, "farPlane"),  shadowFar);

		for (int face = 0; face < 6; face++)
		{
			glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT,
			                       GL_TEXTURE_CUBE_MAP_POSITIVE_X + face, shadowCubeMap, 0);
			glClear(GL_DEPTH_BUFFER_BIT);

			glm::mat4 shadowMat = shadowProj * shadowViews[face];
			glUniformMatrix4fv(glGetUniformLocation(shadowShader, "shadowMatrix"), 1, GL_FALSE, glm::value_ptr(shadowMat));

			for (int p = 0; p < NUM_PLANETS; p++)
			{
				glm::mat4 pm = glm::mat4(1.0f);
				pm = glm::rotate(pm, t     * planets[p].orbitSpeed, glm::vec3(0.0f, 1.0f, 0.0f));
				pm = glm::translate(pm, glm::vec3(planets[p].orbitRadius, 0.0f, 0.0f));
				pm = glm::rotate(pm, realT * planets[p].spinSpeed,  glm::vec3(0.0f, 1.0f, 0.0f));
				pm = glm::scale(pm, glm::vec3(planets[p].size));
				glUniformMatrix4fv(glGetUniformLocation(shadowShader, "model"), 1, GL_FALSE, glm::value_ptr(pm));
				glBindVertexArray(VAO[p + 1]);
				glDrawArrays(GL_TRIANGLES, 0, planets[p].vertCount);
			}
			glUniformMatrix4fv(glGetUniformLocation(shadowShader, "model"), 1, GL_FALSE, glm::value_ptr(moonModel));
			glBindVertexArray(VAO[9]);
			glDrawArrays(GL_TRIANGLES, 0, moonVertCount);

			glUniformMatrix4fv(glGetUniformLocation(shadowShader, "model"), 1, GL_FALSE, glm::value_ptr(asteroidModel));
			glBindVertexArray(asteroidVAO);
			glDrawArrays(GL_TRIANGLES, 0, asteroidVertCount);

			glUniformMatrix4fv(glGetUniformLocation(shadowShader, "model"), 1, GL_FALSE, glm::value_ptr(probeModel));
			glBindVertexArray(probeVAO);
			glDrawArrays(GL_TRIANGLES, 0, probeVertCount);
		}

		glBindFramebuffer(GL_FRAMEBUFFER, sceneFBO);
		glViewport(0, 0, 1920, 1080);

		// Main pass
		static const GLfloat bgd[] = { 0.0f, 0.0f, 0.05f, 1.0f };
		glClearBufferfv(GL_COLOR, 0, bgd);
		glClear(GL_DEPTH_BUFFER_BIT);

		glm::mat4 view       = glm::lookAt(camera.Position, camera.Position + camera.Front, camera.Up);
		glm::mat4 projection = glm::perspective(glm::radians(45.f), 1920.f / 1080.f, 0.1f, 500.f);

		glUseProgram(shaderProgram);

		glActiveTexture(GL_TEXTURE1);
		glBindTexture(GL_TEXTURE_CUBE_MAP, shadowCubeMap);
		glActiveTexture(GL_TEXTURE0);

		glUniformMatrix4fv(glGetUniformLocation(shaderProgram, "view"),       1, GL_FALSE, glm::value_ptr(view));
		glUniformMatrix4fv(glGetUniformLocation(shaderProgram, "projection"), 1, GL_FALSE, glm::value_ptr(projection));
		glUniform1f(glGetUniformLocation(shaderProgram, "shadowFarPlane"), shadowFar);
		glUniform3f(glGetUniformLocation(shaderProgram, "lightPos"),    0.0f, 0.0f, 0.0f);
		glUniform3f(glGetUniformLocation(shaderProgram, "lightColour"), 1.1f, 1.1f, 1.0f);
		glUniform3fv(glGetUniformLocation(shaderProgram, "camPos"), 1, glm::value_ptr(camera.Position));
		glUniform1f(glGetUniformLocation(shaderProgram, "hasTexture"), 1.0f);
		glUniform1f(glGetUniformLocation(shaderProgram, "useShadow"),  1.0f);
		glUniform1f(glGetUniformLocation(shaderProgram, "alpha"),      1.0f);

		// Directional light: faint cool starlight from a fixed angle
		glm::vec3 dirDir = glm::normalize(glm::vec3(1.0f, -0.3f, 0.5f));
		glUniform3fv(glGetUniformLocation(shaderProgram, "dirLightDir"),   1, glm::value_ptr(dirDir));
		glUniform3f(glGetUniformLocation(shaderProgram,  "dirLightColour"), 0.04f, 0.05f, 0.10f);

		// Spotlight: camera headlight, illuminates whatever you point at
		glUniform3fv(glGetUniformLocation(shaderProgram, "spotPos"),    1, glm::value_ptr(camera.Position));
		glUniform3fv(glGetUniformLocation(shaderProgram, "spotDir"),    1, glm::value_ptr(camera.Front));
		glUniform3f(glGetUniformLocation(shaderProgram,  "spotColour"), 0.65f, 0.65f, 0.75f);
		glUniform1f(glGetUniformLocation(shaderProgram,  "spotCutoff"), cosf(glm::radians(6.0f)));
		glUniform1f(glGetUniformLocation(shaderProgram,  "spotOuter"),  cosf(glm::radians(9.0f)));

		// Skysphere: drawn first, depth writes off so it never occludes anything
		glDepthMask(GL_FALSE);
		glm::mat4 skyModel = glm::translate(glm::mat4(1.0f), camera.Position);
		skyModel = glm::scale(skyModel, glm::vec3(-300.0f, 300.0f, 300.0f)); // negative X un-mirrors texture when viewed from inside
		glUniform1f(glGetUniformLocation(shaderProgram, "isLight"),     1.0f);
		glUniform1f(glGetUniformLocation(shaderProgram, "isEmissive"),  0.0f);
		glUniform1f(glGetUniformLocation(shaderProgram, "hasTexture"),  1.0f);
		glUniform1f(glGetUniformLocation(shaderProgram, "useShadow"),   0.0f);
		glBindTexture(GL_TEXTURE_2D, starTex);
		glUniformMatrix4fv(glGetUniformLocation(shaderProgram, "model"), 1, GL_FALSE, glm::value_ptr(skyModel));
		glBindVertexArray(skyVAO);
		glDrawArrays(GL_TRIANGLES, 0, skyVertCount);
		glDepthMask(GL_TRUE);
		glUniform1f(glGetUniformLocation(shaderProgram, "useShadow"), 1.0f);

		// Sun: mark as emissive so bloom reads its colour from the dedicated attachment
		glUniform1f(glGetUniformLocation(shaderProgram, "isLight"),    1.0f);
		glUniform1f(glGetUniformLocation(shaderProgram, "isEmissive"), 1.0f);
		glBindTexture(GL_TEXTURE_2D, textures[0]);
		glm::mat4 model = glm::mat4(1.0f);
		glUniformMatrix4fv(glGetUniformLocation(shaderProgram, "model"), 1, GL_FALSE, glm::value_ptr(model));
		glBindVertexArray(VAO[0]);
		glDrawArrays(GL_TRIANGLES, 0, sunVertCount);

		// Planets
		glUniform1f(glGetUniformLocation(shaderProgram, "isLight"),      0.0f);
		glUniform1f(glGetUniformLocation(shaderProgram, "isEmissive"),   0.0f);
		glUniform1f(glGetUniformLocation(shaderProgram, "useNormalMap"), 0.0f);
		glm::mat4 saturnModel;
		for (int p = 0; p < NUM_PLANETS; p++)
		{
			model = glm::mat4(1.0f);
			model = glm::rotate(model, t     * planets[p].orbitSpeed, glm::vec3(0.0f, 1.0f, 0.0f));
			model = glm::translate(model, glm::vec3(planets[p].orbitRadius, 0.0f, 0.0f));
			model = glm::rotate(model, realT * planets[p].spinSpeed, glm::vec3(0.0f, 1.0f, 0.0f));
			model = glm::scale(model, glm::vec3(planets[p].size));
			if (p == 5) saturnModel = model;
			glBindTexture(GL_TEXTURE_2D, textures[p + 1]);

			if (p == 2) // Earth: bind normal map to unit 2
			{
				glActiveTexture(GL_TEXTURE2);
				glBindTexture(GL_TEXTURE_2D, earthNormalTex);
				glActiveTexture(GL_TEXTURE0);
				glUniform1f(glGetUniformLocation(shaderProgram, "useNormalMap"), 1.0f);
			}
			else
			{
				glUniform1f(glGetUniformLocation(shaderProgram, "useNormalMap"), 0.0f);
			}

			glUniformMatrix4fv(glGetUniformLocation(shaderProgram, "model"), 1, GL_FALSE, glm::value_ptr(model));
			glBindVertexArray(VAO[p + 1]);
			glDrawArrays(GL_TRIANGLES, 0, planets[p].vertCount);
		}
		glUniform1f(glGetUniformLocation(shaderProgram, "useNormalMap"), 0.0f);

		// Moon
		glBindTexture(GL_TEXTURE_2D, textures[9]);
		glUniformMatrix4fv(glGetUniformLocation(shaderProgram, "model"), 1, GL_FALSE, glm::value_ptr(moonModel));
		glBindVertexArray(VAO[9]);
		glDrawArrays(GL_TRIANGLES, 0, moonVertCount);

		// Probe (satellite): orbits Earth, arrow keys control angle/radius, Q/E for height
		glUniform1f(glGetUniformLocation(shaderProgram, "isLight"),    0.0f);
		glUniform1f(glGetUniformLocation(shaderProgram, "hasTexture"), 0.0f);
		glUniform1f(glGetUniformLocation(shaderProgram, "useShadow"),  1.0f);
		glUniformMatrix4fv(glGetUniformLocation(shaderProgram, "model"), 1, GL_FALSE, glm::value_ptr(probeModel));
		glBindVertexArray(probeVAO);
		glDrawArrays(GL_TRIANGLES, 0, probeVertCount);
		glUniform1f(glGetUniformLocation(shaderProgram, "hasTexture"), 1.0f);

		// Bezier curve path visual
		//glUniform1f(glGetUniformLocation(shaderProgram, "isLight"),    1.0f);
		//glUniform1f(glGetUniformLocation(shaderProgram, "hasTexture"), 0.0f);
		//glm::mat4 identity = glm::mat4(1.0f);
		//glUniformMatrix4fv(glGetUniformLocation(shaderProgram, "model"), 1, GL_FALSE, glm::value_ptr(identity));
		//glBindVertexArray(curveVAO);
		//glDrawArrays(GL_LINE_STRIP, 0, CURVE_SEGS + 1);

		// Asteroid sphere travelling along the curve
		glUniform1f(glGetUniformLocation(shaderProgram, "isLight"), 0.0f);
		glUniformMatrix4fv(glGetUniformLocation(shaderProgram, "model"), 1, GL_FALSE, glm::value_ptr(asteroidModel));
		glBindVertexArray(asteroidVAO);
		glDrawArrays(GL_TRIANGLES, 0, asteroidVertCount);
		glUniform1f(glGetUniformLocation(shaderProgram, "hasTexture"), 1.0f);

		// Saturn's rings: transparent, drawn after all opaque geometry
		glDepthMask(GL_FALSE);
		glUniform1f(glGetUniformLocation(shaderProgram, "alpha"),      0.6f);
		glUniform1f(glGetUniformLocation(shaderProgram, "hasTexture"), 1.0f);
		glUniform1f(glGetUniformLocation(shaderProgram, "useShadow"),  0.0f);
		glBindTexture(GL_TEXTURE_2D, ringTex);
		glUniformMatrix4fv(glGetUniformLocation(shaderProgram, "model"), 1, GL_FALSE, glm::value_ptr(saturnModel));
		glBindVertexArray(ringVAO);
		glDrawArrays(GL_TRIANGLES, 0, ringVertCount);
		glDepthMask(GL_TRUE);
		glUniform1f(glGetUniformLocation(shaderProgram, "alpha"),      1.0f);
		glUniform1f(glGetUniformLocation(shaderProgram, "hasTexture"), 1.0f);
		glUniform1f(glGetUniformLocation(shaderProgram, "useShadow"),  1.0f);

		// Bloom post-processing
		// The scene is in sceneFBO. The emissive attachment contains only the sun,
		// so bloom can't spread onto planets (it blew the planets lighting).
		glDisable(GL_DEPTH_TEST);
		glDisable(GL_BLEND);

		// Desaturate the sun toward warm white and downsample to quarter res
		glBindFramebuffer(GL_FRAMEBUFFER, bloomFBO[0]);
		glViewport(0, 0, BW, BH);
		glUseProgram(bloomBrightShader);
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, emissiveTex);
		glUniform1i(glGetUniformLocation(bloomBrightShader, "emissive"), 0);
		glBindVertexArray(flareVAO);
		glDrawArrays(GL_TRIANGLES, 0, 6);

		// Ping-pong Gaussian blur, alternates horizontal and vertical passes
		glUseProgram(bloomBlurShader);
		glUniform1i(glGetUniformLocation(bloomBlurShader, "image"), 0);
		bool horizontal = true;
		for (int i = 0; i < 10; i++)
		{
			glBindFramebuffer(GL_FRAMEBUFFER, bloomFBO[horizontal ? 1 : 0]);
			glUniform1i(glGetUniformLocation(bloomBlurShader, "horizontal"), horizontal ? 1 : 0);
			glBindTexture(GL_TEXTURE_2D, bloomTex[horizontal ? 0 : 1]);
			glDrawArrays(GL_TRIANGLES, 0, 6);
			horizontal = !horizontal;
		}

		// Add the blurred glow on top of the scene and output to the default FBO
		glBindFramebuffer(GL_FRAMEBUFFER, 0);
		glViewport(0, 0, 1920, 1080);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		glUseProgram(bloomCompShader);
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, sceneTex);
		glActiveTexture(GL_TEXTURE1);
		glBindTexture(GL_TEXTURE_2D, bloomTex[0]);
		glUniform1i(glGetUniformLocation(bloomCompShader, "scene"),        0);
		glUniform1i(glGetUniformLocation(bloomCompShader, "bloom"),        1);
		glUniform1f(glGetUniformLocation(bloomCompShader, "bloomStrength"), 3.0f);
		glDrawArrays(GL_TRIANGLES, 0, 6);

		glEnable(GL_DEPTH_TEST);
		glEnable(GL_BLEND);
		glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
		glActiveTexture(GL_TEXTURE0);
		glUseProgram(shaderProgram);

		// Lens flare: projects the sun to screen space, then draws 7 sprites
		// along the axis from the sun toward the screen centre.
		// Sprites use additive blending and are sized/coloured to mimic
		// the look of real camera lenses. 
		{
			glm::vec4 sunClip  = projection * view * glm::vec4(0.0f, 0.0f, 0.0f, 1.0f);
			if (sunClip.w > 0.0f)
			{
				glm::vec2 sunNDC   = glm::vec2(sunClip.x, sunClip.y) / sunClip.w;
				float     intensity = glm::clamp(1.0f - glm::length(sunNDC) * 0.7f, 0.0f, 1.0f);

				if (intensity > 0.01f)
				{
					glDepthMask(GL_FALSE);
					glDepthFunc(GL_ALWAYS);
					glBlendFunc(GL_SRC_ALPHA, GL_ONE);

					glUseProgram(flareShader);
					glBindVertexArray(flareVAO);

					glm::vec2 axis   = glm::vec2(0.0f) - sunNDC;
					float     aspect = 1920.0f / 1080.0f;

					struct Sprite { float t, size, r, g, b, a; };
					Sprite sprites[] = {
						{ 0.0f,  0.20f, 1.0f, 0.95f, 0.70f, 0.35f }, // central glow
						{ 0.0f,  0.42f, 1.0f, 0.80f, 0.40f, 0.12f }, // wide outer halo
						{ 0.30f, 0.06f, 1.0f, 0.80f, 0.50f, 0.60f }, // warm artefact
						{ 0.55f, 0.04f, 0.80f,0.90f, 1.00f, 0.55f }, // blue artefact
						{ 0.80f, 0.09f, 1.0f, 1.00f, 0.80f, 0.35f }, // warm artefact
						{ 1.10f, 0.05f, 0.60f,0.70f, 1.00f, 0.50f }, // blue artefact
						{ 1.50f, 0.11f, 1.0f, 0.90f, 0.60f, 0.20f }, // outer warm artefact
					};

					for (auto& s : sprites)
					{
						glm::vec2 pos = sunNDC + s.t * axis;
						glm::mat4 fm  = glm::translate(glm::mat4(1.0f), glm::vec3(pos, 0.0f));
						fm = glm::scale(fm, glm::vec3(s.size, s.size * aspect, 1.0f));
						glUniformMatrix4fv(glGetUniformLocation(flareShader, "model"), 1, GL_FALSE, glm::value_ptr(fm));
						glUniform4f(glGetUniformLocation(flareShader, "flareColour"), s.r, s.g, s.b, s.a * intensity);
						glDrawArrays(GL_TRIANGLES, 0, 6);
					}

					glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
					glDepthFunc(GL_LESS);
					glDepthMask(GL_TRUE);
					glUseProgram(shaderProgram);
				}
			}
		}

		// Ship HUD: drawn last with cleared depth so it always renders on top
		glClear(GL_DEPTH_BUFFER_BIT);
		glm::mat4 shipView = glm::mat4(1.0f);
		glUniformMatrix4fv(glGetUniformLocation(shaderProgram, "view"),    1, GL_FALSE, glm::value_ptr(shipView));
		glUniform1f(glGetUniformLocation(shaderProgram, "isLight"),    0.0f);
		glUniform1f(glGetUniformLocation(shaderProgram, "hasTexture"), 0.0f);
		glUniform1f(glGetUniformLocation(shaderProgram, "useShadow"),  0.0f);
		glUniform3f(glGetUniformLocation(shaderProgram, "lightPos"),      0.0f, 3.0f, -1.0f);
		glUniform3f(glGetUniformLocation(shaderProgram, "camPos"),        0.0f, 0.0f,  0.0f);
		glUniform3f(glGetUniformLocation(shaderProgram, "lightColour"),   2.0f, 2.0f,  1.8f);
		glUniform3f(glGetUniformLocation(shaderProgram, "dirLightColour"), 0.0f, 0.0f,  0.0f);
		glUniform3f(glGetUniformLocation(shaderProgram, "spotColour"),     0.0f, 0.0f,  0.0f);

		glm::mat4 shipModel = glm::mat4(1.0f);
		shipModel = glm::translate(shipModel, glm::vec3(shipPosX, shipPosY, shipPosZ));
		shipModel = glm::rotate(shipModel, glm::radians(shipRotY), glm::vec3(0.0f, 1.0f, 0.0f));
		shipModel = glm::rotate(shipModel, glm::radians(shipRotX), glm::vec3(1.0f, 0.0f, 0.0f));
		shipModel = glm::rotate(shipModel, glm::radians(shipRotZ), glm::vec3(0.0f, 0.0f, 1.0f));
		shipModel = glm::scale(shipModel, glm::vec3(shipScale));
		shipModel = glm::translate(shipModel, glm::vec3(4.49f, -8.17f, 6.35f));
		glUniformMatrix4fv(glGetUniformLocation(shaderProgram, "model"), 1, GL_FALSE, glm::value_ptr(shipModel));
		glBindVertexArray(shipVAO);
		glDrawArrays(GL_TRIANGLES, 0, shipVertCount);

		glfwSwapBuffers(window);
		glfwPollEvents();
		processKeyboard(window);

		// Satellite probe controls:
		// Left/Right: orbit around Earth, Up/Down: change orbital radius, Q/E: change height
		if (glfwGetKey(window, GLFW_KEY_LEFT)  == GLFW_PRESS) satAngle  -= 0.01f;
		if (glfwGetKey(window, GLFW_KEY_RIGHT) == GLFW_PRESS) satAngle  += 0.01f;
		if (glfwGetKey(window, GLFW_KEY_UP)    == GLFW_PRESS) satRadius  = glm::max(0.22f, satRadius - 0.005f);
		if (glfwGetKey(window, GLFW_KEY_DOWN)  == GLFW_PRESS) satRadius += 0.005f;
		if (glfwGetKey(window, GLFW_KEY_Q)     == GLFW_PRESS) satHeight += 0.005f;
		if (glfwGetKey(window, GLFW_KEY_E)     == GLFW_PRESS) satHeight -= 0.005f;
	}

	glDeleteVertexArrays(10, VAO);
	glDeleteBuffers(10, VBO);
	glDeleteVertexArrays(1, &shipVAO);
	glDeleteBuffers(1, &shipVBO);
	glDeleteVertexArrays(1, &skyVAO);
	glDeleteBuffers(1, &skyVBO);
	glDeleteVertexArrays(1, &probeVAO);
	glDeleteBuffers(1, &probeVBO);
	glDeleteVertexArrays(1, &flareVAO);
	glDeleteBuffers(1, &flareVBO);

	glfwTerminate();
	return 0;
}
