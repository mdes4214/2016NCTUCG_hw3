library:
OpenGL
freeGLUT
GLM

control:
w, a, s, d: eyex, eyey
k, h: lightx
u, j: lighty
m, n: lightz
mouse button push & move: rotate
mouse scroll wheel, z, x: zoom in/out (eyez)
r: reset
f: rotate toggle
c: switch shading mode, including "default", "Flat Shading", "Phong Shading", "Gouraud Shading", "Gooch Shading"

model:
1 apple: default
2 wolf: maybe looks like a dog...
3 falcon: a kind of bird
4 sponza: groups object

note:
If texture format is .png, must to flip it on y axis and convert to .ppm. It's due to the glmReadObject library.