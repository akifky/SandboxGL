#version 330 core
layout (location = 0) in vec3 aPos;
layout (location = 1) in vec2 aInstancePos;
layout (location = 2) in int aTypeIndex;

flat out int TypeIndex;
uniform float uCellSize;

void main() {
    vec2 scaledPos = aPos.xy * uCellSize;
    gl_Position = vec4(scaledPos + aInstancePos, aPos.z, 1.0);
    TypeIndex = aTypeIndex;
}