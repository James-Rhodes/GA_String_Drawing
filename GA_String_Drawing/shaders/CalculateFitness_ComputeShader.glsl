#version 430

layout (local_size_x = 16, local_size_y = 16) in;

layout(rgba8, binding = 0) uniform readonly image2D srcImage;
layout(rgba8, binding = 1) uniform writeonly image2D destImage;


void main(){

	ivec2 texelCoords = ivec2(gl_GlobalInvocationID.xy);

	vec4 pixel = imageLoad(srcImage, texelCoords);

	pixel.g = pixel.r;

	imageStore(destImage, texelCoords, pixel);

}