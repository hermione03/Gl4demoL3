#version 330

uniform float u_time;
uniform float amplitudeSon;

/* sortie du frament shader : une couleur */
out vec4 fragColor;

in vec2 vsoPosition;

void main(void) {

    float pct = abs(sin(u_time));

    float amp = amplitudeSon * 0.0005;

    float x0 = 0.3 * sin(u_time);
    float y0 = 0.6 * cos(u_time * 0.3);

    // float resolution = -10.0  * (u_time * 0.5);
    float resolution = -3.5 + 1.5 * sin(u_time * 0.1);

    vec2 fragCoord = (vsoPosition + 1.0) / 2;

    float x = resolution * fragCoord.x - resolution * 0.5;
    float y = resolution * fragCoord.y - resolution * 0.5;

    int i;
    for(i = 0; x * x + y * y <= 4.0 && i < 1000; i++) {
        float newX = x * x - y * y + x0;
        float newY = 2.0 * x * y + y0;
        x = newX;
        y = newY;
    }

    float intensity = float(i) / 1000.0;
    intensity *= 64.0;

      // Utiliser l'amplitude sonore pour ajuster la couleur
    vec3 color = vec3(0.0, 0.0, 1.0 - intensity) * (1.0 + amp);

    // Ajouter un effet de pulsation en utilisant l'amplitude sonore
    //color += vec3(amp * sin(u_time * 4.0), amp * cos(u_time * 3.0), amp * sin(u_time * 5.0));

    // Appliquer une variation temporelle à la couleur
    //color *= abs(sin(u_time));

    // fragColor = vec4(1.0 - intensity, 0.0, 0.0, 1.0);
    fragColor = vec4(color, 1.0);

}
