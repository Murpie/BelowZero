#include <string>
#include "glm\glm\glm.hpp" 
#include "glm\glm\gtc\matrix_transform.hpp"
#include "glm\glm\gtc\type_ptr.hpp"
#include <vector>

class Joint
{
public:
	Joint();
	~Joint();

	int parentIndex;
	std::string name;
	std::string parentName;

	glm::mat4 offsetMatrix;
	glm::mat4 transformMatrix;
	glm::mat4 localTransform;
	glm::mat4 nodeTransform;

	glm::quat rot = glm::quat(0.0, 0.0, 0.0, 0.0);
	glm::vec3 pos = glm::vec3(1.0);
	glm::vec3 scale = glm::vec3(1.0);

	std::vector<std::string> childrenNameVector;
	std::vector<int> childrenIDVector;

	std::vector<glm::vec3> posVector;
	std::vector<glm::quat> rotVector;
	std::vector<glm::vec3> scaleVector;

	std::vector<float> posTimes;
	std::vector<float> rotTimes;
	std::vector<float> scaleTimes;


	bool Joint::operator==(const Joint&other);
private:

};