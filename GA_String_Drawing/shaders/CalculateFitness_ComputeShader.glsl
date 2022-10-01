#version 430

layout (local_size_x = 16, local_size_y = 16) in;

layout(rgba8, binding = 0) uniform readonly image2D srcImage;
layout(rgba8, binding = 1) uniform image2D destImage;


// Command buffer
layout(std430, binding = 2) buffer FitnessLayout
{
	uint squaredDistance; // For storing the squared distance between the two images
    float greenColor; // For testing that the SSBO is working
}FitnessDetails;


void main(){

	ivec2 texelCoords = ivec2(gl_GlobalInvocationID.xy);

	vec4 srcPixel = 255.0 * imageLoad(srcImage, texelCoords);
	vec4 destPixel = 255.0 *  imageLoad(destImage, texelCoords);
	//vec3 distanceBetween = destPixel.rgb - srcPixel.rgb;
	float dist = distance(srcPixel.rgb,destPixel.rgb);

	highp uint result = uint(floor(dist));

	//imageStore(destImage, texelCoords, vec4(dist,dist,dist,1.0));
	atomicAdd(FitnessDetails.squaredDistance,result);
}