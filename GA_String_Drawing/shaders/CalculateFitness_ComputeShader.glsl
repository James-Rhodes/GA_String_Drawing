#version 430

layout (local_size_x = 16, local_size_y = 16) in;

layout(rgba8, binding = 0) uniform readonly image2D currentAttemptImage;
layout(rgba8, binding = 1) uniform readonly image2D toApproximateImage;


// Command buffer
layout(std430, binding = 2) buffer FitnessLayout
{
	uint squaredDistance; // For storing the squared distance between the two images
}FitnessDetails;


void main(){

	ivec2 texelCoords = ivec2(gl_GlobalInvocationID.xy);

	vec4 srcPixel = 255.0 * imageLoad(currentAttemptImage, texelCoords);
	vec4 destPixel = 255.0 *  imageLoad(toApproximateImage, texelCoords);
	float dist = distance(srcPixel.rgb,destPixel.rgb);

	highp uint result = uint(floor(dist));

	atomicAdd(FitnessDetails.squaredDistance,result);
}