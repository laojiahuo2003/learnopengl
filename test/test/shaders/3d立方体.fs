#version 330 core
out vec4 FragColor;
in vec2 TexCoord;
in vec3 ourColor;
uniform sampler2D texture1;
uniform sampler2D texture2;
void main()
{
    //FragColor = texture(texture1,TexCoord)*vec4(ourColor,1.0f);
     FragColor = mix(texture(texture1,TexCoord),texture(texture2,TexCoord),0.8);
}