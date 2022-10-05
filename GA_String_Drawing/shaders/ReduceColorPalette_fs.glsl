#version 430

// Input vertex attributes (from vertex shader)
in vec2 fragTexCoord;
in vec4 fragColor;

// Input uniform values
uniform sampler2D texture0;
uniform vec4 colDiffuse;

uniform float thresholdMap[4];
uniform int size;
uniform float spread;
uniform vec2 resolution;

// Output fragment color
out vec4 finalColor;

// NOTE: Add here your custom variables

void main()
{
    // Texel color fetching from texture sampler
    vec4 texelColor = texture(texture0, fragTexCoord);

    // NOTE: Implement here your fragment shader code
    int n = int(sqrt(size));
    
   //finalColor = texelColor*colDiffuse;
   vec2 pos = fragTexCoord * resolution;
   int x = int(pos.x) % n;
   int y = int(pos.y) % n;

   int index = x + y*n;

   float mVal = thresholdMap[index];
   vec3 spreadColor = texelColor.rgb;
   spreadColor = (spreadColor + spread*mVal);

   spreadColor = floor(spreadColor*(n-1) + 0.5)/(n-1);

   finalColor = vec4(spreadColor,1.0);

}
