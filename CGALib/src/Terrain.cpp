/*
 * Terrain.cpp
 *
 *  Created on: Nov 14, 2019
 *      Author: rey
 */

#include "Headers/Terrain.h"


Terrain::Terrain(float yScale, float yShift, std::string heightMap) {
	this->heightMap = heightMap;

	this->yScale = yScale;
	this->yShift = yShift;

	Texture textureHeightMap = Texture(heightMap);
	textureHeightMap.loadImage(true);
	unsigned char * data = textureHeightMap.getData();

	this->imageWidth = textureHeightMap.getWidth();
	this->imageHeight = textureHeightMap.getHeight();

	int W_VERTEX_COUNT = this->imageWidth;
	int H_VERTEX_COUNT = this->imageHeight;

	heights = new float*[W_VERTEX_COUNT];
	for(int i = 0; i < W_VERTEX_COUNT; ++i)
		heights[i] = new float[H_VERTEX_COUNT];

	normals = new glm::vec3*[W_VERTEX_COUNT];
	for(int i = 0; i < W_VERTEX_COUNT; ++i)
		normals[i] = new glm::vec3[H_VERTEX_COUNT];

	for (int i = 0; i < H_VERTEX_COUNT; i++) {
		for (int j = 0; j < W_VERTEX_COUNT; j++) {
			float height = getHeight(j, i, data, imageWidth, this->imageHeight, textureHeightMap.getChannels());
			glm::vec3 normal = computeNormal(j, i, data, imageWidth,
					this->imageHeight, textureHeightMap.getChannels());
			heights[j][i] = height;
			normals[j][i] = normal;
		}
	}
	
	textureHeightMap.freeImage();
}

Terrain::~Terrain() {
	int VERTEX_COUNT = this->imageWidth;
	for(int i = 0; i < VERTEX_COUNT; ++i)
		delete (heights[i]);
	delete heights;
}

void Terrain::init(){
	
	int W_VERTEX_COUNT = this->imageWidth;
	int H_VERTEX_COUNT = this->imageHeight;

	int count = W_VERTEX_COUNT * H_VERTEX_COUNT;
	vertexArray.resize(count * 3);
	index.resize( 6 * (W_VERTEX_COUNT - 1) * (H_VERTEX_COUNT - 1));

	int vertexPointer = 0;
	for (int i = 0; i < H_VERTEX_COUNT; i++) {
		for (int j = 0; j < W_VERTEX_COUNT; j++) {
			float height = heights[j][i];
			glm::vec3 normal = normals[j][i];
			vertexArray[vertexPointer] = Vertex(
				glm::vec3(-(float)W_VERTEX_COUNT / 2.0f +
							  (float)j,
						  height,
						  -(float)H_VERTEX_COUNT / 2.0f +
							  (float)i),
				glm::vec2((float)j / W_VERTEX_COUNT,
						  (float)i / H_VERTEX_COUNT),
				normal);
			vertexPointer++;
		}
	}

	int pointer = 0;
	for(int gz = 0; gz < H_VERTEX_COUNT - 1; gz++){
		for(int gx = 0; gx < W_VERTEX_COUNT - 1; gx++){
			int topLeft = (gz * W_VERTEX_COUNT) + gx;
			int topRight = topLeft + 1;
			int bottomLeft = ((gz + 1) * W_VERTEX_COUNT) + gx;
			int bottomRight = bottomLeft + 1;
			index[pointer++] = topLeft;
			index[pointer++] = bottomLeft;
			index[pointer++] = topRight;
			index[pointer++] = topRight;
			index[pointer++] = bottomLeft;
			index[pointer++] = bottomRight;
		}
	}
	
	AbstractModel::init();
}

float Terrain::getHeight(int x, int z, unsigned char * data, int imageWidth, int imageHeight, int numeroCanales){
	if(x < 0 || x >= imageWidth || z < 0 || z >=  imageHeight)
		return 0;

	//float r = data[x * numeroCanales + z * (imageWidth * numeroCanales)];
	float g = data[x * numeroCanales + z * (imageWidth * numeroCanales) + 1];
	//float b = data[x * numeroCanales + z * (imageWidth * numeroCanales) + 2];
	float height = (int) g * yScale / 256.0f + yShift;

	return height;

}

glm::vec3 Terrain::computeNormal(int x, int z, unsigned char * data, int imageWidth, int imageHeight, int numeroCanales){
	float heightL = getHeight(x - 1, z, data, imageWidth, imageHeight, numeroCanales);
	float heightR = getHeight(x + 1, z, data, imageWidth, imageHeight, numeroCanales);
	float heightD = getHeight(x, z - 1, data, imageWidth, imageHeight, numeroCanales);
	float heightU = getHeight(x, z + 1, data, imageWidth, imageHeight, numeroCanales);
	glm::vec3 normal = glm::vec3(heightL - heightR, 2.0, heightD - heightU);
	return glm::normalize(normal);
}

float Terrain::getHeightTerrain(float worldX, float worldZ){
	float terrainX = worldX + this->imageWidth / 2.0f - position.x;
	float terrainZ = worldZ + this->imageHeight / 2.0f - position.y;
	float gridSquareSizeX = 1.0f;
	float gridSquareSizeZ = 1.0f;
	int gridX = floor(terrainX / gridSquareSizeX);
	int gridZ = floor(terrainZ / gridSquareSizeZ);
	if(gridX < 0 || gridX > this->imageHeight - 1 || gridZ < 0 || gridZ > this->imageHeight - 1)
		return position.y;
	float xCoord = fmod(terrainX, gridSquareSizeX) / gridSquareSizeX;
	float zCoord = fmod(terrainZ, gridSquareSizeZ) / gridSquareSizeZ;
	float answer;
	if(xCoord <= (1 - zCoord)){
		answer = barryCentric(glm::vec3(0, heights[gridX][gridZ], 0),
				glm::vec3(1, heights[gridX + 1][gridZ], 0),
				glm::vec3(0, heights[gridX][gridZ + 1], 1),
				glm::vec2(xCoord, zCoord));
	}
	else{
		answer = barryCentric(glm::vec3(1, heights[gridX + 1][gridZ], 0),
				glm::vec3(1, heights[gridX + 1][gridZ + 1], 1),
				glm::vec3(0, heights[gridX][gridZ + 1], 1),
				glm::vec2(xCoord, zCoord));
	}
	return answer + position.y;
}

glm::vec3 Terrain::getNormalTerrain(float worldX, float worldZ){
	float terrainX = worldX + this->imageWidth / 2.0f - position.x;
	float terrainZ = worldZ + this->imageHeight / 2.0f - position.y;
	float gridSquareSizeX = 1.0f;
	float gridSquareSizeZ = 1.0f;
	int gridX = floor(terrainX / gridSquareSizeX);
	int gridZ = floor(terrainZ / gridSquareSizeZ);
	if(gridX < 0 || gridX > this->imageHeight - 1 || gridZ < 0 || gridZ > this->imageHeight - 1)
		return glm::vec3(0, 1, 0);
	float xCoord = fmod(terrainX, gridSquareSizeX) / gridSquareSizeX;
	float zCoord = fmod(terrainZ, gridSquareSizeZ) / gridSquareSizeZ;
	glm::vec3 answer;
	if(xCoord <= (1 - zCoord)){
		answer = barryCentric(glm::vec3(0, heights[gridX][gridZ], 0),
				glm::vec3(1, heights[gridX + 1][gridZ], 0),
				glm::vec3(0, heights[gridX][gridZ + 1], 1),
				glm::vec2(xCoord, zCoord), normals[gridX][gridZ],
				normals[gridX + 1][gridZ], normals[gridX][gridZ + 1]);
	}
	else{
		answer = barryCentric(glm::vec3(1, heights[gridX + 1][gridZ], 0),
				glm::vec3(1, heights[gridX + 1][gridZ + 1], 1),
				glm::vec3(0, heights[gridX][gridZ + 1], 1),
				glm::vec2(xCoord, zCoord), normals[gridX + 1][gridZ],
				normals[gridX + 1][gridZ + 1], normals[gridX][gridZ + 1]);
	}
	return glm::normalize(answer);
}

float Terrain::barryCentric(glm::vec3 p1, glm::vec3 p2, glm::vec3 p3, glm::vec2 pos){
	float det = (p2.z - p3.z) * (p1.x - p3.x) + (p3.x - p2.x) * (p1.z - p3.z);
	float l1 = ((p2.z - p3.z) * (pos.x - p3.x) + (p3.x - p2.x) * (pos.y - p3.z)) / det;
	float l2 = ((p3.z - p1.z) * (pos.x - p3.x) + (p1.x - p3.x) * (pos.y - p3.z)) / det;
	float l3 = 1.0f - l1 - l2;
	return l1 * p1.y + l2 * p2.y + l3 * p3.y;
}

glm::vec3 Terrain::barryCentric(glm::vec3 p1, glm::vec3 p2, glm::vec3 p3,
		glm::vec2 pos, glm::vec3 n1, glm::vec3 n2, glm::vec3 n3) {
	float det = (p2.z - p3.z) * (p1.x - p3.x) + (p3.x - p2.x) * (p1.z - p3.z);
	float l1 = ((p2.z - p3.z) * (pos.x - p3.x) + (p3.x - p2.x) * (pos.y - p3.z))
			/ det;
	float l2 = ((p3.z - p1.z) * (pos.x - p3.x) + (p1.x - p3.x) * (pos.y - p3.z))
			/ det;
	float l3 = 1.0f - l1 - l2;
	return l1 * n1 + l2 * n2 + l3 * n3;
}

