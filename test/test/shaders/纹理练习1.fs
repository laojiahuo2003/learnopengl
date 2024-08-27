#version 330 core
out vec4 FragColor;
in vec2 TexCoord;
in vec3 ourColor;
uniform sampler2D texture1;
uniform sampler2D texture2;
uniform float factor;
void main()
{
    //FragColor = texture(texture1,TexCoord);
     FragColor = mix(texture(texture1,vec2(-TexCoord.x,TexCoord.y)),texture(texture2,TexCoord),factor);
}