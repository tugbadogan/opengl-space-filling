#version 150


out vec4 fColor;
uniform vec4 color;

void
main()
{
    fColor = vec4(color);
}
