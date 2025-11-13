# ZombieGuy40

ZombieGuy40 is a game made in 2 weeks for a college game jam in C with only a handful of low-level single header libraries (Sokol + CGLM + stb_image).

# Features

✏ - Working 2D Sprite Renderer (Texture Atlas)!

🏃‍♂️ - Sprite-animation system!

📦 - Axis-Aligned Bounding-Box based collision!

😱 - Performant zombie-hordes with multiple AI types!

🎵 - Sound system with a custom soundtrack!

🖥 - Resizable window for differing aspect ratios and resolutions!

And last but not least a fully playable roguelike with build variety, a story and more! 🎮

# Development

ZombieGuy40 was my first ever project programmed from the ground up in pure C. I had to learn about various graphics programming concepts such as how to set up vertex & indices buffers to pass into the GPU, vertex and fragment shaders, creating a draw queue and more. Sokol is deliberately designed to be unintrusive, providing only a minimal abstraction layer compared to APIs like OpenGL. This forced me to step out of my comfort zone from engines such as Unity and implement a sprite renderer completely from scratch, all under a tight two-week deadline. I had to also adapt to procedural programming concepts and patterns when writing in C, this was very different to the object-oriented nature of languages I had experience in prior such as C#. However I felt I really learned a lot from this project.

# Technical Challenges

- Setting up a smooth development environment on my Windows machine. Typically Windows isn't great for C dev as it isn't a Unix based operating system, this required using tools like MSYS2 terminal to simulate a Unix style environment.
- Learning C after 4 years of OOP experience in C#, the shift was quite drastic. I find C a lot more explicit in the way it's written however I think I've began to prefer the clear logical flow of the language.
- Studying graphics programming concepts and applying that knowledge with the Sokol library.
- Learning basic GLSL to program my vertex & fragment shaders.
- Using smart workarounds (object-pooling) to runtime memory allocation to avoid error prone malloc() and free() functions.
- Organization. C is wrote very differently to C# making organization a lot different and more challenging.
 
# Compilation

**Windows**

Clone repo and (with Make & GCC installed) run "make".

**Mac/Linux**
Build instructions coming soon — the project currently depends on Windows-specific paths and libraries.

# Further Planned Features

- PS Vita port through the VitaSDK library.
- Smart sprite batching to draw everything in a single draw-call.

# Redistribution

This project is licensed under the Creative Commons Attribution-NonCommercial 4.0 International License.
See the LICENSE file or visit https://creativecommons.org/licenses/by-nc/4.0/ for details.
