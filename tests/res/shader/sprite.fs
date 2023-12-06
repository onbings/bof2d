#version 330 core
in vec2 TexCoords;
out vec4 color;

uniform sampler2D sprite;

uniform vec4 spriteColor;

void main()
{
//discard transparent textel
//   vec4 texcol = texture2D(sprite, TexCoords);
   vec4 texcol = texture(sprite, TexCoords);
   if (texcol.a == 0.0) 
   {
     discard;
   }
//  color = vec4(spriteColor, texcol.a) * texture(sprite, TexCoords);
  color = spriteColor * texcol;
}
