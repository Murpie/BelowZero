#version 440 core
out vec3 FragColor;

in vec2 TexCoords;
in vec2 inverseScreenSize;

uniform sampler2D screenTexture;
uniform int swap;

float rgb2luma(in vec3 rgb)
{
	return sqrt(dot(rgb, vec3(0.299, 0.587, 0.114)));
}

void main()
{
	if (swap == 1)
	{
		FragColor = texture(screenTexture, TexCoords).rgb;
	}
	else
	{
		//... Get the luma for current fragment and neighbours
		//Magic numbers to get luma
		vec3 colorCenter = texture(screenTexture, TexCoords).rgb;

		//Get the luma at the current fragment
		float lumaCenter = rgb2luma(colorCenter);

		//Get the neighbours lumas from the current fragment
		float lumaDown = rgb2luma(textureOffset(screenTexture, TexCoords, ivec2(0, -1)).rgb);
		float lumaUp = rgb2luma(textureOffset(screenTexture, TexCoords, ivec2(0, 1)).rgb);
		float lumaLeft = rgb2luma(textureOffset(screenTexture, TexCoords, ivec2(-1, 0)).rgb);
		float lumaRight = rgb2luma(textureOffset(screenTexture, TexCoords, ivec2(1, 0)).rgb);

		//Get the min and max luma from around the current fragment
		float lumaMin = min(lumaCenter, min(min(lumaDown, lumaUp), min(lumaLeft, lumaRight)));
		float lumaMax = max(lumaCenter, max(max(lumaDown, lumaUp), max(lumaLeft, lumaRight)));

		//Compute the delta (Difference between those two, so we'll get a contrast)
		float lumaRange = lumaMax - lumaMin;

		//Don't compute if it reaches a the threshhold
		if (lumaRange<max(0.0312, lumaMax*0.125))
		{
			FragColor = colorCenter;
			return;
		}

		//... Check if it's horizontal or vertical
		//Query the remaining corners for the lumas (diagonala fragments/pixlar)
		float lumaDownLeft = rgb2luma(textureOffset(screenTexture, TexCoords, ivec2(-1, -1)).rgb);
		float lumaDownRight = rgb2luma(textureOffset(screenTexture, TexCoords, ivec2(1, -1)).rgb);
		float lumaUpLeft = rgb2luma(textureOffset(screenTexture, TexCoords, ivec2(-1, 1)).rgb);
		float lumaUpRight = rgb2luma(textureOffset(screenTexture, TexCoords, ivec2(1, 1)).rgb);

		//Combine all the edge lumas for future computations
		float lumaDownUp = lumaDown + lumaUp;
		float lumaLeftRight = lumaLeft + lumaRight;

		//And for corners
		float lumaLeftCorners = lumaDownLeft + lumaUpLeft;
		float lumaDownCorners = lumaDownLeft + lumaDownRight;
		float lumaRightCorners = lumaDownRight + lumaUpRight;
		float lumaUpCorners = lumaUpRight + lumaUpLeft;

		//Computer the gradient from the horizontal and vertical axis
		float edgeHorizontal = abs(-2.0 * lumaLeft + lumaLeftCorners)
			+ abs(-2.0 * lumaCenter + lumaLeftRight) * 2.0
			+ abs(-2.0 * lumaRight + lumaRightCorners);

		float edgeVertical = abs(-2.0 * lumaUp + lumaUpCorners)
			+ abs(-2.0 * lumaCenter + lumaDownUp) * 2.0
			+ abs(-2.0 * lumaDown + lumaDownCorners);

		//Check if the local edge is horizontal or vertical
		bool isHorizontal = (edgeHorizontal >= edgeVertical);

		//... Choosing the edge's orientation
		float luma1 = isHorizontal ? lumaDown : lumaLeft;
		float luma2 = isHorizontal ? lumaUp : lumaRight;

		//Compute the gradients
		float gradient1 = luma1 - lumaCenter;
		float gradient2 = luma2 - lumaCenter;

		//Check which one is the "real"/steepest edge
		bool is1Steepest = abs(gradient1) >= abs(gradient2);

		//Normalize the direction's gradient
		float gradientScaled = 0.02 * max(abs(gradient1), abs(gradient2));

		//... Time to get the length and move half a pixel in the chosen direction
		//Choose step size for this direction
		float stepLength = isHorizontal ? inverseScreenSize.y : inverseScreenSize.x;

		//Average the luma in that correct
		float lumaLocalAverage = 0.0;

		if (is1Steepest)
		{
			//Switch the directions
			stepLength = -stepLength;
			lumaLocalAverage = 0.5 * (luma1 + lumaCenter);
		}
		else
		{
			lumaLocalAverage = 0.5 * (luma2 + lumaCenter);
		}

		//Shift the UV by half a pixel
		vec2 currentUV = TexCoords;
		if (isHorizontal)
		{
			currentUV.y += stepLength * 0.5;
		}
		else
		{
			currentUV.x += stepLength * 0.5;
		}

		//... Time to explore! (Iteration)
		//Compute the offset (We always assume that we're working on the correct direction)
		vec2 offset = isHorizontal ? vec2(inverseScreenSize.x, 0.0) : vec2(0.0, inverseScreenSize.y);

		//Compute the UVs for each side of the edge (orthogonally)
		vec2 uv1 = currentUV - offset;
		vec2 uv2 = currentUV + offset;

		//Read the lumas at both of the current extremities, and calculate the gradient from that end to the local average luma
		float lumaEnd1 = rgb2luma(texture(screenTexture, uv1).rbg);
		float lumaEnd2 = rgb2luma(texture(screenTexture, uv2).rbg);
		lumaEnd1 = lumaEnd1 - lumaLocalAverage;
		lumaEnd2 = lumaEnd2 - lumaLocalAverage;

		//If the gradients at the extremities are larger than the local gradient --> We've reached the side of the edge
		bool reached1 = abs(lumaEnd1) >= gradientScaled;
		bool reached2 = abs(lumaEnd2) >= gradientScaled;
		bool reachedBoth = reached1 && reached2;

		//If side isn't reached, continue exploring
		if (!reached1)
		{
			uv1 -= offset;
		}
		if (!reached2)
		{
			uv2 += offset;
		}

		//Continue exploration
		if (!reachedBoth)
		{
			for (int i = 2; i < 12; i++)
			{
				//safe check --> read luma in 1st direction and compute the gradient
				if (!reached1)
				{
					lumaEnd1 = rgb2luma(texture(screenTexture, uv1).rgb);
					lumaEnd1 = lumaEnd1 - lumaLocalAverage;
				}
				//and for the opposite direction
				if (!reached2)
				{
					lumaEnd2 = rgb2luma(texture(screenTexture, uv2).rgb);
					lumaEnd2 = lumaEnd2 - lumaLocalAverage;
				}

				//If luma gradient at the current extremeties is larger than local --> We've reached the side of the edge
				//We use the QUALITY to speed things up 																	
				//<-- IF IT FAILS, IT CAN BE HERE BY THE QUALITY
				if (!reached1)
				{
					uv1 -= offset;
				}
				if (!reached2)
				{
					uv2 -= offset;
				}

				if (reachedBoth)
				{
					break;
				}
			}
		}

		//... Time to estimate the offset
		//Compute the sitance to each extremity
		float distance1 = isHorizontal ? (TexCoords.x - uv1.x) : (TexCoords.y - uv1.y);
		float distance2 = isHorizontal ? (uv2.x - TexCoords.x) : (uv2.y - TexCoords.y);

		//In which direction is the extremity closer?
		bool isDirection1 = distance1 < distance2;
		float distanceFinal = min(distance1, distance2);

		//Length of the edge
		float edgeThickness = (distance1 + distance2);

		//UV offset: read in the direction of the closest extremity
		float pixelOffset = -distanceFinal / edgeThickness + 0.5;

		//... Check so we haven't stepped too far
		//is the luma in the center smaller than the local average?
		bool isLumaCenterSmaller = lumaCenter < lumaLocalAverage;

		//Is the luma's gradient in the center smaller than it's neighbours?
		bool correctVariation = ((isDirection1 ? lumaEnd1 : lumaEnd2) < 0.0) != isLumaCenterSmaller;

		//If the variation is incorrect, don't offset
		float finalOffset = correctVariation ? pixelOffset : 0.0;

		//... Subpixel Antialiasing
		//Check the luma average in a 3x3 "neighbourhood"
		float lumaAverage = (1.0 / 12.0) * (2.0 * (lumaDownUp + lumaLeftRight) + lumaLeftCorners + lumaRightCorners);

		//Ratio of the gradient between the global average and the center luma
		float subPixelOffset1 = clamp(abs(lumaAverage - lumaCenter) / lumaRange, 0.0, 1.0);
		float subPixelOffset2 = (-2.0 * subPixelOffset1 + 3.0) * subPixelOffset1 * subPixelOffset1;

		//Compute the final sub-pixel based on this gradient
		float subPixelOffsetFinal = subPixelOffset2 * subPixelOffset2 * 0.75;

		//Pick the biggest of them two
		finalOffset = max(finalOffset, subPixelOffsetFinal);

		//... Final read to get the color
		//Compute the final UV coordinates
		vec2 finalUV = TexCoords;
		if (isHorizontal)
		{
			finalUV.y += finalOffset * stepLength;
		}
		else
		{
			finalUV.x += finalOffset * stepLength;
		}

		//Read the Color of the UVs, then use it
		vec3 finalColor = texture(screenTexture, finalUV).rgb;
		FragColor = finalColor;
	}
}