#include "BasePass.h"

#include <cassert>

#include "Program.h"
#include "ResourceManager.h"
#include "Resources.h"

void BasePass::render(
	const std::vector<std::reference_wrapper<Primitive>>& primitives,
	ResourceManager& resourceManager
) {
	assert(m_material != MaterialHandle::UNASSIGNED);
	if (primitives.size() == 0) return;

	Program& program = resourceManager.getProgram(
		resourceManager.getMaterial(m_material).getProgram()
	);
	resourceManager.getMaterial(m_material).bind(resourceManager);

	for (Primitive& primitive : primitives) {
		auto vertexArray = primitive.getVertexArray();
		vertexArray.bind();

		program.setUniform("model", primitive.getTransformationMatrix());
		program.setUniform(
			"instance_index", (int32_t)primitive.getMaterialIndex()
		);

		glDrawElements(
			GL_TRIANGLES,
			vertexArray.getIndexCount(),
			vertexArray.getIndexFormat(),
			0
		);
	}
}
