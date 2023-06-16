# RayTracing

This project is my version of the ["Ray Tracing: In a Weekend," and "Ray Tracing: The Next week,"](https://github.com/RayTracing/raytracing.github.io) course.  The raytracer itself follows much of the design of the book, however I've swapped from writing to a PPM file (as my write speeds are rather slow) over to using SDL for rendering. I've also utilized a few modern C++ techniques such as parallilizing the main render loop to increase runtime.

# Troubleshooting
When opening the project, if you get the error "cannot include file SDL," you must include it in the build you're attempting following this step by step: https://lazyfoo.net/tutorials/SDL/01_hello_SDL/windows/msvc2019/index.php

The source is located in the upper directory, just include the files for the debug/shipping version that you intend to run.  You just need to include either the x86/x64 version you need.

This project was originally uploaded as just an x86 project with debug/release set properly.  If you're pulling this code, be sure you have swapped your project over before attempting to run.

# Renders

This was the final render utilizing a sample size of 10,000 (vectors per px), and a max bounce of 50:
![FinalRender](https://github.com/TechSupportSparky/RayTracing/assets/39195543/90580999-0de1-49ef-a880-4c41f517b7c3)

This image was with a very low sample size and bounce for a fairly quick write speed (<5 minutes).  Metallic and lambertian objects reflect the correct amount and a depth of field effect is applied to objects far away:
![LowerSample](https://github.com/TechSupportSparky/RayTracing/assets/39195543/0ff5ab21-c78f-4d3c-a9d3-299573992a8d)

This image was utilizing a much larger sample size of around 500 (vectors per px) and a bounce of around 200.  This led to a bit of color bleed where even lambertian objects appeared mirror-like.  This is far from ideal as it does ot appear normal, however it is an interesting effect nonetheless!
![BallRender](https://github.com/TechSupportSparky/RayTracing/assets/39195543/7dcb33b4-1501-4553-b360-e1d4b060dc77)

Repeated sin wave texture mapping to the ground:

![SmallTexture](https://github.com/TechSupportSparky/RayTracing/assets/39195543/a1ca4b4d-8e51-4eee-8b61-19701e3e2117)

Perlin noise with no background and a light source:

![PerlinWithLight](https://github.com/TechSupportSparky/RayTracing/assets/39195543/87820be8-497d-4026-83df-1765a8576043)

Not a showcase of the raytracer itself, however a neat result of turning the sample size down very low on the final image render.  This was a sample size of 2 (vectors per ppx) and a max bounce of 2.  Overall render took less than a minute:
![LowSampleFinalRender](https://github.com/TechSupportSparky/RayTracing/assets/39195543/bf5720d7-8004-4088-843a-24a86680bff1)
