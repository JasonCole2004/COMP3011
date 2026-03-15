#pragma once

//fill every pixel in the colour buffer with the given colour.
void ClearColourBuffer(float col[4])
{
	for (int i = 0; i < PIXEL_W * PIXEL_H; i++)
	{
		colour_buffer[i * 3 + 0] = col[0];
		colour_buffer[i * 3 + 1] = col[1];
		colour_buffer[i * 3 + 2] = col[2];
	}
}

// reset every depth value to the maximum possible value.
void ClearDepthBuffer()
{
	for (int i = 0; i < PIXEL_W * PIXEL_H; i++)
	{
		depth_buffer[i] = FLT_MAX;
	}
}

// multiply every vertex position by the given matrix.
void ApplyTransformationMatrix(glm::mat4 T, vector<triangle>& tris)
{
	for (triangle& tri : tris)
	{
		tri.v1.pos = T * tri.v1.pos;
		tri.v2.pos = T * tri.v2.pos;
		tri.v3.pos = T * tri.v3.pos;
	}
}

// divide x, y, z by w 
void ApplyPerspectiveDivision(vector<triangle>& tris)
{
	for (triangle& tri : tris)
	{
		tri.v1.pos = glm::vec4(tri.v1.pos.x / tri.v1.pos.w, tri.v1.pos.y / tri.v1.pos.w, tri.v1.pos.z / tri.v1.pos.w, 1.0f);
		tri.v2.pos = glm::vec4(tri.v2.pos.x / tri.v2.pos.w, tri.v2.pos.y / tri.v2.pos.w, tri.v2.pos.z / tri.v2.pos.w, 1.0f);
		tri.v3.pos = glm::vec4(tri.v3.pos.x / tri.v3.pos.w, tri.v3.pos.y / tri.v3.pos.w, tri.v3.pos.z / tri.v3.pos.w, 1.0f);
	}
}

// map NDC coordinates (-1 to +1) to pixel coordinates (0 to w/h).
void ApplyViewportTransformation(int w, int h, vector<triangle>& tris)
{
	for (triangle& tri : tris)
	{
		tri.v1.pos.x = (tri.v1.pos.x + 1.0f) / 2.0f * w;
		tri.v1.pos.y = (1.0f - tri.v1.pos.y) / 2.0f * h;

		tri.v2.pos.x = (tri.v2.pos.x + 1.0f) / 2.0f * w;
		tri.v2.pos.y = (1.0f - tri.v2.pos.y) / 2.0f * h;

		tri.v3.pos.x = (tri.v3.pos.x + 1.0f) / 2.0f * w;
		tri.v3.pos.y = (1.0f - tri.v3.pos.y) / 2.0f * h;
	}
}

// compute barycentric coordinates for a pixel relative to a triangle.
void ComputeBarycentricCoordinates(float px, float py, triangle t, float& alpha, float& beta, float& gamma)
{
	// total signed area of the triangle (used to normalise the weights).
	float total = (t.v2.pos.x - t.v1.pos.x) * (t.v3.pos.y - t.v1.pos.y)
				- (t.v2.pos.y - t.v1.pos.y) * (t.v3.pos.x - t.v1.pos.x);

	alpha = ((t.v2.pos.x - px) * (t.v3.pos.y - py) - (t.v2.pos.y - py) * (t.v3.pos.x - px)) / total;
	beta  = ((t.v3.pos.x - px) * (t.v1.pos.y - py) - (t.v3.pos.y - py) * (t.v1.pos.x - px)) / total;
	gamma = 1.0f - alpha - beta;
}

// interpolate the colour and depth at a pixel using the barycentric weights.
void ShadeFragment(triangle tri, float& alpha, float& beta, float& gamma, glm::vec3& col, float& depth)
{
	col   = alpha * tri.v1.col + beta * tri.v2.col + gamma * tri.v3.col;
	depth = alpha * tri.v1.pos.z + beta * tri.v2.pos.z + gamma * tri.v3.pos.z;
}

// loop over every pixel and every triangle to determine what colour each pixel should be.
void Rasterise(vector<triangle> tris)
{
	for (int py = 0; py < PIXEL_H; py++)
	{
		float percf = (float)py / (float)PIXEL_H;
		int perci = percf * 100;
		std::clog << "\rScanlines done: " << perci << "%" << ' ' << std::flush;

		for (int px = 0; px < PIXEL_W; px++)
		{
			for (triangle& tri : tris)
			{
				float alpha, beta, gamma;
				// sample at pixel centre for accurate edge handling
				ComputeBarycentricCoordinates(px + 0.5f, py + 0.5f, tri, alpha, beta, gamma);

				//all weights positive means the pixel is inside this triangle
				if (alpha >= 0.0f && beta >= 0.0f && gamma >= 0.0f)
				{
					glm::vec3 col;
					float depth;
					ShadeFragment(tri, alpha, beta, gamma, col, depth);

					// only draw if this triangle is closer than whatever was drawn here before
					int idx = py * PIXEL_W + px;
					if (depth < depth_buffer[idx])
					{
						depth_buffer[idx] = depth;
						writeColToDisplayBuffer(col, px, py);
					}
				}
			}
		}
	}
	std::clog << "\rFinish rendering.           \n";
}

void render(vector<triangle>& tris)
{
	// start with a white background and far depth values
	float white[4] = { 255, 255, 255, 255 };
	ClearColourBuffer(white);
	ClearDepthBuffer();

	// build the MVP matrix.
	glm::mat4 model = glm::translate(glm::mat4(1.0f), glm::vec3(0.1f, -2.5f, -6.0f));
	glm::mat4 view = glm::mat4(1.0f);
	glm::mat4 proj = glm::perspective(glm::radians(55.f), (float)PIXEL_W / (float)PIXEL_H, 0.1f, 10.f);
	glm::mat4 MVP = proj * view * model;

	// work on a copy so the orignal triangle data is not modified.
	vector<triangle> transformed = tris;

	ApplyTransformationMatrix(MVP, transformed);
	ApplyPerspectiveDivision(transformed);
	ApplyViewportTransformation(PIXEL_W, PIXEL_H, transformed);
	Rasterise(transformed);
}
