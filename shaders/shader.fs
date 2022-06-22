#version 330 core
out vec4 FragColor;

in vec3 vertexColor;
in vec2 textCoord;

uniform sampler2D texture1;
uniform sampler2D texture2;

void main()
{
    //FragColor = vertexColor;
    //FragColor = texture(texture2, textCoord);
    //FragColor = texture(texture1, textCoord) * vec4(vertexColor, 1.0);
    FragColor = mix(texture(texture1, textCoord), texture(texture2, textCoord), 0.2);
}
