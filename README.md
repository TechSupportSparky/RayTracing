# RayTracing

This project is my version of the ["Ray Tracing: In a Weekend," and "Ray Tracing: The Next week,"](https://github.com/RayTracing/raytracing.github.io) course.  The raytracer itself follows much of the design of the book, however I've swapped from writing to a PPM file (as my write speeds are rather slow) over to using SDL for rendering. I've also utilized a few modern C++ techniques such as parallilizing the main render loop to increase runtime.

# Troubleshooting
When opening the project, if you get the error "cannot include file SDL," you must include it in the build you're attempting following this step by step: https://lazyfoo.net/tutorials/SDL/01_hello_SDL/windows/msvc2019/index.php

The source is located in the upper directory, just include the files for the debug/shipping version that you intend to run.  You just need to include either the x86/x64 version you need.

This project was originally uploaded as just an x86 project with debug/release set properly.  If you're pulling this code, be sure you have swapped your project over before attempting to run.

# Renders

This was the final render utilizing a sample size of 10,000 (vectors per px), and a max bounce of 50:
![FinalRender](https://github.com/TechSupportSparky/RayTracing/assets/39195543/90580999-0de1-49ef-a880-4c41f517b7c3)

This image was utilizing a large sample size with a simple ground object.  This led to some color bleeding as most of the objects in this image are metallic and thus reflective (and even lambertian leads to a bit of color bleed).
![BallRender](https://github.com/TechSupportSparky/RayTracing/assets/39195543/7dcb33b4-1501-4553-b360-e1d4b060dc77)
![SmallTexture](https://github.com/TechSupportSparky/RayTracing/assets/39195543/a1ca4b4d-8e51-4eee-8b61-19701e3e2117)
![PerlinWithLight](https://github.com/TechSupportSparky/RayTracing/assets/39195543/87820be8-497d-4026-83df-1765a8576043)
