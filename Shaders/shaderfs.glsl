#version 330 core
flat in int TypeIndex;
out vec4 FragColor;

void main() {
    vec4 color;
    if (TypeIndex == 1) { // TILE_SAND
        color = vec4(0.76, 0.70, 0.50, 1.0);
    }
    else if (TypeIndex == 2) {
        color = vec4(0.0f, 0.4f, 0.65f, 1.0f);
    }

    else {
        color = vec4(0.0, 0.0, 0.0, 0.0); // TILE_EMPTY: Þeffaf
    }
    FragColor = color;

}