#pragma once


// return 
glm::vec3 DoNothing(triangle* tri, int depth, glm::vec3 p, glm::vec3 dir)
{
	return vec3(0);
}

// shade the hit surface using the Whitted model
glm::vec3 Shade(triangle* tri, int depth, glm::vec3 p, glm::vec3 dir)
{
	// use the first vertex colour and normal as the uniform surface properties
	vec3 surface_col = tri->v1.col;
	vec3 normal = normalize(tri->v1.nor);

	// ambient term 
	vec3 col = 0.1f * surface_col;

	// shadow ray
	vec3 to_light   = light_pos - p;
	float light_dist = length(to_light);
	vec3 light_dir  = to_light / light_dist;

	float shadow_t;
	vec3  shadow_col;

	// offset the origin slightly 
	trace(p + normal * 1e-4f, light_dir, shadow_t, shadow_col, depth, DoNothing);

	// diffuse term 
	if (shadow_t >= light_dist)
	{
		// clamp to zero ( negative dot means light is behind the surface)
		float diffuse = max(dot(normal, light_dir), 0.f);
		col += diffuse * surface_col;
	}

	// mirror reflection
	if (tri->reflect && depth < max_recursion_depth)
	{
		vec3 reflect_dir = reflect(dir, normal);
		float reflect_t;
		vec3  reflect_col;
		trace(p + normal * 1e-4f, reflect_dir, reflect_t, reflect_col, depth + 1, Shade);
		col += reflect_col;
	}

	return col;
}

/*	
	Test whether 3D point 'pt' lies inside the triangle(v1, v2, v3).
	for each edge the cross product with (pt - vertex) is computed and
	compared against the triangle normal – all three must share the same sign. 
*/
bool PointInTriangle(glm::vec3 pt, glm::vec3 v1, glm::vec3 v2, glm::vec3 v3)
{
	vec3 n = cross(v2 - v1, v3 - v1);

	float d0 = dot(cross(v2 - v1, pt - v1), n);
	float d1 = dot(cross(v3 - v2, pt - v2), n);
	float d2 = dot(cross(v1 - v3, pt - v3), n);

	return (d0 >= 0.f && d1 >= 0.f && d2 >= 0.f) || (d0 <= 0.f && d1 <= 0.f && d2 <= 0.f);
}

// find where the ray hits the triangle plane and test if the point is inside the triangle.
float RayTriangleIntersection(glm::vec3 o, glm::vec3 dir, triangle* tri, glm::vec3& point)
{
	vec3 v1 = tri->v1.pos, v2 = tri->v2.pos, v3 = tri->v3.pos;

	// plane normal
	vec3 n = cross(v2 - v1, v3 - v1);
	float denom = dot(n, dir);

	// ray is parallel to the plane
	if (abs(denom) < 1e-6f) return FLT_MAX;

	// distance to the plane
	float t = dot(n, v1 - o) / denom;

	// intersection is behind the ray
	if (t < 0.f) return FLT_MAX;

	// intersection point
	point = o + t * dir;

	// check point is inside the triangle
	if (!PointInTriangle(point, v1, v2, v3)) return FLT_MAX;

	return t;
}

// find the closest triangle hit and call p_hit to get the colour.
void trace(glm::vec3 o, glm::vec3 dir, float& t, glm::vec3& io_col, int depth, closest_hit p_hit)
{
	t = FLT_MAX;
	triangle* hit_tri = nullptr;
	vec3 hit_point;

	// find nearest hit
	for (triangle& tri : tris)
	{
		vec3  point;
		float curr_t = RayTriangleIntersection(o, dir, &tri, point);

		if (curr_t < t)
		{
			t = curr_t;
			hit_tri = &tri;
			hit_point = point;
		}
	}

	if (hit_tri != nullptr)
		io_col = p_hit(hit_tri, depth, hit_point, dir);
	else
		io_col = bkgd;
}

// compute the ray direction for a pixel using vfov = 90 degrees.
vec3 GetRayDirection(float px, float py, int W, int H, float aspect_ratio, float fov)
{
	// map pixel to [-1, 1], scaled by aspect ratio horizontally
	float u = (2.f * (px + 0.5f) / (float)W - 1.f) * aspect_ratio;
	float v =  2.f * (py + 0.5f) / (float)H - 1.f;

	// scale for fov
	float scale = tan(fov / 2.f);

	// basis vectors
	vec3 R = vec3( 1,  0,  0);
	vec3 U = vec3( 0, -1,  0);
	vec3 F = vec3( 0,  0, -1);

	return normalize(F + scale * u * R + scale * v * U);
}

// shoot a ray per pixel and write the colour to the pixel buffer.
void raytrace()
{
	float aspect_ratio = (float)PIXEL_W / (float)PIXEL_H;
	float fov = glm::radians(90.f);

	for (int pixel_y = 0; pixel_y < PIXEL_H; ++pixel_y)
	{
		float percf = (float)pixel_y / (float)PIXEL_H;
		int perci = (int)(percf * 100);
		std::clog << "\rScanlines done: " << perci << "%" << ' ' << std::flush;

		for (int pixel_x = 0; pixel_x < PIXEL_W; ++pixel_x)
		{
			// ray direction for this pixel
			vec3 dir = GetRayDirection((float)pixel_x, (float)pixel_y, PIXEL_W, PIXEL_H, aspect_ratio, fov);

			float t;
			vec3  col;
			// cast the ray and shade
			trace(eye, dir, t, col, 0, Shade);
			writeCol(col, pixel_x, pixel_y);
		}
	}
	std::clog << "\rFinish rendering.           \n";
}
