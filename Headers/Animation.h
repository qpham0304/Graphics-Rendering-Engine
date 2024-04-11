#pragma once

#include <vector>
#include <map>
#include <glm/glm.hpp>
#include <assimp/scene.h>
#include <bone.h>
#include <functional>
#include <model.h>

struct AssimpNodeData
{
	glm::mat4 transformation;
	std::string name;
	int childrenCount;
	std::vector<AssimpNodeData> children;
};

class Animation
{
public:
	Animation();
	Animation(const std::string& animationPath, Model* model);
	~Animation();

	Bone* FindBone(const std::string& name);

	inline float GetTicksPerSecond() { return m_TicksPerSecond; }

	inline float GetDuration() { return m_Duration; }

	inline const AssimpNodeData& GetRootNode() { return m_RootNode; }

	inline const std::map<std::string, BoneInfo>& GetBoneIDMap()
	{
		return m_BoneInfoMap;
	}


private:
	void ReadMissingBones(const aiAnimation* animation, Model& model);

	void ReadHierarchyData(AssimpNodeData& dest, const aiNode* src);
	float m_Duration = 0.0f;
	int m_TicksPerSecond;
	std::vector<Bone> m_Bones;	// joint animations
	AssimpNodeData m_RootNode;	// joint transformation
	std::map<std::string, BoneInfo> m_BoneInfoMap;
};

