![App screenshot](git_images/logo.png)

# ZombieGuy40 🕹

ZombieGuy40 is a game made in 2 weeks for a college game jam in C with only a handful of low-level single header libraries (Sokol + CGLM + stb_image + miniaudio).

# Features 🎉

✏ - Working custom 2D Sprite Renderer (Texture Atlas usage)

🏃‍♂️ - Sprite-animation system

📦 - Axis-Aligned Bounding-Box based collision with physics

👿 - Performant zombie-hordes with multiple AI types

🎵 - Sound system with a custom soundtrack

🖱 - UI systems for text rendering and buttons

🖥 - Resizable Window

And last but not least a fully playable roguelike with build variety and more! 🎮

# Development 🔨

**FOR THE ORIGINAL GAME ACHIEVED IN 2 WEEKS, DOWNLOAD AND RUN ZG40 1.0.0 IN RELEASES. MINOR QOL AND BUGFIXING HAS TAKEN PLACE SINCE THEN.**

ZombieGuy40 was my first ever project programmed from the ground up in pure C. I had to learn about various graphics programming concepts such as how to set up vertex and index buffers to pass into the GPU, vertex and fragment shaders, creating a draw queue and more. Sokol is deliberately designed to be unintrusive, providing only a minimal abstraction layer compared to APIs like OpenGL. This forced me to step out of my comfort zone from engines such as Unity and implement a sprite renderer completely from scratch, all under a tight two-week deadline. I also had to adapt to procedural programming concepts and patterns when writing in C, this was very different to the object-oriented nature of languages I had experience in prior such as C#. However I felt I really learned a lot from this project.

![Demo](git_images/prototype1.gif)

**For clarity and transparency: although 4 weeks were allocated for this college game jam, the first week was dedicated to planning and the final week to documentation. The actual development phase lasted roughly 2 weeks.**

# Technical Challenges 👨‍💻

- Setting up a smooth development environment on my Windows machine. Typically Windows isn't great for C dev as it isn't a Unix based operating system, this required using tools like MSYS2 terminal to simulate a Unix style environment.
- Learning C after 4 years of OOP experience in C#, the shift was quite drastic. I find C a lot more explicit in the way it's written however I think I've began to prefer the clear logical flow of the language.
- Studying graphics programming concepts and applying that knowledge with the Sokol library.
- Learning basic GLSL to program my vertex & fragment shaders.
- Using smart workarounds (object-pooling) to runtime memory allocation to avoid error prone malloc() and free() functions.
- Organization. C is wrote very differently to C# making organization a lot different and more challenging.
- Pathfinding was especially challenging as I didn't have time to work on a more complex A* system. I had to devise a custom simpler algorithm, this was one of the bigger problems I overcame this project even if the solution wasn't the most elegant (I'm not particularly happy with the pathfinding in the game, I think I could work on a better implementation now).
 
# Compilation 👷‍♂️

**Windows**

Clone repo and (with Make & GCC installed) run "make".

**Mac/Linux**
Build instructions coming soon — the project currently depends on Windows-specific paths and libraries.

# Further Planned Features (When/if I have the time) 🗺

- PS Vita port through the VitaSDK library.
- A larger content update with more zombie types, a reworked scene/level system to enable larger, more complex levels and more!

# How If/Any AI Has Been Used 🤖

All art, code, music and sound is entirely produced by myself and myself only. Nothing has been directly copy-pasted from any AI chatbot, no generated content has been used. The use of AI chatbots for debugging has been the only use of AI this project, I have tried to use AI in such a way that it works WITH me when I need it, as opposed to working FOR me.

# Redistribution 🎁

This project is licensed under the Creative Commons Attribution-NonCommercial 4.0 International License.
See the LICENSE file or visit https://creativecommons.org/licenses/by-nc/4.0/ for details.
