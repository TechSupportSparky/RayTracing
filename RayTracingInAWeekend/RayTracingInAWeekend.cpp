#include "rtweekend.h"

#include "aarect.h"
#include "box.h"
#include "bvh.h"
#include "camera.h"
#include "Color.h"
#include "constant_medium.h"
#include "hittable_list.h"
#include "moving_sphere.h"
#include "material.h"
#include "sphere.h"
#include "texture.h"

#include <fstream>
#include <thread>
#include <functional>
#include <ranges>
#include <mutex>
#include <execution>
#include <iostream>

#define SDL_MAIN_HANDLED
#include "SDL.h"

color ray_color(const ray& r, const color& background, const hittable& world, int depth) {
	hit_record rec;

	// If we've exceeded the ray bounce limit, no more light is gathered.
	if (depth <= 0)
		return color(0, 0, 0);

	// if the ray hits nothing, return the bg color
	if (!world.hit(r, 0.001, infinity, rec))
		return background;

	ray scattered;
	color attenuation;
	color emitted = rec.mat_ptr->emitted(rec.u, rec.v, rec.p);
	if (!rec.mat_ptr->scatter(r, rec, attenuation, scattered))
		return emitted;
	return emitted + attenuation * ray_color(scattered, background, world, depth - 1);
}

hittable_list two_spheres() {
	hittable_list objects;

	auto checker = make_shared<checker_texture>(color(0.2f, 0.3f, 0.1f), color(0.9f, 0.9f, 0.9f));

	objects.add(make_shared<sphere>(point3(0, -10, 0), 10, make_shared<lambertian>(checker)));
	objects.add(make_shared<sphere>(point3(0, 10, 0), 10, make_shared <lambertian>(checker)));
	
	return objects;
}

hittable_list two_perlin_spheres() {
	hittable_list objects;

    auto pertext = make_shared<noise_texture>(4);
    objects.add(make_shared<sphere>(point3(0,-1000,0), 1000, make_shared<lambertian>(pertext)));
    objects.add(make_shared<sphere>(point3(0, 2, 0), 2, make_shared<lambertian>(pertext)));

    return objects;
}

hittable_list earth() {
	auto earth_texture = make_shared<image_texture>("earthmap.jpg");
	auto earth_surface = make_shared<lambertian>(earth_texture);
	auto globe = make_shared<sphere>(point3(0, 0, 0), 2, earth_surface);

	return hittable_list(globe);
}

hittable_list random_scene() {
	hittable_list world;

	//auto ground_material = make_shared<lambertian>(color(0.5, 0.5, 0.5));
	std::shared_ptr<checker_texture> checker = make_shared<checker_texture>(color(0.2f, 0.3f, 0.1f), color(0.9f, 0.9f, 0.9f));
	world.add(make_shared<sphere>(point3(0, -1000, 0), 1000, make_shared<lambertian>(checker)));

	for (int a = -11; a < 11; a++) {
		for (int b = -11; b < 11; b++) {
			auto choose_mat = random_float();
			point3 center(a + 0.9f * random_float(), 0.2f, b + 0.9f * random_float());

			if ((center - point3(4, 0.2, 0)).length() > 0.9) {
				shared_ptr<material> sphere_material;

				if (choose_mat < 0.8) {
					// diffuse
					auto albedo = color::random() * color::random();
					sphere_material = make_shared<lambertian>(albedo);
					//auto center2 = center + vec3(0, random_float(0, 0.5f), 0);
					//world.add(make_shared<moving_sphere>(center, center2, 0.0f, 1.0f, 0.02f, sphere_material));
					world.add(make_shared<sphere>(center, 0.2f, sphere_material));
				}
				else if (choose_mat < 0.95) {
					// metal
					auto albedo = color::random(0.5, 1);
					auto fuzz = random_float(0, 0.5);
					sphere_material = make_shared<metal>(albedo, fuzz);
					world.add(make_shared<sphere>(center, 0.2, sphere_material));
				}
				else {
					// glass
					sphere_material = make_shared<dielectric>(1.5);
					world.add(make_shared<sphere>(center, 0.2, sphere_material));
				}
			}
		}
	}

	auto material1 = make_shared<dielectric>(1.5);
	world.add(make_shared<sphere>(point3(0, 1, 0), 1.0f, material1));

	auto material2 = make_shared<lambertian>(color(0.4f, 0.2f, 0.1f));
	world.add(make_shared<sphere>(point3(-4, 1, 0), 1.0f, material2));

	auto material3 = make_shared<metal>(color(0.7f, 0.6f, 0.5f), 0.0f);
	world.add(make_shared<sphere>(point3(4, 1, 0), 1.0, material3));

	return world;
}

hittable_list simple_light() {
	hittable_list objects;

	auto pertext = make_shared<noise_texture>(4);
	objects.add(make_shared<sphere>(point3(0, -1000, 0), 1000, make_shared<lambertian>(pertext)));
	objects.add(make_shared<sphere>(point3(0, 2, 0), 2, make_shared<lambertian>(pertext)));

	auto difflight = make_shared<diffuse_light>(color(4, 4, 4));
	objects.add(make_shared<xy_rect>(3, 5, 1, 3, -2, difflight));

	auto spherelight = make_shared<diffuse_light>(color(50, 50, 50));
	objects.add(make_shared<sphere>(point3(-3, 5, 6), 1, difflight));

	return objects;
}

hittable_list cornell_box() {
	hittable_list objects;

	auto red = make_shared<lambertian>(color(.65f, .05f, .05f));
	auto white = make_shared<lambertian>(color(.73f, .73f, .73f));
	auto green = make_shared<lambertian>(color(.12f, .45f, .15f));
	auto light = make_shared<diffuse_light>(color(15, 15, 15));

	objects.add(make_shared<yz_rect>(0, 555, 0, 555, 555, green));
	objects.add(make_shared<yz_rect>(0, 555, 0, 555, 0, red));
	objects.add(make_shared<xz_rect>(213, 343, 227, 332, 554, light));
	objects.add(make_shared<xz_rect>(0, 555, 0, 555, 0, white));
	objects.add(make_shared<xz_rect>(0, 555, 0, 555, 555, white));
	objects.add(make_shared<xy_rect>(0, 555, 0, 555, 555, white));

	shared_ptr<hittable> box1 = make_shared<box>(point3(0, 0, 0), point3(165, 330, 165), white);
	box1 = make_shared<rotate_y>(box1, 15);
	box1 = make_shared<translate>(box1, vec3(265, 0, 295));
	objects.add(box1);

	//shared_ptr<hittable> box2 = make_shared<box>(point3(0, 0, 0), point3(165, 165, 165), white);
	//box2 = make_shared<rotate_y>(box2, -18);
	//box2 = make_shared<translate>(box2, vec3(130, 0, 65));
	//objects.add(box2);

	auto material1 = make_shared<dielectric>(1.5);
	objects.add(make_shared<sphere>(point3(0, 1, 0), 1.0f, material1));

	return objects;
}

hittable_list cornell_box_smoke() {
	hittable_list objects;

	auto red = make_shared<lambertian>(color(.65f, .05f, .05f));
	auto white = make_shared<lambertian>(color(.73f, .73f, .73f));
	auto green = make_shared<lambertian>(color(.12f, .45f, .15f));
	auto light = make_shared<diffuse_light>(color(7, 7, 7));

	objects.add(make_shared<yz_rect>(0, 555, 0, 555, 555, green));
	objects.add(make_shared<yz_rect>(0, 555, 0, 555, 0, red));
	objects.add(make_shared<xz_rect>(113, 443, 127, 432, 554, light));
	objects.add(make_shared<xz_rect>(0, 555, 0, 555, 0, white));
	objects.add(make_shared<xz_rect>(0, 555, 0, 555, 555, white));
	objects.add(make_shared<xy_rect>(0, 555, 0, 555, 555, white));

	shared_ptr<hittable> box1 = make_shared<box>(point3(0, 0, 0), point3(165, 330, 165), white);
	box1 = make_shared<rotate_y>(box1, 15);
	box1 = make_shared<translate>(box1, vec3(265, 0, 295));

	shared_ptr<hittable> box2 = make_shared<box>(point3(0, 0, 0), point3(165, 165, 165), white);
	box2 = make_shared<rotate_y>(box2, -18);
	box2 = make_shared<translate>(box2, vec3(130, 0, 65));

	objects.add(make_shared<constant_medium>(box1, 0.01, color(0, 0, 0)));
	objects.add(make_shared<constant_medium>(box2, 0.01, color(1, 1, 1)));

	return objects;
}

hittable_list final_scene() {
	hittable_list boxes1;
	auto ground = make_shared<lambertian>(color(0.48, 0.83, 0.53));

	const int boxes_per_side = 20;
	for (int i = 0; i < boxes_per_side; i++) {
		for (int j = 0; j < boxes_per_side; j++) {
			auto w = 100.0;
			auto x0 = -1000.0 + i * w;
			auto z0 = -1000.0 + j * w;
			auto y0 = 0.0;
			auto x1 = x0 + w;
			auto y1 = random_float(1, 101);
			auto z1 = z0 + w;

			boxes1.add(make_shared<box>(point3(x0, y0, z0), point3(x1, y1, z1), ground));
		}
	}

	hittable_list objects;

	objects.add(make_shared<bvh_node>(boxes1, 0, 1));

	auto light = make_shared<diffuse_light>(color(7, 7, 7));
	objects.add(make_shared<xz_rect>(123, 423, 147, 412, 554, light));

	auto center1 = point3(400, 400, 200);
	auto center2 = center1 + vec3(30, 0, 0);
	auto moving_sphere_material = make_shared<lambertian>(color(0.7, 0.3, 0.1));
	objects.add(make_shared<moving_sphere>(center1, center2, 0, 1, 50, moving_sphere_material));

	objects.add(make_shared<sphere>(point3(260, 150, 45), 50, make_shared<dielectric>(1.5)));
	objects.add(make_shared<sphere>(
		point3(0, 150, 145), 50, make_shared<metal>(color(0.8, 0.8, 0.9), 1.0)
		));

	auto boundary = make_shared<sphere>(point3(360, 150, 145), 70, make_shared<dielectric>(1.5));
	objects.add(boundary);
	objects.add(make_shared<constant_medium>(boundary, 0.2, color(0.2, 0.4, 0.9)));
	boundary = make_shared<sphere>(point3(0, 0, 0), 5000, make_shared<dielectric>(1.5));
	objects.add(make_shared<constant_medium>(boundary, .0001, color(1, 1, 1)));

	auto emat = make_shared<lambertian>(make_shared<image_texture>("earthmap.jpg"));
	objects.add(make_shared<sphere>(point3(400, 200, 400), 100, emat));
	auto pertext = make_shared<noise_texture>(0.1);
	objects.add(make_shared<sphere>(point3(220, 280, 300), 80, make_shared<lambertian>(pertext)));

	hittable_list boxes2;
	auto white = make_shared<lambertian>(color(.73, .73, .73));
	int ns = 1000;
	for (int j = 0; j < ns; j++) {
		boxes2.add(make_shared<sphere>(point3::random(0, 165), 10, white));
	}

	objects.add(make_shared<translate>(
		make_shared<rotate_y>(
			make_shared<bvh_node>(boxes2, 0.0, 1.0), 15),
		vec3(-100, 270, 395)
		)
	);

	return objects;
}


struct Pixel {
	int x = 0;
	int y = 0;
};

void ProcessPixel(const Pixel& pixel, int image_width, int image_height, const camera& cam, const hittable_list& world,
	int max_depth, int samples_per_pixel, color& background, SDL_Renderer* renderer, std::mutex& renderMutex)
{
	color pixel_color(0, 0, 0);
	for (int sam = 0; sam < samples_per_pixel; ++sam) {
		int y = image_height - 1 - pixel.y;

		float u = (pixel.x + random_float()) / (image_width - 1);
		float v = (y + random_float()) / (image_height - 1);
		ray r = cam.get_ray(u, v);
		pixel_color += ray_color(r, background, world, max_depth);
	}

	// Lock before attempting to write pixel data
	std::lock_guard<std::mutex> lock(renderMutex);
	color outputColor = calculate_color_with_gamma(pixel_color, samples_per_pixel);
	SDL_SetRenderDrawColor(renderer, outputColor.x(), outputColor.y(), outputColor.z(), SDL_ALPHA_OPAQUE);
	SDL_RenderDrawPoint(renderer, pixel.x, pixel.y);
	SDL_RenderPresent(renderer);
}

int main()
{
	std::ofstream ppmFile("RayTracingInAWeekend.ppm");

	// Image
	auto aspect_ratio = 16.0f / 9.0f;
	int image_width = 400;
	int samples_per_pixel = 100;
	int max_depth = 50;
	
	// World
	hittable_list world;

	point3 lookfrom;
	point3 lookat;
	auto vfov = 40.0;
	auto aperture = 0.0;
	color background(0, 0, 0);

	switch (8) {
	case 1:
		world = random_scene();
		lookfrom = point3(13, 2, 3);
		lookat = point3(0, 0, 0);
		vfov = 20.0f;
		aperture = 0.1f;
		background = color(0.70f, 0.80f, 1.00f);
		break;

	case 2:
		world = two_spheres();
		lookfrom = point3(13, 2, 3);
		lookat = point3(0, 0, 0);
		vfov = 20.0f;
		background = color(0.70f, 0.80f, 1.00f);
		break;
	case 3:
		world = two_perlin_spheres();
		lookfrom = point3(13, 2, 3);
		lookat = point3(0, 0, 0);
		vfov = 20.0f;
		background = color(0.70f, 0.80f, 1.00f);
		break;
	case 4:
		world = earth();
		lookfrom = point3(13, 2, 3);
		lookat = point3(0, 0, 0);
		vfov = 20.0f;
		background = color(0.70f, 0.80f, 1.00f);
		break;
	case 5:
		world = simple_light();
		background = color(0, 0, 0);
		lookfrom = point3(26, 3, 6);
		lookat = point3(0, 2, 0);
		vfov = 20.0f;
		samples_per_pixel = 400;
		break;
	case 6:
		world = cornell_box();
		aspect_ratio = 1.0;
		image_width = 600;
		samples_per_pixel = 200;
		lookfrom = point3(278, 278, -800);
		lookat = point3(278, 278, 0);
		vfov = 40.0;
		break;
	case 7:
		world = cornell_box_smoke();
		aspect_ratio = 1.0;
		image_width = 600;
		samples_per_pixel = 200;
		lookfrom = point3(278, 278, -800);
		lookat = point3(278, 278, 0);
		vfov = 40.0;
		break;
	case 8:
	default:
		world = final_scene();
		aspect_ratio = 1.0;
		image_width = 800;
		samples_per_pixel = 10000;
		lookfrom = point3(478, 278, -600);
		lookat = point3(278, 278, 0);
		vfov = 40.0;
		break;
	}

	// Camera

	const vec3 vup(0, 1, 0);
	const auto dist_to_focus = 10.0f;
	const int image_height = static_cast<int>(image_width / aspect_ratio);
	camera cam(lookfrom, lookat, vup, vfov, aspect_ratio, aperture, dist_to_focus, 0.0f, 1.0f);

	// Rendering out to a file (very slow)
	/*
	ppmFile << "P3\n" << image_width << ' ' << image_height << "\n255\n";

	for (int j = image_height - 1; j >= 0; --j) {
		std::cerr << "\rScanlines remaining: " << j << ' ' << std::flush;
		for (int i = 0; i < image_width; ++i) {
			color pixel_color(0, 0, 0);
			for (int s = 0; s < samples_per_pixel; ++s) {
				auto u = (i + random_float()) / (image_width - 1);
				auto v = (j + random_float()) / (image_height - 1);
				ray r = cam.get_ray(u, v);
				pixel_color += ray_color(r, world, max_depth);
			}
			write_color(ppmFile, pixel_color, samples_per_pixel);
		}
	}
	*/

	// Initialize SDL
	if (SDL_Init(SDL_INIT_VIDEO) < 0) {
		SDL_LogError(SDL_LOG_CATEGORY_APPLICATION, "SDL could not initialize! SDL_Error: %s", SDL_GetError());
		return -1;
	}

	// Create a window
	SDL_Window* window = SDL_CreateWindow("SDL Window", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, image_width, image_height, SDL_WINDOW_SHOWN);
	if (window == NULL) {
		SDL_LogError(SDL_LOG_CATEGORY_APPLICATION, "Window could not be created! SDL_Error: %s", SDL_GetError());
		return -1;
	}

	// Create a renderer
	SDL_Renderer* renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);
	if (renderer == NULL) {
		SDL_LogError(SDL_LOG_CATEGORY_APPLICATION, "Renderer could not be created! SDL_Error: %s", SDL_GetError());
		return -1;
	}

	// Clear the renderer
	SDL_SetRenderDrawColor(renderer, 0, 0, 0, SDL_ALPHA_OPAQUE);
	SDL_RenderClear(renderer);	

	// Setting up 
	std::mutex renderMutex;
	std::vector<Pixel> pixels;
	for (int j = 0; j <= image_height - 1; ++j) {
		for (int i = 0; i < image_width; ++i) {
			pixels.push_back({ i, j });
		}
	}

	// Render!
	std::for_each(std::execution::par, pixels.begin(), pixels.end(),
		[&](const Pixel& pixel) {ProcessPixel(pixel, image_width, image_height, cam, world, max_depth, samples_per_pixel, background, renderer, renderMutex); }
	);

	// ------------ Finished -----------------
	std::cerr << "\nDone.\n";
	
	// Wait for user exit
	bool quit = false;
	SDL_Event event;
	while (!quit) {
		while (SDL_PollEvent(&event)) {
			if (event.type == SDL_QUIT) {
				quit = true;
			}
		}
	}

	// Clean up and exit
	SDL_DestroyRenderer(renderer);
	SDL_DestroyWindow(window);
	SDL_Quit();
	
	
	ppmFile.close();
}
